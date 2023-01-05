// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SKGMuzzleInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGMuzzleInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ULTIMATEFPSFRAMEWORK_API ISKGMuzzleInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | MuzzleInterface")
	UNiagaraSystem* GetFireNiagaraSystem();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | MuzzleInterface")
	FTransform GetMuzzleSocketTransform();
};
