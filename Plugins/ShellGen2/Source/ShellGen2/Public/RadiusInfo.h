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
