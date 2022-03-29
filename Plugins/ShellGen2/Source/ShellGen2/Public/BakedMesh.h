#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include "StaticMeshAttributes.h"
#include "BakedMesh.generated.h"

USTRUCT(BlueprintType, Category = "Shell Shape Generator")
struct SHELLGEN2_API FBakedMesh {
  GENERATED_BODY()
  std::shared_ptr<std::vector<FVector> > vertices;
  std::shared_ptr<std::vector<FVector2D> > texcoords;
  std::shared_ptr<std::vector<uint32_t> > indices;
  std::vector<FVector> calculate_normals() const;
  void build_tangent_space(const TArray<FVertexInstanceID>& viid_map,
                           TMeshAttributesRef<FVertexInstanceID, FVector>&
                           normals,
                           TMeshAttributesRef<FVertexInstanceID, FVector>&
                           tangents,
                           TMeshAttributesRef<FVertexInstanceID, float>&
                           binormal_signs) const;
  FBakedMesh() {}
  FBakedMesh(std::shared_ptr<std::vector<FVector> > vertices, std::shared_ptr<std::vector<FVector2D> > texcoords, std::shared_ptr<std::vector<uint32_t> > indices)
  : vertices(std::move(vertices)), texcoords(std::move(texcoords)), indices(std::move(indices)) {}
};
