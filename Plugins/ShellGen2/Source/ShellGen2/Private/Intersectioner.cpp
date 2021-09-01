// Fill out your copyright notice in the Description page of Project Settings.

#include "Intersectioner.h"

UIntersectioner::~UIntersectioner() {
  if(thread) {
    {
      std::unique_lock<std::mutex> lock(mutex);
      bg.quitting = true;
      new_to_process.notify_all();
    }
    thread->join();
    thread = nullptr;
  }
}

namespace {
  struct StoredLine;
  struct Line {
    FVector2D p1, p2, tangent;
    void possibly_intersect(TArray<FVector2D>& output,
                            const StoredLine& other);
  };
  struct StoredLine {
    FVector2D p1, p2, tangent;
    StoredLine(const Line& other) : p1(other.p1), p2(other.p2),
                                    tangent(other.tangent) {}
  };
  void Line::possibly_intersect(TArray<FVector2D>& output,
                                const StoredLine& other) {
    if(p1 == other.p1 || p1 == other.p2 || p2 == other.p1 || p2 == other.p2)
      return;
    // https://stackoverflow.com/a/1968345
    float s = (-tangent.Y * (p1.X - other.p1.X) + tangent.X * (p1.Y - other.p1.Y)) / (-other.tangent.X * tangent.Y + tangent.X * other.tangent.Y);
    float t = (other.tangent.X * (p1.Y - other.p1.Y) - other.tangent.Y * (p1.X - other.p1.X)) / (-other.tangent.X * tangent.Y + tangent.X * other.tangent.Y);
    if(s >= 0 && s <= 1 && t >= 0 && t <= 1) {
      FVector2D intersection_point = p1 + t * tangent;
      output.Add(intersection_point);
    }
  }
  void process_line_for_intersection(TArray<FVector2D>& output,
                                     std::vector<StoredLine>& lines,
                                     FVector2D p1, FVector2D p2) {
    FVector2D tangent = (p2 - p1);
    Line new_line = {p1, p2, tangent};
    for(const auto& line : lines) {
      new_line.possibly_intersect(output, line);
    }
    lines.emplace_back(new_line);
  }
  FVector2D intersect_between(float xn, float yn, float zn,
                              float xp, float yp, float zp) {
    float theta = zn / -(zp - zn);
    FVector2D ret(xn + (xp - xn) * theta,
                  yn + (yp - yn) * theta);
    return ret;
  }
  void process_triangle_for_intersection(const FBakedMesh& mesh,
                                         TArray<FVector2D>& output,
                                         std::vector<StoredLine>& lines,
                                         float plane_z,
                                         int32 i1, int32 i2, int32 i3) {
    auto& vertices = *mesh.vertices;
    float x1 = vertices[i1];
    float y1 = vertices[i1+1];
    float z1 = vertices[i1+2] - plane_z;
    float x2 = vertices[i2];
    float y2 = vertices[i2+1];
    float z2 = vertices[i2+2] - plane_z;
    float x3 = vertices[i3];
    float y3 = vertices[i3+1];
    float z3 = vertices[i3+2] - plane_z;
    if(z1 < 0.0f) {
      if(z2 < 0.0f) {
        if(z3 < 0.0f) {
          // no intersection, skip this triangle
        }
        else {
          // intersection on 1→3 and 2→3
          process_line_for_intersection(output, lines,
                                        intersect_between(x1, y1, z1,
                                                          x3, y3, z3),
                                        intersect_between(x2, y2, z2,
                                                          x3, y3, z3));
        }
      }
      else {
        if(z3 < 0.0f) {
          // intersection on 1→2 and 3→2
          process_line_for_intersection(output, lines,
                                        intersect_between(x1, y1, z1,
                                                          x2, y2, z2),
                                        intersect_between(x3, y3, z3,
                                                          x2, y2, z2));
        }
        else {
          // intersection on 1→2 and 1→3
          process_line_for_intersection(output, lines,
                                        intersect_between(x1, y1, z1,
                                                          x2, y2, z2),
                                        intersect_between(x1, y1, z1,
                                                          x3, y3, z3));
        }
      }
    }
    else {
      if(z2 < 0.0f) {
        if(z3 < 0.0f) {
          // intersection on 2→1 and 3→1
          process_line_for_intersection(output, lines,
                                        intersect_between(x2, y2, z2,
                                                          x1, y1, z1),
                                        intersect_between(x3, y3, z3,
                                                          x1, y1, z1));
        }
        else {
          // intersection on 2→1 and 2→3
          process_line_for_intersection(output, lines,
                                        intersect_between(x2, y2, z2,
                                                          x1, y1, z1),
                                        intersect_between(x2, y2, z2,
                                                          x3, y3, z3));
        }
      }
      else {
        if(z3 < 0.0f) {
          // intersection on 3→1 and 3→2
          process_line_for_intersection(output, lines,
                                        intersect_between(x3, y3, z3,
                                                          x1, y1, z1),
                                        intersect_between(x3, y3, z3,
                                                          x2, y2, z2));
        }
        else {
          // no intersection, skip this triangle
        }
      }
    }
  }
}

void UIntersectioner::GetSelfIntersectionsOnPlane(const FBakedMesh& mesh,
                                                  TArray<FVector2D>& output, 
                                                  float plane_z) {
  if(!mesh.vertices || !mesh.indices || !mesh.texcoords) {
    UE_LOG(LogTemp, Warning, TEXT("Attempted to get self intersections of a nulled-out mesh!"));
    return;
  }
  std::vector<StoredLine> lines;
  auto& indices = *mesh.indices;
  for(unsigned int n = 0; n + 2 < indices.size(); n += 3) {
    process_triangle_for_intersection(mesh, output, lines, plane_z,
                                      indices[n]*3, indices[n+1]*3,
                                      indices[n+2]*3);
  }
}

void UIntersectioner::StartBGGetSelfIntersectionsOnPlane(const FBakedMesh&mesh,
                                                         float plane_z) {
  std::unique_lock<std::mutex> lock(mutex);
  ++bg.generation;
  bg.mesh = std::make_unique<FBakedMesh>(mesh);
  bg.plane_z = plane_z;
  if(thread == nullptr)
    thread = std::make_unique<std::thread>(&UIntersectioner::bg_intersector,
                                           this);
  new_to_process.notify_one();
}

void UIntersectioner::bg_intersector() {
  unsigned long cur_generation;
  std::unique_ptr<FBakedMesh> mesh;
  float plane_z;
  while(true) {
    {
      std::unique_lock<std::mutex> lock(mutex);
      while(true) {
        if(bg.quitting) return;
        mesh = std::move(bg.mesh);
        if(mesh == nullptr) {
          new_to_process.wait(lock);
        }
        else {
          cur_generation = bg.generation;
          plane_z = bg.plane_z;
          break;
        }
      }
    }
    TArray<FVector2D> result;
    GetSelfIntersectionsOnPlane(*mesh, result, plane_z);
    std::unique_lock<std::mutex> lock(mutex);
    bg.finished_generation = cur_generation;
    bg.last_result = std::move(result);
  }
}

bool UIntersectioner::IsBGSelfIntersectionInProgress() {
  std::unique_lock<std::mutex> lock(mutex);
  return bg.finished_generation == bg.generation;
}

void UIntersectioner::GetBGSelfIntersectionResult(TArray<FVector2D>& intersections) {
  std::unique_lock<std::mutex> lock(mutex);
  intersections = bg.last_result;
}

float UIntersectioner::GetPlatformTimeSeconds() {
  static double epoch = FPlatformTime::Seconds();
  return static_cast<float>(FPlatformTime::Seconds() - epoch);
}
