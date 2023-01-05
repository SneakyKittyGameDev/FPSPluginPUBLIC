
#pragma once

#include "SKGSceneCaptureDataTypes.generated.h"

UENUM(BlueprintType)
enum class ESKGMeasurementType : uint8
{
	Metric		UMETA(DisplayName = "Metric"),
	Imperial	UMETA(DisplayName = "Imperial")
};

UENUM(BlueprintType)
enum class ESKGRotationDirection : uint8
{
	ClockWise			UMETA(DisplayName = "ClockWise"),
	CounterClockWise	UMETA(DisplayName = "CounterClockWise")
};

UENUM(BlueprintType)
enum class ESKGElevationWindage : uint8
{
	Elevation	UMETA(DisplayName = "Elevation"),
	Windage		UMETA(DisplayName = "Windage"),
	Both		UMETA(DisplayName = "Both")
};

UENUM(BlueprintType)
enum class ESKGScopeAdjustment : uint8
{
	MRAD	UMETA(DisplayName = "MRAD"),
	MOA		UMETA(DisplayName = "MOA")
};

class UMaterialInterface;

USTRUCT(BlueprintType)
struct FSKGSightZoomSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | ZoomSettings")
	bool bSmoothZoom = false;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | ZoomSettings", meta = (EditCondition = "bSmoothZoom", EditConditionHides))
	float SmoothZoomSmoothness = 0.02f;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | ZoomSettings", meta = (EditCondition = "bSmoothZoom", EditConditionHides))
	float SmoothZoomSpeed = 25.0f;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | ZoomSettings")
	bool bFreeZoom = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | ZoomSettings", meta = (EditCondition = "bFreeZoom", EditConditionHides))
	float ZoomIncrementAmount = 1.0f;
};

USTRUCT(BlueprintType)
struct FSKGSightMagnification
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | MagnificationSettings")
	TArray<float> Magnifications = { 1.0f };
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | MagnificationSettings")
	bool bIsFirstFocalPlane = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | MagnificationSettings")
	float EyeboxShrinkOnZoomAmount = 30.0f;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | MagnificationSettings")
	float DecreaseReticleScaleAmount = 8.0f;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | MagnificationSettings")
	FSKGSightZoomSettings ZoomSettings;

	uint8 MagnificationIndex = 0;
	float CurrentMagnification = 1.0f;
};

USTRUCT(BlueprintType)
struct FSKGSceneCaptureOptimization
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization")
	bool bOverrideCaptureEveryFrame = false;
	// The refresh rate of the Scene Capture Component (60 = 60 times per second)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization", meta = (EditCondition = "!bOverrideCaptureEveryFrame", EditConditionHides))
	float RefreshRate = 60.0f;
};

USTRUCT(BlueprintType)
struct FSKGSightOptimization
{
	GENERATED_BODY()
	// Use this instead of setting Capture Every Frame
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization")
	bool bOverrideCaptureEveryFrame = false;
	// The refresh rate of the optic (60 = 60 times per second)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization", meta = (EditCondition = "!bOverrideCaptureEveryFrame", EditConditionHides))
	float RefreshRate = 60.0f;
	// Disable the scene capture component when not aiming down sights
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization", meta = (EditCondition = "!bOverrideCaptureEveryFrame", EditConditionHides))
	bool bDisableWhenNotAiming = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization", meta = (EditCondition = "bDisableWhenNotAiming && !bOverrideCaptureEveryFrame", EditConditionHides))
	float StopCaptureDelay = 0.0f;
	// Continue running scene capture component at a set refresh rate (5 = 5 times per second)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization", meta = (EditCondition = "!bDisableWhenNotAiming && !bOverrideCaptureEveryFrame", EditConditionHides))
	float NotAimingRefreshRate = 5.0f;
	// When not aiming clear the scope with a color
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization", meta = (EditCondition = "bDisableWhenNotAiming", EditConditionHides))
	bool bClearScopeWithColor = true;
	// Color to clear the scope with
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization", meta = (EditCondition = "bDisableWhenNotAiming && bClearScopeWithColor", EditConditionHides))
	FLinearColor ClearedColor = FLinearColor::Black;
	// When not aiming clear the scope with a material
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization", meta = (EditCondition = "bDisableWhenNotAiming && !bClearScopeWithColor", EditConditionHides))
	bool bClearScopeWithMaterial = false;
	// Material to clear the scope with
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Optimization", meta = (EditCondition = "bDisableWhenNotAiming && !bClearScopeWithColor && bClearScopeWithMaterial", EditConditionHides))
	UMaterialInterface* ClearedScopeMaterial = nullptr;
	
	TWeakObjectPtr<UMaterialInterface> OriginalScopeMaterial = nullptr;
};

USTRUCT(BlueprintType)
struct FSKGRenderTargetSize
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	int32 Width = 512;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	int32 Height = 512;
};

USTRUCT(BlueprintType)
struct FSKGSightZero
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float Elevation = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float Windage = 0.0f;
};

USTRUCT(BlueprintType)
struct FSKGReticleBrightness
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | ReticleBrightness")
	TArray<float> ReticleBrightnessSettings = { 1.0f };
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | ReticleBrightness")
	int32 ReticleDefaultBrightnessIndex = 0;
};

class UMaterialInstance;
class UMaterialInstanceDynamic;

USTRUCT(BlueprintType)
struct FSKGRenderTargetMaterial
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | ReticleMaterial")
	UMaterialInstance* Material = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | ReticleMaterial")
	UMaterialInstanceDynamic* DynamicMaterial = nullptr;
};

USTRUCT(BlueprintType)
struct FSKGReticleMaterial
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | ReticleMaterial")
	FSKGRenderTargetMaterial RenderTargetMaterial;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | ReticleMaterial")
	float ReticleSize = 1.0f;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | ReticleMaterial")
	float StartingEyeboxRange = -2000.0f;
};

USTRUCT(BlueprintType)
struct FSKGReticleSettings
{
	GENERATED_BODY()
	// Set this to the material index (element) that the reticle will be on
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | ReticleSettings")
	int32 ReticleMaterialIndex = 1;
	// Ignore for now, this is for a future feature
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | ReticleSettings")
	float Radius = 1.0f;
	// Reticle materials you wish to use and cycle through (such as red and green dots)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | ReticleSettings")
	TArray<FSKGReticleMaterial> ReticleMaterials;
	// Reticle Brightness settings to make your reticle brighter/darker
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | ReticleSettings")
	FSKGReticleBrightness ReticleBrightness;
};

USTRUCT(BlueprintType)
struct FSKGManualSceneCaptureSetup
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | SceneCapture")
	int32 MaterialIndex = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | SceneCapture")
	TArray<FSKGRenderTargetMaterial> RenderTargetMaterials;
};