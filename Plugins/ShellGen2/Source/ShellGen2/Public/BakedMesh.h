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
#include <vector>
#include <memory>
#include "StaticMeshAttributes.h"
#include "BakedMesh.generated.h"

USTRUCT(BlueprintType, Category = "Shell Shape Generator")
struct SHELLGEN2_API FBakedMesh {
  GENERATED_BODY()
  std::shared_ptr<std::vector<FVector> > vertices;
  std::shared_ptr<std::vector<FVector2D> > texcoords;
  std::shared_ptr<std::vector<uint32_t> > indices;
  std::vector<FVector> calculate_normals() const;
  void build_tangent_space(const TArray<FVertexInstanceID>& viid_map,
                           TMeshAttributesRef<FVertexInstanceID, FVector>&
                           normals,
                           TMeshAttributesRef<FVertexInstanceID, FVector>&
                           tangents,
                           TMeshAttributesRef<FVertexInstanceID, float>&
                           binormal_signs) const;
  FBakedMesh() {}
  FBakedMesh(std::shared_ptr<std::vector<FVector> > vertices, std::shared_ptr<std::vector<FVector2D> > texcoords, std::shared_ptr<std::vector<uint32_t> > indices)
  : vertices(std::move(vertices)), texcoords(std::move(texcoords)), indices(std::move(indices)) {}
};
