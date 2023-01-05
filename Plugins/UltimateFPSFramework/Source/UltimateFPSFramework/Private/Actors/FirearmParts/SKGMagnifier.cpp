// Copyright 2021, Dakota Dawe, All rights reserved


#include "Actors/FirearmParts/SKGMagnifier.h"
#include "Actors/SKGFirearm.h"
#include "Components/SKGCharacterComponent.h"
#include "Components/SKGSceneCaptureOptic.h"

#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

ASKGMagnifier::ASKGMagnifier()
{
	PrimaryActorTick.bCanEverTick = false;
	NetUpdateFrequency = 1.0f;
	PartType = ESKGPartType::Magnifier;

	bFlippedOut = false;
	bFullyFlippedOut = true;
}

void ASKGMagnifier::OnRep_FlippedOut()
{
	OnUse();
}

void ASKGMagnifier::BeginPlay()
{
	Super::BeginPlay();
	
	if (AttachmentMesh.IsValid())
	{
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ASKGMagnifier::SetupMagnifier, 0.2f, false);
	}
}

void ASKGMagnifier::OnRep_Owner()
{
	if (AttachmentMesh.IsValid())
	{
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ASKGMagnifier::SetupMagnifier, 0.2f, false);
	}
}

void ASKGMagnifier::SetupMagnifier()
{
	if (AttachmentMesh.IsValid())
	{
		FVector Start = Execute_GetAimSocketTransform(this).GetLocation();
		FVector End = Start + Execute_GetAimSocketTransform(this).Rotator().Vector() * 20.0f;
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		if (GetOwningActor())
		{
			QueryParams.AddIgnoredActor(OwningActor.Get());
		}
		
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
		{
			if (ASKGSight* HitSight = Cast<ASKGSight>(HitResult.GetActor()))
			{
				ISKGAimInterface::Execute_SetMagnifier(HitSight, this);
				SightInfront = HitSight;
			}
		}
	}
}

void ASKGMagnifier::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASKGMagnifier, bFlippedOut, COND_SkipOwner);
}

bool ASKGMagnifier::Server_Flip_Validate(bool bFlip)
{
	return true;
}

void ASKGMagnifier::Server_Flip_Implementation(bool bFlip)
{
	Use();
}

void ASKGMagnifier::Use_Implementation()
{
	bFlippedOut = !bFlippedOut;
	bFullyFlippedOut = false;
	
	if (bFlippedOut)
	{
		Execute_DisableRenderTarget(this, true);
	}
	else
	{
		if (GetOwningCharacterComponent() && OwningCharacterComponent->IsAiming())
		{
			if (GetOwningFirearm() && !ISKGAimInterface::Execute_IsPointAiming(OwningFirearm.Get()) && ISKGFirearmPartsInterface::Execute_GetCurrentSight(OwningFirearm.Get()) == SightInfront)
			{
				Execute_DisableRenderTarget(this, false);
			}
		}
	}
	
	OnUse();
	
	if (!HasAuthority())
	{
		Server_Flip(bFlippedOut);
	}
}
