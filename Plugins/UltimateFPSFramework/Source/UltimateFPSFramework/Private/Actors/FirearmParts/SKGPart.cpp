// Copyright 2021, Dakota Dawe, All rights reserved


#include "Actors/FirearmParts/SKGPart.h"
#include "Actors/SKGFirearm.h"
#include "Components/SKGCharacterComponent.h"
#include "Components/ShapeComponent.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ASKGPart::ASKGPart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	NetUpdateFrequency = 1.0f;
	MinNetUpdateFrequency = 0.5f;

	FirearmCollisionChannel = ECC_GameTraceChannel2;
	
	PartStats.Weight = 0.22f;
	PartStats.ErgonomicsChangePercentage = 0.0f;
	PartStats.VerticalRecoilChangePercentage = 0.0f;
	PartStats.HorizontalRecoilChangePercentage = 0.0f;

	PartType = ESKGPartType::Other;
}

// Called when the game starts or when spawned
void ASKGPart::BeginPlay()
{
	Super::BeginPlay();
}

void ASKGPart::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASKGPart::PostInitProperties()
{
	Super::PostInitProperties();
	PartData.PartClass = GetClass();
}

void ASKGPart::Destroyed()
{
	if (IsValid(GetOwningFirearm()))
	{
		if (this == OwningFirearm->GetCurrentSightActor())
		{
			ISKGFirearmInterface::Execute_CycleSights(OwningFirearm.Get(), true, false);
		}
	}
	Super::Destroyed();
}

bool ASKGPart::Server_Use_Validate()
{
	return true;
}

void ASKGPart::Server_Use_Implementation()
{
}

void ASKGPart::CacheCharacterAndFirearm()
{
	GetOwningFirearm();
	ISKGFirearmAttachmentsInterface::Execute_GetOwningCharacterComponent(this);
}

FSKGFirearmPartStats ASKGPart::GetPartStats_Implementation()
{
	FSKGFirearmPartStats ReturnPartStats = PartStats;
	for (const USKGAttachmentComponent* PartComponent : AttachmentComponents)
	{
		if (IsValid(PartComponent))
		{
			ASKGPart* Part = PartComponent->GetAttachment<ASKGPart>();
			if (IsValid(Part))
			{
				ReturnPartStats += ISKGFirearmAttachmentsInterface::Execute_GetPartStats(Part);
			}
		}
	}
	return ReturnPartStats;
}

AActor* ASKGPart::GetOwningActor_Implementation()
{
	if (OwningActor.IsValid())
	{
		return OwningActor.Get();
	}
	AActor* PartOwner = GetOwner();
	for (uint8 i = 0; i < MAX_PartStack; ++i)
	{
		if (IsValid(PartOwner))
		{
			//if (PartOwner->Implements<UFPSTemplate_AttachmentInterface>())
			{
				OwningActor = PartOwner;
				return OwningActor.Get();
			}

			PartOwner = PartOwner->GetOwner();
		}
	}
	return nullptr;
}

USKGCharacterComponent* ASKGPart::GetOwningCharacterComponent_Implementation()
{
	if (OwningCharacterComponent.IsValid())
	{
		return OwningCharacterComponent.Get();
	}

	if (OwningFirearm.IsValid())
	{
		if (USKGCharacterComponent* CharacterComponent = OwningFirearm->GetCharacterComponent())
		{
			OwningCharacterComponent = CharacterComponent;
			return CharacterComponent;
		}
	}
	else
	{
		OwningFirearm = Cast<ASKGFirearm>(ISKGFirearmAttachmentsInterface::Execute_GetOwningActor(this));
		if (OwningFirearm.IsValid())
		{
			if (USKGCharacterComponent* CharacterComponent = OwningFirearm->GetCharacterComponent())
			{
				OwningCharacterComponent = CharacterComponent;
				return CharacterComponent;
			}
		}
	}
	return nullptr;
}

ASKGFirearm* ASKGPart::GetOwningFirearm()
{
	if (OwningFirearm.IsValid())
	{
		return OwningFirearm.Get();
	}

	if (ISKGFirearmAttachmentsInterface::Execute_GetOwningActor(this))
	{
		OwningFirearm = Cast<ASKGFirearm>(OwningActor);
	}
	return OwningFirearm.Get();
}

void ASKGPart::Use_Implementation()
{
	OnUse();
}