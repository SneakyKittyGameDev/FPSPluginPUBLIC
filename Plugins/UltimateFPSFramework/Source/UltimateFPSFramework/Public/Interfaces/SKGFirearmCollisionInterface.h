// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "SKGFirearmCollisionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGFirearmCollisionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ULTIMATEFPSFRAMEWORK_API ISKGFirearmCollisionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmCollisionInterface")
	FName GetFirearmGripSocket() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmCollisionInterface")
	FSKGCollisionSettings GetCollisionSettings();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmCollisionInterface")
	FTransform GetCollisionShortStockPose();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmCollisionInterface")
	FTransform GetCollisionPose();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmCollisionInterface")
	FTransform GetCollisionMuzzleSocketTransform();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmCollisionInterface")
	TArray<AActor*> GetPartsToIgnore();
};
