// Fill out your copyright notice in the Description page of Project Settings.

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
