// Copyright 2021, Dakota Dawe, All rights reserved


#include "Components/SKGMapEditorCharacterMovement.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"

USKGMapEditorCharacterMovement::USKGMapEditorCharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxSpeedMultiplier = 4.0f;
	MinSpeedMultiplier = 0.1f;
	RequestedSpeedMultiplier = 1.0f;
	bCenterMouseOnStopMovementMode = false;
	
	bInMovementMode = false;
	MovementMode = EMovementMode::MOVE_Flying;
	DefaultLandMovementMode = EMovementMode::MOVE_Flying;
	DefaultWaterMovementMode = EMovementMode::MOVE_Flying;
	BrakingFrictionFactor = 80.0f;
	MaxAcceleration = 25000.0f;
}

void USKGMapEditorCharacterMovement::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

void USKGMapEditorCharacterMovement::Init()
{
	EnterMovementMode(bInMovementMode);
	if (PawnOwner)
	{
		if (const APlayerController* PC = PawnOwner->GetController<APlayerController>())
		{
			PC->GetViewportSize(ScreenCenterX, ScreenCenterY);
			ScreenCenterX /= 2;
			ScreenCenterY /= 2;
		}
	}
}

void USKGMapEditorCharacterMovement::EnterMovementMode(bool Enter)
{
	bInMovementMode = Enter;

	if (PawnOwner)
	{
		if (APlayerController* PC = PawnOwner->GetController<APlayerController>())
		{
			PC->SetShowMouseCursor(!bInMovementMode);
			if (bInMovementMode)
			{
				PC->GetMousePosition(MouseStartX, MouseStartY);
			}
			
			if (bCenterMouseOnStopMovementMode)
			{
				if (!bInMovementMode)
				{
					PC->SetMouseLocation(ScreenCenterX, ScreenCenterY);
				}
			}
		}
	}
}

bool USKGMapEditorCharacterMovement::Server_SetSpeedMultiplier_Validate(float SpeedMultiplier)
{
	return true;
}

void USKGMapEditorCharacterMovement::Server_SetSpeedMultiplier_Implementation(float SpeedMultiplier)
{
	RequestedSpeedMultiplier = SpeedMultiplier;
}

void USKGMapEditorCharacterMovement::IncreaseSpeedMultiplier(float IncreaseAmount)
{
	RequestedSpeedMultiplier += IncreaseAmount;
	if (RequestedSpeedMultiplier > MaxSpeedMultiplier)
	{
		RequestedSpeedMultiplier = MaxSpeedMultiplier;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_SetSpeedMultiplier(RequestedSpeedMultiplier);
	}
}

void USKGMapEditorCharacterMovement::DecreaseSpeedMultiplier(float DecreaseAmount)
{
	RequestedSpeedMultiplier -= DecreaseAmount;
	if (RequestedSpeedMultiplier < MinSpeedMultiplier)
	{
		RequestedSpeedMultiplier = MinSpeedMultiplier;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_SetSpeedMultiplier(RequestedSpeedMultiplier);
	}
}

void USKGMapEditorCharacterMovement::MoveForward(float Value)
{
	if (!bInMovementMode || Value == 0.0f) return;
	const FVector Movement = PawnOwner->GetActorForwardVector() * Value;
	//AddInputVector(Movement);
	PawnOwner->AddMovementInput(Movement);
}

void USKGMapEditorCharacterMovement::MoveRight(float Value)
{
	if (!bInMovementMode || Value == 0.0f) return;
	const FVector Movement = PawnOwner->GetActorRightVector() * Value;
	AddInputVector(Movement);
}

void USKGMapEditorCharacterMovement::MoveUp(float Value)
{
	if (!bInMovementMode || Value == 0.0f) return;
	const FVector Movement = PawnOwner->GetActorUpVector() * Value;
	AddInputVector(Movement);
}

void USKGMapEditorCharacterMovement::LookUp(float Value)
{
	if (!bInMovementMode || !PawnOwner || Value == 0.0f) return;
	PawnOwner->AddControllerPitchInput(Value);
}

void USKGMapEditorCharacterMovement::Turn(float Value)
{
	if (!bInMovementMode || !PawnOwner || Value == 0.0f) return;
	PawnOwner->AddControllerYawInput(Value);
}



FNetworkPredictionData_Client* USKGMapEditorCharacterMovement::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		USKGMapEditorCharacterMovement* MutableThis = const_cast<USKGMapEditorCharacterMovement*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_MapEditor(*this);
	}
	return ClientPredictionData;
}

float USKGMapEditorCharacterMovement::GetMaxSpeed() const
{
	return Super::GetMaxSpeed() * RequestedSpeedMultiplier;
}

void USKGMapEditorCharacterMovement::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (bInMovementMode && PawnOwner)
	{
		if (APlayerController* PC = PawnOwner->GetController<APlayerController>())
		{
			PC->SetMouseLocation(MouseStartX, MouseStartY);
		}
	}
}

void FSavedMove_MapEditor::Clear()
{
	Super::Clear();
	SavedRequestedSpeedMultiplier = 1.0f;
}

bool FSavedMove_MapEditor::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (SavedRequestedSpeedMultiplier != ((FSavedMove_MapEditor*)&NewMove)->SavedRequestedSpeedMultiplier)
	{
		return false;
	}
	return FSavedMove_Character::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_MapEditor::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	if (const USKGMapEditorCharacterMovement* CustomMovementComponent = Cast<USKGMapEditorCharacterMovement>(C->GetCharacterMovement()))
	{
		SavedRequestedSpeedMultiplier = CustomMovementComponent->RequestedSpeedMultiplier;
	}
}

void FSavedMove_MapEditor::PrepMoveFor(ACharacter* Character)
{
	FSavedMove_Character::PrepMoveFor(Character);

	if (USKGMapEditorCharacterMovement* CustomMovementComponent = Cast<USKGMapEditorCharacterMovement>(Character->GetCharacterMovement()))
	{
		CustomMovementComponent->RequestedSpeedMultiplier = SavedRequestedSpeedMultiplier;
	}
}

FSavedMovePtr FNetworkPredictionData_Client_MapEditor::AllocateNewMove()
{
	return FNetworkPredictionData_Client_Character::AllocateNewMove();
}