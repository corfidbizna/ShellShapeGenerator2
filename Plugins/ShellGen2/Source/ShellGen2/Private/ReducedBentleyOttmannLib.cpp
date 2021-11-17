#include "ReducedBentleyOttmannLib.h"

#include "bnlytmn.hpp"

UReducedBentleyOttmannLib::UReducedBentleyOttmannLib(const class FObjectInitializer& _) : Super(_) {}

TArray<FVector2D> UReducedBentleyOttmannLib::ReducedBentleyOttmann
(const TArray<FTransformedCrossSection>& cross_sections) {
  TArray<LineSeg> lines;
  size_t line_count = 0;
  for(auto& cross_section : cross_sections) {
    line_count += cross_section.points.Num();
  }
  lines.Reserve(line_count);
  for(auto& cross_section : cross_sections) {
    for(size_t a = 0; a < cross_section.points.Num(); ++a) {
      size_t b = a + 1;
      if(b >= cross_section.points.Num()) b = 0;
      lines.Add(LineSeg{cross_section.points[a] * cross_section.scale + cross_section.translation,
			cross_section.points[b] * cross_section.scale + cross_section.translation});
    }
  }
  return bnlytmn(lines);
}
