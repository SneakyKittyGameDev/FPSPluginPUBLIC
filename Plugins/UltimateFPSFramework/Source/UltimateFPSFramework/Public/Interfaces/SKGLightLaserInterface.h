// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataTypes/SKGLightLaserDataTypes.h"
#include "SKGLightLaserInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGLightLaserInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ULTIMATEFPSFRAMEWORK_API ISKGLightLaserInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | LightLaserInterface")
	void SetLightLaserState(const ESKGLightLaserState State);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | LightLaserInterface")
	void RestorePreviousBeforeOffState();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | LightLaserInterface")
	void ToggleLight();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | LightLaserInterface")
	void ToggleLaser();
	/* If bDownArray is true it will go 0,1,2,etc, if it is false it will go 0,2,1,etc. If ForceIndex is within the count of LightLaserModeCycle's it will use that index*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | LightLaserInterface")
	void CycleLightLaserModes(const bool bDownArray = true, const uint8 ForceIndex = 254);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | LightLaserInterface")
	ESKGLightLaserState GetLightLaserState() const;
	/* If bDownArray is true it will go 0,1,2,etc, if it is false it will go 0,2,1,etc.*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | LightLaserInterface")
	void CycleLaserColor(bool bDownArray = true);
	/* If bDownArray is true it will go 0,1,2,etc, if it is false it will go 0,2,1,etc.*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | LightLaserInterface")
	void CyclePowerModes(bool bDownArray = true);
};
