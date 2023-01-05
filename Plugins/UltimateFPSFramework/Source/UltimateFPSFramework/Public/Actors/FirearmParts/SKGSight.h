// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/SKGSceneCaptureOptic.h"
#include "Actors/FirearmParts/SKGPart.h"
#include "Interfaces/SKGAimInterface.h"
#include "Interfaces/SKGProceduralAnimationInterface.h"
#include "Interfaces/SKGSightInterface.h"
#include "Interfaces/SKGRenderTargetInterface.h"
#include "SKGSight.generated.h"

class UMaterialInstance;
class ASKGMagnifier;
class USKGSceneCaptureOptic;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGSight : public ASKGPart, public ISKGProceduralAnimationInterface,
	public ISKGAimInterface, public ISKGRenderTargetInterface, public ISKGSightInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGSight();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	FSKGReticleSettings ReticleSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FSKGAimCameraSettings CameraSettings;
	// Used to blend between different state machines in the anim graph using an integer
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Animation")
	int32 AnimationIndex;
	// Used to blend between different state machines in the anim graph using gameplay tags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Animation")
	FGameplayTag AnimationGameplayTag;
	// How much faster to ADS with this optic
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Animation")
	float AimInterpolationMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Animation")
	float RotationLagInterpolationMultiplier;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Animation")
	FName GripSocket;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Animation")
	FRotator HeadAimRotation;
	// Whether or not this optic can have its zero adjusted (point of impact shifts)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Zero")
	bool bCanBeZeroed;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Zero", meta = (EditCondition = "bCanBeZeroed", EditConditionHides))
	bool bStartWithRandomZero;
	// What is the max amount of clicks for the random elevation zero
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Zero", meta = (EditCondition = "bCanBeZeroed", EditConditionHides))
	uint8 RandomMaxElevationStartClicks;
	// What is the max amount of clicks for the random windage zero
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Zero", meta = (EditCondition = "bCanBeZeroed", EditConditionHides))
	uint8 RandomMaxWindageStartClicks;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Aim")
	bool bIsAimable;
	// Socket that is used for aiming such as S_Aim that is on optics and the lightlaser mesh
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Aim")
	FName AimSocket;
	
	uint8 ReticleIndex;
	uint8 ReticleBrightnessIndex;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Zero")
	float MilliradianAdjustment;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Zero")
	float MOAAdjustment;

	bool bHasRenderTarget;

	const float MilliradianClick = 0.1f;
	const float MOAClick = 0.25f;
	float RedDotZeroAmount;
	float CurrentDotElevation, CurrentDotWindage, StartingDotElevation, StartingDotWindage;
	FSKGSightZero DotZero;
	FName ReticleZeroName = FName("ReticleZero");

	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	ASKGMagnifier* Magnifier;

	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	UMaterialParameterCollection* MPC;
	TWeakObjectPtr<UMaterialParameterCollectionInstance> MPCInstance;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void SetRandomZero();
	// Leave as UPROPERTY due to garbage collection
	UPROPERTY()
	USKGSceneCaptureOptic* SceneCapture;

	virtual void PointOfImpactUp(bool bUp, uint8 Clicks = 1);
	virtual void PointOfImpactLeft(bool bLeft, uint8 Clicks = 1);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | Zero")
	void OnElevationChanged(float OldAdjustment, float NewAdjustment, int32 Clicks);
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | Zero")
	void OnWindageChanged(float OldAdjustment, float NewAdjustment, int32 Clicks);
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | Zero")
	void OnReturnToZeroElevation();
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | Zero")
	void OnReturnToZeroWindage();

	virtual float GetMagnification() const;
public:	
	// Aim Interface defaults
	virtual void SetMagnifier_Implementation(ASKGMagnifier* INMagnifier) override { Magnifier = INMagnifier; }
	virtual ASKGMagnifier* GetMagnifier_Implementation() override { return Magnifier; }
	virtual void GetCameraSettings_Implementation(FSKGAimCameraSettings& OutCameraSettings) override;
	virtual bool IsAimable_Implementation() override { return bIsAimable && (AttachmentMesh.IsValid() && AttachmentMesh.Get()->DoesSocketExist(AimSocket)); }
	virtual void EnableAiming_Implementation() override { bIsAimable = true; }
	virtual void DisableAiming_Implementation() override { bIsAimable = false; }
	// END OF AIM INTERFACE
	
	// PROCEDURAL ANIMATION INTERFACE
	virtual FTransform GetDefaultAimSocketTransform_Implementation() override { return Execute_GetAimSocketTransform(this); }
	virtual FTransform GetAimSocketTransform_Implementation() override;
	virtual int32 GetAnimationIndex_Implementation() const override { return AnimationIndex; }
	virtual FGameplayTag GetAnimationGameplayTag_Implementation() const override { return AnimationGameplayTag; }
	virtual float GetAimInterpolationMultiplier_Implementation() override { return AimInterpolationMultiplier; }
	virtual float GetRotationLagInterpolationMultiplier_Implementation() override { return RotationLagInterpolationMultiplier; }
	virtual FRotator GetHeadRotation_Implementation() const override { return HeadAimRotation; }
	virtual FName GetGripSocketName_Implementation() const override { return GripSocket; }
	// END OF PROCEDUAL ANIMATION INTERFACE

	// START OF RENDER TARGET INTERFACE
	virtual bool HasRenderTarget_Implementation() const override { return bHasRenderTarget; }
	virtual void DisableRenderTarget_Implementation(bool Disable) override;
	// END OF RENDER TARGET INTERFACE
	
	// START OF SIGHT INTERFACE
	virtual void ZoomOptic_Implementation(bool bZoom) override;
	virtual float GetCurrentMagnification_Implementation() const override { return GetMagnification(); }
	
	virtual void CycleReticle_Implementation() override;
	virtual void SetReticle_Implementation(uint8 Index) override;
	virtual void IncreaseBrightness_Implementation() override;
	virtual void DecreaseBrightness_Implementation() override;
	virtual void SetReticleBrightness_Implementation(uint8 Index) override;
	virtual void ReturnToZeroElevation_Implementation() override;
	virtual void ReturnToZeroWindage_Implementation() override;
	virtual void SetNewZeroElevation_Implementation() override;
	virtual void SetNewZeroWindage_Implementation() override;
	virtual void MovePointOfImpact_Implementation(ESKGElevationWindage Turret, ESKGRotationDirection Direction, uint8 Clicks = 1) override;
	virtual FSKGSightZero GetSightZero_Implementation() const override;
	// END OF SIGHT INTERFACE
};
