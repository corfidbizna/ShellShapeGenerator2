// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BakedMesh.h"
#include "ObjWriter.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Category = "Shell Shape Generator")
class SHELLGEN2_API UObjWriter : public UBlueprintFunctionLibrary {
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, Category="Shell Shape Generator",
	    DisplayName="Output OBJ File")
  static void OutputObjFile(const TArray<FString>& comments,
                            const TArray<FBakedMesh>& meshes,
                            const TArray<FTransform>& transforms,
                            const FString& filename,
                            bool& saving_succeeded,
                            FString& failure_reason);
};
