//Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "SKGHoleComponentDataTypes.generated.h"

class UMaterialInstanceDynamic;

USTRUCT(BlueprintType)
struct FSKGHoleMaterial
{
	GENERATED_BODY()
	TWeakObjectPtr<UMaterialInstanceDynamic> MaterialInstance = nullptr;
	TArray<FVector> HoleLocations;
	uint8 HoleIndex = 0;
	uint8 MaxHoleCount = 8;
};

USTRUCT(BlueprintType)
struct FSKGHoleMaterialSetting
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	int32 MaxHoleCount = 8;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	int32 MaterialIndex = 0;
};