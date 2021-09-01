#include "cook_path.h"

FString shellgen_cook_path(const FString& filename) {
  return FPaths::ProjectContentDir() + TEXT("/") + filename;
}

