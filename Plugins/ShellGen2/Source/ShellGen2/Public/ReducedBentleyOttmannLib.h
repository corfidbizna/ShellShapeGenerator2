#pragma once

#include "CoreMinimal.h"
#include "TransformedCrossSection.h"
#include "ReducedBentleyOttmannLib.generated.h"

UCLASS(meta=(BlueprintThreadSafe), Category = "Shell Shape Generator")
class SHELLGEN2_API UReducedBentleyOttmannLib : public UBlueprintFunctionLibrary {
  GENERATED_UCLASS_BODY()
  /**
   * Perform Reduced Bentley-Ottmann on the given transformed cross sections.
   * Returns a list giving every point at which the cross sections intersect.
   *
   * Hopefully there are no exactly coincident endpoints between the two cross
   * sections. That would be bad! Ha ha ha ha ha ha ha ha ha... haha...
   */
  UFUNCTION(BlueprintPure, Category = "Shell Shape Generator")
  static TArray<FVector2D> ReducedBentleyOttmann
    (const TArray<FTransformedCrossSection>& cross_sections);
};
