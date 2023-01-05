// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "GameplayTagContainer.h"
#include "SKGAimInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGAimInterface : public UInterface
{
	GENERATED_BODY()
};


class ASKGSight;
class ASKGMagnifier;

class ULTIMATEFPSFRAMEWORK_API ISKGAimInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | AimInterface")
	void SetMagnifier(ASKGMagnifier* INMagnifier);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | AimInterface")
	ASKGMagnifier* GetMagnifier();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | AimInterface")
	bool IsPointAiming();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | AimInterface")
	bool IsAimable();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | AimInterface")
	void DisableAiming();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | AimInterface")
	void EnableAiming();
};
