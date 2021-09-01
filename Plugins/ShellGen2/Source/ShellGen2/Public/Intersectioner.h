// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BakedMesh.h"
#include <thread>
#include <mutex>
#include "Intersectioner.generated.h"

struct bg_intersecting_state {
  std::unique_ptr<FBakedMesh> mesh;
  TArray<FVector2D> last_result;
  unsigned long generation, finished_generation;
  float plane_z;
  bool quitting;
  bg_intersecting_state() : generation(0), finished_generation(0),
                            quitting(false) {}
};

/**
 * An object that encapsulates a background thread for calculating
 * self-intersections on a Z plane.
 */
UCLASS(BlueprintType, Category = "Shell Shape Generator")
class SHELLGEN2_API UIntersectioner : public UObject {
  GENERATED_BODY()
  std::unique_ptr<std::thread> thread;
  bg_intersecting_state bg;
  std::mutex mutex;
  std::condition_variable new_to_process;
  UIntersectioner() {}
  virtual ~UIntersectioner();
  /**
   * Calculate every self-intersection on a given Z plane for a given baked
   * mesh.
   */
  UFUNCTION(BlueprintCallable, Category="Shell Shape Generator")
  static void GetSelfIntersectionsOnPlane(const FBakedMesh& mesh,
                                          TArray<FVector2D>& intersections, 
                                          float plane_z = 0.0f);
  /**
   * Start a background calculation of every self-intersection on a given Z
   * plane for a given baked mesh.
   */
  UFUNCTION(BlueprintCallable, Category="Shell Shape Generator")
  void StartBGGetSelfIntersectionsOnPlane(const FBakedMesh& mesh,
                                          float plane_z = 0.0f);
  /**
   * Returns true if a background self-intersection calculation is currently
   * in progress, false if it has finished (or never started).
   */
  UFUNCTION(BlueprintCallable, Category="Shell Shape Generator")
  bool IsBGSelfIntersectionInProgress();
  /**
   * Get the result of the most recent background self-intersection calculation
   * that completed.
   */
  UFUNCTION(BlueprintCallable, Category="Shell Shape Generator")
  void GetBGSelfIntersectionResult(TArray<FVector2D>& intersections);
  /**
   * Get a time stamp for benchmarking purposes.
   */
  UFUNCTION(BlueprintCallable)
  static float GetPlatformTimeSeconds();
  void bg_intersector();
};
