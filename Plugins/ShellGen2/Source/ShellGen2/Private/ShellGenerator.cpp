/*
 * This file is part of Shell Shape Generator 2.
 *
 * Copyright ©2023 Olivia Jenkins
 *
 * Shell Shape Generator 2 is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Shell Shape Generator 2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Shell Shape Generator 2. If not, see <https://www.gnu.org/licenses/>.
 */

#include "ShellGenerator.h"

#include <cassert>

namespace {
  float powf_munged(float a, float b) {
    if(a < 1.0f) return a;
    else return powf(a, b);
  }
  void eval_segment(std::vector<FVector2D>& out,
                    FVector2D p0, FVector2D c0,
                    FVector2D c1, FVector2D p1,
                    int depth_left) {
    bool should_divide;
    if(depth_left < 0) {
      // assert(depth_left < 0)
      // We're doing dynamic subdivision. We want to subdivide iff the
      // control points are nearly collinear with the other points.
      FVector2D forward = p1 - p0;
      float forward_magnitude = forward.Size();
      if(forward_magnitude < (1.f/1024.f)) {
        // Things are getting too close together for our taste.
        // Unconditionally DO NOT subdivide.
        should_divide = false;
      }
      else {
        FVector2D normal = FVector2D(-forward.Y, forward.X);
	normal.Normalize(0.0f);
        float linear_dot = p0 | normal;
        float c0_dot = c0 | normal;
        float c1_dot = c1 | normal;
        float threshold = forward.Size() * (1.f/128.f);
        // Subdivide if the deviation is greater than roughly a percent.
        should_divide =
          fabs(c0_dot - linear_dot) >= threshold
          || fabs(c1_dot - linear_dot) >= threshold;
      }
    }
    else {
      should_divide = depth_left > 0;
    }
    if(should_divide) {
      FVector2D q0 = (p0+c0)*0.5f;
      FVector2D q1 = (c0+c1)*0.5f;
      FVector2D q2 = (c1+p1)*0.5f;
      FVector2D r0 = (q0+q1)*0.5f;
      FVector2D r1 = (q1+q2)*0.5f;
      FVector2D b = (r0+r1)*0.5f;
      --depth_left;
      eval_segment(out, p0, q0, r0, b, depth_left);
      eval_segment(out, b, r1, q2, p1, depth_left);
      return;
    }
    // If we didn't descend, output a point.
    out.push_back(p0);
  }
  std::vector<FVector> smoosh_curves(const Curve& cross_curve,
				     const Curve& grain_curve,
				     int curve_depth) {
    auto cross = cross_curve.evaluate(curve_depth);
    // before we proceed, normalize the Y coordinate...
    float max_y = (1.f/128.f);
    for(const auto& v : cross) {
      if(max_y < v.Y) max_y = v.Y;
    }
    float r_max_y = 1.f / max_y;
    for(auto& v : cross) {
      v.Y *= r_max_y;
    }
    auto grain = grain_curve.evaluate_dynamic();
    // Process the grain so that x monotonically increases. Any time x "goes
    // backward", we use the mapping that occurs *later in the array*. (We
    // actually want to use the mapping with the higher y, but I can't wrap
    // my head around that math right now.)
    std::vector<FVector2D> grain_sorted;
    grain_sorted.reserve(grain.size());
    grain_sorted.push_back(grain[0]);
    for(unsigned int n = 1; n < grain.size(); ++n) {
      auto& nu = grain[n];
      // As long as there are at least two elements left, and the second to
      // last element is to our right, delete the last element.
      while(grain_sorted.size() >= 2
            && grain_sorted[grain_sorted.size()-2].X > nu.X) {
        grain_sorted.pop_back();
      }
      // If the last element is to our right (we can assume there is no
      // element before it that is to our right, because the above loop
      // ensured that that would not happen) then...
      if(grain_sorted.size() >= 1
         && grain_sorted[grain_sorted.size()-1].X > nu.X) {
        // If it's the last element, just kill it.
        if(grain_sorted.size() == 1) {
          grain_sorted.pop_back();
        }
        // Otherwise, we need to create a cliff...
        else {
          auto& second_from_last = grain_sorted[grain_sorted.size()-2];
          auto& last = grain_sorted[grain_sorted.size()-1];
          if last.X != second_from_last.X) {
            float y_per_x = (last.Y - second_from_last.Y)
              / (last.X - second_from_last.X);
            last.Y = second_from_last.Y + (last.X - second_from_last.X) * y_per_x;
          }
          last.X = nu.X;
        }
      }
      grain_sorted.push_back(nu);
    }
    // So now grain_sorted contains mappings between the tube X coordinate
    // (tube X = out from center of spiral) and the forward displacement of
    // the shell at that X coordinate.
    std::vector<FVector> smooshed;
    smooshed.reserve(cross.size());
    auto left_grain = grain_sorted.cbegin();
    for(const auto& in : cross) {
      // Find the sweet spot in the grain to give us a Z...
      while(left_grain->X > in.X
            && left_grain != grain_sorted.cbegin())
        --left_grain;
      while(left_grain != grain_sorted.cend()-1
            && (left_grain+1)->X < in.X)
        ++left_grain;
      // either left_grain is now to our left, XOR it is the first element
      // in grain_sorted.
      float z;
      if(left_grain->X > in.X) {
        // it's the first element, just use its Y directly (extrapolate)
        z = left_grain->Y;
      }
      else if(left_grain == grain_sorted.cend()-1) {
        // it's the last element. also use its Y directly. (extrapolate)
        z = left_grain->Y;
      }
      else {
        // it's between two elements. interpolate.
        auto right_grain = left_grain + 1;
        float diff = right_grain->X - left_grain->X;
        if(diff == 0) {
          // wow, unlucky, we landed right on a cliff.
          // use whichever Z has a higher value.
          z = fmax(left_grain->Y, right_grain->Y);
        }
        else {
          float i = (in.X-left_grain->X) / (right_grain->X-left_grain->X);
          z = left_grain->Y + (right_grain->Y-left_grain->Y) * i;
        }
      }
      smooshed.emplace_back(in.X, in.Y, z);
    }
    return smooshed;
  }
  float young_old_curve(float base, float in_theta,
			float young_rate, float young_end,
			float old_start, float old_rate,
			float old_end, float aperture_start,
			float aperture_rate) {
    float theta, theta_scaled, left_coeff, right_coeff;
    float ret = base * powf(young_rate, fmin(in_theta, young_end));
    if(in_theta > young_end) {
      theta = fmin(in_theta, old_start) - young_end;
      theta_scaled = theta / (old_start - young_end);
      right_coeff = (theta_scaled * theta_scaled) * 0.5f;
      left_coeff = theta_scaled - right_coeff;
      ret *= powf(young_rate, left_coeff * (old_start - young_end))
        * powf(old_rate, right_coeff * (old_start - young_end));
      if(in_theta > old_start) {
        theta = fmin(in_theta, old_end) - old_start;
        ret *= powf(old_rate, theta);
        if(in_theta > old_end) {
          theta = fmin(in_theta, aperture_start) - old_end;
          theta_scaled = theta / (aperture_start - old_end);
          right_coeff = (theta_scaled * theta_scaled) * 0.5f;
          left_coeff = theta_scaled - right_coeff;
          ret *= powf(old_rate, left_coeff * (aperture_start - old_end))
            * powf(aperture_rate, right_coeff * (aperture_start - old_end));
          if(in_theta > aperture_start) {
            theta = in_theta - aperture_start;
            ret *= powf(aperture_rate, theta);
          }
        }
      }
    }
    return ret;
  }
}

UShellGenerator::~UShellGenerator() {
  if(thread) {
    {
      std::unique_lock<std::mutex> lock(bg.mutex);
      bg.quitting = true;
      bg.new_to_process.notify_all();
    }
    thread->join();
    thread = nullptr;
  }
}

UShellGenerator::UShellGenerator(const FObjectInitializer& initializer)
  : Super(initializer) {}

void UShellGenerator::BeginGeneratingShell
(float starting_normal_rad,
 float starting_binormal_rad,
 float starting_spiral_rad,
 const TArray<FCurveNode>& young_cross,
 const TArray<FCurveNode>& young_grain,
 float normal_growth_young,
 float binormal_growth_young,
 float spiral_growth_young,
 float young_end,
 float old_start,
 const TArray<FCurveNode>& old_cross,
 const TArray<FCurveNode>& old_grain,
 float normal_growth_old,
 float binormal_growth_old,
 float spiral_growth_old,
 float old_end,
 float aperture_start,
 const TArray<FCurveNode>& aperture_cross,
 const TArray<FCurveNode>& aperture_grain,
 float normal_growth_aperture,
 float binormal_growth_aperture,
 float spiral_growth_aperture,
 float current_age,
 float final_age,
 const TArray<FVector2D>& young_endcaps,
 const TArray<FVector2D>& old_endcaps,
 const TArray<float>& radius_requests,
 float length_per_iteration,
 int curve_subdivision,
 float theta_exponent,
 float spiral_offset_constant) {
  std::unique_lock<std::mutex> lock(bg.mutex);
  ++bg.generation;
  // oh boy
  if(theta_exponent < 1.0f / 8.0f) theta_exponent = 1.0f / 8.0f;
  else if(theta_exponent > 8.0f) theta_exponent = 8.0f;
  bg.desired_params.theta_exponent = theta_exponent;
  bg.desired_params.starting_normal_rad = starting_normal_rad;
  bg.desired_params.starting_binormal_rad = starting_binormal_rad;
  bg.desired_params.starting_spiral_rad = starting_spiral_rad;
  bg.desired_params.young_cross.curve = young_cross;
  bg.desired_params.young_grain.curve = young_grain;
  bg.desired_params.normal_growth_young = normal_growth_young;
  bg.desired_params.binormal_growth_young = binormal_growth_young;
  bg.desired_params.spiral_growth_young = spiral_growth_young;
  bg.desired_params.lin_young_end = young_end;
  bg.desired_params.lin_old_start = old_start;
  bg.desired_params.young_end = powf_munged(young_end, theta_exponent);
  bg.desired_params.old_start = powf_munged(old_start, theta_exponent);
  bg.desired_params.old_cross.curve = old_cross;
  bg.desired_params.old_grain.curve = old_grain;
  bg.desired_params.normal_growth_old = normal_growth_old;
  bg.desired_params.binormal_growth_old = binormal_growth_old;
  bg.desired_params.spiral_growth_old = spiral_growth_old;
  bg.desired_params.lin_old_end = old_end;
  bg.desired_params.lin_aperture_start = aperture_start;
  bg.desired_params.old_end = powf_munged(old_end, theta_exponent);
  bg.desired_params.aperture_start = powf_munged(aperture_start, theta_exponent);
  bg.desired_params.aperture_cross.curve = aperture_cross;
  bg.desired_params.aperture_grain.curve = aperture_grain;
  bg.desired_params.normal_growth_aperture = normal_growth_aperture;
  bg.desired_params.binormal_growth_aperture = binormal_growth_aperture;
  bg.desired_params.spiral_growth_aperture = spiral_growth_aperture;
  bg.desired_params.current_age = current_age;
  bg.desired_params.final_age = final_age;
  bg.desired_params.length_per_iteration = length_per_iteration;
  bg.desired_params.curve_subdivision = curve_subdivision;
  bg.desired_params.radius_requests = radius_requests;
  bg.desired_params.young_endcaps = young_endcaps;
  bg.desired_params.old_endcaps = old_endcaps;
  bg.desired_params.spiral_offset_constant = spiral_offset_constant;
  bg.params_available = true;
  if(thread == nullptr)
    thread = std::make_unique<std::thread>(&bg_gen_state::bg_thread_func, &bg);
  bg.new_to_process.notify_one();
}

void bg_gen_state::bg_thread_func() {
  unsigned long cur_generation;
  while(true) {
    {
      std::unique_lock<std::mutex> lock(mutex);
      while(true) {
        if(quitting) return;
        if(!params_available) {
          new_to_process.wait(lock);
        }
        else {
          cur_generation = generation;
	  cur_params = desired_params;
          break;
        }
      }
    }
    const auto& p = cur_params;
    FBakedMesh mesh;
    mesh.vertices = std::make_shared<std::vector<FVector>>();
    mesh.texcoords = std::make_shared<std::vector<FVector2D>>();
    mesh.indices = std::make_shared<std::vector<uint32_t>>();
    first_segment_attached = false;
    last_segment_is_full = true;
    auto young_curve = smoosh_curves(p.young_cross, p.young_grain, p.curve_subdivision);
    auto old_curve = smoosh_curves(p.old_cross, p.old_grain, p.curve_subdivision);
    auto aperture_curve = smoosh_curves(p.aperture_cross, p.aperture_grain, p.curve_subdivision);
    assert(young_curve.size() == old_curve.size());
    assert(aperture_curve.size() == old_curve.size());
    std::vector<FVector> temp;
    temp.reserve(young_curve.size());
    TArray<FRadiusInfo> radius_info;
    radius_info.Reserve(p.radius_requests.Num());
    for(auto linear_theta : p.radius_requests) {
      float theta = powf_munged(linear_theta, p.theta_exponent);
      struct FRadiusInfo i;
      i.spiral_radius = p.get_tube_center_d(linear_theta, theta);
      i.tube_normal_radius = p.get_tube_normal_radius(theta);
      i.tube_binormal_radius = p.get_tube_binormal_radius(theta);
      auto cross_section = p.curve_at(young_curve, old_curve, aperture_curve,
				      temp, theta);
      // hey, isn't it great that Unreal has its own equivalent to std::vector
      // that isn't compatible at all? What a useful thing.
      i.cross_section.Reserve(cross_section->size());
      for(auto el : *cross_section) {
	i.cross_section.Emplace(std::move(el));
      }
      radius_info.Emplace(std::move(i));
    }
    for(int i = 0; i < p.young_endcaps.Num(); ++i) {
      const auto& v = p.young_endcaps[i];
      if(v.Y <= 0.0f) {
	p.point_at(mesh, v.X);
	attach_shell_segment(mesh, false, young_curve.size());
      }
      else {
	p.build_shell_at(mesh, young_curve, old_curve, aperture_curve, temp,
			 v.X, v.Y);
	attach_shell_segment(mesh, true, young_curve.size());
      }
    }
    float target_age = p.final_age * p.current_age;
    float theta = 0.0f;
    while(theta < target_age) {
      p.build_shell_at(mesh, young_curve, old_curve, aperture_curve, temp,
		       theta, 1.f);
      attach_shell_segment(mesh, true, young_curve.size());
      float buff = fmin(fmax(p.length_per_iteration / fmax(1.f, p.get_tube_center_d(theta, powf_munged(theta, p.theta_exponent))), 0.01f), 3.14159265358979323846264328f/3.0f);
      theta += buff;
    }
    for(int i = 0; i < p.old_endcaps.Num(); ++i) {
      const auto& v = p.old_endcaps[i];
      if(v.Y <= 0.0f) {
	p.point_at(mesh, target_age + v.X);
	attach_shell_segment(mesh, false, young_curve.size());
      }
      else {
	p.build_shell_at(mesh, young_curve, old_curve, aperture_curve, temp,
			 target_age + v.X, v.Y);
	attach_shell_segment(mesh, true, young_curve.size());
      }
    }
    std::unique_lock<std::mutex> lock(mutex);
    last_baked_mesh = mesh;
    last_radius_info = radius_info;
    finished_generation = cur_generation;
    all_done.notify_all();
  }
}

bool UShellGenerator::IsGenerationStillInProgress() {
  std::unique_lock<std::mutex> lock(bg.mutex);
  return bg.finished_generation != bg.generation;
}

FBakedMesh UShellGenerator::TakeLastGeneratedShell(TArray<FRadiusInfo>& i) {
  std::unique_lock<std::mutex> lock(bg.mutex);
  i = bg.last_radius_info;
  return bg.last_baked_mesh;
}

FBakedMesh UShellGenerator::BlockForGeneratedShell(TArray<FRadiusInfo>& i) {
  std::unique_lock<std::mutex> lock(bg.mutex);
  while(bg.finished_generation != bg.generation)
    bg.all_done.wait(lock);
  i = bg.last_radius_info;
  return bg.last_baked_mesh;
}

std::vector<FVector2D> Curve::evaluate(int max_depth) const {
  std::vector<FVector2D> ret;
  if(max_depth >= 0) {
    ret.reserve((1 << max_depth) * (curve.Num()-1)
                * (type == CurveType::Circle ? 2 : 1)
                + (type == CurveType::Flat ? 0 : 0));
  }
  /* If max_depth < 0, dynamic subdivision has been requested, and we don't
     even want to guess how much that will need. */
  for(int n = 0; n < curve.Num() - 1; ++n) {
    auto& p0 = curve[n].anchor;
    auto& c0 = curve[n].control;
    auto& p1 = curve[n+1].anchor;
    auto c1 = curve[n+1].get_virtual();
    eval_segment(ret, p0, c0, c1, p1, max_depth);
  }
  if(type == CurveType::Circle) {
    // And again, but backwards!
    for(int n = curve.Num() - 2; n >= 0; --n) {
      auto p0 = curve[n].anchor;
      auto c0 = curve[n].control;
      auto p1 = curve[n+1].anchor;
      auto c1 = curve[n+1].get_virtual();
      p0.Y *= -1.f;
      c0.Y *= -1.f;
      c1.Y *= -1.f;
      p1.Y *= -1.f;
      eval_segment(ret, p1, c1, c0, p0, max_depth);
    }
  }
  else {
    ret.push_back(curve[curve.Num()-1].anchor);
  }
  return ret;
}

void shell_params::point_at(FBakedMesh& mesh, float theta) const {
  float spiral_rad = get_tube_center_d(theta, powf_munged(theta, theta_exponent));
  float theta_radians = theta * -PI;
  float c = cos(theta_radians);
  float s = sin(theta_radians);
  mesh.vertices->emplace_back(spiral_rad * c, spiral_rad * s, 0.f);
  mesh.texcoords->emplace_back(theta, 1);
}

const std::vector<FVector>*
shell_params::curve_at(const std::vector<FVector>& young_curve,
		       const std::vector<FVector>& old_curve,
		       const std::vector<FVector>& aperture_curve,
		       std::vector<FVector>& temp,
		       float theta) const {
  const std::vector<FVector>* curve;
  if(theta <= young_end) {
    curve = &young_curve;
  }
  else if(theta < old_start) {
    float i = (theta - young_end) / (old_start - young_end);
    temp.clear();
    for(size_t n = 0; n < young_curve.size(); ++n) {
      temp.push_back(young_curve[n] + (old_curve[n] - young_curve[n]) * i);
    }
    curve = &temp;
  }
  else if(theta <= old_end) {
    curve = &old_curve;
  }
  else if(theta < aperture_start) {
    float i = (theta - old_end) / (aperture_start - old_end);
    temp.clear();
    for(size_t n = 0; n < old_curve.size(); ++n) {
      temp.push_back(old_curve[n] + (aperture_curve[n] - old_curve[n]) * i);
    }
    curve = &temp;
  }
  else {
    curve = &aperture_curve;
  }
  return curve;
}


void shell_params::build_shell_at(FBakedMesh& mesh,
				  const std::vector<FVector>& young_curve,
				  const std::vector<FVector>& old_curve,
                                  const std::vector<FVector>& aperture_curve,
                                  std::vector<FVector>& temp,
                                  float linear_theta, float scale) const {
  float theta = powf_munged(linear_theta, theta_exponent);
  const std::vector<FVector>* curve = curve_at(young_curve, old_curve,
					       aperture_curve, temp, theta);
  float tube_rad = get_tube_normal_radius(theta) * scale;
  float tube_width = get_tube_binormal_radius(theta) * scale;
  float spiral_rad = get_tube_center_d(linear_theta, theta);
  float theta_radians = linear_theta * -PI;
  float c = cos(theta_radians);
  float s = sin(theta_radians);
  float xplus = spiral_rad * c;
  float yplus = spiral_rad * s;
  FVector transform_x(-tube_rad * c, 0.f, tube_rad * s);
  FVector transform_y(-tube_rad * s, 0.f, -tube_rad * c);
  FVector transform_z(0.f, tube_width, 0.f);
  float v_mul = 1.f / (curve->size() / 2);
  for(unsigned int i = 0; i < curve->size(); ++i) {
    const auto& in = (*curve)[i];
    mesh.vertices->emplace_back((in | transform_x) + xplus,
				(in | transform_y) + yplus,
				in | transform_z);
    float v = i * v_mul;
    if(v > 1.f) v -= 2.f; // not >=
    mesh.texcoords->emplace_back(linear_theta, v);
  }
}

void bg_gen_state::attach_shell_segment(FBakedMesh& mesh, bool is_full, unsigned int num_points) {
  auto& vertices = *mesh.vertices;
  auto& indices = *mesh.indices;
  if(!first_segment_attached) {
    first_segment_attached = true;
  }
  else {
    assert(is_full || last_segment_is_full);
    if(is_full && last_segment_is_full) {
      for(unsigned int i = 0; i < num_points; ++i) {
        int next_i = (i+1) % num_points;
        indices.push_back(vertices.size() - num_points * 2 + i);
        indices.push_back(vertices.size() - num_points * 2 + next_i);
        indices.push_back(vertices.size() - num_points + next_i);
        indices.push_back(vertices.size() - num_points * 2 + i);
        indices.push_back(vertices.size() - num_points + next_i);
        indices.push_back(vertices.size() - num_points + i);
      }
    }
    else if(is_full) {
      for(unsigned int i = 0; i < num_points; ++i) {
        int next_i = (i+1) % num_points;
        indices.push_back(vertices.size() - num_points - 1);
        indices.push_back(vertices.size() - num_points + next_i);
        indices.push_back(vertices.size() - num_points + i);
      }
    }
    else if(last_segment_is_full) {
      for(unsigned int i = 0; i < num_points; ++i) {
        int next_i = (i+1) % num_points;
        indices.push_back(vertices.size() - num_points - 1 + i);
        indices.push_back(vertices.size() - num_points - 1 + next_i);
        indices.push_back(vertices.size() - 1);
      }
    }
  }
  last_segment_is_full = is_full;
}

float shell_params::get_tube_normal_radius(float theta) const {
  return young_old_curve(starting_normal_rad, theta,
                         normal_growth_young, young_end,
                         old_start, normal_growth_old,
                         old_end, aperture_start,
                         normal_growth_aperture);
}

float shell_params::get_tube_binormal_radius(float theta) const {
  return young_old_curve(starting_binormal_rad, theta,
                         binormal_growth_young, young_end,
                         old_start, binormal_growth_old,
                         old_end, aperture_start,
                         binormal_growth_aperture);
}

float shell_params::get_spiral_radius(float theta) const {
  return young_old_curve(starting_spiral_rad, theta,
                         spiral_growth_young,
                         lin_young_end,
                         lin_old_start,
                         spiral_growth_old,
                         lin_old_end, lin_aperture_start,
                         spiral_growth_aperture) + spiral_offset_constant;
}

float shell_params::get_tube_center_d(float linear_theta, float theta) const {
  return get_spiral_radius(linear_theta) + get_tube_normal_radius(theta);
}

UShellGenerator* UShellGenerator::MakeShellGenerator() {
  return NewObject<UShellGenerator>();
}
