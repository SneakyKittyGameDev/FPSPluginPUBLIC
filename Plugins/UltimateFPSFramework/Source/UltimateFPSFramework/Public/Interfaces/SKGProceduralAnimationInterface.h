// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "SKGProceduralAnimationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGProceduralAnimationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ULTIMATEFPSFRAMEWORK_API ISKGProceduralAnimationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetBasePoseOffset();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	UAnimSequence* GetGripAnimation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	bool UseLeftHandIK();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetHighPortPose();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetLowPortPose();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetShortStockPose();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetOppositeShoulderPose();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetBlindFireLeftPose();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetBlindFireUpPose();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FSKGFirearmPoseCurveSettings GetHighPortCurveSettings();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FSKGFirearmPoseCurveSettings GetLowPortCurveSettings();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FSKGFirearmPoseCurveSettings GetShortStockCurveSettings();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FSKGFirearmPoseCurveSettings GetOppositeShoulderCurveSettings();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FSKGFirearmPoseCurveSettings GetBlindFireLeftCurveSettings();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FSKGFirearmPoseCurveSettings GetBlindFireUpCurveSettings();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetLeftHandIKTransform();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	float GetHighLowPortPoseInterpolationSpeed();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	bool GetPerformShakeAfterPortPose();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	UCurveVector* GetShakeCurve();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	float GetShakeCurveSpeed();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	float GetShakeCurveDuration();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	UCurveVector* GetMovementSwayCurve();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	bool UseStatsForMovementSway();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	float GetSwayMultiplier();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetSprintPose();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetSuperSprintPose();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetDefaultAimSocketTransform();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FTransform GetAimSocketTransform();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	void GetCameraSettings(FSKGAimCameraSettings& OutCameraSettings);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	int32 GetAnimationIndex() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FGameplayTag GetAnimationGameplayTag() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	float GetAimInterpolationMultiplier();
	virtual float GetAimInterpolationMultiplier_Implementation() { return 1.0f; }
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	float GetUnAimInterpolationMultiplier();
	virtual float GetUnAimInterpolationMultiplier_Implementation() { return 1.0f; }
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	float GetRotationLagInterpolationMultiplier();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	float GetLeaningSpeedMultiplier();
	virtual float GetLeaningSpeedMultiplier_Implementation() { return 1.0f; }
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FSKGLeanCurves GetLeanCurves();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FRotator GetHeadRotation() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	bool GetUseBasePoseCorrection();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | ProceduralAnimInterface")
	FSKGSwayMultipliers GetSwayMultipliers();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | AimInterface")
	FName GetGripSocketName() const;
};
