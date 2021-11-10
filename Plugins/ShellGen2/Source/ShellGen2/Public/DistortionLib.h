#pragma once

#include "CoreMinimal.h"
#include "Distortion.h"
#include "ComposeOperation.h"
#include "DistortionLib.generated.h"

UCLASS(meta=(BlueprintThreadSafe), Category = "Shell Shape Generator")
class SHELLGEN2_API UDistortionLib : public UBlueprintFunctionLibrary {
  GENERATED_UCLASS_BODY()
  /**
   * Create a new Distortion with the given displacement map, UV scale factors,
   * and magnitude.
   */
  UFUNCTION(BlueprintPure, meta = (Keywords = "construct build",
                                   NativeMakeFunc),
            Category = "Shell Shape Generator")
  static UDistortion* MakeDistortion(ULoadedGrayPNG* Map = nullptr,
                                     FVector2D NumUVRepeats
                                     = FVector2D(1.0f, 1.0f),
                                     FVector2D UVOffset= FVector2D(0.0f, 0.0f),
                                     float Magnitude = 1.0f,
                                     float MagnitudeOffset = 0.0f,
				     bool WrapAtV = false);
  /**
   * Compose two Distortions together. Returns a Distortion that will apply the
   * requested operation to the results two passed-in Distortions and apply the
   * result of that operation.
   */
  UFUNCTION(BlueprintPure, Category = "Shell Shape Generator")
  static UDistortion* MakeComposedDistortion(UDistortion* a,
                                             UDistortion* b,
                                             ComposeOperation operation);
};
