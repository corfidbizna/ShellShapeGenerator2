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
