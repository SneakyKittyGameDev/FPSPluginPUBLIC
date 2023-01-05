// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "SKGFirearmInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGFirearmInterface : public UInterface
{
	GENERATED_BODY()
};

class USKGCharacterComponent;

class ULTIMATEFPSFRAMEWORK_API ISKGFirearmInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	FSKGFirearmStats GetFirearmStats();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	FSKGRecoilData GetRecoilData();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void SetCharacterComponent(USKGCharacterComponent* INCharacterComponent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void SetupStabilizerComponent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void Equip();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void UnEquip();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void Fire();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void StopFire();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void Reload();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void CycleFireMode(bool bReverse = false);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void CycleSights(bool bDownArray = true, bool bStopAtEndOfArray = false);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void CyclePointAim(bool bDownArray = true, bool bStopAtEndOfArray = false);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmInterface")
	void SetPointAimIndex(int32 Index);
};
