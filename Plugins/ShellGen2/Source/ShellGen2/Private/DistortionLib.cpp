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

#include "DistortionLib.h"

UDistortionLib::UDistortionLib(const class FObjectInitializer& _) : Super(_) {}

UDistortion* UDistortionLib::MakeDistortion(ULoadedGrayPNG* Map,
                                            FVector2D NumUVRepeats,
                                            FVector2D UVOffset,
                                            float Magnitude,
                                            float MagnitudeOffset,
					    bool WrapAtV) {
  auto ret = NewObject<UDistortion>();
  ret->Map = Map;
  ret->UVScale = NumUVRepeats;
  ret->UVOffset = UVOffset;
  if(Map != nullptr) {
    ret->UVScale.X *= static_cast<float>(Map->GetImage()->width);
    ret->UVScale.Y *= static_cast<float>(Map->GetImage()->height);
    ret->UVOffset.X = ret->UVOffset.X
      * static_cast<float>(Map->GetImage()->width) - 0.5f;
    ret->UVOffset.Y = ret->UVOffset.Y
      * static_cast<float>(Map->GetImage()->height) - 0.5f;
  }
  ret->Magnitude = Magnitude;
  ret->MagnitudeOffset = MagnitudeOffset;
  ret->WrapAtV = WrapAtV;
  return ret;
}

UDistortion* UDistortionLib::MakeComposedDistortion(UDistortion* a,
                                                    UDistortion* b,
                                                    ComposeOperation op) {
  auto ret = NewObject<UDistortion>();
  ret->Map = a->Map;
  ret->UVScale = a->UVScale;
  ret->UVOffset = a->UVOffset;
  ret->Magnitude = a->Magnitude;
  ret->MagnitudeOffset = b->MagnitudeOffset;
  ret->ComposeWith = a->ComposeWith;
  ret->Operation = a->Operation;
  ret->ComposeWith.Add(b);
  ret->Operation.Add(op);
  ret->WrapAtV = a->WrapAtV;
  return ret;
}
