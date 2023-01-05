// Copyright 2022, Dakota Dawe, All rights reserved


#include "Components/SKGCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

USKGCharacterMovementComponent::USKGCharacterMovementComponent()
{
	MaxAcceleration = 1365.0f;
	BrakingFriction = 1.0f;
	MaxWalkSpeed = 150.0f;
	
	SprintSpeed = 350.0f;
	SuperSprintSpeed = 400.0f;
	bCanOnlySprintForwards = false;
	SprintSidewaysSpeed = 300.0f;
	SprintBackwardsSpeed = 250.0f;
	MoveDirectionTolerance = 0.5f;
	DefaultMaxWalkSpeed = 150.0f;

	bCanSprintWhileCrouched = false;
	bCrouchCanOnlySprintForwards = false;
	CrouchSprintSpeed = 125.0f;
	CrouchSprintSidewaysSpeed = 100.0f;
	CrouchSprintBackwardsSpeed = 80.0f;

	MaxWalkSpeedCrouched = 50.0f;

	RequestedWalkSpeedMultiplier = 1.0f;
	MinMaxWalkSpeedMultiplier = FSKGMinMax(0.4f, 1.0f);
}

void USKGCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	DefaultMaxWalkSpeed = MaxWalkSpeed;
}

bool USKGCharacterMovementComponent::IsMovingForwards() const
{
	if (CharacterOwner)
	{
		FVector Forward = CharacterOwner->GetActorForwardVector();
		FVector MoveDirection = Velocity.GetSafeNormal();

		Forward.Z = 0.0f;
		MoveDirection.Z = 0.0f;
		return FVector::DotProduct(Forward, MoveDirection) > MoveDirectionTolerance;
	}
	return false;
}

bool USKGCharacterMovementComponent::IsMovingBackwards() const
{
	if (CharacterOwner)
	{
		FVector Forward = CharacterOwner->GetActorForwardVector();
		FVector MoveDirection = Velocity.GetSafeNormal();

		Forward.Z = 0.0f;
		MoveDirection.Z = 0.0f;
		return FVector::DotProduct(Forward, MoveDirection) < -MoveDirectionTolerance;
	}
	return false;
}

bool USKGCharacterMovementComponent::IsMovingSideways() const
{
	if (CharacterOwner)
	{
		FVector Right = CharacterOwner->GetActorRightVector();
		FVector MoveDirection = Velocity.GetSafeNormal();

		Right.Z = 0.0f;
		MoveDirection.Z = 0.0f;
		const float DotProduct = FVector::DotProduct(Right, MoveDirection);
		return DotProduct > MoveDirectionTolerance || DotProduct < -MoveDirectionTolerance;
	}
	return false;
}

void USKGCharacterMovementComponent::SetSprinting(ESKGSprintType SprintType)
{
	switch (SprintType)
	{
	case ESKGSprintType::None : bWantsToSprint = false; bWantsToSuperSprint = false; break;
	case ESKGSprintType::Sprint : bWantsToSprint = true; bWantsToSuperSprint = false; break;
	case ESKGSprintType::SuperSprint : bWantsToSprint = false; bWantsToSuperSprint = true; break;
	default : bWantsToSprint = false; bWantsToSuperSprint = false;
	}
}

bool USKGCharacterMovementComponent::Server_SetWalkSpeedMultiplier_Validate(float WalkSpeedMultiplier)
{
	return true;
}

void USKGCharacterMovementComponent::Server_SetWalkSpeedMultiplier_Implementation(float WalkSpeedMultiplier)
{
	WalkSpeedMultiplier = FMath::Clamp(WalkSpeedMultiplier, MinMaxWalkSpeedMultiplier.Min, MinMaxWalkSpeedMultiplier.Max);
	RequestedWalkSpeedMultiplier = WalkSpeedMultiplier;
}

void USKGCharacterMovementComponent::SetWalkSpeedMultiplier(float WalkSpeedMultiplier)
{
	WalkSpeedMultiplier = FMath::Clamp(WalkSpeedMultiplier, MinMaxWalkSpeedMultiplier.Min, MinMaxWalkSpeedMultiplier.Max);
	RequestedWalkSpeedMultiplier = WalkSpeedMultiplier;
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_SetWalkSpeedMultiplier(RequestedWalkSpeedMultiplier);
	}
}

void USKGCharacterMovementComponent::IncreaseWalkSpeedMultiplier(float IncreaseAmount)
{
	SetWalkSpeedMultiplier(RequestedWalkSpeedMultiplier + IncreaseAmount);
}

void USKGCharacterMovementComponent::DecreaseWalkSpeedMultiplier(float DecreaseAmount)
{
	SetWalkSpeedMultiplier(RequestedWalkSpeedMultiplier - DecreaseAmount);
}

float USKGCharacterMovementComponent::HandleSprintSpeed() const
{
	float MaxSpeed = DefaultMaxWalkSpeed;
	const float NewSprintSpeed = bWantsToSuperSprint ? SuperSprintSpeed : SprintSpeed;

	if (IsCrouching() && !bCanSprintWhileCrouched)
	{
		return MaxWalkSpeedCrouched;
	}

	if (IsCrouching())
	{
		MaxSpeed = MaxWalkSpeedCrouched;
		if (IsMovingForwards())
		{
			MaxSpeed = CrouchSprintSpeed;
			if (bCrouchCanOnlySprintForwards && IsMovingSideways())
			{
				MaxSpeed = MaxWalkSpeedCrouched;
			}
		}
		if (!bCrouchCanOnlySprintForwards)
		{
			if (bCrouchSprintSideways && IsMovingSideways())
			{
				MaxSpeed = CrouchSprintSidewaysSpeed;
			}
			else if (bCrouchSprintBackwards && IsMovingBackwards())
			{
				MaxSpeed = CrouchSprintBackwardsSpeed;
			}
		}
		return MaxSpeed;
	}
	
	if (IsMovingForwards())
	{
		IsCrouching() ? MaxSpeed = CrouchSprintSpeed : MaxSpeed = NewSprintSpeed;

		if (bCanOnlySprintForwards && IsMovingSideways())
		{
			MaxSpeed = DefaultMaxWalkSpeed;
		}
	}
	if (!bCanOnlySprintForwards)
	{
		if (bSprintSideways && IsMovingSideways())
		{
			MaxSpeed = SprintSidewaysSpeed;
		}
		else if (bSprintBackwards && IsMovingBackwards())
		{
			MaxSpeed = SprintBackwardsSpeed;
		}
	}
	return MaxSpeed;
}

float USKGCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	if (bWantsToSprint || bWantsToSuperSprint)
	{
		MaxSpeed = HandleSprintSpeed();
	}
	else
	{
		MaxSpeed *= RequestedWalkSpeedMultiplier;
	}
	if (GetOwnerRole() != ENetRole::ROLE_Authority)
	{
		//UE_LOG(LogTemp, Warning, TEXT("MaxSpeed: %f"), MaxSpeed);
	}
	return MaxSpeed;
}

void USKGCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	
}

void USKGCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	
	bWantsToSprint = (Flags&FSavedMove_Character::FLAG_Custom_0) != 0;
	bWantsToSuperSprint = (Flags&FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client* USKGCharacterMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		USKGCharacterMovementComponent* MutableThis = const_cast<USKGCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_FPSTemplate(*this);
	}
	return ClientPredictionData;
}

void FSavedMove_FPSTemplate::Clear()
{
	Super::Clear();
	bSavedWantsToSprint = false;
	bSavedWantsToSuperSprint = false;
	SavedRequestedWalkSpeedMultiplier = 1.0f;
}

uint8 FSavedMove_FPSTemplate::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (bSavedWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}
	if (bSavedWantsToSuperSprint)
	{
		Result |= FLAG_Custom_1;
	}
	return Result;
}

bool FSavedMove_FPSTemplate::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (bSavedWantsToSprint != ((FSavedMove_FPSTemplate*)&NewMove)->bSavedWantsToSprint)
	{
		return false;
	}
	if (bSavedWantsToSuperSprint != ((FSavedMove_FPSTemplate*)&NewMove)->bSavedWantsToSuperSprint)
	{
		return false;
	}
	if (SavedRequestedWalkSpeedMultiplier != ((FSavedMove_FPSTemplate*)&NewMove)->SavedRequestedWalkSpeedMultiplier)
	{
		return false;
	}
	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_FPSTemplate::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	if (const USKGCharacterMovementComponent* CustomMovementComponent = Cast<USKGCharacterMovementComponent>(C->GetCharacterMovement()))
	{
		bSavedWantsToSprint = CustomMovementComponent->bWantsToSprint;
		bSavedWantsToSuperSprint = CustomMovementComponent->bWantsToSuperSprint;
		SavedRequestedWalkSpeedMultiplier = CustomMovementComponent->RequestedWalkSpeedMultiplier;
	}
}

void FSavedMove_FPSTemplate::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	if (USKGCharacterMovementComponent* CustomMovementComponent = Cast<USKGCharacterMovementComponent>(Character->GetCharacterMovement()))
	{
		CustomMovementComponent->bWantsToSprint = bSavedWantsToSprint;
		CustomMovementComponent->bWantsToSuperSprint = bSavedWantsToSuperSprint;
		CustomMovementComponent->RequestedWalkSpeedMultiplier = SavedRequestedWalkSpeedMultiplier;
	}
}

FSavedMovePtr FNetworkPredictionData_Client_FPSTemplate::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_FPSTemplate);
}