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
#include "CurveNode.generated.h"

/**
 * Used by the Shell Generator. This is a single anchor point, a single control
 * point along the tangent of the curve, and a "virtual proportion", which is
 * the PROPORTION of the control point's "magnitude" that the anti-tangent
 * control point has.
 */
USTRUCT(BlueprintType, Category = "Shell Shape Generator")
struct SHELLGEN2_API FCurveNode {
  GENERATED_BODY()
  /**
   * The anchor point. This is a point through which the curve passes.
   */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D anchor;
  /**
   * The control point. This is a point toward which the curve bends.
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D control;
  /**
   * The magnitude of the anti-tangent control point, as a proportion of the
   * magnitude of the tangent control point. This allows more control over the
   * exact shape of the curve, without permitting discontinuities.
   */
  UPROPERTY(meta=(ClampMin="0",ClampMax="99"),
	    EditAnywhere, BlueprintReadWrite) float virtual_proportion;
  FVector2D get_virtual() const { return anchor - (control - anchor); }
};
