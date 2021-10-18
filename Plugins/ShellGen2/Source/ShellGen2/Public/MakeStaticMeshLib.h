#pragma once

#include "CoreMinimal.h"
#include "BakedMesh.h"
#include "MakeStaticMeshLib.generated.h"

UCLASS(meta=(BlueprintThreadSafe), Category = "Shell Shape Generator")
class UMakeStaticMeshLib : public UBlueprintFunctionLibrary {
  GENERATED_UCLASS_BODY()
  /**
   * Process a BakedMesh into a StaticMesh.
   */
  UFUNCTION(BlueprintCallable, Category = "Shell Shape Generator")
  static UStaticMesh* BakedMeshToStaticMesh(const FBakedMesh& in);
};
