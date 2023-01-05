// Copyright 2021, Dakota Dawe, All rights reserved

#include "Actors/FirearmParts//SKGSight.h"
#include "Actors/SKGFirearm.h"
#include "Camera/CameraComponent.h"
#include "Actors/FirearmParts//SKGMagnifier.h"
#include "Components/SKGSceneCaptureOptic.h"
#include "SKGSceneCaptureDataTypes.h"
#include "Components/SKGCharacterComponent.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Net/UnrealNetwork.h"

#define OpticLocation FName("OpticLocation")
#define DistanceFromOptic FName("DistanceFromOptic")
#define OpticRadius FName("OpticRadius")

ASKGSight::ASKGSight()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	NetUpdateFrequency = 1.0f;
	
	PartStats.Weight = 0.1f;
	PartStats.ErgonomicsChangePercentage = -2.0f;
	CameraSettings.CameraFOVZoom = 10.0f;
	CameraSettings.CameraFOVZoomSpeed = 10.0f;
	CameraSettings.CameraDistance = 0.0f;
	CameraSettings.bUsedFixedCameraDistance = false;

	bIsAimable = true;
	AimSocket = FName("S_Aim");

	PartType = ESKGPartType::Sight;

	DotZero = FSKGSightZero();

	ReticleIndex = 0;
	ReticleBrightnessIndex = 0;

	AnimationIndex = -1;
	AimInterpolationMultiplier = 1.0f;
	RotationLagInterpolationMultiplier = 50.0f;
	GripSocket = FName("cc_FirearmGrip");
	HeadAimRotation = FRotator(0.0f, 0.0f, 0.0f);

	bCanBeZeroed = true;
	bStartWithRandomZero = false;
	RandomMaxElevationStartClicks = 25;
	RandomMaxWindageStartClicks = 25;
	
	MilliradianAdjustment = 0.005625f;
	MOAAdjustment = 0.004167f;

	RedDotZeroAmount = 0.0005f;
	CurrentDotElevation = 0.0f;
	CurrentDotWindage = 0.0f;
	StartingDotElevation = 0.0f;
	StartingDotWindage = 0.0f;
}

// Called when the game starts or when spawned
void ASKGSight::BeginPlay()
{
	Super::BeginPlay();
	
	ReticleBrightnessIndex = ReticleSettings.ReticleBrightness.ReticleDefaultBrightnessIndex;
	if (ReticleBrightnessIndex > ReticleSettings.ReticleBrightness.ReticleBrightnessSettings.Num())
	{
		ReticleBrightnessIndex = ReticleSettings.ReticleBrightness.ReticleBrightnessSettings.Num();
		ReticleSettings.ReticleBrightness.ReticleDefaultBrightnessIndex = ReticleBrightnessIndex;
	}

	for (FSKGReticleMaterial& Reticle : ReticleSettings.ReticleMaterials)
	{
		if (Reticle.RenderTargetMaterial.Material)
		{
			Reticle.RenderTargetMaterial.DynamicMaterial = UMaterialInstanceDynamic::Create(Reticle.RenderTargetMaterial.Material, this);
			Reticle.RenderTargetMaterial.DynamicMaterial->GetScalarParameterValue(FName("ReticleSize"), Reticle.ReticleSize);
		}
	}

	if (!IsValid(SceneCapture))
	{
		SceneCapture = FindComponentByClass<USKGSceneCaptureOptic>();
		if (SceneCapture)
		{
			bHasRenderTarget = true;
			SceneCapture->SetHiddenInGame(false);
			SceneCapture->SetReticleSettings(ReticleSettings);
			SceneCapture->SetOwningMesh(AttachmentMesh.Get());
			Execute_DisableRenderTarget(this, true);
		}
	}

	Execute_SetReticleBrightness(this, ReticleBrightnessIndex);
	Execute_SetReticle(this, ReticleIndex);
	
	if (bStartWithRandomZero)
	{
		SetRandomZero();
	}

	if (MPC)
	{
		MPCInstance = GetWorld()->GetParameterCollectionInstance(MPC);
		if (MPCInstance.IsValid())
		{
			CacheCharacterAndFirearm();
			if (OwningCharacterComponent.IsValid() && OwningCharacterComponent->IsLocallyControlled())
			{
				//SetActorTickEnabled(true);
			}
		}
	}
}

void ASKGSight::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MPCInstance.IsValid() && OwningCharacterComponent.IsValid() && OwningCharacterComponent->GetCurrentSight() == this)
	{
		MPCInstance->SetVectorParameterValue(OpticLocation, GetAimSocketTransform().GetLocation());
		const float DistanceToOptic = FVector::Distance(GetAimSocketTransform().GetLocation(), OwningCharacterComponent->GetCameraComponent()->GetComponentLocation());
		MPCInstance->SetScalarParameterValue(DistanceFromOptic, DistanceToOptic);
		MPCInstance->SetScalarParameterValue(OpticRadius, ReticleSettings.Radius);
		//UE_LOG(LogTemp, Warning, TEXT("DistFromOptic: %f   Optic: %s"), DistanceToOptic, *GetName());
	}
}

void ASKGSight::SetRandomZero()
{
	const uint8 ElevationStart = FMath::RandRange(0, RandomMaxElevationStartClicks);
	const uint8 WindageStart = FMath::RandRange(0, RandomMaxWindageStartClicks);
	if (FMath::RandBool())
	{
		MovePointOfImpact(ESKGElevationWindage::Elevation, ESKGRotationDirection::CounterClockWise, ElevationStart);
	}
	else
	{
		MovePointOfImpact(ESKGElevationWindage::Elevation, ESKGRotationDirection::ClockWise, ElevationStart);
	}
	if (FMath::RandBool())
	{
		MovePointOfImpact(ESKGElevationWindage::Windage, ESKGRotationDirection::ClockWise, ElevationStart);
	}
	else
	{
		MovePointOfImpact(ESKGElevationWindage::Windage, ESKGRotationDirection::CounterClockWise, ElevationStart);
	}
}

void ASKGSight::SetReticleBrightness_Implementation(uint8 Index)
{
	if (IsValid(SceneCapture))
	{
		SceneCapture->SetReticleBrightness(Index);
		return;
	}
	if (Index < ReticleSettings.ReticleBrightness.ReticleBrightnessSettings.Num())
	{
		for (const FSKGReticleMaterial& Reticle : ReticleSettings.ReticleMaterials)
		{
			if (Reticle.RenderTargetMaterial.DynamicMaterial)
			{
				Reticle.RenderTargetMaterial.DynamicMaterial->SetScalarParameterValue(FName("ReticleBrightness"), ReticleSettings.ReticleBrightness.ReticleBrightnessSettings[Index]);
			}
		}
	}
}

void ASKGSight::ReturnToZeroElevation_Implementation()
{
	if (bCanBeZeroed)
	{
		if (IsValid(SceneCapture))
		{
			SceneCapture->ReturnToZeroElevation();
		}
		else
		{
			DotZero.Elevation = 0.0f;
			CurrentDotElevation = StartingDotElevation;
			for (const FSKGReticleMaterial& Reticle : ReticleSettings.ReticleMaterials)
			{
				if (Reticle.RenderTargetMaterial.DynamicMaterial)
				{
					Reticle.RenderTargetMaterial.DynamicMaterial->SetVectorParameterValue(ReticleZeroName, FLinearColor(CurrentDotWindage, CurrentDotElevation, 0.0f));
				}
			}
		}
		OnReturnToZeroElevation();
	}
}

void ASKGSight::ReturnToZeroWindage_Implementation()
{
	if (bCanBeZeroed)
	{
		if (IsValid(SceneCapture))
		{
			SceneCapture->ReturnToZeroWindage();
		}
		else
		{
			DotZero.Windage = 0.0f;
			CurrentDotWindage = StartingDotWindage;
			for (const FSKGReticleMaterial& Reticle : ReticleSettings.ReticleMaterials)
			{
				if (Reticle.RenderTargetMaterial.DynamicMaterial)
				{
					Reticle.RenderTargetMaterial.DynamicMaterial->SetVectorParameterValue(ReticleZeroName, FLinearColor(CurrentDotWindage, CurrentDotElevation, 0.0f));
				}
			}
		}
		OnReturnToZeroWindage();
	}
}

void ASKGSight::SetNewZeroElevation_Implementation()
{
	if (bCanBeZeroed)
	{
		if (IsValid(SceneCapture))
		{
			SceneCapture->SetNewZeroElevation();
		}
		else
		{
			DotZero.Elevation = 0.0f;
			StartingDotElevation = CurrentDotElevation;
		}
		OnReturnToZeroElevation();
	}
}

void ASKGSight::SetNewZeroWindage_Implementation()
{
	if (bCanBeZeroed)
	{
		if (IsValid(SceneCapture))
		{
			SceneCapture->SetNewZeroWindage();
		}
		else
		{
			DotZero.Windage = 0.0f;
			StartingDotWindage = CurrentDotWindage;
		}
		OnReturnToZeroWindage();
	}
}

void ASKGSight::PointOfImpactUp(bool bUp, uint8 Clicks)
{
	if (bCanBeZeroed)
	{
		FVector2D AdjustmentMade;
		if (IsValid(SceneCapture))
		{
			AdjustmentMade = SceneCapture->PointOfImpactUp(bUp, Clicks);
		}
		else
		{
			AdjustmentMade.X = CurrentDotElevation;
			if (bUp)
			{
				CurrentDotElevation -= RedDotZeroAmount * Clicks;
				DotZero.Elevation -= RedDotZeroAmount * Clicks;
			}
			else
			{
				CurrentDotElevation += RedDotZeroAmount * Clicks;
				DotZero.Elevation += RedDotZeroAmount * Clicks;
			}
			AdjustmentMade.Y = CurrentDotElevation;
			for (const FSKGReticleMaterial& Reticle : ReticleSettings.ReticleMaterials)
			{
				if (Reticle.RenderTargetMaterial.DynamicMaterial)
				{
					Reticle.RenderTargetMaterial.DynamicMaterial->SetVectorParameterValue(ReticleZeroName, FLinearColor(CurrentDotWindage, CurrentDotElevation, 0.0f));
				}
			}
		}
		OnElevationChanged(AdjustmentMade.X, AdjustmentMade.Y, Clicks);
	}
}

void ASKGSight::PointOfImpactLeft(bool bLeft, uint8 Clicks)
{
	if (bCanBeZeroed)
	{
		FVector2D AdjustmentMade;
		if (IsValid(SceneCapture))
		{
			AdjustmentMade = SceneCapture->PointOfImpactLeft(bLeft, Clicks);
		}
		else
		{
			AdjustmentMade.X = CurrentDotWindage;
			if (bLeft)
			{
				CurrentDotWindage -= RedDotZeroAmount * Clicks;
				DotZero.Windage -= RedDotZeroAmount * Clicks;
			}
			else
			{
				CurrentDotWindage += RedDotZeroAmount * Clicks;
				DotZero.Windage += RedDotZeroAmount * Clicks;
			}
			AdjustmentMade.Y = CurrentDotWindage;
			for (const FSKGReticleMaterial& Reticle : ReticleSettings.ReticleMaterials)
			{
				if (Reticle.RenderTargetMaterial.DynamicMaterial)
				{
					Reticle.RenderTargetMaterial.DynamicMaterial->SetVectorParameterValue(ReticleZeroName, FLinearColor(CurrentDotWindage, CurrentDotElevation, 0.0f));
				}
			}
		}
		OnWindageChanged(AdjustmentMade.X, AdjustmentMade.Y, Clicks);
	}
}

void ASKGSight::MovePointOfImpact_Implementation(ESKGElevationWindage Turret, ESKGRotationDirection Direction, uint8 Clicks)
{
	switch (Turret)
	{
	case ESKGElevationWindage::Elevation :
		{
			if (Direction == ESKGRotationDirection::CounterClockWise)
			{
				PointOfImpactUp(true, Clicks);
			}
			else
			{
				PointOfImpactUp(false, Clicks);
			}
			break;
		}
	case ESKGElevationWindage::Windage :
		{
			if (Direction == ESKGRotationDirection::CounterClockWise)
			{
				PointOfImpactLeft(true, Clicks);
			}
			else
			{
				PointOfImpactLeft(false, Clicks);
			}
			break;
		}
	case ESKGElevationWindage::Both :
		{
			if (Direction == ESKGRotationDirection::CounterClockWise)
			{
				PointOfImpactUp(true, Clicks);
				PointOfImpactLeft(true, Clicks);
			}
			else
			{
				PointOfImpactUp(false, Clicks);
				PointOfImpactLeft(false, Clicks);
			}
		}
	}
}

FSKGSightZero ASKGSight::GetSightZero_Implementation() const
{
	if (IsValid(SceneCapture))
	{
		return SceneCapture->GetSightZero();
	}
	return DotZero;
}

FTransform ASKGSight::GetAimSocketTransform_Implementation()
{
	if (IsValid(Magnifier) && Magnifier->IsFullyFlipped() && !Magnifier->IsFlippedOut() && Magnifier->GetSightInfront() == this)
	{
		return Execute_GetAimSocketTransform(Magnifier);
	}
	return AttachmentMesh.IsValid() ? AttachmentMesh->GetSocketTransform(AimSocket) : FTransform();
}

void ASKGSight::GetCameraSettings_Implementation(FSKGAimCameraSettings& OutCameraSettings)
{
	if (IsValid(Magnifier) && !Magnifier->IsFlippedOut() && Magnifier->GetSightInfront() == this)
	{
		if (Magnifier->Implements<USKGAimInterface>())
		{
			Execute_GetCameraSettings(Magnifier, OutCameraSettings);
			return;
		}
	}
	OutCameraSettings = CameraSettings;
}

void ASKGSight::ZoomOptic_Implementation(bool bZoom)
{
	if (IsValid(SceneCapture))
	{
		if (bZoom)
		{
			SceneCapture->ZoomIn();
		}
		else
		{
			SceneCapture->ZoomOut();
		}
	}
}

void ASKGSight::DisableRenderTarget_Implementation(bool Disable)
{
	if (IsValid(Magnifier) && Magnifier->GetClass()->ImplementsInterface(USKGRenderTargetInterface::StaticClass()) && !Magnifier->IsFlippedOut() && Magnifier->GetSightInfront() == this)
	{
		ISKGRenderTargetInterface::Execute_DisableRenderTarget(Magnifier, Disable);
		return;
	}
	if (IsValid(SceneCapture))
	{
		if (!Disable)
		{
			SceneCapture->StartCapture();
		}
		else
		{
			SceneCapture->StopCapture();
		}
	}
}

float ASKGSight::GetMagnification() const
{
	if (IsValid(Magnifier) && !Magnifier->IsFlippedOut() && Magnifier->GetSightInfront() == this)
	{
		return Magnifier->GetMagnification();
	}
	if (IsValid(SceneCapture))
	{
		return SceneCapture->GetMagnification();
	}
	return 1.0f;
}

void ASKGSight::CycleReticle_Implementation()
{
	if (IsValid(SceneCapture))
	{
		SceneCapture->CycleReticle();
		return;
	}
	if (ReticleSettings.ReticleMaterials.Num())
	{
		if (++ReticleIndex >= ReticleSettings.ReticleMaterials.Num())
		{
			ReticleIndex = 0;
		}
		Execute_SetReticle(this, ReticleIndex);
	}
}

void ASKGSight::SetReticle_Implementation(uint8 Index)
{
	if (IsValid(SceneCapture))
	{
		SceneCapture->SetReticle(Index);
		return;
	}
	if (AttachmentMesh.IsValid() && Index < ReticleSettings.ReticleMaterials.Num() && ReticleSettings.ReticleMaterials[Index].RenderTargetMaterial.DynamicMaterial)
	{
		ReticleIndex = Index;
		AttachmentMesh->SetMaterial(ReticleSettings.ReticleMaterialIndex, ReticleSettings.ReticleMaterials[ReticleIndex].RenderTargetMaterial.DynamicMaterial);
	}
}

void ASKGSight::IncreaseBrightness_Implementation()
{
	if (++ReticleBrightnessIndex < ReticleSettings.ReticleBrightness.ReticleBrightnessSettings.Num())
	{
		Execute_SetReticleBrightness(this, ReticleBrightnessIndex);
	}
	else
	{
		ReticleBrightnessIndex = ReticleSettings.ReticleBrightness.ReticleBrightnessSettings.Num() - 1;
	}
}

void ASKGSight::DecreaseBrightness_Implementation()
{
	if (ReticleBrightnessIndex - 1 >= 0)
	{
		--ReticleBrightnessIndex;
		Execute_SetReticleBrightness(this, ReticleBrightnessIndex);
	}
	else
	{
		ReticleBrightnessIndex = 0;
	}
}