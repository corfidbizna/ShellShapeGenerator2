#pragma once

#include <thread>

#include "CoreMinimal.h"
#include "BakedMesh.h"
#include "CurveNode.h"
#include "ShellGenerator.generated.h"

// hey, Ma! come see all the internal state that got leaked into my public API
// because C++ is so goshdang primitive!

enum class CurveType { Circle, Flat };

class SHELLGEN2_API Curve {
  CurveType type;
public:
  TArray<FCurveNode> curve;
  Curve(CurveType type) : type(type) {}
  ~Curve() {}
  CurveType get_type() const { return type; }
  std::vector<FVector2D> evaluate(int max_depth) const;
  std::vector<FVector2D> evaluate_dynamic() const { return evaluate(-1); }
};

struct SHELLGEN2_API shell_params {
  float starting_normal_rad;
  float starting_binormal_rad;
  float starting_spiral_rad;
  float normal_growth_young;
  float binormal_growth_young;
  float spiral_growth_young;
  float young_end;
  float old_start;
  float normal_growth_old;
  float binormal_growth_old;
  float spiral_growth_old;
  float old_end;
  float aperture_start;
  float normal_growth_aperture;
  float binormal_growth_aperture;
  float spiral_growth_aperture;
  float current_age;
  float final_age;
  float length_per_iteration;
  int curve_subdivision;
  Curve young_cross = Curve(CurveType::Circle);
  Curve young_grain = Curve(CurveType::Flat);
  Curve old_cross = Curve(CurveType::Circle);
  Curve old_grain = Curve(CurveType::Flat);
  Curve aperture_cross = Curve(CurveType::Circle);
  Curve aperture_grain = Curve(CurveType::Flat);
  TArray<FVector2D> young_endcaps;
  TArray<FVector2D> old_endcaps;
  float get_tube_normal_radius(float theta) const;
  float get_tube_binormal_radius(float theta) const;
  float get_spiral_radius(float theta) const;
  float get_tube_center_d(float theta) const;
  void point_at(FBakedMesh& out, float theta) const;
  void build_shell_at(FBakedMesh& out,
		      const std::vector<FVector>& young_cross,
		      const std::vector<FVector>& old_cross,
		      const std::vector<FVector>& aperture_cross,
		      std::vector<FVector>& temp,
		      float theta, float scale) const;
  void build_shell(FBakedMesh& out) const;
};

struct SHELLGEN2_API bg_gen_state {
  std::mutex mutex;
  std::condition_variable new_to_process, all_done;
  FBakedMesh last_baked_mesh;
  shell_params desired_params, cur_params;
  bool params_available = false;
  bool processing = false, quitting = false;
  unsigned long generation = 0, finished_generation = 0;
  bool first_segment_attached, last_segment_is_full;
  void bg_thread_func();
  void attach_shell_segment(FBakedMesh& out,
			    bool is_full, unsigned int num_points);
  void build_shell_at(FBakedMesh& mesh,
		      const std::vector<FVector>& young_cross,
		      const std::vector<FVector>& old_cross,
		      const std::vector<FVector>& aperture_cross,
		      std::vector<FVector>& temp_cross,
		      float target_theta, float radius = 1.f);
};

UCLASS(BlueprintType, Category = "Shell Shape Generator")
class SHELLGEN2_API UShellGenerator : public UObject {
  GENERATED_UCLASS_BODY()
  std::unique_ptr<std::thread> thread;
  bg_gen_state bg;
  UShellGenerator() {}
  virtual ~UShellGenerator();
  /**
   * Make a new Shell Generator.
   */
  UFUNCTION(BlueprintCallable, Category = "Shell Shape Generator")
  static UShellGenerator* MakeShellGenerator();
  /**
   * Begin generating a shell with the given parameters. Any in-progress shell
   * generation process will be abandoned, though the last finished shell will
   * continue to be accessible.
   */
  UFUNCTION(BlueprintCallable, Category = "Shell Shape Generator")
  void BeginGeneratingShell
    (UPARAM(DisplayName="Initial tube height")
     float starting_normal_rad,
     UPARAM(DisplayName="Initial tube thickness")
     float starting_binormal_rad,
     UPARAM(DisplayName="Initial umbilical radius")
     float starting_spiral_rad,
     UPARAM(DisplayName="Young cross section curve")
     const TArray<FCurveNode>& young_cross,
     UPARAM(DisplayName="Young grain curve")
     const TArray<FCurveNode>& young_grain,
     UPARAM(DisplayName="Young whorl height expansion rate per 180°")
     float normal_growth_young,
     UPARAM(DisplayName="Young whorl thickness expansion rate per 180°")
     float binormal_growth_young,
     UPARAM(DisplayName="Young umbilical radius per 180°")
     float spiral_growth_young,
     UPARAM(DisplayName="Pure young end (in 180° units)")
     float young_end,
     UPARAM(DisplayName="Pure old start (in 180° units)")
     float old_start,
     UPARAM(DisplayName="Old cross section curve")
     const TArray<FCurveNode>& old_cross,
     UPARAM(DisplayName="Old grain curve")
     const TArray<FCurveNode>& old_grain,
     UPARAM(DisplayName="Old whorl height expansion rate per 180°")
     float normal_growth_old,
     UPARAM(DisplayName="Old whorl thickness expansion rate per 180°")
     float binormal_growth_old,
     UPARAM(DisplayName="Old umbilical radius per 180°")
     float spiral_growth_old,
     UPARAM(DisplayName="Pure old end")
     float old_end,
     UPARAM(DisplayName="Pure aperture start")
     float aperture_start,
     UPARAM(DisplayName="Aperture cross section curve")
     const TArray<FCurveNode>& aperture_cross,
     UPARAM(DisplayName="Aperture grain curve")
     const TArray<FCurveNode>& aperture_grain,
     UPARAM(DisplayName="Aperture whorl height expansion rate per 180°")
     float normal_growth_aperture,
     UPARAM(DisplayName="Aperture whorl thickness expansion rate per 180°")
     float binormal_growth_aperture,
     UPARAM(DisplayName="Aperture umbilical radius per 180°")
     float spiral_growth_aperture,
     UPARAM(DisplayName="Current age (fraction of final)")
     float current_age,
     UPARAM(DisplayName="Final age (in 180° units)")
     float final_age,
     UPARAM(DisplayName="Endcap spec (young end)")
     TArray<FVector2D> young_endcaps,
     UPARAM(DisplayName="Endcap spec (old end)")
     TArray<FVector2D> old_endcaps,
     UPARAM(DisplayName="Distance per iteration")
     float length_per_iteration,
     UPARAM(DisplayName="Curve subdivision iterations")
     int curve_subdivision = 4
     );
  /**
   * Returns true if a shell is currently being generated in the background,
   * false if generation has completed (or there never was a shell being
   * generated in the first place).
   */
  UFUNCTION(BlueprintCallable, Category = "Shell Shape Generator")
  bool IsGenerationStillInProgress();
  /**
   * Get the most recent generated shell.
   */
  UFUNCTION(BlueprintCallable, Category = "Shell Shape Generator")
  FBakedMesh TakeLastGeneratedShell();
  /**
   * Get the latest generated shell, waiting (if necessary) for the current
   * in-progress generation process to complete.
   */
  UFUNCTION(BlueprintCallable, Category = "Shell Shape Generator")
  FBakedMesh BlockForGeneratedShell();
};
