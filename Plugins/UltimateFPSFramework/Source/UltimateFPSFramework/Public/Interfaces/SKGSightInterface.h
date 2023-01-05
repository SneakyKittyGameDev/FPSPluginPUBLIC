// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SKGSceneCaptureDataTypes.h"
#include "SKGSightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGSightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ULTIMATEFPSFRAMEWORK_API ISKGSightInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	float GetCurrentMagnification() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void ZoomOptic(bool bZoom);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void CycleReticle();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void SetReticle(uint8 Index);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void IncreaseBrightness();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void DecreaseBrightness();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void SetReticleBrightness(uint8 Index);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void ReturnToZeroElevation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void ReturnToZeroWindage();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void SetNewZeroElevation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void SetNewZeroWindage();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	void MovePointOfImpact(ESKGElevationWindage Turret, ESKGRotationDirection Direction, uint8 Clicks = 1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | SightInterface")
	FSKGSightZero GetSightZero() const;
};
