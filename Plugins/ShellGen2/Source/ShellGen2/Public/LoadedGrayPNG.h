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
