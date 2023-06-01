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

#include "MakeStaticMeshLib.h"
#include "Engine/StaticMesh.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"

UMakeStaticMeshLib::UMakeStaticMeshLib(const class FObjectInitializer& _)
  : Super(_) {}

UStaticMesh* UMakeStaticMeshLib::BakedMeshToStaticMesh(const FBakedMesh& in) {
  if(in.vertices == nullptr) return nullptr; // nulled out mesh...
  auto& vertices = *in.vertices;
  auto& texcoords = *in.texcoords;
  auto& indices = *in.indices;
  FMeshDescription mdesc;
  FStaticMeshAttributes attr(mdesc);
  attr.Register();
  mdesc.ReserveNewVertices(vertices.size());
  mdesc.ReserveNewVertexInstances(vertices.size());
  mdesc.ReserveNewTriangles(indices.size()/3);
  FMeshDescriptionBuilder builder;
  builder.SetMeshDescription(&mdesc);
  TArray<FVertexInstanceID> viid_map;
  auto all_group = builder.AppendPolygonGroup();
  viid_map.SetNum(vertices.size(), false);
  for(int32_t n = 0; n < vertices.size(); ++n) {
    FVertexID vid = builder.AppendVertex(vertices[n]);
    FVertexInstanceID viid = builder.AppendInstance(vid);
    // we'll build the normals later
    builder.SetInstance(viid, texcoords[n], FVector(0.0f, 0.0f, 0.0f));
    viid_map[n] = viid;
  }
  for(int32_t n = 0; n < indices.size(); n += 3) {
    // 0, 2, 1? yeah, to invert the winding, because apparently UE4 wants
    // clockwise winding because DirectX
    builder.AppendTriangle(viid_map[indices[n+0]],
			   viid_map[indices[n+2]],
			   viid_map[indices[n+1]],
			   all_group);
  }
  UStaticMesh* ret = NewObject<UStaticMesh>();
  ret->StaticMaterials.Add(FStaticMaterial());
  auto normals = mdesc.VertexInstanceAttributes().GetAttributesRef<FVector>(MeshAttribute::VertexInstance::Normal);
  auto tangents = mdesc.VertexInstanceAttributes().GetAttributesRef<FVector>(MeshAttribute::VertexInstance::Tangent);
  auto binormal_signs = mdesc.VertexInstanceAttributes().GetAttributesRef<float>(MeshAttribute::VertexInstance::BinormalSign);
  in.build_tangent_space(viid_map, normals, tangents, binormal_signs);
  TArray<const FMeshDescription*> ugh;
  ugh.Emplace(&mdesc);
  ret->BuildFromMeshDescriptions(ugh);
  return ret;
}
