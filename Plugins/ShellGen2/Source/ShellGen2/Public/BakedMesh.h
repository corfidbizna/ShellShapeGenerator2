#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <memory>
#include "BakedMesh.generated.h"

USTRUCT(BlueprintType, Category = "Shell Shape Generator")
struct SHELLGEN2_API FBakedMesh {
  GENERATED_BODY()
  std::shared_ptr<std::vector<FVector> > vertices;
  std::shared_ptr<std::vector<FVector2D> > texcoords;
  std::shared_ptr<std::vector<uint32_t> > indices;
  FBakedMesh() {}
  FBakedMesh(std::shared_ptr<std::vector<FVector> > vertices, std::shared_ptr<std::vector<FVector2D> > texcoords, std::shared_ptr<std::vector<uint32_t> > indices)
  : vertices(std::move(vertices)), texcoords(std::move(texcoords)), indices(std::move(indices)) {}
};
