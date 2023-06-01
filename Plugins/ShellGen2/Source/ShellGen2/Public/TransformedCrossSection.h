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
#include <vector>
#include <memory>
#include "TransformedCrossSection.generated.h"

/**
 * Used as input to Reduced Bentley-Ottmann. Given a cross section outputted
 * by the shell generator, this provides a translation and scale factor for the
 * individual lines before they are inputted into RBO. (This is much, much,
 * much faster than doing the transformation yourself in Blueprints.)
 */
USTRUCT(BlueprintType, Category = "Shell Shape Generator")
struct SHELLGEN2_API FTransformedCrossSection {
  GENERATED_BODY()
  /**
   * The amount by which the cross section will be translated, after it is
   * scaled.
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D translation;
  /**
   * The amount by which the cross section will be scaled, before it is
   * scaled.
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D scale;
  /**
   * The individual cross section to which these transformations are applied,
   * represented as a line segment. (Each point n forms a line with point n+1
   * modulo the length of the array).
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FVector2D> points;
};
