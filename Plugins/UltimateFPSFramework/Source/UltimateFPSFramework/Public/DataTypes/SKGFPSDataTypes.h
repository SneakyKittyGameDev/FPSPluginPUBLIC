//Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "Templates/SubclassOf.h"
#include "Engine/NetSerialization.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Particles/WorldPSCPool.h"
#include "Particles/ParticleSystem.h"
#include "Curves/CurveVector.h"
#include "Sound/SoundBase.h"
#include "Camera/CameraShakeBase.h"
#include "Components/SKGAttachmentComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "SKGFPSDataTypes.generated.h"

#define SKG_DEPRECATED(Message) [[deprecated("Will be removed in future versions. " Message)]]

UENUM(BlueprintType)
enum class ESKGMagazineState : uint8
{
	InUse		UMETA(DisplayName = "InUse"),
	NotInUse	UMETA(DisplayName = "NotInUse"),
	OnGround	UMETA(DisplayName = "OnGround")
};

UENUM(BlueprintType)
enum class ESKGFirearmFireMode : uint8
{
	Safe			UMETA(DisplayName = "Safe"),
	Semi			UMETA(DisplayName = "Semi"),
	Burst			UMETA(DisplayName = "Burst"),
	FullAuto		UMETA(DisplayName = "FullAuto"),
	Manual			UMETA(DisplayName = "Manual")
};

UENUM(BlueprintType)
enum class ESKGLeaning : uint8
{
	None	UMETA(DisplayName = "None"),
	Left	UMETA(DisplayName = "Left"),
	Right	UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class ESKGFirearmPose : uint8
{
	None				UMETA(DisplayName = "None"),
	High				UMETA(DisplayName = "High"),
	Low					UMETA(DisplayName = "Low"),
	ShortStock			UMETA(DisplayName = "ShortStock"),
	OppositeShoulder	UMETA(DisplayName = "OppositeShoulder"),
	BlindFireLeft		UMETA(DisplayName = "BlindFireLeft"),
	BlindFireUp			UMETA(DisplayName = "BlindFireUp")
};

UENUM(BlueprintType)
enum class ESKGPose : uint8
{
	ShortStockPose			UMETA(DisplayName = "ShortStockPose"),
	BasePoseOffset			UMETA(DisplayName = "BasePoseOffset"),
	SprintPose				UMETA(DisplayName = "SprintPose"),
	SuperSprintPose			UMETA(DisplayName = "SuperSprintPose"),
	HighPortPose			UMETA(DisplayName = "HighPortPose"),
	LowPortPose				UMETA(DisplayName = "LowPortPose"),
	OppositeShoulderPose	UMETA(DisplayName = "OppositeShoulderPose"),
	BlindFireLeftPose		UMETA(DisplayName = "BlindFireLeftPose"),
	BlindFireUpPose			UMETA(DisplayName = "BlindFireUpPose")
};

UENUM(BlueprintType)
enum class ESKGFirearmPoseType : uint8
{
	ToPose		UMETA(DisplayName = "ToPose"),
	ReturnPose	UMETA(DisplayName = "ReturnPose"),
	Both		UMETA(DisplayName = "Both")
};

UENUM(BlueprintType)
enum class ESKGPartType : uint8
{
	Sight			UMETA(DisplayName = "Sight"),
	Magnifier		UMETA(DisplayName = "Magnifier"),
	LightLaser		UMETA(DisplayName = "LightLaser"),
	Handguard		UMETA(DisplayName = "Handguard"),
	Barrel			UMETA(DisplayName = "Barrel"),
	MuzzleDevice	UMETA(DisplayName = "MuzzleDevice"),
	Stock			UMETA(DisplayName = "Stock"),
	PistolGrip		UMETA(DisplayName = "PistolGrip"),
	ForwardGrip		UMETA(DisplayName = "ForwardGrip"),
	Magazine		UMETA(DisplayName = "Magazine"),
	Bipod			UMETA(DisplayName = "Bipod"),
	Other			UMETA(DisplayName = "Other")
};

UENUM(BlueprintType)
enum class ESKGFirearmInHand : uint8
{
	None		UMETA(DisplayName = "None"),
	M4			UMETA(DisplayName = "M4"),
	Test		UMETA(DisplayName = "Test"),
};

UENUM(BlueprintType)
enum class ESKGSprintType : uint8
{
	None		UMETA(DisplayName = "None"),
	Sprint		UMETA(DisplayName = "Sprint"),
	SuperSprint	UMETA(DisplayName = "SuperSprint")
};

// Forward Declarations
class UMaterialInstance;
class UMaterialInstanceDynamic;
class UFXSystemAsset;
class USoundBase;
class UFPSTemplate_PartComponent;
class ASKGSight;
class ASKGPart;
class UTexture2D;
class ASKGFirearm;
class USKGAttachmentComponent;

USTRUCT(BlueprintType)
struct FSKGFirearmCachedParts
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	USKGAttachmentComponent* Barrel = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	USKGAttachmentComponent* Handguard = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	USKGAttachmentComponent* Stock = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	USKGAttachmentComponent* ForwardGrip = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	USKGAttachmentComponent* Muzzle = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	USKGAttachmentComponent* Magazine = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	USKGAttachmentComponent* Bipod = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	TArray<USKGAttachmentComponent*> Parts;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	TArray<USKGAttachmentComponent*> Sights;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	TArray<USKGAttachmentComponent*> LightLasers;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	TArray<USKGAttachmentComponent*> Magnifiers;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Attachments")
	TArray<USKGAttachmentComponent*> RenderTargets;

	void Empty()
	{
		Barrel = nullptr;
		Handguard = nullptr;
		Stock = nullptr;
		ForwardGrip = nullptr;
		Muzzle = nullptr;
		Magazine = nullptr;
		Bipod = nullptr;
		Parts.Empty();
		Sights.Empty();
		LightLasers.Empty();
		Magnifiers.Empty();
		RenderTargets.Empty();
	}

	void AddAttachment(USKGAttachmentComponent* AttachmentComponent, ESKGPartType PartType, bool bIsAimable, bool bHasRenderTarget)
	{
		if (AttachmentComponent)
		{
			if (bIsAimable && PartType != ESKGPartType::Sight)
			{
				Sights.Add(AttachmentComponent);
			}
			if (bHasRenderTarget)
			{
				RenderTargets.Add(AttachmentComponent);
			}
			switch (PartType)
			{
			case ESKGPartType::Sight : Sights.Add(AttachmentComponent); break;
			case ESKGPartType::LightLaser : LightLasers.Add(AttachmentComponent); break;
			case ESKGPartType::Magnifier : Magnifiers.Add(AttachmentComponent); break;
			case ESKGPartType::Barrel : Barrel = AttachmentComponent; break;
			case ESKGPartType::MuzzleDevice : Muzzle = AttachmentComponent; break;
			case ESKGPartType::ForwardGrip : ForwardGrip = AttachmentComponent; break;
			case ESKGPartType::Handguard : Handguard = AttachmentComponent; break;
			case ESKGPartType::Stock : Stock = AttachmentComponent; break;
			case ESKGPartType::Magazine : Magazine = AttachmentComponent; break;
			case ESKGPartType::Bipod : Bipod = AttachmentComponent; break;
			}
		}
	}
};

USTRUCT(BlueprintType)
struct FSKGAimCameraSettings
{
	GENERATED_BODY()
	// How much should the camera zoom when aiming with this optic
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float CameraFOVZoom;
	// How fast should the camera zoom when aiming with this optic
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float CameraFOVZoomSpeed;
	// How close/far should the camera be from this optic when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float CameraDistance;
	// This will lock the distance from optic to the camera when aiming. Useful for magnified optics with a simulated eyebox
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	bool bUsedFixedCameraDistance;
};

USTRUCT(BlueprintType)
struct FSKGFirearmPartStats
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float Weight = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float ErgonomicsChangePercentage = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float VerticalRecoilChangePercentage = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float HorizontalRecoilChangePercentage = 0.0f;
	// Higher = higher MOA/Worse Accuracy, Lower = lower MOA/Better Accuracy
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float AccuracyChangePercentage = 0.0f;
	// Higher = Faster Velocity, Lower = Lower Velocity
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float MuzzleVelocityChangePercentage = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float CycleRateIncreasePercentage = 0.0f;

	FSKGFirearmPartStats operator+ (const FSKGFirearmPartStats& Stats) const
	{
		FSKGFirearmPartStats NewStats;
		NewStats.Weight = this->Weight + Stats.Weight;
		NewStats.ErgonomicsChangePercentage = this->ErgonomicsChangePercentage + Stats.ErgonomicsChangePercentage;
		NewStats.VerticalRecoilChangePercentage = this->VerticalRecoilChangePercentage + Stats.VerticalRecoilChangePercentage;
		NewStats.HorizontalRecoilChangePercentage = this->HorizontalRecoilChangePercentage + Stats.HorizontalRecoilChangePercentage;
		NewStats.AccuracyChangePercentage = this->AccuracyChangePercentage + Stats.AccuracyChangePercentage;
		NewStats.MuzzleVelocityChangePercentage = this->MuzzleVelocityChangePercentage + Stats.MuzzleVelocityChangePercentage;
		NewStats.CycleRateIncreasePercentage = this->CycleRateIncreasePercentage + Stats.CycleRateIncreasePercentage;
		
		return NewStats;
	}
	
	FSKGFirearmPartStats& operator+= (const FSKGFirearmPartStats& Stats)
	{
		this->Weight += Stats.Weight;
		this->ErgonomicsChangePercentage += Stats.ErgonomicsChangePercentage;
		this->VerticalRecoilChangePercentage += Stats.VerticalRecoilChangePercentage;
		this->HorizontalRecoilChangePercentage += Stats.HorizontalRecoilChangePercentage;
		this->AccuracyChangePercentage += Stats.AccuracyChangePercentage;
		this->MuzzleVelocityChangePercentage += Stats.MuzzleVelocityChangePercentage;
		this->CycleRateIncreasePercentage += Stats.CycleRateIncreasePercentage;
		
		return *this;
	}
};

USTRUCT(BlueprintType)
struct FSKGFirearmStats
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float Weight = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float Ergonomics = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float VertialRecoilMultiplier = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float HorizontalRecoilMultiplier = 1.0f;
	// This is used to alter the MOA of your firearm (accuracy)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float AccuracyMultiplier = 1.0f;
	// This is used to alter how fast the projectile comes out of the muzzle
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float MuzzleVelocityMultiplier = 1.0f;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework")
	float CycleRateMultiplier = 1.0f;

	static inline void UpdateStats(FSKGFirearmStats& FirearmStats, const FSKGFirearmStats& DefaultFirearmStats, const FSKGFirearmPartStats& PartStats)
	{
		FirearmStats.Weight += PartStats.Weight;
		float PercentMultiplier = 1.0f + (PartStats.VerticalRecoilChangePercentage / 100.0f);
		PercentMultiplier = FMath::Clamp(PercentMultiplier, 0.1f, 100.0f);
		FirearmStats.VertialRecoilMultiplier = DefaultFirearmStats.VertialRecoilMultiplier;
		FirearmStats.VertialRecoilMultiplier *= PercentMultiplier;

		PercentMultiplier = 1.0f + (PartStats.HorizontalRecoilChangePercentage / 100.0f);
		PercentMultiplier = FMath::Clamp(PercentMultiplier, 0.1f, 100.0f);
		FirearmStats.HorizontalRecoilMultiplier = DefaultFirearmStats.HorizontalRecoilMultiplier;
		FirearmStats.HorizontalRecoilMultiplier *= PercentMultiplier;

		PercentMultiplier = 1.0f + (PartStats.ErgonomicsChangePercentage / 100.0f);
		FirearmStats.Ergonomics *= PercentMultiplier;

		PercentMultiplier = 1.0f + PartStats.AccuracyChangePercentage;
		PercentMultiplier = FMath::Clamp(PercentMultiplier, 0.1f, 10000.0f);
		FirearmStats.AccuracyMultiplier = DefaultFirearmStats.AccuracyMultiplier;
		FirearmStats.AccuracyMultiplier *= PercentMultiplier;

		PercentMultiplier = 1.0f + (PartStats.MuzzleVelocityChangePercentage / 100.0f);
		PercentMultiplier = FMath::Clamp(PercentMultiplier, 0.1f, 100.0f);
		FirearmStats.MuzzleVelocityMultiplier = DefaultFirearmStats.MuzzleVelocityMultiplier;
		FirearmStats.MuzzleVelocityMultiplier *= PercentMultiplier;

		PercentMultiplier = (PartStats.CycleRateIncreasePercentage / 100.0f);
		PercentMultiplier = FMath::Clamp(PercentMultiplier, 0.0f, 100.0f);
		FirearmStats.CycleRateMultiplier = DefaultFirearmStats.CycleRateMultiplier;
		FirearmStats.CycleRateMultiplier += -PercentMultiplier;
		FirearmStats.CycleRateMultiplier = FMath::Clamp(FirearmStats.CycleRateMultiplier, 0.1f, 10.0f);
	}
};

USTRUCT(BlueprintType)
struct FSKGFirearmPartData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	TSubclassOf<ASKGPart> PartClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	UTexture2D* PartImage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	FString PartName;
};

USTRUCT(BlueprintType)
struct FSKGLightDeviceSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	bool bSupportsInfrared = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float StrobeInterval = 1.0f;
};

USTRUCT(BlueprintType)
struct FSKGInfraredMaterialSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	bool bUseInfraredMaterialSettings = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework", meta = (EditCondition = "bUseInfraredMaterialSettings", EditConditionHides))
	UMaterialParameterCollection* InfraredMaterialParameterCollection = nullptr;
	// 1 = 1 second, 2 = 2 seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework", meta = (EditCondition = "bUseInfraredMaterialSettings", EditConditionHides))
	float InfraredStrobeIntervalSeconds = 1.0f;
	
	TWeakObjectPtr<UMaterialParameterCollectionInstance> InfraredMPC = nullptr;
	float TimeElapsed = 0.0f;
	bool bInfraredOn = false;
};

USTRUCT(BlueprintType)
struct FSKGSoundSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	USoundBase* Sound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	float VolumeMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	float PitchMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FSKGEffectImpactSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	UFXSystemAsset* ImpactEffect = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	UMaterialInstance* ImpactDecal = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	EPSCPoolMethod PoolingMethod = EPSCPoolMethod::AutoRelease;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	bool bUseParticlesRotation = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	FVector DecalSize = FVector(2.5f, 2.5f, 2.5f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	float DecalLifeTime = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	float DecalFadeScreenSize = 0.001f;
};

USTRUCT(BlueprintType)
struct FSKGImpactEffects
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	FSKGEffectImpactSettings EffectSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	FSKGSoundSettings SoundSettings;
};

USTRUCT(BlueprintType)
struct FSKGProjectileTransform
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "SKGFPSFramework | Projectile")
	FVector_NetQuantize10 Location;
	UPROPERTY(BlueprintReadWrite, Category = "SKGFPSFramework | Projectile")
	FRotator Rotation;

	FSKGProjectileTransform() {Location = FVector_NetQuantize10(); Rotation = FRotator();}
	FSKGProjectileTransform(const FVector& INLocation, const FRotator& INRotation)
	{
		Location = FVector_NetQuantize10(INLocation);
		//Rotation = FVector_NetQuantize100(INRotation.Pitch, INRotation.Yaw, INRotation.Roll);
		Rotation = INRotation;
	}
	FSKGProjectileTransform(const FTransform& INTransform)
	{
		Location = INTransform.GetLocation();
		Rotation = INTransform.Rotator();
	}
	
	static FTransform GetTransformFromProjectile(const FSKGProjectileTransform& ProjectileTransform)
	{
		return FTransform(ProjectileTransform.Rotation, ProjectileTransform.Location, FVector::OneVector);
	}
	FTransform GetTransformFromProjectile() const
	{
		return FTransform(Rotation, Location, FVector::OneVector);
	}
	
	operator FTransform() const { return GetTransformFromProjectile(FTransform(Rotation, Location)); }
};

USTRUCT(BlueprintType)
struct FSKGMinMax
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	float Min = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFPSFramework")
	float Max = 0.0f;

	FSKGMinMax() {}
	FSKGMinMax(float Minimum, float Maximum)
	{
		Min = Minimum;
		Max = Maximum;
	}
};

USTRUCT(BlueprintType)
struct FSKGSwayMultipliers
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float MovementPitchMultiplier = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float MovementYawMultiplier = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float MovementRollMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FSKGLeanCurves
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	UCurveFloat* LeanCurve = nullptr;
	// How fast you will lean left/right (used in conjunction with the Curve)
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	float DefaultLeanSpeed = 70.0f;
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	UCurveFloat* LeanEndCurve = nullptr;
	// How fast you will lean when unleaning (used in conjunction with the Curve)
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	float DefaultEndLeanSpeed = 50.0f;

	void HandleNewLeanCurveSettings(const FSKGLeanCurves& NewLeanCurves)
	{
		DefaultLeanSpeed = NewLeanCurves.DefaultLeanSpeed;
		DefaultEndLeanSpeed = NewLeanCurves.DefaultEndLeanSpeed;

		if (NewLeanCurves.LeanCurve)
		{
			LeanCurve = NewLeanCurves.LeanCurve;
		}
		if (NewLeanCurves.LeanEndCurve)
		{
			LeanEndCurve = NewLeanCurves.LeanEndCurve;
		}
	}
	
	bool IsValid() const
	{
		return LeanCurve != nullptr;
	}
};

USTRUCT(BlueprintType)
struct FSKGLeanSettings
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework")
	float ReplicatedLeanCurveTime = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework")
	bool bIncrementalLeaning = false;

	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	bool bAllowOverrideFromAimingActor = false;
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	FSKGLeanCurves LeanCurves;
	
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	float LeaningSmoothInterpSpeed = 80.0f;
	// How much to lean per incremental lean (each time to incremental lean how far do you lean)
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	float IncrementalLeanCurveTimeAmount = 5.0f;
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	float DefaultIncrementalLeanSpeed = 20.0f;
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	float LeanCurveTimeRight = 35.0f;
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	float LeanCurveTimeLeft = 35.0f;
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	float IncrementalLeanMaxLeft = 20.0f;
	UPROPERTY(EditDefaultsOnly, NotReplicated, BlueprintReadOnly, Category = "SKGFPSFramework")
	float IncrementalLeanMaxRight = 20.0f;
	
	ESKGLeaning CurrentLean = ESKGLeaning::None;
	float CurrentLeanCurveTime = 0.0f;
	float CurrentIncrementalLean = 0.0f;

	bool operator==(const FSKGLeanSettings& INLeanSettings) const
	{
		return CurrentLeanCurveTime == INLeanSettings.CurrentLeanCurveTime;
	}
};

USTRUCT(BlueprintType)
struct FSKGFirearmPoseCurveSetting
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	float CurveSpeedMultiplier = 25.0f;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	float CurveLength = 6.0f;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	bool bUseFirstPersonCurveForThird = true;
	// Blue = X(left/right), Red = Z(up/down), Yellow = Y(forward/back)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	UCurveVector* FirstPersonLocationCurve = nullptr;
	// Blue = Yaw(pivot left/right), Red = Roll(rotate left/right), Yellow = Pitch(pivot up/down)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	UCurveVector* FirstPersonRotationCurve = nullptr;
	// Blue = X(left/right), Red = Z(up/down), Yellow = Y(forward/back)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework", meta = (EditCondition = "!bUseFirstPersonCurveForThird", EditConditionHides))
	UCurveVector* ThirdPersonLocationCurve = nullptr;
	// Blue = Yaw(pivot left/right), Red = Roll(rotate left/right), Yellow = Pitch(pivot up/down)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework", meta = (EditCondition = "!bUseFirstPersonCurveForThird", EditConditionHides))
	UCurveVector* ThirdPersonRotationCurve = nullptr;

	
	FTransform GetTransform(const float CurveTime, bool bLocallyControlled) const
	{
		FTransform Transform;
		if (bLocallyControlled || bUseFirstPersonCurveForThird)
		{
			const FVector Location = FirstPersonLocationCurve->GetVectorValue(CurveTime);
			const FVector RotationVec = FirstPersonRotationCurve->GetVectorValue(CurveTime);
			const FRotator Rotation = FRotator(RotationVec.X, RotationVec.Y, RotationVec.Z);
			Transform = FTransform(Rotation, Location);
		}
		else
		{
			const FVector Location = ThirdPersonLocationCurve->GetVectorValue(CurveTime);
			const FVector RotationVec = ThirdPersonRotationCurve->GetVectorValue(CurveTime);
			const FRotator Rotation = FRotator(RotationVec.X, RotationVec.Y, RotationVec.Z);
			Transform = FTransform(Rotation, Location);
		}
		return Transform;
	}
	
	bool IsValid() const
	{
		if (bUseFirstPersonCurveForThird)
		{
			return FirstPersonLocationCurve && FirstPersonRotationCurve;
		}
		return FirstPersonLocationCurve && ThirdPersonLocationCurve && FirstPersonRotationCurve && ThirdPersonRotationCurve;
	}
};

USTRUCT(BlueprintType)
struct FSKGFirearmPoseCurveSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	bool bUseLegacySystem = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework", meta = (EditCondition = "!bUseLegacySystem", EditConditionHides))
	FSKGFirearmPoseCurveSetting PoseCurveSetting;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework", meta = (EditCondition = "!bUseLegacySystem", EditConditionHides))
	FSKGFirearmPoseCurveSetting ReturnCurveSetting;

	bool bGoIntoPose = true;

	FSKGFirearmPoseCurveSetting GetActiveCurveSetting() const
	{
		return bGoIntoPose ? PoseCurveSetting : ReturnCurveSetting;
	}

	FTransform GetTransform(const float CurveTime, bool bLocallyControlled) const
	{
		FTransform Transform;
		if (IsValid())
		{
			if (bGoIntoPose)
			{
				Transform = PoseCurveSetting.GetTransform(CurveTime, bLocallyControlled);
			}
			else
			{
				Transform = ReturnCurveSetting.GetTransform(CurveTime, bLocallyControlled);
			}
		}
		return Transform;
	}
	
	bool IsValid() const
	{
		return PoseCurveSetting.IsValid() && ReturnCurveSetting.IsValid();
	}
};

USTRUCT(BlueprintType)
struct FSKGPoseSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float AimInterpolationMultiplier = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float HighLowPortPoseInterpolationSpeed = 15.0f;
};

USTRUCT(BlueprintType)
struct FSKGCollisionSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float InterpolationSpeed = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float DistanceBeforePush = 2.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float TraceRadius = 3.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	FVector MuzzlePositionOffset = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float PushDistanceToStopAiming = 8.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	TEnumAsByte<EDrawDebugTrace::Type> DebugTrace = EDrawDebugTrace::Type::None;

	// The logic involving this gets compiled out
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	bool bPrintTraceHit = false;
};

USTRUCT(BlueprintType)
struct FSKGCurveData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	UCurveVector* LocationCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	UCurveVector* RotationCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float CurveDuration = 3.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float CurveSpeed = 8.0f;
};

USTRUCT(BlueprintType)
struct FSKGCurveAndShakeSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	UCurveVector* ShakeCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float ShakeCurveDuration = 3.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	UCurveVector* MovementSwayCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	bool PerformShakeAfterPortPose = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	float ShakeCurveSpeed = 8.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	bool ControlMovementSwayByStats = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework")
	bool ScaleCameraShakeByStats = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	TSubclassOf<UCameraShakeBase> FireCameraShake;
};

USTRUCT(BlueprintType)
struct FSKGRecoilData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	UCurveVector* ControlRotationCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	float ControlRotationScale = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	FSKGMinMax ControlRotationPitchRandomness = FSKGMinMax(0.1f, 0.5f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	FSKGMinMax ControlRotationYawRandomness = FSKGMinMax(-0.2f, 0.2f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	UCurveVector* RecoilLocationCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	FSKGMinMax RecoilLocationRandomness = FSKGMinMax(1.0f, 2.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	UCurveVector* RecoilRotationCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	FSKGMinMax RecoilPitchRandomness = FSKGMinMax(1.0f, 2.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	FSKGMinMax RecoilRollRandomness = FSKGMinMax(-3.0f, 3.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework")
	FSKGMinMax RecoilYawRandomness = FSKGMinMax(-8.0f, 8.0f);

	bool bUseControlRotation = false;
	
	FVector GetFixedLocationVector(const float Time) const
	{
		if (RecoilLocationCurve)
		{
			return RecoilLocationCurve->GetVectorValue(Time);
		}
		return FVector::ZeroVector;
	}

	FVector GetFixedRotationVector(const float Time) const
	{
		if (RecoilRotationCurve)
		{
			return RecoilRotationCurve->GetVectorValue(Time);
		}
		return FVector::ZeroVector;
	}
};