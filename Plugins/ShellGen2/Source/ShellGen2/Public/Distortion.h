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
#include "LoadedGrayPNG.h"
#include "ComposeOperation.h"
#include "Distortion.generated.h"

/**
 * Used by the Distortion Baker. This associates a PNG displacement map with
 * scaling factors.
 */
UCLASS(BlueprintType, Category = "Shell Shape Generator")
class SHELLGEN2_API UDistortion : public UObject {
  GENERATED_BODY()
 public:
  /**
   * The distortion map.
   */
  UPROPERTY() ULoadedGrayPNG* Map = nullptr;
  /**
   * UV scaling factor.
   */
  UPROPERTY() FVector2D UVScale = FVector2D(1.0f, 1.0f);
  /**
   * UV offset.
   */
  UPROPERTY() FVector2D UVOffset = FVector2D(0.0f, 0.0f);
  /**
   * Magnitude of distortion, in model units.
   */
  UPROPERTY() float Magnitude = 1.0f;
  /**
   * A fixed offset added to the magnitude of the distortion, in model units.
   */
  UPROPERTY() float MagnitudeOffset = 0.0f;
  /* (The following two members should be replaced with an array of
     <UDistortion*,ComposeOperation> pairs, but I'm fairly sure that would
     confuse Unreal's object lifespan magic. */
  /**
   * If true, the V texture coordinate will WRAP at V=0. Otherwise, it will
   * mirror.
   */
  UPROPERTY() bool WrapAtV = false;
  /**
   * Other distortions to compose with this one.
   */
  UPROPERTY() TArray<UDistortion*> ComposeWith;
  /**
   * The composition operators to compose with.
   */
  UPROPERTY() TArray<ComposeOperation> Operation;
  /* Why the !@#$ can I not pass parameters to NewObject<...>()!?!! */
  UDistortion() {}
  float sample(const FVector2D& uv) {
    const auto& png = Map->GetImage();
    auto uvscaled = uv * UVScale + UVOffset;
    if(!WrapAtV && uvscaled.Y < 0.0f) uvscaled.Y *= -1.0f;
    auto sample = png->sample(uvscaled.X, uvscaled.Y);
    auto ret = sample * Magnitude + MagnitudeOffset;
    for(int i = 0; i < ComposeWith.Num(); ++i) {
      switch(Operation[i]) {
      case ComposeOperation::ComposeAdd:
        ret += ComposeWith[i]->sample(uv);
        break;
      case ComposeOperation::ComposeMultiply:
        ret *= ComposeWith[i]->sample(uv);
        break;
      case ComposeOperation::ComposeDivide:
        ret /= ComposeWith[i]->sample(uv);
        break;
      }
    }
    return ret;
  }
};
