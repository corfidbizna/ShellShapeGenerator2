#pragma once

#include "CoreMinimal.h"
#include "BakedMesh.h"
#include "Distortion.h"
#include "Distorter.generated.h"

/**
 * The object responsible for applying distortion maps to a shell.
 */
UCLASS(BlueprintType, Category = "Shell Shape Generator")
class SHELLGEN2_API UDistorter : public UObject {
  GENERATED_BODY()
public:
  /**
   * Applies the given Distortions to the given BakedMesh, and produces a new
   * BakedMesh.
   */
  UFUNCTION(BlueprintCallable, Category="Morph Baker")
  static FBakedMesh ApplyDistortions(const FBakedMesh& mesh,
                                     const TArray<UDistortion*>& distorts);
};
