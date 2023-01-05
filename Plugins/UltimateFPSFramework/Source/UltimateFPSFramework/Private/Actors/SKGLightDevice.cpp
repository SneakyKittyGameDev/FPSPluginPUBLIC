// Copyright 2022, Dakota Dawe, All rights reserved


#include "Actors/SKGLightDevice.h"
#include "Components/SKGCharacterComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/LightComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASKGLightDevice::ASKGLightDevice()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	NetUpdateFrequency = 1.5f;
	MinNetUpdateFrequency = 0.5f;
	
	LightModeIndex = 0;
	bInfraredMode = false;
}

void ASKGLightDevice::SetLightsOn(bool bOn)
{
	if (bInfraredMode && (CharacterComponent.IsValid() && !CharacterComponent->NightVisionOn()))
	{
		bOn = false;
	}
	
	for (TWeakObjectPtr<ULightComponent> LightComponent : LightComponents)
	{
		if (LightComponent.IsValid())
		{
			LightComponent->SetVisibility(bOn);
		}
	}
}

// Called when the game starts or when spawned
void ASKGLightDevice::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickInterval(LightDeviceSettings.StrobeInterval);
	
	SetupCharacterComponent();
	SetupLightComponents();
}

void ASKGLightDevice::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (LightMode == ESKGLightMode::Strobe && LightComponents.Num())
	{
		const TWeakObjectPtr<ULightComponent> LightComponent = LightComponents[0];
		if (LightComponent.IsValid())
		{
			SetLightsOn(!LightComponent->IsVisible());
		}
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

void ASKGLightDevice::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASKGLightDevice, LightMode, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ASKGLightDevice, bInfraredMode, COND_SkipOwner);
}

void ASKGLightDevice::SetupCharacterComponent()
{
	if (LightDeviceSettings.bSupportsInfrared)
	{
		if (const APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			CharacterComponent = Pawn->FindComponentByClass<USKGCharacterComponent>();
			if (CharacterComponent.IsValid())
			{
				CharacterComponent->AddInfraredDevice(this);
			}
		}
	}
}

void ASKGLightDevice::SetupLightComponents()
{
	TArray<ULightComponent*> LightComponentList;
	GetComponents<ULightComponent>(LightComponentList);
	if (LightComponentList.Num())
	{
		LightComponents.Reserve(LightComponentList.Num());
		for (ULightComponent* LightComponent : LightComponentList)
		{
			LightComponents.Add(LightComponent);
		}
	}
	CycleModes(true, LightModeIndex);
}

bool ASKGLightDevice::DoIHaveNightVisionOn() const
{
	return CharacterComponent.IsValid() ? CharacterComponent.Get()->NightVisionOn() : false;
}

void ASKGLightDevice::HandleIRDevice_Implementation()
{
	OnRep_LightMode();
}

void ASKGLightDevice::OnRep_InfraredMode()
{
	if (bInfraredMode && !DoIHaveNightVisionOn())
	{
		SetLightsOn(false);
	}
}

bool ASKGLightDevice::Server_SetInfraredMode_Validate(bool bInfrared)
{
	return true;
}

void ASKGLightDevice::Server_SetInfraredMode_Implementation(bool bInfrared)
{
	if (LightDeviceSettings.bSupportsInfrared)
	{
		bInfraredMode = bInfrared;
		OnRep_InfraredMode();
	}
}

void ASKGLightDevice::SetInfraredMode_Implementation(bool bInfrared)
{
	if (LightDeviceSettings.bSupportsInfrared)
	{
		bInfraredMode = bInfrared;
		OnRep_InfraredMode();
		if (!HasAuthority())
		{
			Server_SetInfraredMode(bInfraredMode);
		}
	}
}

bool ASKGLightDevice::Server_SetLightMode_Validate(ESKGLightMode Mode)
{
	return true;
}

void ASKGLightDevice::Server_SetLightMode_Implementation(ESKGLightMode Mode)
{ // CHECK IF EXISTS IN LightModes
	LightMode = Mode;
	OnRep_LightMode();
}

void ASKGLightDevice::CycleModes(const bool bDownArray, const uint8 ForceIndex)
{
	const uint8 ModeCount = LightModes.Num();
	if (ModeCount)
	{
		if (ForceIndex < ModeCount)
		{
			LightModeIndex = ForceIndex;
		}
		else
		{
			if (bDownArray)
			{
				if (++LightModeIndex >= ModeCount)
				{
					LightModeIndex = 0;
				}
			}
			else
			{
				if (LightModeIndex - 1 < 0)
				{
					LightModeIndex = ModeCount - 1;
				}
				else
				{
					--LightModeIndex;
				}
			}
		}

		LightMode = LightModes[LightModeIndex];

		OnRep_LightMode();
		if (!HasAuthority())
		{
			Server_SetLightMode(LightMode);
		}
	}
}

void ASKGLightDevice::OnRep_LightMode()
{
	switch (LightMode)
	{
	case ESKGLightMode::Off :
		{
			//UE_LOG(LogTemp, Warning, TEXT("Off"));
			SetLightsOn(false);
			break;
		}
	case ESKGLightMode::Steady :
		{
			//UE_LOG(LogTemp, Warning, TEXT("Steady"));
			SetLightsOn(true);
			break;
		}
	case ESKGLightMode::Strobe :
		{
			//UE_LOG(LogTemp, Warning, TEXT("Strobe"));
			SetActorTickEnabled(true);
			break;
		}
	}

	if (bInfraredMode && !DoIHaveNightVisionOn())
	{
		SetLightsOn(false);
	}
}
