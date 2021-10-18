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
  auto normals = mesh.calculate_normals();
  /* Now, at each vertex... */
  auto new_vertices = std::make_shared<std::vector<FVector>>();
  new_vertices->reserve(vertices.size());
  for(auto index = 0; index < normals.size(); ++index) {
    auto v = vertices[index];
    auto& n = normals[index];
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
