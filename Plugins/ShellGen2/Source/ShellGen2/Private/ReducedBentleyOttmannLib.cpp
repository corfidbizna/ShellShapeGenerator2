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
