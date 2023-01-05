// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SKGMeshComponentFunctionLibrary.generated.h"


class UMeshComponent;
class APlayerController;

UCLASS()
class SKGMESHCOMPONENT_API USKGMeshComponentFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool CalculateAdjustedMatrix(const UMeshComponent* Component, APlayerController* PlayerController, float DesiredFOV, FMatrix& OutMatrix);
};
