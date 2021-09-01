#pragma once

#include "CoreMinimal.h"
#include "ComposeOperation.generated.h"

/**
 * An operation to use when composing two Distortions.
 */
UENUM(Category = "Shell Shape Generator")
enum class ComposeOperation : uint8 {
  ComposeAdd UMETA(DisplayName = "Add"),
  ComposeMultiply UMETA(DisplayName = "Multiply"),
  ComposeDivide UMETA(DisplayName = "Divide"),
};
