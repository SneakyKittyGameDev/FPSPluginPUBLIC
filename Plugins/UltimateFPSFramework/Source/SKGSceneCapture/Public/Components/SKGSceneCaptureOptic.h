// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "SKGSceneCaptureDataTypes.h"
#include "SKGSceneCaptureOptic.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnElevationChanged, float, OldAdjustment, float, NewAdjustment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWindageChanged, float, OldAdjustment, float, NewAdjustment);

class APawn;
class UMeshComponent;

UCLASS(hidecategories=(Collision, Object, Physics, SceneComponent), ClassGroup=Rendering, editinlinenew, meta=(BlueprintSpawnableComponent))
class SKGSCENECAPTURE_API USKGSceneCaptureOptic : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:
	USKGSceneCaptureOptic(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	FSKGSightOptimization Optimization;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	FSKGSightMagnification MagnificationSettings;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	FSKGRenderTargetSize RenderTargetSize;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Zero")
	ESKGScopeAdjustment TurretAdjustmentType;
	// Material parameter name for the render target
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Setup")
	FName RenderTargetMaterialVarName;

	TWeakObjectPtr<UMeshComponent> OwningMesh;
	uint8 ReticleIndex;
	uint8 ReticleBrightnessIndex;
	FSKGSightZero SightZero;
	UPROPERTY()
	FSKGReticleSettings ReticleSettings;

	FTimerHandle TZoomHandle;
	float SmoothMagnification;
	float PreviousMagnification;
	float MagnificationToInterpTo;
	int32 ZoomInAmount;
	bool bZoomingIn;

	FTimerHandle TDelayedStopCaptureHandle;

	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Zero")
	float MilliradianAdjustment;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Zero")
	float MOAAdjustment;

	const float MilliradianClick = 0.1f;
	const float MOAClick = 0.25f;

	UPROPERTY(BlueprintAssignable, Category = "SKGFPSFramework | Zero")
	FOnElevationChanged OnElevationChanged;
	UPROPERTY(BlueprintAssignable, Category = "SKGFPSFramework | Zero")
	FOnWindageChanged OnWindageChanged;

	TWeakObjectPtr<UTextureRenderTarget2D> RenderTarget;

	FRotator StartingSceneCaptureRot;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetupRenderTarget();

	void Zoom();
	
	void HandleZoom(float Magnification);
	void HandleReticleSmoothZoom();
	float GetMaxMagnification();
	float GetMinMagnification();

	void DelayStopCapture();
	
public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	void StartCapture();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	void StopCapture(bool bForce = false);
	
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	void SetOwningMesh(UMeshComponent* Mesh);

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	void ZoomIn();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	void ZoomOut();

	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | SceneComponent")
	float GetMagnification() const;
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | SceneComponent")
	float GetMagnificationSensitivity() const;

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Zero")
	void ReturnToZeroElevation();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Zero")
	void ReturnToZeroWindage();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Zero")
	void SetNewZeroElevation();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Zero")
	void SetNewZeroWindage();
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | SceneComponent")
	FSKGSightZero GetSightZero() const { return SightZero; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	FVector2D PointOfImpactUp(bool bUp, uint8 Clicks = 1);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	FVector2D PointOfImpactLeft(bool bLeft, uint8 Clicks = 1);

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Reticle")
	virtual void CycleReticle();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Reticle")
	virtual void SetReticle(uint8 Index);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Reticle")
	void SetReticleBrightness(uint8 Index);

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	void SetReticleSettings(const FSKGReticleSettings& INReticleSettings);

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	void SetupManually(UPARAM(ref) FSKGManualSceneCaptureSetup& ManualSettings);
};
