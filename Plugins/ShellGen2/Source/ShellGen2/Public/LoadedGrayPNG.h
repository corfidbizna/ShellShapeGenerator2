// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <memory>
#include "loaded_gray_png.h"
#include "LoadedGrayPNG.generated.h"

/**
 * An object that encapsulates a loaded grayscale PNG image, suitable for use
 * as a displacement map.
 */
UCLASS(BlueprintType, Category = "Shell Shape Generator")
class SHELLGEN2_API ULoadedGrayPNG : public UObject {
  GENERATED_BODY()
  std::shared_ptr<loaded_gray_png> image;
  ULoadedGrayPNG() {}
  virtual ~ULoadedGrayPNG();
  /**
   * Attempts to load a grayscale PNG. Make sure the resulting reference isn't
   * null before trying to use it!
   */
  UFUNCTION(BlueprintCallable, Category="Shell Shape Generator")
  static ULoadedGrayPNG* LoadGrayPNG(const FString& path);
 public:
  const std::shared_ptr<loaded_gray_png>& GetImage() const { return image; }
};
