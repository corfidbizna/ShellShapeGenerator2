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
