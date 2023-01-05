// Copyright 2022, Dakota Dawe, All rights reserved

#include "Components/SKGFirearmStabilizerComponent.h"

#include "DrawDebugHelpers.h"
#include "SKGCharacterAnimInstance.h"
#include "Actors/SKGFirearm.h"
#include "Components/SKGCharacterComponent.h"

USKGFirearmStabilizerComponent::USKGFirearmStabilizerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bUseAttachParentBound = true;
	
	DebugType = EDrawDebugTrace::None;
	bOnlyRunOnOwner = true;
	Size = FVector(8.0f, 10.0f, 5.0f);
	UpdateRate = 10.0f;

	bIsStabilized = false;
	bOnlyWithKeyPress = false;
}

void USKGFirearmStabilizerComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickInterval(1.0f / UpdateRate);
	SetComponentTickEnabled(false);

	CacheEssentials();
}

void USKGFirearmStabilizerComponent::CacheEssentials()
{
	Firearm = Cast<ASKGFirearm>(GetOwner());
	if (IsValid(Firearm))
	{
		CharacterComponent = Firearm->GetCharacterComponent();
	}
}

void USKGFirearmStabilizerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (FindStabilizingSurface())
	{
		if (!bIsStabilized)
		{
			StabilizeFirearm();
		}
	}
	else if (bIsStabilized)
	{
		UnStabilizeFirearm();
	}
}

bool USKGFirearmStabilizerComponent::FindStabilizingSurface()
{
	FHitResult HitResult;
	TArray<AActor*> ActorsIgnored;
	ActorsIgnored.Add(GetOwner());

	if (IsValid(Firearm))
	{// FUNCTION REMOVED
		ActorsIgnored.Append(Firearm->GetCachedParts());
	}
	if (IsValid(CharacterComponent))
	{
		ActorsIgnored.Add(CharacterComponent->GetOwner());
	}
	
	return UKismetSystemLibrary::BoxTraceSingle(GetWorld(), GetComponentLocation(), GetComponentLocation(), Size,
		GetComponentRotation(), CollisionChannel, false, ActorsIgnored, DebugType, HitResult, true);
}

void USKGFirearmStabilizerComponent::StabilizeFirearm()
{
	if (IsValid(Firearm) && IsValid(CharacterComponent) && IsValid(CharacterComponent->GetAnimationInstance()))
	{
		CharacterComponent->GetAnimationInstance()->PlayCustomCurve(StabilizedCurveData);
		Firearm->SetSwayMultiplier(0.15f);
		bIsStabilized = true;
		FirearmStabilized.Broadcast();
	}
}

void USKGFirearmStabilizerComponent::AttemptStabilizeFirearm()
{
	if (FindStabilizingSurface())
	{
		StabilizeFirearm();
		SetComponentTickEnabled(true);
	}
}

void USKGFirearmStabilizerComponent::UnStabilizeFirearm()
{
	if (IsValid(Firearm) && IsValid(CharacterComponent) && IsValid(CharacterComponent->GetAnimationInstance()))
	{
		CharacterComponent->GetAnimationInstance()->PlayCustomCurve(UnStabilizedCurveData);
		Firearm->ResetSwayMultiplier();
		bIsStabilized = false;
		FirearmUnStabilized.Broadcast();
	}
	if (bOnlyWithKeyPress)
	{
		SetComponentTickEnabled(false);
	}
}

void USKGFirearmStabilizerComponent::Enable()
{
	if (bOnlyWithKeyPress)
	{
		return;
	}
	if (bOnlyRunOnOwner)
	{
		if (IsValid(Firearm->GetOwner()))
		{
			const APawn* OwningPawn = Cast<APawn>(Firearm->GetOwner());
			if (IsValid(OwningPawn) && OwningPawn->IsLocallyControlled())
			{
				SetComponentTickInterval(0.1f);
				SetComponentTickEnabled(true);
			}
		}
	}
	else
	{
		SetComponentTickEnabled(true);
	}
}

void USKGFirearmStabilizerComponent::Disable()
{
	SetComponentTickEnabled(false);
}