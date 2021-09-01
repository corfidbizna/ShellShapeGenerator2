#include "Distorter.h"

FBakedMesh UDistorter::ApplyDistortions(const FBakedMesh& mesh,
                                        const TArray<UDistortion*>& distorts) {
  if(!mesh.vertices || !mesh.indices || !mesh.texcoords) {
    UE_LOG(LogTemp, Warning, TEXT("Attempted to apply distortions to a nulled-out mesh!"));
    return mesh;
  }
  for(const auto& distortion : distorts) {
    if(distortion->Map == nullptr) {
      UE_LOG(LogTemp, Warning, TEXT("Attempted to apply distortions with a nulled-out LoadedGrayPNG!"));
      return mesh;
    }
  }
  const auto& vertices = *mesh.vertices;
  const auto& texcoords = *mesh.texcoords;
  const auto& indices = *mesh.indices;
  /* Calculate the normals for the mesh. */
  std::vector<FVector> normals(vertices.size()/3);
  for(auto&& n : normals) {
    n = FVector(0.0f);
  }
  /* For each triangle... */
  for(auto it = indices.cbegin(); it != indices.cend();) {
    /* (The three points of the triangle) */
    auto a_index = *it++;
    auto b_index = *it++;
    auto c_index = *it++;
    /* (can we count on the compiler to do constant propagation? WHO KNOWS) */
    auto a = FVector(vertices[a_index*3], vertices[a_index*3+1], vertices[a_index*3+2]);
    auto b = FVector(vertices[b_index*3], vertices[b_index*3+1], vertices[b_index*3+2]);
    auto c = FVector(vertices[c_index*3], vertices[c_index*3+1], vertices[c_index*3+2]);
    auto d = b-a;
    auto e = c-a;
    /* and we have a normal! */
    auto n = FVector::CrossProduct(d, e);
    /* it's not a unit normal, but its magnitude is proportional to the area
       of the triangle. this provides free area-weighting of the contributions
       of each triangle to the resulting normal. I guess? */
    /* accumulate this at each of the three points */
    normals[a_index] += n;
    normals[b_index] += n;
    normals[c_index] += n;
  }
  /* Now, at each vertex... */
  auto new_vertices = std::make_shared<std::vector<float>>();
  new_vertices->reserve(vertices.size());
  for(auto index = 0; index < normals.size(); ++index) {
    auto v = FVector(vertices[index*3], vertices[index*3+1], vertices[index*3+2]);
    auto& n = normals[index];
    n.Normalize(0.0f); // NaN to meet you too.
    auto uv = FVector2D(texcoords[index*2], texcoords[index*2+1]);
    /* For each distortion... */
    float amount = 0.0f;
    for(const auto& distortion : distorts) {
      amount += distortion->sample(uv);
    }
    v += n * amount;
    new_vertices->push_back(v.X);
    new_vertices->push_back(v.Y);
    new_vertices->push_back(v.Z);
  }
  return FBakedMesh(std::move(new_vertices), mesh.texcoords, mesh.indices);
}
