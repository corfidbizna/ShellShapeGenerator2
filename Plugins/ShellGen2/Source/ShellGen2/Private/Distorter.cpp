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
  if(normals.size() != vertices.size()) {
    UE_LOG(LogTemp, Error, TEXT("Calculating normals went horribly wrong somehow! (Needed %u, got %u)"), (unsigned int)vertices.size(), (unsigned int)normals.size());
    return mesh;
  }
  /* Now, at each vertex... */
  auto new_vertices = std::make_shared<std::vector<FVector>>();
  new_vertices->reserve(vertices.size());
  for(auto index = 0; index < normals.size(); ++index) {
    auto v = vertices[index];
    auto& n = normals[index];
    auto& uv = texcoords[index];
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
