#include "DistortionLib.h"

UDistortionLib::UDistortionLib(const class FObjectInitializer& _) : Super(_) {}

UDistortion* UDistortionLib::MakeDistortion(ULoadedGrayPNG* Map,
                                            FVector2D NumUVRepeats,
                                            FVector2D UVOffset,
                                            float Magnitude,
                                            float MagnitudeOffset) {
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
  return ret;
}
