// Copyright 2021, Dakota Dawe, All rights reserved


#include "Actors/FirearmParts/SKGLightLaser.h"

#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SKGCharacterComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"
#include "Net/UnrealNetwork.h"

ASKGLightLaser::ASKGLightLaser()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	NetUpdateFrequency = 6.5f;
	MinNetUpdateFrequency = 1.0f;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = Root;
	
	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent"));
	SpotLightComponent->SetVisibility(false);
	
	LaserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMeshComponent"));
	LaserMesh->CastShadow = false;
	LaserMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LaserDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	LaserDecalComponent->SetupAttachment(RootComponent);
	LaserDecalComponent->SetVisibility(false);
	LaserDecalComponent->DecalSize = FVector(3.0f, 3.0f, 3.0f);
	LaserDecalComponent->FadeScreenSize = 0.0f;

	SpotLightComponent->bUseInverseSquaredFalloff = false;
	SpotLightComponent->Intensity = 2.0f;
	SpotLightComponent->AttenuationRadius = 50000.0f;
	SpotLightComponent->InnerConeAngle = 10.0f;
	SpotLightComponent->OuterConeAngle = 25.0;
	
	LaserSocket = FName("S_Laser");
	AimSocket = FName("S_Aim");

	PartType = ESKGPartType::LightLaser;
	LightLaserState = ESKGLightLaserState::Off;
	bInfraredMode = false;
	LightLaserModeCycleIndex = 0;
}

void ASKGLightLaser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASKGLightLaser, LightLaserState, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASKGLightLaser, LightLaserSettings, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASKGLightLaser, bInfraredMode, COND_SkipOwner);
}

void ASKGLightLaser::PostInitProperties()
{
	Super::PostInitProperties();
}

void ASKGLightLaser::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Execute_SetLightLaserState(this, ESKGLightLaserState::Off);
	}

	if (LightLaserSettings.bSupportsInfrared)
	{
		if (const ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			CharacterComponent = Character->FindComponentByClass<USKGCharacterComponent>();
			if (CharacterComponent.IsValid())
			{
				CharacterComponent->AddInfraredDevice(this);
			}
		}
	}
}

void ASKGLightLaser::SetupAttachmentMesh()
{
	Super::SetupAttachmentMesh();
	if (AttachmentMesh.IsValid())
	{
		if (IsValid(LaserMesh) && LaserMesh->GetStaticMesh())
		{
			if (UMaterialInterface* LaserMaterial = LightLaserSettings.LaserSettings.GetCurrentColor().LaserMaterial)
			{
				LaserMesh->AttachToComponent(AttachmentMesh.Get(), FAttachmentTransformRules::KeepRelativeTransform);
				LaserMesh->SetVisibility(false);
				LaserMesh->SetMaterial(LightLaserSettings.LaserMaterialIndex, LaserMaterial);

				if (IsValid(LaserDecalComponent))
				{
					if (UMaterialInterface* LaserDotMaterial = LightLaserSettings.LaserSettings.GetCurrentColor().LaserDotMaterial)
					{
						LaserDecalComponent->AttachToComponent(AttachmentMesh.Get(), FAttachmentTransformRules::KeepRelativeTransform);
						LaserDecalComponent->SetVisibility(false);
						LaserDecalComponent->SetMaterial(0, LaserDotMaterial);
					}
				}
			}
		}
		if (IsValid(SpotLightComponent))
		{
			SpotLightComponent->AttachToComponent(AttachmentMesh.Get(), FAttachmentTransformRules::KeepRelativeTransform);
			SpotLightComponent->SetVisibility(false);
			SpotLightComponent->SetIntensity(LightLaserSettings.LightSettings.GetCurrentIntensity());
			OnLightPowerChanged(SpotLightComponent->Intensity);
			OnLightToggled(false);
		}
	}
}

void ASKGLightLaser::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if ((LightLaserState == ESKGLightLaserState::Laser || LightLaserState == ESKGLightLaserState::Both) && AttachmentMesh.IsValid())
	{
		if (AttachmentMesh->DoesSocketExist(LaserSocket))
		{
			FVector Start = AttachmentMesh->GetSocketLocation(LaserSocket);
			FRotator Rot = AttachmentMesh->GetSocketRotation(LaserSocket);
			FVector End = Start + Rot.Vector() * LightLaserSettings.MaxLaserDistance;
        
			FHitResult HitResult;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);
    
			if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, LightLaserSettings.LaserCollisionChannel, Params))
			{
				if (LaserDecalComponent)
				{
					LaserDecalComponent->SetVisibility(true);
					FRotator NormalizedRotator = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);
					LaserDecalComponent->SetWorldLocationAndRotation(HitResult.Location, NormalizedRotator);
					float ScaleAmount = HitResult.Distance / 10000.0f * LightLaserSettings.LaserDistanceScaleMultiplier;
					ScaleAmount = FMath::Clamp(ScaleAmount, 3.0f, 25.0f);

					LaserDecalComponent->DecalSize = FVector(ScaleAmount);
				}
				LaserMesh->SetWorldScale3D(FVector(1.0f, HitResult.Distance / 20.0f, 1.0f));
			}
			else
			{
				if (LaserDecalComponent)
				{
					LaserDecalComponent->SetVisibility(false);
				}
			}
		}
	}
}

void ASKGLightLaser::OnRep_LightLaserState()
{
	if (bInfraredMode && !DoIHaveNightVisionOn())
	{
		SetLightOn(false);
		SetLaserOn(false);
		return;
	}
	switch (LightLaserState)
	{
	case ESKGLightLaserState::Light :
		{
			SetLightOn(true);
			SetLaserOn(false);
			OnLightToggled(true);
			break;
		}
	case ESKGLightLaserState::Laser :
		{
			SetLaserOn(true);
			SetLightOn(false);
			OnLightToggled(false);
			break;
		}
	case ESKGLightLaserState::Both :
		{
			SetLightOn(true);
			SetLaserOn(true);
			OnLightToggled(true);
			break;
		}
	case ESKGLightLaserState::Off :
		{
			SetLightOn(false);
			SetLaserOn(false);
			OnLightToggled(false);
			break;
		}
	}
}

bool ASKGLightLaser::DoIHaveNightVisionOn() const
{
	return CharacterComponent.IsValid() ? CharacterComponent.Get()->NightVisionOn() : false;
}

void ASKGLightLaser::SetLightOn(bool bOn)
{
	if (IsValid(SpotLightComponent))
	{
		SpotLightComponent->SetVisibility(bOn);
	}
}

void ASKGLightLaser::SetLaserOn(bool bOn)
{
	if (IsValid(LaserMesh) && LaserMesh->GetStaticMesh())
	{
		LaserMesh->SetVisibility(bOn);
		if (IsValid(LaserDecalComponent))
		{
			LaserDecalComponent->SetVisibility(bOn);
		}
		SetActorTickEnabled(bOn);
	}
}

bool ASKGLightLaser::Server_SetLightLaserState_Validate(ESKGLightLaserState State)
{
	return true;
}

void ASKGLightLaser::Server_SetLightLaserState_Implementation(ESKGLightLaserState State)
{
	Execute_SetLightLaserState(this, State);
}

void ASKGLightLaser::SetLightLaserState_Implementation(const ESKGLightLaserState State)
{
	if (LightLaserState != State)
	{
		if (State == ESKGLightLaserState::Off && LightLaserState != ESKGLightLaserState::Off)
		{
			PreviousBeforeOffLightLaserState = LightLaserState;
		}
		LightLaserState = State;
		OnRep_LightLaserState();
		
		if (!HasAuthority())
		{
			Server_SetLightLaserState(LightLaserState);
		}
	}
}

void ASKGLightLaser::RestorePreviousBeforeOffState_Implementation()
{
	if (LightLaserState != PreviousBeforeOffLightLaserState)
	{
		LightLaserState = PreviousBeforeOffLightLaserState;
		OnRep_LightLaserState();
		
		if (!HasAuthority())
		{
			Server_SetLightLaserState(LightLaserState);
		}
	}
}

void ASKGLightLaser::ToggleLight_Implementation()
{
	switch (LightLaserState)
	{
	case ESKGLightLaserState::Light :
		{
			LightLaserState = ESKGLightLaserState::Off;
			break;
		}
	case ESKGLightLaserState::Laser :
		{
			LightLaserState = ESKGLightLaserState::Both;
			break;
		}
	case ESKGLightLaserState::Both :
		{
			LightLaserState = ESKGLightLaserState::Laser;
			break;
		}
	case ESKGLightLaserState::Off :
		{
			LightLaserState = ESKGLightLaserState::Light;
			break;
		}
	}
	
	OnRep_LightLaserState();
		
	if (!HasAuthority())
	{
		Server_SetLightLaserState(LightLaserState);
	}
}

void ASKGLightLaser::ToggleLaser_Implementation()
{
	if (IsValid(LaserMesh) && LaserMesh->GetStaticMesh())
	{
		switch (LightLaserState)
		{
		case ESKGLightLaserState::Light :
			{
				LightLaserState = ESKGLightLaserState::Both;
				break;
			}
		case ESKGLightLaserState::Laser :
			{
				LightLaserState = ESKGLightLaserState::Off;
				break;
			}
		case ESKGLightLaserState::Both :
			{
				LightLaserState = ESKGLightLaserState::Light;
				break;
			}
		case ESKGLightLaserState::Off :
			{
				LightLaserState = ESKGLightLaserState::Laser;
				break;
			}
		}
	
		OnRep_LightLaserState();
		
		if (!HasAuthority())
		{
			Server_SetLightLaserState(LightLaserState);
		}
	}
}

void ASKGLightLaser::CycleLightLaserModes_Implementation(const bool bDownArray, const uint8 ForceIndex)
{
	const uint8 ModeCount = LightLaserModeCycle.Num();
	if (ModeCount)
	{
		if (ForceIndex < ModeCount)
		{
			LightLaserModeCycleIndex = ForceIndex;
		}
		else
		{
			if (bDownArray)
			{
				if (++LightLaserModeCycleIndex >= ModeCount)
				{
					LightLaserModeCycleIndex = 0;
				}
			}
			else
			{
				if (LightLaserModeCycleIndex - 1 < 0)
				{
					LightLaserModeCycleIndex = ModeCount - 1;
				}
				else
				{
					--LightLaserModeCycleIndex;
				}
			}
		}
		
		switch (LightLaserModeCycle[LightLaserModeCycleIndex])
		{
		case ESKGAllLightLaser::Light :
			{
				if (bInfraredMode)
				{
					Execute_SetInfraredMode(this, false);
				}
				Execute_SetLightLaserState(this, ESKGLightLaserState::Light);
				break;
			}
		case ESKGAllLightLaser::Laser :
			{
				if (bInfraredMode)
				{
					Execute_SetInfraredMode(this, false);
				}
				Execute_SetLightLaserState(this, ESKGLightLaserState::Laser);
				break;
			}
		case ESKGAllLightLaser::Both :
			{
				if (bInfraredMode)
				{
					Execute_SetInfraredMode(this, false);
				}
				Execute_SetLightLaserState(this, ESKGLightLaserState::Both);
				break;
			}
		case ESKGAllLightLaser::Off :
			{
				Execute_SetLightLaserState(this, ESKGLightLaserState::Off);
				break;
			}
		case ESKGAllLightLaser::IRLight :
			{
				if (!bInfraredMode)
				{
					Execute_SetInfraredMode(this, true);
				}
				Execute_SetLightLaserState(this, ESKGLightLaserState::Light);
				break;
			}
		case ESKGAllLightLaser::IRLaser :
			{
				if (!bInfraredMode)
				{
					Execute_SetInfraredMode(this, true);
				}
				Execute_SetLightLaserState(this, ESKGLightLaserState::Laser);
				break;
			}
		case ESKGAllLightLaser::IRBoth :
			{
				if (!bInfraredMode)
				{
					Execute_SetInfraredMode(this, true);
				}
				Execute_SetLightLaserState(this, ESKGLightLaserState::Both);
				break;
			}
		}
	}
}

bool ASKGLightLaser::Server_SetLaserColorIndex_Validate(uint8 Index)
{
	return true;
}

void ASKGLightLaser::Server_SetLaserColorIndex_Implementation(uint8 Index)
{
	LightLaserSettings.LaserSettings.SetColorIndex(Index);
	OnRep_LightLaserSettings();
}

void ASKGLightLaser::CycleLaserColor_Implementation(bool bDownArray)
{
	if (!bInfraredMode && IsValid(LaserMesh))
	{
		FSKGLaserSettings& LaserSettings = LightLaserSettings.LaserSettings;
		const uint8 LaserMaterialCount = LaserSettings.LaserColors.Num();
		if (LaserMaterialCount)
		{
			const uint8 OldIndex = LaserSettings.LaserColorIndex;
			const uint8 LaserColorIndex = LaserSettings.GetNextIndex(bDownArray);

			if (OldIndex != LaserColorIndex)
			{
				if (IsValid(LaserMesh))
				{
					LaserMesh->SetMaterial(0, LaserSettings.GetCurrentColor().LaserMaterial);
					if (IsValid(LaserDecalComponent))
					{
						LaserDecalComponent->SetMaterial(0, LaserSettings.GetCurrentColor().LaserDotMaterial);
					}
				}
			
				if (!HasAuthority())
				{
					Server_SetLaserColorIndex(LaserSettings.LaserColorIndex);
				}
			}
		}
	}
}

bool ASKGLightLaser::Server_SetLightIntensityIndex_Validate(uint8 Index)
{
	return true;
}

void ASKGLightLaser::Server_SetLightIntensityIndex_Implementation(uint8 Index)
{
	FSKGLightSettings& LightSettings = bInfraredMode ? LightLaserSettings.IRLightSettings : LightLaserSettings.LightSettings;
	LightSettings.SetIntensityIndex(Index);
	OnRep_LightLaserSettings();
}

void ASKGLightLaser::CyclePowerModes_Implementation(bool bDownArray)
{ // check if in IR state and use IR if true
	FSKGLightSettings& LightSettings = bInfraredMode ? LightLaserSettings.IRLightSettings : LightLaserSettings.LightSettings;
	const uint8 LightModeCount = LightSettings.LightIntensityLevels.Num();
	
	if (IsValid(SpotLightComponent) && LightModeCount)
	{
		const uint8 OldIndex = LightSettings.LightIntensityIndex;
		const uint8 LightIntensityIndex = LightSettings.GetNextIndex(bDownArray);
		if (OldIndex != LightIntensityIndex)
		{
			SpotLightComponent->SetIntensity(LightSettings.GetCurrentIntensity());
			if (!bInfraredMode)
			{
				OnLightPowerChanged(SpotLightComponent->Intensity);
			}
			else
			{
				OnLightPowerChanged(0);
			}
			if (!(HasAuthority()))
			{
				Server_SetLightIntensityIndex(LightIntensityIndex);
			}
		}
	}
}

FTransform ASKGLightLaser::GetAimSocketTransform_Implementation()
{
	return AttachmentMesh.IsValid() ? AttachmentMesh->GetSocketTransform(AimSocket) : FTransform();
}

void ASKGLightLaser::HandleIRDevice_Implementation()
{
	OnRep_LightLaserState();
	OnInfraredModeChanged(bInfraredMode, DoIHaveNightVisionOn());
}

void ASKGLightLaser::OnRep_InfraredMode()
{	
	FSKGLightSettings& LightSettings = bInfraredMode ? LightLaserSettings.IRLightSettings : LightLaserSettings.LightSettings;
	FSKGLaserSettings& LaserSettings = bInfraredMode ? LightLaserSettings.IRLaserSettings : LightLaserSettings.LaserSettings;

	if (LaserSettings.LaserColors.Num())
	{
		if (IsValid(LaserMesh))
		{
			LaserMesh->SetMaterial(LightLaserSettings.LaserMaterialIndex, LaserSettings.GetCurrentColor().LaserMaterial);
			if (IsValid(LaserDecalComponent))
			{
				LaserDecalComponent->SetMaterial(0, LaserSettings.GetCurrentColor().LaserDotMaterial);
			}
		}
	}
		
	if (IsValid(SpotLightComponent))
	{
		SpotLightComponent->SetIntensity(LightSettings.GetCurrentIntensity());
		if (!bInfraredMode)
		{
			OnLightPowerChanged(SpotLightComponent->Intensity);
		}
		else
		{
			OnLightPowerChanged(0);
		}
	}

	Execute_HandleIRDevice(this);
}

bool ASKGLightLaser::Server_SetInfraredMode_Validate(bool bInfrared)
{
	return true;
}

void ASKGLightLaser::Server_SetInfraredMode_Implementation(bool bInfrared)
{
	bInfraredMode = bInfrared;
	OnRep_InfraredMode();
}

void ASKGLightLaser::SetInfraredMode_Implementation(bool bInfrared)
{
	const bool bOldMode = bInfraredMode;
	LightLaserSettings.bSupportsInfrared ? bInfraredMode = bInfrared : bInfraredMode = false;
	if (bOldMode != bInfraredMode)
	{		
		FSKGLightSettings& LightSettings = bInfraredMode ? LightLaserSettings.IRLightSettings : LightLaserSettings.LightSettings;
		FSKGLaserSettings& LaserSettings = bInfraredMode ? LightLaserSettings.IRLaserSettings : LightLaserSettings.LaserSettings;

		if (LaserSettings.LaserColors.Num())
		{
			if (IsValid(LaserMesh))
			{
				LaserMesh->SetMaterial(LightLaserSettings.LaserMaterialIndex, LaserSettings.GetCurrentColor().LaserMaterial);
				if (IsValid(LaserDecalComponent))
				{
					LaserDecalComponent->SetMaterial(0, LaserSettings.GetCurrentColor().LaserDotMaterial);
				}
			}
		}
		
		if (IsValid(SpotLightComponent))
		{
			SpotLightComponent->SetIntensity(LightSettings.GetCurrentIntensity());
			if (!bInfraredMode)
			{
				OnLightPowerChanged(SpotLightComponent->Intensity);
			}
			else
			{
				OnLightPowerChanged(0);
			}
		}

		Execute_HandleIRDevice(this);
		
		if (!HasAuthority())
		{
			Server_SetInfraredMode(bInfraredMode);
		}
	}
}

void ASKGLightLaser::OnRep_LightLaserSettings()
{
	FSKGLightSettings& LightSettings = bInfraredMode ? LightLaserSettings.IRLightSettings : LightLaserSettings.LightSettings;
	FSKGLaserSettings& LaserSettings = bInfraredMode ? LightLaserSettings.IRLaserSettings : LightLaserSettings.LaserSettings;

	if (IsValid(LaserMesh) && LaserMesh->GetMaterial(LightLaserSettings.LaserMaterialIndex) != LaserSettings.GetCurrentColor().LaserMaterial)
	{
		LaserMesh->SetMaterial(LightLaserSettings.LaserMaterialIndex, LaserSettings.GetCurrentColor().LaserMaterial);
		if (IsValid(LaserDecalComponent) && LaserDecalComponent->GetMaterial(0) != LaserSettings.GetCurrentColor().LaserDotMaterial)
		{
			LaserDecalComponent->SetMaterial(0, LaserSettings.GetCurrentColor().LaserDotMaterial);
		}
	}

	if (IsValid(SpotLightComponent))
	{
		SpotLightComponent->SetIntensity(LightSettings.GetCurrentIntensity());
		if (!bInfraredMode)
		{
			OnLightPowerChanged(SpotLightComponent->Intensity);
		}
		else
		{
			OnLightPowerChanged(0);
		}
	}
}