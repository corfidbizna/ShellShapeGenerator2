// Fill out your copyright notice in the Description page of Project Settings.

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
  UPROPERTY() FVector2D anchor;
  /**
   * The control point. This is a point toward which the curve bends.
   */
  UPROPERTY() FVector2D control;
  /**
   * The magnitude of the anti-tangent control point, as a proportion of the
   * magnitude of the tangent control point. This allows more control over the
   * exact shape of the curve, without permitting discontinuities.
   */
  UPROPERTY(meta=(ClampMin="0",ClampMax="99")) float virtual_proportion;
};
