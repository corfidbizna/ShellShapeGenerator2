#pragma once

#include "CoreMinimal.h"
#include "RadiusInfo.generated.h"

USTRUCT(BlueprintType, Category = "Shell Shape Generator")
struct SHELLGEN2_API FRadiusInfo {
  GENERATED_BODY()
  /** Distance of the CENTER of the tube at the given theta. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite) float spiral_radius;
  /** Distance between the CENTER and the INNER/OUTER EDGE of the tube at the
      given theta. **/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)float tube_normal_radius;
  /** Distance between the CENTER and the SPIRAL-PERPENDICULAR EDGE of the tube
      at the given theta. **/
  UPROPERTY(EditAnywhere, BlueprintReadWrite)float tube_binormal_radius;
  /** Cross section at the given theta, untransformed and centered around
      0,0. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FVector2D> cross_section;
  FRadiusInfo() {}
};
