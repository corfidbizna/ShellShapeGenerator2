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
  std::vector<FVector> normals(vertices.size());
  for(auto&& n : normals) {
    n = FVector(0.0f);
  }
  /* For each triangle... */
  for(auto it = indices.cbegin(); it != indices.cend();) {
    /* (The three points of the triangle) */
    auto a_index = *it++;
    auto b_index = *it++;
    auto c_index = *it++;
    auto& a = vertices[a_index];
    auto& b = vertices[b_index];
    auto& c = vertices[c_index];
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
  auto new_vertices = std::make_shared<std::vector<FVector>>();
  new_vertices->reserve(vertices.size());
  for(auto index = 0; index < normals.size(); ++index) {
    auto v = vertices[index];
    auto& n = normals[index];
    n.Normalize(0.0f); // NaN to meet you too.
    auto& uv = texcoords[index*2];
    /* For each distortion... */
    float amount = 0.0f;
    for(const auto& distortion : distorts) {
      amount += distortion->sample(uv);
    }
    v += n * amount;
    new_vertices->push_back(v);
  }
  return FBakedMesh(std::move(new_vertices), mesh.texcoords, mesh.indices);
}
