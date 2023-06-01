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
