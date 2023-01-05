//Copyright 2021, Dakota Dawe, All rights reserved

#include "Actors/SKGFirearm.h"
#include "SKGCharacterAnimInstance.h"
#include "Misc/BlueprintFunctionsLibraries/SKGFPSStatics.h"
#include "Actors/FirearmParts/SKGSight.h"
#include "Actors/FirearmParts/SKGHandguard.h"
#include "Actors/FirearmParts/SKGForwardGrip.h"
#include "Actors/FirearmParts/SKGMuzzle.h"
#include "Actors/FirearmParts/SKGPart.h"
#include "Components/SKGCharacterComponent.h"
#include "Components/SKGFirearmStabilizerComponent.h"
#include "Components/SKGAttachmentManager.h"
#include "Components/SKGAttachmentComponent.h"

#include "DrawDebugHelpers.h"
#include "NiagaraSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "AIController.h"
#include "Interfaces/SKGStockInterface.h"

#define DEFAULT_STATS_MULTIPLIER (FirearmStats.Ergonomics * (10.0f / (FirearmStats.Weight * 1.5f)))

// Sets default values
ASKGFirearm::ASKGFirearm(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	NetUpdateFrequency = 8.0f;

	AttachmentManager = CreateDefaultSubobject<USKGAttachmentManager>(TEXT("FPSTemplate Attachment Manager"));
	
	/*FirearmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	FirearmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = FirearmMesh;*/
	PoseCollision = ECollisionChannel::ECC_GameTraceChannel2;

	bFirearmHidden = false;
	
	FirearmAnimationIndex = 0;
	PointAimIndex = -1;

	FirearmGripSocket = FName("cc_FirearmGrip");
	GripSocketOffset = FTransform();
	MuzzleSocket = FName("S_Muzzle");
	LeftHandIKSocket = FName("S_LeftHandIK");
	AimSocket = FName("S_Aim");

	CameraSettings.CameraFOVZoom = 10.0f;
	CameraSettings.CameraFOVZoomSpeed = 10.0f;
	CameraSettings.CameraDistance = 0.0f;
	CameraSettings.bUsedFixedCameraDistance = false;

	bSpawnDefaultPartsFromPreset = false;
	bShouldSpawnDefaultsFromPreset = true;

	bUseLeftHandIK = false;

	LeaningSpeedMultiplier = 1.4f;
	
	DefaultFirearmStats.Weight = 7.0f;
	DefaultFirearmStats.Ergonomics = 50.0f;
	DefaultFirearmStats.VertialRecoilMultiplier = 1.0f;
	DefaultFirearmStats.HorizontalRecoilMultiplier = 1.0f;

	PointAimADSInterpolationMultiplier = 1.2f;
	bUnAimMultiplierSameAsADS = false;
	UnAimInterpolationMultiplier = 1.0f;
	ShoulderStockOffset = FVector(-8.0f, 0.0f, 2.0f);
	HeadAimRotation = FRotator(45.0f, 0.0f, 0.0f);
	bUseBasePoseCorrection = false;

	bCanCycleSights = true;
	
	FireModeIndex = 0;
	BurstFireCount = 0;
	BurstCount = 3;

	FireRateRPM = 800.0f;

	TimeSinceLastShot = 0.0f;

	FirstPersonShortStockPose = FTransform();
	ThirdPersonShortStockPose = FTransform();
	FirstPersonBasePoseOffset = FTransform();
	FirstPersonSprintPose.SetLocation(FVector(-5.0f, 0.0f, -10.0f));
	FirstPersonSprintPose.SetRotation(FRotator(-45.0f, 0.0f, 20.0f).Quaternion());
	FirstPersonSuperSprintPose.SetLocation(FVector());
	FirstPersonSuperSprintPose.SetRotation(FRotator().Quaternion());
	FirstPersonHighPortPose.SetLocation(FVector(-10.0f, 0.0f, -10.0f));
	FirstPersonHighPortPose.SetRotation(FRotator(80.0f, 45.0f, 0.0f).Quaternion());
	FirstPersonLowPortPose.SetLocation(FVector(-10.0f, 0.0f, -10.0f));
	FirstPersonLowPortPose.SetRotation(FRotator(80.0f, -45.0f, 0.0f).Quaternion());

	ThirdPersonBasePoseOffset = FTransform();
	ThirdPersonSprintPose.SetLocation(FVector(-5.0f, 0.0f, -10.0f));
	ThirdPersonSprintPose.SetRotation(FRotator(-45.0f, 0.0f, 20.0f).Quaternion());
	ThirdPersonSuperSprintPose.SetLocation(FVector());
	ThirdPersonSuperSprintPose.SetRotation(FRotator().Quaternion());
	ThirdPersonHighPortPose.SetLocation(FVector(-10.0f, 0.0f, -3.0f));
	ThirdPersonHighPortPose.SetRotation(FRotator(80.0f, 45.0f, 0.0f).Quaternion());
	ThirdPersonLowPortPose.SetLocation(FVector(0.0f, 8.0f, 8.0f));
	ThirdPersonLowPortPose.SetRotation(FRotator(80.0f, -45.0f, 0.0f).Quaternion());

	DefaultSwayMultiplier = 2.0f;
	SwayMultiplier = DefaultSwayMultiplier;

	RotationLagInterpolationMultiplier = 1.0f;
	
	CharacterComponent = nullptr;

	//AttachedToSocket = NAME_None;
}

void ASKGFirearm::OnRep_CharacterComponent()
{
	//CycleSights();
	/*if (IsValid(this) && AttachedToSocket != NAME_None)
	{
		AttachToSocket(AttachedToSocket);
	}*/
}

// Called when the game starts or when spawned
void ASKGFirearm::BeginPlay()
{
	Super::BeginPlay();
	
	FirearmMesh = GetSkeletalMeshComponent();
	
	if (const AActor* OwningActor = GetOwner())
	{
		CharacterComponent = Cast<USKGCharacterComponent>(OwningActor->GetComponentByClass(USKGCharacterComponent::StaticClass()));
	}

	if (HasAuthority())
	{
		HandleSightComponents();
	}
	
	FixPoseTransforms(FirstPersonCollisionPose, ThirdPersonCollisionPose);
	FixPoseTransforms(FirstPersonBasePoseOffset, ThirdPersonBasePoseOffset);
	FixPoseTransforms(FirstPersonHighPortPose, ThirdPersonHighPortPose);
	FixPoseTransforms(FirstPersonLowPortPose, ThirdPersonLowPortPose);
	FixPoseTransforms(FirstPersonShortStockPose, ThirdPersonShortStockPose);
	FixPoseTransforms(FirstPersonOppositeShoulderPose, ThirdPersonOppositeShoulderPose);
	FixPoseTransforms(FirstPersonBlindFireLeftPose, ThirdPersonBlindFireLeftPose);
	FixPoseTransforms(FirstPersonBlindFireUpPose, ThirdPersonBlindFireUpPose);
	FixPoseTransforms(FirstPersonSprintPose, ThirdPersonSprintPose);
	FixPoseTransforms(FirstPersonSuperSprintPose, ThirdPersonSuperSprintPose);
}

void ASKGFirearm::PostInitProperties()
{
	Super::PostInitProperties();
	SwayMultiplier = DefaultSwayMultiplier;
	
	FirearmStats = DefaultFirearmStats;
	if (FireModes.Num())
	{
		FireMode = FireModes[0];
	}
	TimerAutoFireRate = 60 / FireRateRPM;

	bool bValidMesh = false;
	if (GetSkeletalMeshComponent())
	{
		bValidMesh = true;
		GetSkeletalMeshComponent()->SetCollisionResponseToChannel(PoseCollision, ECR_Ignore);
		GetSkeletalMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetSkeletalMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
		GetSkeletalMeshComponent()->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
	}
	{
		ensureMsgf(bValidMesh, TEXT("Firearm: %s Has an INVALID skeletal mesh component"), *GetName());
	}
}

void ASKGFirearm::OnRep_Owner()
{
	if (const AActor* OwningActor = GetOwner())
	{
		CharacterComponent = Cast<USKGCharacterComponent>(OwningActor->GetComponentByClass(USKGCharacterComponent::StaticClass()));
	}
	RefreshCurrentSight();
	//CycleSights();
}

void ASKGFirearm::OnRep_AttachmentReplication()
{
	if (GetCharacterComponent())
	{
		USceneComponent* AttachParentComponent = CharacterComponent->GetInUseMesh();

		if (AttachParentComponent)
		{
			RootComponent->SetRelativeLocation_Direct(GetAttachmentReplication().LocationOffset);
			RootComponent->SetRelativeRotation_Direct(GetAttachmentReplication().RotationOffset);
			RootComponent->SetRelativeScale3D_Direct(GetAttachmentReplication().RelativeScale3D);

			// If we're already attached to the correct Parent and Socket, then the update must be position only.
			// AttachToComponent would early out in this case.
			// Note, we ignore the special case for simulated bodies in AttachToComponent as AttachmentReplication shouldn't get updated
			// if the body is simulated (see AActor::GatherMovement).
			const bool bAlreadyAttached = (AttachParentComponent == RootComponent->GetAttachParent() && GetAttachmentReplication().AttachSocket == RootComponent->GetAttachSocketName() && AttachParentComponent->GetAttachChildren().Contains(RootComponent));
			if (bAlreadyAttached)
			{
				// Note, this doesn't match AttachToComponent, but we're assuming it's safe to skip physics (see comment above).
				RootComponent->UpdateComponentToWorld(EUpdateTransformFlags::SkipPhysicsUpdate, ETeleportType::None);
			}
			else
			{
				RootComponent->AttachToComponent(AttachParentComponent, FAttachmentTransformRules::KeepRelativeTransform, GetAttachmentReplication().AttachSocket);
			}
		}
	}
	else
	{
		Super::OnRep_AttachmentReplication();
	}
}

FPrimaryAssetId ASKGFirearm::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(AssetType, AssetName);
}

void ASKGFirearm::FixPoseTransforms(FTransform& FirstPerson, FTransform& ThirdPerson)
{
	FirstPerson = USKGFPSStatics::FixTransform(FirstPerson);
	ThirdPerson = USKGFPSStatics::FixTransform(ThirdPerson);
}

void ASKGFirearm::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASKGFirearm, CharacterComponent);
	DOREPLIFETIME(ASKGFirearm, bFirearmHidden);
	DOREPLIFETIME(ASKGFirearm, CurrentSightComponent);
	DOREPLIFETIME(ASKGFirearm, PointAimIndex);
	DOREPLIFETIME(ASKGFirearm, CachedComponents);
	DOREPLIFETIME_CONDITION(ASKGFirearm, FirearmStats, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASKGFirearm, FireMode, COND_SkipOwner);
}

float ASKGFirearm::GetStockLengthOfPull_Implementation()
{
	if (IsValid(CachedComponents.Stock))
	{
		const AActor* Stock = CachedComponents.Stock->GetAttachment();
		if (IsValid(Stock) && Stock->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()) && Stock->GetClass()->ImplementsInterface(USKGStockInterface::StaticClass()))
		{
			if (Stock->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()))
			{
				return ISKGAttachmentInterface::Execute_GetAttachmentOffset(Stock) + ISKGStockInterface::Execute_GetStockLengthOfPull(Stock);	
			}
		}
	}
	return 0.0f;
}

void ASKGFirearm::SetFirearmPoseMultiplier(FSKGFirearmPoseCurveSettings& FirearmPose, ESKGFirearmPoseType PoseType, float Multiplier)
{
	switch (PoseType)
	{
	case ESKGFirearmPoseType::ToPose:
		{
			FirearmPose.PoseCurveSetting.CurveSpeedMultiplier = Multiplier;
			break;
		}
	case ESKGFirearmPoseType::ReturnPose:
		{
			FirearmPose.ReturnCurveSetting.CurveSpeedMultiplier = Multiplier;
			break;
		}
	case ESKGFirearmPoseType::Both:
		{
			FirearmPose.PoseCurveSetting.CurveSpeedMultiplier = Multiplier;
			FirearmPose.ReturnCurveSetting.CurveSpeedMultiplier = Multiplier;
			break;
		}
	}
}

void ASKGFirearm::SetFirearmPoseCurveSpeedMultiplier(ESKGFirearmPose Pose, ESKGFirearmPoseType PoseType, float NewMultiplier)
{
	switch (Pose)
	{
	case ESKGFirearmPose::High:
		{
			SetFirearmPoseMultiplier(HighPortCurveSettings, PoseType, NewMultiplier);
			break;
		}
	case ESKGFirearmPose::Low:
		{
			SetFirearmPoseMultiplier(LowPortCurveSettings, PoseType, NewMultiplier);
			break;
		}
	case ESKGFirearmPose::ShortStock:
		{
			SetFirearmPoseMultiplier(ShortStockCurveSettings, PoseType, NewMultiplier);
			break;
		}
	}
}

float ASKGFirearm::GetAimInterpolationMultiplier_Implementation()
{
	float SightAimMultiplier = 1.0f;
	if (PointAimIndex > INDEX_NONE)
	{
		SightAimMultiplier = PointAimADSInterpolationMultiplier;
	}
	else
	{
		ASKGSight* Sight = Execute_GetCurrentSight(this);
		if (IsValid(Sight) && Sight->Implements<USKGAimInterface>())
		{
			SightAimMultiplier = ISKGProceduralAnimationInterface::Execute_GetAimInterpolationMultiplier(Sight);
		}
	}
	return (DEFAULT_STATS_MULTIPLIER * PoseSettings.AimInterpolationMultiplier) * SightAimMultiplier;
}

float ASKGFirearm::GetUnAimInterpolationMultiplier_Implementation()
{
	if (bUnAimMultiplierSameAsADS)
	{
		return ISKGProceduralAnimationInterface::Execute_GetAimInterpolationMultiplier(this);
	}
	return (DEFAULT_STATS_MULTIPLIER * PoseSettings.AimInterpolationMultiplier) * UnAimInterpolationMultiplier;
}

float ASKGFirearm::GetRotationLagInterpolationMultiplier_Implementation()
{
	return DEFAULT_STATS_MULTIPLIER * RotationLagInterpolationMultiplier;
}

void ASKGFirearm::GetCameraSettings_Implementation(FSKGAimCameraSettings& OutCameraSettings)
{
	if (PointAimIndex > INDEX_NONE)
	{
		OutCameraSettings = CameraSettings;
		return;
	}
	ASKGSight* CurrentSight = Execute_GetCurrentSight(this);
	if (IsValid(CurrentSight))
	{
		if (CurrentSight->Implements<USKGAimInterface>())
		{
			Execute_GetCameraSettings(CurrentSight, OutCameraSettings);
			return;
		}
	}
	OutCameraSettings = CameraSettings;
}

float ASKGFirearm::GetCurrentMagnification_Implementation() const
{
	if (IsValid(CurrentSightComponent))
	{
		const AActor* CurrentSight = CurrentSightComponent->GetAttachment();
		if (IsValid(CurrentSight) && CurrentSight->GetClass()->ImplementsInterface(USKGSightInterface::StaticClass()))
		{
			return ISKGSightInterface::Execute_GetCurrentMagnification(CurrentSight);
		}
	}
	return 1.0f;
}

FTransform ASKGFirearm::GetAimSocketTransform_Implementation()
{
	if (PointAimIndex > INDEX_NONE && PointAimIndex < PointAimSockets.Num() && FirearmMesh->DoesSocketExist(PointAimSockets[PointAimIndex]))
	{
		return FirearmMesh->GetSocketTransform(PointAimSockets[PointAimIndex]);
	}
	
	if (CurrentSightComponent)
	{
		AActor* CurrentAimingDevice= CurrentSightComponent->GetAttachment();
		if (IsValid(CurrentAimingDevice) && CurrentAimingDevice->GetClass()->ImplementsInterface(USKGAimInterface::StaticClass()))
		{
			return ISKGProceduralAnimationInterface::Execute_GetAimSocketTransform(CurrentAimingDevice);
		}
	}

	return FirearmMesh->GetSocketTransform(AimSocket);
}

FTransform ASKGFirearm::GetDefaultAimSocketTransform_Implementation()
{
	return FirearmMesh->DoesSocketExist(AimSocket) ? FirearmMesh->GetSocketTransform(AimSocket) : Execute_GetAimSocketTransform(this);
}

void ASKGFirearm::HandleSightComponents()
{
	if (!IsValid(CurrentSightComponent) || (IsValid(CurrentSightComponent) && IsValid(CurrentSightComponent->GetAttachment()) && (IsValid(CurrentSightComponent) && !IsValid(CurrentSightComponent->GetAttachment()))))
	{
		Execute_CycleSights(this, true, false);
	}
}

USKGCharacterComponent* ASKGFirearm::GetCharacterComponent()
{
	if (!CharacterComponent)
	{
		if (IsValid(GetOwner()))
		{
			UActorComponent* Component = GetOwner()->GetComponentByClass(USKGCharacterComponent::StaticClass());
			CharacterComponent = Cast<USKGCharacterComponent>(Component);
			OnRep_CharacterComponent();
		}
	}
	return CharacterComponent;
}

bool ASKGFirearm::IsSuppressed()
{
	const ASKGMuzzle* Muzzle = GetMuzzleDevice();
	if (IsValid(Muzzle) && Muzzle->IsSuppressor())
	{
		return true;
	}
	return false;
}

UNiagaraSystem* ASKGFirearm::GetFireNiagaraSystem()
{
	AActor* MuzzleActor = GetMuzzleActor();
	if (IsValid(MuzzleActor) && MuzzleActor->GetClass()->ImplementsInterface(USKGMuzzleInterface::StaticClass()))
	{
		if (UNiagaraSystem* NiagaraSystem = ISKGMuzzleInterface::Execute_GetFireNiagaraSystem(MuzzleActor))
		{
			return NiagaraSystem;
		}
	}
	
	const int32 RandomIndex = USKGFPSStatics::GetRandomIndexForArray(FireNiagaraSystems.Num());
	if (RandomIndex != INDEX_NONE)
	{
		return FireNiagaraSystems[RandomIndex];
	}
	return nullptr;
}

bool ASKGFirearm::Server_SetPointAiming_Validate(int32 Index)
{
	return true;
}

void ASKGFirearm::Server_SetPointAiming_Implementation(int32 Index)
{
	Execute_SetPointAimIndex(this, Index);
}

void ASKGFirearm::SetPointAimIndex_Implementation(int32 Index)
{
	if (Index < PointAimSockets.Num())
	{
		PointAimIndex = Index;
		if (GetCharacterComponent() && CharacterComponent->IsAiming())
		{
			if (PointAimIndex > INDEX_NONE)
			{
				Execute_ActivateCurrentSight(this, false);
			}
			else
			{
				Execute_ActivateCurrentSight(this, true);
			}
		}
		//OnRep_PointAimIndex();
	}
	else
	{
		PointAimIndex = INDEX_NONE;
	}
	
	if (!HasAuthority())
	{
		Server_SetPointAiming(PointAimIndex);
	}
}

void ASKGFirearm::CyclePointAim_Implementation(bool bDownArray, bool bStopAtEndOfArray)
{
	if (bDownArray)
	{
		if (++PointAimIndex > PointAimSockets.Num() - 1)
		{
			if (!bStopAtEndOfArray)
			{
				PointAimIndex = 0;
			}
			else
			{
				PointAimIndex = PointAimSockets.Num() - 1;
			}
		}
	}
	else
	{
		if (--PointAimIndex < 0)
		{
			if (!bStopAtEndOfArray)
			{
				PointAimIndex = PointAimSockets.Num() - 1;
			}
			else
			{
				PointAimIndex = 0;
			}
		}
	}
	Execute_SetPointAimIndex(this, PointAimIndex);
}

ASKGMuzzle* ASKGFirearm::GetMuzzleDevice()
{
	return Cast<ASKGMuzzle>(GetMuzzleActor());
}

AActor* ASKGFirearm::GetMuzzleActor()
{
	USKGAttachmentComponent* AttachmentComponent = CachedComponents.Muzzle;
	if (AttachmentComponent)
	{
		AActor* Muzzle = AttachmentComponent->GetAttachment();
		if (IsValid(Muzzle))
		{
			return Muzzle;
		}
	}
	
	AttachmentComponent = CachedComponents.Barrel;
	if (AttachmentComponent)
	{
		AActor* Barrel = AttachmentComponent->GetAttachment();
		if (IsValid(Barrel))
		{
			return Barrel;
		}
	}
	return this;
}

FHitResult ASKGFirearm::MuzzleTrace(float Distance, ECollisionChannel CollisionChannel, bool& bMadeBlockingHit, bool bDrawDebugLine)
{
	FTransform MuzzleTransform = FSKGProjectileTransform::GetTransformFromProjectile(GetMuzzleSocketTransform());
	FVector End = MuzzleTransform.GetLocation() + MuzzleTransform.Rotator().Vector() * Distance;
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bMadeBlockingHit = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleTransform.GetLocation(), End, CollisionChannel, Params);
	if (bDrawDebugLine)
	{
		DrawDebugLine(GetWorld(), MuzzleTransform.GetLocation(), End, FColor::Red, false, 3.0f, 0, 2.0f);
	}
	return HitResult;
}

TArray<FHitResult> ASKGFirearm::MuzzleTraceMulti(float Distance, ECollisionChannel CollisionChannel, bool& bMadeBlockingHit, bool bDrawDebugLine)
{
	const FTransform MuzzleTransform = FSKGProjectileTransform::GetTransformFromProjectile(GetMuzzleSocketTransform());
	const FVector End = MuzzleTransform.GetLocation() + MuzzleTransform.Rotator().Vector() * Distance;
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bMadeBlockingHit = GetWorld()->LineTraceMultiByChannel(HitResults, MuzzleTransform.GetLocation(), End, CollisionChannel, Params);
	if (bDrawDebugLine)
	{
		DrawDebugLine(GetWorld(), MuzzleTransform.GetLocation(), End, FColor::Red, false, 3.0f, 0, 2.0f);
	}
	return HitResults;
}

void ASKGFirearm::DisableAllRenderTargets(bool Disable)
{
	for (const USKGAttachmentComponent* PartComponent : CachedComponents.RenderTargets)
	{
		if (PartComponent != CurrentSightComponent)
		{
			AActor* Part = PartComponent->GetAttachment();
			if (IsValid(Part) && Part->GetClass()->ImplementsInterface(USKGRenderTargetInterface::StaticClass()))
			{
				if (!Disable)
				{
					ISKGRenderTargetInterface::Execute_DisableRenderTarget(Part, true);
					continue;
				}

				ISKGRenderTargetInterface::Execute_DisableRenderTarget(Part, Disable);
			}
		}
	}
	//RefreshCurrentSight();
}

FTransform ASKGFirearm::GetSprintPose_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonSprintPose; // THIRD PERSON HERE
		}
	}
	return FirstPersonSprintPose;
}

FTransform ASKGFirearm::GetSuperSprintPose_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonSuperSprintPose; // THIRD PERSON HERE
		}
	}
	return FirstPersonSuperSprintPose;
}

FTransform ASKGFirearm::GetCollisionShortStockPose_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonCollisionShortStockPose;
		}
	}
	return FirstPersonCollisionShortStockPose;
}

FTransform ASKGFirearm::GetCollisionPose_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonCollisionPose;
		}
	}
	return FirstPersonCollisionPose;
}

FTransform ASKGFirearm::GetHighPortPose_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonHighPortPose;
		}
	}
	return FirstPersonHighPortPose;
}

FTransform ASKGFirearm::GetLowPortPose_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonLowPortPose;
		}
	}
	return FirstPersonLowPortPose;
}

FTransform ASKGFirearm::GetBasePoseOffset_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonBasePoseOffset;
		}
	}
	return FirstPersonBasePoseOffset;
}

FTransform ASKGFirearm::GetShortStockPose_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonShortStockPose;
		}
	}
	return FirstPersonShortStockPose;
}

FTransform ASKGFirearm::GetOppositeShoulderPose_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonOppositeShoulderPose;
		}
	}
	return FirstPersonOppositeShoulderPose;
}

FTransform ASKGFirearm::GetBlindFireLeftPose_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonBlindFireLeftPose;
		}
	}
	return FirstPersonBlindFireLeftPose;
}

FTransform ASKGFirearm::GetBlindFireUpPose_Implementation()
{
	if (GetCharacterComponent())
	{
		if (!CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
		{
			return ThirdPersonBlindFireUpPose;
		}
	}
	return FirstPersonBlindFireUpPose;
}

void ASKGFirearm::SetShortStockPose(const FTransform& Transform, bool bFirstPerson)
{
	const FTransform FixedTransform = USKGFPSStatics::FixTransform(Transform);
	if (bFirstPerson)
	{
		FirstPersonShortStockPose = FixedTransform;
	}
	else
	{
		ThirdPersonShortStockPose = FixedTransform;
	}
}

void ASKGFirearm::SetBasePoseOffset(const FTransform& Transform, bool bFirstPerson)
{
	const FTransform FixedTransform = USKGFPSStatics::FixTransform(Transform);
	if (bFirstPerson)
	{
		FirstPersonBasePoseOffset = FixedTransform;
	}
	else
	{
		ThirdPersonBasePoseOffset = FixedTransform;
	}
}

void ASKGFirearm::SetHighPortPose(const FTransform& Transform, bool bFirstPerson)
{
	const FTransform FixedTransform = USKGFPSStatics::FixTransform(Transform);
	if (bFirstPerson)
	{
		FirstPersonHighPortPose = FixedTransform;
	}
	else
	{
		ThirdPersonHighPortPose = FixedTransform;
	}
}

void ASKGFirearm::SetLowPortPose(const FTransform& Transform, bool bFirstPerson)
{
	const FTransform FixedTransform = USKGFPSStatics::FixTransform(Transform);
	if (bFirstPerson)
	{
		FirstPersonLowPortPose = FixedTransform;
	}
	else
	{
		ThirdPersonLowPortPose = FixedTransform;
	}
}

void ASKGFirearm::SetOppositeShoulderPose(const FTransform& Transform, bool bFirstPerson)
{
	const FTransform FixedTransform = USKGFPSStatics::FixTransform(Transform);
	if (bFirstPerson)
	{
		FirstPersonOppositeShoulderPose = FixedTransform;
	}
	else
	{
		ThirdPersonOppositeShoulderPose = FixedTransform;
	}
}

void ASKGFirearm::SetBlindFireleftPose(const FTransform& Transform, bool bFirstPerson)
{
	const FTransform FixedTransform = USKGFPSStatics::FixTransform(Transform);
	if (bFirstPerson)
	{
		FirstPersonBlindFireLeftPose = FixedTransform;
	}
	else
	{
		ThirdPersonBlindFireLeftPose = FixedTransform;
	}
}

void ASKGFirearm::SetBlindFireUpPose(const FTransform& Transform, bool bFirstPerson)
{
	const FTransform FixedTransform = USKGFPSStatics::FixTransform(Transform);
	if (bFirstPerson)
	{
		FirstPersonBlindFireUpPose = FixedTransform;
	}
	else
	{
		ThirdPersonBlindFireUpPose = FixedTransform;
	}
}

void ASKGFirearm::SetSprintPose(const FTransform& Transform, bool bFirstPerson)
{
	const FTransform FixedTransform = USKGFPSStatics::FixTransform(Transform);
	if (bFirstPerson)
	{
		FirstPersonSprintPose = FixedTransform;
	}
	else
	{
		ThirdPersonSprintPose = FixedTransform;
	}
}

void ASKGFirearm::SetSuperSprintPose(const FTransform& Transform, bool bFirstPerson)
{
	const FTransform FixedTransform = USKGFPSStatics::FixTransform(Transform);
	if (bFirstPerson)
	{
		FirstPersonSuperSprintPose = FixedTransform;
	}
	else
	{
		ThirdPersonSuperSprintPose = FixedTransform;
	}
}

UAnimSequence* ASKGFirearm::GetGripAnimation_Implementation()
{
	if (const USKGAttachmentComponent* AttachmentComponent = CachedComponents.ForwardGrip)
	{
		const ASKGForwardGrip* ForwardGrip = AttachmentComponent->GetAttachment<ASKGForwardGrip>();
		if (IsValid(ForwardGrip))
		{
			return ForwardGrip->GetGripAnimation();
		}
	}

	if (const USKGAttachmentComponent* AttachmentComponent = CachedComponents.Handguard)
	{
		const ASKGHandguard* Handguard = AttachmentComponent->GetAttachment<ASKGHandguard>();
		if (IsValid(Handguard))
		{
			return Handguard->GetGripAnimation();
		}
	}
	return GripAnimation;
}

void ASKGFirearm::SetupStabilizerComponent_Implementation()
{
	if (!StabilizerComponent.IsValid())
	{
		StabilizerComponent = Cast<USKGFirearmStabilizerComponent>(GetComponentByClass(USKGFirearmStabilizerComponent::StaticClass()));
	}
	else
	{
		StabilizerComponent->CacheEssentials();
	}
}

USKGFirearmStabilizerComponent* ASKGFirearm::GetStabilizerComponent()
{
	Execute_SetupStabilizerComponent(this);
	return StabilizerComponent.Get();
}

bool ASKGFirearm::IsStabilized() const
{
	if (StabilizerComponent.IsValid())
	{
		return StabilizerComponent->IsStabilized();
	}
	return false;
}

void ASKGFirearm::OnRep_FireMode()
{
	if (GetCharacterComponent() && CharacterComponent->IsLocallyControlled())
	{
		return;
	}
	//OnFireModeChanged();
}

void ASKGFirearm::OnRep_FirearmHidden() const
{
	FirearmMesh->SetHiddenInGame(bFirearmHidden, true);
}

bool ASKGFirearm::Server_SetFireMode_Validate(ESKGFirearmFireMode NewFireMode)
{
	return true;
}

void ASKGFirearm::Server_SetFireMode_Implementation(ESKGFirearmFireMode NewFireMode)
{
	FireMode = NewFireMode;
	OnRep_FireMode();
}

void ASKGFirearm::CycleFireMode_Implementation(bool bReverse)
{
	if (FireModes.Num() > 1)
	{
		if (bReverse)
		{
			if (--FireModeIndex < 0)
			{
				FireModeIndex = FireModes.Num() - 1;
			}
		}
		else
		{
			if (++FireModeIndex > FireModes.Num() - 1)
			{
				FireModeIndex = 0;
			}
		}
		FireMode = FireModes[FireModeIndex];
		//OnFireModeChanged();
		if (!HasAuthority())
		{
			Server_SetFireMode(FireMode);
		}
	}
}

void ASKGFirearm::PerformProceduralRecoil(float Multiplier, bool PlayCameraShake, bool bModifyControlRotation)
{
	if (GetCharacterComponent() && CharacterComponent->GetAnimationInstance())
	{
		RecoilData.bUseControlRotation = bModifyControlRotation;
		CharacterComponent->GetAnimationInstance()->PerformRecoil(Multiplier);
		if (PlayCameraShake && CurveAndShakeSettings.FireCameraShake)
		{
			CharacterComponent->PlayCameraShake(CurveAndShakeSettings.FireCameraShake, Multiplier * 1.2f);
		}
	}
}

FTransform ASKGFirearm::GetLeftHandIKTransform_Implementation()
{
	if (const USKGAttachmentComponent* AttachmentComponent = CachedComponents.ForwardGrip)
	{
		const ASKGForwardGrip* ForwardGrip = AttachmentComponent->GetAttachment<ASKGForwardGrip>();
		if (IsValid(ForwardGrip))
		{
			return ForwardGrip->GetGripTransform();
		}
	}

	if (const USKGAttachmentComponent* AttachmentComponent = CachedComponents.Handguard)
	{
		const ASKGHandguard* Handguard = AttachmentComponent->GetAttachment<ASKGHandguard>();
		if (IsValid(Handguard))
		{
			return Handguard->GetGripTransform();
		}
	}
	return FirearmMesh->GetSocketTransform(LeftHandIKSocket);
}

FTransform ASKGFirearm::GetMuzzleSocketTransform_Implementation()
{
	AActor* MuzzleActor = GetMuzzleActor();
	if (IsValid(MuzzleActor) && MuzzleActor->GetClass()->ImplementsInterface(USKGMuzzleInterface::StaticClass()))
	{
		return ISKGMuzzleInterface::Execute_GetMuzzleSocketTransform(MuzzleActor);
	}
	return FirearmMesh->GetSocketTransform(MuzzleSocket);
}

TArray<FSKGProjectileTransform> ASKGFirearm::GetMultipleMuzzleProjectileSocketTransforms(float RangeMeters, float InchSpreadAt25Yards,	uint8 ShotCount)
{
	TArray<FSKGProjectileTransform> ProjectileTransforms;
	ProjectileTransforms.Reserve(ShotCount);	InchSpreadAt25Yards *= 4.0f;
	for (uint8 i = 0; i < ShotCount; ++i)
	{
		ProjectileTransforms.Add(GetMuzzleProjectileSocketTransform(RangeMeters, InchSpreadAt25Yards));
	}
	return ProjectileTransforms;
}

FSKGProjectileTransform ASKGFirearm::GetMuzzleProjectileSocketTransform(float RangeMeters, float MOA)
{
	RangeMeters *= 100.0f;
	if (RangeMeters > 10000)
	{
		RangeMeters = 10000;
	}
	else if (RangeMeters < 2500)
	{
		RangeMeters = 2500;
	}

	const FTransform SightTransform = Execute_GetAimSocketTransform(this);
	FTransform MuzzleTransform = Execute_GetMuzzleSocketTransform(this);
	FRotator MuzzleRotation = USKGFPSStatics::GetEstimatedMuzzleToScopeZero(MuzzleTransform, SightTransform, RangeMeters);	
	MuzzleRotation = USKGFPSStatics::SetMuzzleMOA(MuzzleRotation, MOA);

	MuzzleTransform.SetRotation(MuzzleRotation.Quaternion());
	return MuzzleTransform;
}

FSKGProjectileTransform ASKGFirearm::GetProjectileSocketTransformToCenter(const float MaxDistanceToTest, float MOA, bool bAimingOverride, float RangeMetersOverride)
{
	FSKGProjectileTransform MuzzleProjectileTransform = GetMuzzleProjectileSocketTransform(100, MOA);
	if (GetCharacterComponent() && CharacterComponent->GetAnimationInstance())
	{
		if (bAimingOverride && CharacterComponent->IsAiming())
		{
			return GetMuzzleProjectileSocketTransform(RangeMetersOverride, MOA);
		}
		FVector Start = FVector::ZeroVector;
		FVector End = FVector::ZeroVector;
		
		if (AAIController* AIController = Cast<AAIController>(CharacterComponent->GetOwningController()))
		{
			Start = CharacterComponent->GetInUseMesh()->GetSocketLocation(FName(CharacterComponent->GetCameraSocket()));
			End = Start + AIController->GetControlRotation().Vector() * MaxDistanceToTest;
		}
		else
		{
			Start = CharacterComponent->GetCameraComponent()->GetComponentLocation();
			End = Start + CharacterComponent->GetCameraComponent()->GetForwardVector() * MaxDistanceToTest;
		}
		
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(GetOwner());
		QueryParams.AddIgnoredActors(GetCachedParts());
		
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
		{
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleProjectileTransform.Location, HitResult.Location);
			MuzzleProjectileTransform.Rotation = LookAtRotation;
			FRotator AddedRotator = CharacterComponent->GetAnimationInstance()->GetRecoilTransform().Rotator();
			float Temp = AddedRotator.Pitch;
			AddedRotator.Pitch = -AddedRotator.Yaw;
			AddedRotator.Yaw = Temp;
			MuzzleProjectileTransform.Rotation += AddedRotator;

			AddedRotator = CharacterComponent->GetAnimationInstance()->GetSwayTransform().Rotator();
			Temp = AddedRotator.Pitch;
			AddedRotator.Pitch = -AddedRotator.Yaw;
			AddedRotator.Yaw = Temp;
			MuzzleProjectileTransform.Rotation += AddedRotator;

			AddedRotator = CharacterComponent->GetAnimationInstance()->GetRotationLagTransform().Rotator();
			Temp = AddedRotator.Pitch;
			AddedRotator.Pitch = -AddedRotator.Yaw;
			AddedRotator.Yaw = Temp;
			MuzzleProjectileTransform.Rotation += AddedRotator;

			AddedRotator = CharacterComponent->GetAnimationInstance()->GetMovementLagTransform().Rotator();
			Temp = AddedRotator.Pitch;
			AddedRotator.Pitch = -AddedRotator.Yaw;
			AddedRotator.Yaw = Temp;
			MuzzleProjectileTransform.Rotation += AddedRotator;

			MuzzleProjectileTransform.Rotation = USKGFPSStatics::SetMuzzleMOA(MuzzleProjectileTransform.Rotation, MOA);
		}
	}
	
	return MuzzleProjectileTransform;
}

void ASKGFirearm::OnRep_CurrentSightComponent()
{
	if (GetCharacterComponent() && CharacterComponent->GetAnimationInstance())
	{
		if (!CharacterComponent->IsLocallyControlled())
		{
			CharacterComponent->GetAnimationInstance()->CycledSights();
		}
	}
}

bool ASKGFirearm::Server_CycleSights_Validate(USKGAttachmentComponent* SightComponent)
{
	return true;
}

void ASKGFirearm::Server_CycleSights_Implementation(USKGAttachmentComponent* SightComponent)
{
	if (SightComponent)
	{
		CurrentSightComponent = SightComponent;
		OnRep_CurrentSightComponent();
	}
}

void ASKGFirearm::ActivateCurrentSight_Implementation(bool bActivate) const
{
	if (IsValid(CurrentSightComponent))
	{
		AActor* CurrentSight = CurrentSightComponent->GetAttachment();
		if (IsValid(CurrentSight) && CurrentSight->GetClass()->ImplementsInterface(USKGRenderTargetInterface::StaticClass()))
		{
			if (bActivate && PointAimIndex > INDEX_NONE)
			{
				ISKGRenderTargetInterface::Execute_DisableRenderTarget(CurrentSight, true);
				return;
			}
			ISKGRenderTargetInterface::Execute_DisableRenderTarget(CurrentSight, !bActivate);
		}
	}
}

void ASKGFirearm::OnAttachmentUpdated_Implementation()
{
	CachedComponents.Empty();
	CachedParts.Empty();
	CachedParts.Reserve(AttachmentManager->GetAllAttachmentComponents(false).Num());
	FirearmStats = DefaultFirearmStats;
	for (USKGAttachmentComponent* AttachmentComponent : AttachmentManager->GetAllAttachmentComponents(false))
	{
		HandleCachingAttachment(AttachmentComponent);
		HandleUpdateFirearmStats(AttachmentComponent);
		if (AActor* Attachment = AttachmentComponent->GetAttachment())
		{
			CachedParts.Add(AttachmentComponent->GetAttachment());
		}
	}
	CachedParts.Shrink();
	RefreshCurrentSight();
	HandleSightComponents();
}

void ASKGFirearm::HandleCachingAttachment(USKGAttachmentComponent* AttachmentComponent)
{
	if (AttachmentComponent)
	{
		AActor* Part = AttachmentComponent->GetAttachment();
		if (IsValid(Part) && Part->GetClass()->ImplementsInterface(USKGFirearmAttachmentsInterface::StaticClass()))
		{
			bool bAimable = false;
			bool bHasRenderTarget = false;
			if (Part->GetClass()->ImplementsInterface(USKGAimInterface::StaticClass()))
			{
				bAimable = ISKGAimInterface::Execute_IsAimable(Part);
			}
			if (Part->GetClass()->ImplementsInterface(USKGRenderTargetInterface::StaticClass()))
			{
				bHasRenderTarget = ISKGRenderTargetInterface::Execute_HasRenderTarget(Part);
			}
			CachedComponents.AddAttachment(AttachmentComponent, ISKGFirearmAttachmentsInterface::Execute_GetPartType(Part), bAimable, bHasRenderTarget);
		}
	}
}

void ASKGFirearm::HandleUpdateFirearmStats(USKGAttachmentComponent* AttachmentComponent)
{
	if (HasAuthority() && AttachmentComponent)
	{
		AActor* Part = AttachmentComponent->GetAttachment();
		if (IsValid(Part) && Part->GetClass()->ImplementsInterface(USKGFirearmAttachmentsInterface::StaticClass()))
		{
			const FSKGFirearmPartStats PartStats = ISKGFirearmAttachmentsInterface::Execute_GetPartStats(Part);

			FSKGFirearmStats::UpdateStats(FirearmStats, DefaultFirearmStats, PartStats);
		}
	}
}

TArray<USKGAttachmentComponent*> ASKGFirearm::GetAttachmentComponents_Implementation()
{
	return AttachmentManager->GetAttachmentComponents();
}

TArray<USKGAttachmentComponent*> ASKGFirearm::GetAllAttachmentComponents_Implementation(bool bReCache)
{
	return AttachmentManager->GetAllAttachmentComponents(false);
}

void ASKGFirearm::CycleSights_Implementation(bool bDownArray, bool bStopAtEndOfArray)
{
	if (!bCanCycleSights || PointAimIndex > INDEX_NONE)
	{
		return;
	}
	const USKGAttachmentComponent* CurrentComponent = CurrentSightComponent;
	USKGAttachmentComponent* NewSightComponent = nullptr;
	
	bool bFoundValidSight = false;
	TArray<USKGAttachmentComponent*> PartComponents = CachedComponents.Sights;
	//UE_LOG(LogTemp, Warning, TEXT("CycleSights: %s"), *PartComponents[0]->GetPart()->GetName());
	if (GetAllAttachmentComponents_Implementation(false).Num())
	{
		if (bDownArray)
		{
			for (uint8 i = SightComponentIndex; i < PartComponents.Num(); ++i)
			{
				if (USKGAttachmentComponent* SightComponent = PartComponents[i])
				{
					if (CurrentSightComponent != SightComponent)
					{
						AActor* AimingPart = SightComponent->GetAttachment();
						if (IsValid(AimingPart) && AimingPart->GetClass()->ImplementsInterface(USKGAimInterface::StaticClass()))
						{
							if (ISKGAimInterface::Execute_IsAimable(AimingPart))
							{
								NewSightComponent = SightComponent;
								bFoundValidSight = true;
								SightComponentIndex = i;
								break;
							}
						}
					}
				}
			}
			if (!bFoundValidSight && !bStopAtEndOfArray)
			{
				for (uint8 i = 0; i < PartComponents.Num(); ++i)
				{
					if (USKGAttachmentComponent* SightComponent = PartComponents[i])
					{
						if (CurrentSightComponent != SightComponent)
						{
							AActor* AimingPart = SightComponent->GetAttachment();
							if (IsValid(AimingPart) && AimingPart->GetClass()->ImplementsInterface(USKGAimInterface::StaticClass()))
							{
								if (ISKGAimInterface::Execute_IsAimable(AimingPart))
								{
									NewSightComponent = SightComponent;
									bFoundValidSight = true;
									SightComponentIndex = i;
									break;
								}
							}
						}
					}
				}
			}
		}
		else if (PartComponents.Num())
		{
			for (int16 i = SightComponentIndex - 1; i > -1; --i)
			{
				if (USKGAttachmentComponent* SightComponent = PartComponents[i])
				{
					if (CurrentSightComponent != SightComponent)
					{
						AActor* AimingPart = SightComponent->GetAttachment();
						if (IsValid(AimingPart) && AimingPart->GetClass()->ImplementsInterface(USKGAimInterface::StaticClass()))
						{
							if (ISKGAimInterface::Execute_IsAimable(AimingPart))
							{
								NewSightComponent = SightComponent;
								bFoundValidSight = true;
								SightComponentIndex = i;
								break;
							}
						}
					}
				}
			}
			if (!bFoundValidSight && !bStopAtEndOfArray)
			{
				for (uint8 i = PartComponents.Num() - 1; i > -1; --i)
				{
					if (USKGAttachmentComponent* SightComponent = PartComponents[i])
					{
						if (CurrentSightComponent != SightComponent)
						{
							AActor* AimingPart = SightComponent->GetAttachment();
							if (IsValid(AimingPart) && AimingPart->GetClass()->ImplementsInterface(USKGAimInterface::StaticClass()))
							{
								if (ISKGAimInterface::Execute_IsAimable(AimingPart))
								{
									NewSightComponent = SightComponent;
									bFoundValidSight = true;
									SightComponentIndex = i;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	if (NewSightComponent)
	{
		Execute_ActivateCurrentSight(this, false);
		CurrentSightComponent = NewSightComponent;
	}
	
	if (bFoundValidSight)
	{
		if (!HasAuthority())
		{
			Server_CycleSights(CurrentSightComponent);
		}
	}
	
	if (GetCharacterComponent())
	{
		if (USKGCharacterAnimInstance* AnimInstance = CharacterComponent->GetAnimationInstance())
		{
			AnimInstance->CycledSights();
		}
	}

	if (GetCharacterComponent())
	{
		Execute_ActivateCurrentSight(this, GetCharacterComponent()->IsAiming());
	}
}

void ASKGFirearm::RefreshCurrentSight()
{
	if (!bCanCycleSights)
	{
		return;
	}
	
	if (CurrentSightComponent)
	{
		AActor* Sight = CurrentSightComponent->GetAttachment();
		if (IsValid(Sight) && Sight->GetClass()->ImplementsInterface(USKGAimInterface::StaticClass()))
		{
				if (ISKGAimInterface::Execute_IsAimable(Sight))
				{
					if (!HasAuthority())
					{
						Server_CycleSights(CurrentSightComponent);
					}
					if (GetCharacterComponent())
					{
						if (USKGCharacterAnimInstance* AnimInstance = CharacterComponent->GetAnimationInstance())
						{
							AnimInstance->CycledSights();
						}
					}
				}
		}
		else
		{
			Execute_CycleSights(this, true, false);
		}
	}
	else
	{
		Execute_CycleSights(this, true, false);
	}

	if (GetCharacterComponent())
	{
		Execute_ActivateCurrentSight(this, GetCharacterComponent()->IsAiming());
	}
}

void ASKGFirearm::SetSight_Implementation(USKGAttachmentComponent* SightComponent)
{
	const TArray<USKGAttachmentComponent*> PartComponents = CachedComponents.Sights;
	if (IsValid(SightComponent) && PartComponents.Contains(SightComponent))
	{
		CurrentSightComponent = SightComponent;
		RefreshCurrentSight();
	}
}

bool ASKGFirearm::Server_SetHidden_Validate(bool Hide)
{
	return true;
}

void ASKGFirearm::Server_SetHidden_Implementation(bool Hide)
{
	HideFirearm(Hide);
}

void ASKGFirearm::HideFirearm(bool Hide)
{
	if (Hide != bFirearmHidden)
	{
		bFirearmHidden = Hide;
		OnRep_FirearmHidden();
		if (!HasAuthority())
		{
			Server_SetHidden(Hide);
		}
	}
}

#pragma region Attachment
/*bool AFPSTemplateFirearm::Server_AttachToSocket_Validate(const FName& Socket)
{
	return true;
}

void AFPSTemplateFirearm::Server_AttachToSocket_Implementation(const FName& Socket)
{
	AttachToSocket(Socket);
}

void AFPSTemplateFirearm::OnRep_AttachedToSocket()
{
	if (AttachedToSocket == NAME_None)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		return;
	}
	if (GetCharacterComponent())
	{
		CharacterComponent->AttachItem(this, AttachedToSocket);
		OnAttachedToSocket(AttachedToSocket);
	}
}

void AFPSTemplateFirearm::AttachToSocket(const FName Socket)
{
	AttachedToSocket = Socket;
	OnRep_AttachedToSocket();
	if (!HasAuthority())
	{
		Server_AttachToSocket(Socket);
	}
}*/
#pragma endregion Attachment

void ASKGFirearm::DestroyAllParts()
{
	if (IsValid(AttachmentManager))
	{
		AttachmentManager->DestroyAllAttachments();
	}
}

float ASKGFirearm::GetCycleRate(bool bGetDefault)
{
	if (bGetDefault)
	{
		return TimerAutoFireRate;
	}
	return TimerAutoFireRate * FirearmStats.CycleRateMultiplier;
}

ASKGSight* ASKGFirearm::GetCurrentSight_Implementation() const
{
	if (CurrentSightComponent)
	{
		return CurrentSightComponent->GetAttachment<ASKGSight>();
	}
	return nullptr;
}

AActor* ASKGFirearm::GetCurrentSightActor() const
{
	if (CurrentSightComponent)
	{
		return CurrentSightComponent->GetAttachment();
	}
	return nullptr;
}