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
  auto normals = in.calculate_normals();
  FMeshDescriptionBuilder builder;
  builder.SetMeshDescription(&mdesc);
  TArray<FVertexInstanceID> viid_map;
  auto all_group = builder.AppendPolygonGroup();
  viid_map.SetNum(vertices.size(), false);
  for(int32_t n = 0; n < vertices.size(); ++n) {
    FVertexID vid = builder.AppendVertex(vertices[n]);
    FVertexInstanceID viid = builder.AppendInstance(vid);
    builder.SetInstance(viid, texcoords[n], normals[n]);
    viid_map[n] = viid;
  }
  for(int32_t n = 0; n < indices.size(); n += 3) {
    // 0, 2, 1? yeah, to invert the winding, because apparently UE4 wants
    // clockwise winding because DirectX
    builder.AppendTriangle(viid_map[indices[0]],
			   viid_map[indices[2]],
			   viid_map[indices[1]],
			   all_group);
  }
  UStaticMesh* ret = NewObject<UStaticMesh>();
  TArray<const FMeshDescription*> ugh;
  ugh.Emplace(&mdesc);
  ret->BuildFromMeshDescriptions(ugh);
  return ret;
}
