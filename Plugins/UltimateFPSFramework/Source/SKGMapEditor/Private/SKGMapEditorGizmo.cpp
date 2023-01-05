// Copyright 2021, Dakota Dawe, All rights reserved


#include "SKGMapEditorGizmo.h"
#include "Components/SKGMapEditorHandlerComponent.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASKGMapEditorGizmo::ASKGMapEditorGizmo()
{
	PrimaryActorTick.bCanEverTick = true;

	Origin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OriginComponent"));
	RootComponent = Origin;
	
	ZAxis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZAxisComponent"));
	XAxis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("XAxisComponent"));
	YAxis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("YAxisComponent"));
	
	ZAxis->SetupAttachment(Origin);
	XAxis->SetupAttachment(ZAxis);
	YAxis->SetupAttachment(XAxis);

	Yaw = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("YawComponent"));
	Roll = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RollComponent"));
	Pitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PitchComponent"));

	Yaw->SetupAttachment(Origin);
	Roll->SetupAttachment(Yaw);
	Pitch->SetupAttachment(Roll);

	ScaleZ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScaleZComponent"));
	ScaleX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScaleXComponent"));
	ScaleY = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScaleYComponent"));
	
	ScaleZ->SetupAttachment(Origin);
	ScaleX->SetupAttachment(ScaleZ);
	ScaleY->SetupAttachment(ScaleX);
	
	MovementSlowdown = 1.0f;
	
	bGrabbed = false;
}

void ASKGMapEditorGizmo::BeginPlay()
{
	Super::BeginPlay();

	OwningController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ShowMovement();

	SetActorHiddenInGame(true);
}

void ASKGMapEditorGizmo::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!IsHidden() && CurrentActor && OwningController.IsValid())
	{
		if (APawn* Pawn = OwningController->GetPawn())
		{
			float Distance = FVector::Distance(Pawn->GetActorLocation(), GetActorLocation());
			FVector Scale = FVector(1.0f, 1.0f, 1.0f);
			Scale *= Distance / 1000.0f;
			SetActorScale3D(Scale);
		}

		if (bGrabbed)
		{
			switch (CurrentGizmo)
			{
				case ESKGGizmoType::Location: HandleMovement(); break;
				case ESKGGizmoType::Rotation: HandleRotation(); break;
				case ESKGGizmoType::Scale: HandleScale(); break;
			}
		}
	}
}

bool ASKGMapEditorGizmo::GreaterThanSnapAmount(float Current, float New)
{
	if (HandlerComponent)
	{
		float SnapAmount = 0.0f;
		switch (CurrentGizmo)
		{
		case ESKGGizmoType::Location: SnapAmount = HandlerComponent->GetSnapAmount().Location; break;
		case ESKGGizmoType::Rotation: SnapAmount = HandlerComponent->GetSnapAmount().Rotation; break;
		case ESKGGizmoType::Scale: SnapAmount = 2.0f; break;
		}

		float Diff = New - (Current + SnapAmount);

		if (New >= Current + SnapAmount)
		{
			return true;
		}
		else if (New <= Current - SnapAmount)
		{
			return true;
		}
	}
	return false;
}

void ASKGMapEditorGizmo::HandleMovement()
{
	FVector MouseLocation;
	FVector MouseDirection;
	OwningController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
	
	const float Distance = FVector::Distance(MouseLocation, CurrentActor->GetActorLocation());
	FVector MouseWorldPOS = MouseLocation + MouseDirection * Distance;
	
	FVector CurrentLocation = GetActorLocation();

	const FVector Difference = MouseWorldPOS - CurrentLocation;
	MouseWorldPOS -= Difference;
	
	const FVector Diff = MouseLocation - CurrentMouseWorldPos;
	MouseWorldPOS += Diff * Distance / (MovementSlowdown > 0.0f ? MovementSlowdown : 1.0f); // Scale based on distance

	const float SnapAmount = HandlerComponent->GetSnapAmount().Rotation;
	
	bool bMoved = false;
	int32 NumberOfSnaps;
	switch (MoveAxis)
	{
	case ESKGMoveAxis::XAxis:
		{
			if (GreaterThanSnapAmount(CurrentLocation.X, MouseWorldPOS.X))
			{
				NumberOfSnaps = ( MouseWorldPOS.X - CurrentLocation.X ) / HandlerComponent->GetSnapAmount().Location;
				CurrentLocation.X += SnapAmount * NumberOfSnaps;
				bMoved = true;
			}
			break;
		}
	case ESKGMoveAxis::YAxis:
		{
			if (GreaterThanSnapAmount(CurrentLocation.Y, MouseWorldPOS.Y))
			{
				NumberOfSnaps = ( MouseWorldPOS.Y - CurrentLocation.Y ) / HandlerComponent->GetSnapAmount().Location;
				CurrentLocation.Y += SnapAmount * NumberOfSnaps;
				bMoved = true;
			}
			break;
		}
	case ESKGMoveAxis::ZAxis:
		{
			if (GreaterThanSnapAmount(CurrentLocation.Z, MouseWorldPOS.Z))
			{
				NumberOfSnaps = ( MouseWorldPOS.Z - CurrentLocation.Z ) / HandlerComponent->GetSnapAmount().Location;
				CurrentLocation.Z += SnapAmount * NumberOfSnaps;
				bMoved = true;
			}
			break;
		}
	}
	
	if (bMoved)
	{
		SetActorLocation(CurrentLocation);
		CurrentMouseWorldPos = MouseLocation;
		CurrentActor->SetActorLocation(CurrentLocation);
	}
}

void ASKGMapEditorGizmo::HandleRotation()
{
	const FVector Difference = ClickedMouseWorldPos - GetMouseWorldPosition();	
	const float SnapAmount = HandlerComponent->GetSnapAmount().Rotation;
	
	const float MousePOSDifference = (Difference.X + Difference.Y) * 10.0f;
	if (MousePOSDifference > SnapAmount || MousePOSDifference < -SnapAmount)
	{
		bool  bRotated= false;
		const float bRotatingLeftOfScreen = ((Difference.X + Difference.Y) * 10.0f) > 0.0f ? 1.0f : -1.0f;
		FRotator CurrentRotation = FRotator::ZeroRotator;
		
		switch (RotationAxis)
		{
		case ESKGRotationAxis::Yaw: // Yaw
			{
				CurrentRotation.Yaw += HandlerComponent->GetSnapAmount().Rotation * bRotatingLeftOfScreen;
				bRotated = true;
				break;
			}
		case ESKGRotationAxis::Roll:
			{
				CurrentRotation.Pitch += HandlerComponent->GetSnapAmount().Rotation * bRotatingLeftOfScreen;
				bRotated = true;
				break;
			}
		case ESKGRotationAxis::Pitch:
			{
				CurrentRotation.Roll -= HandlerComponent->GetSnapAmount().Rotation * bRotatingLeftOfScreen;
				bRotated = true;
				break;
			}
		}

		if (bRotated)
		{
			CurrentActor->AddActorWorldRotation(CurrentRotation);
			ClickedMousePos = GetMousePosition();
			ClickedMouseWorldPos = GetMouseWorldPosition();
		}
	}
}

void ASKGMapEditorGizmo::HandleScale()
{
	FVector MousePosition = GetMouseWorldPosition();
	FVector Difference = ClickedMouseWorldPos - MousePosition;
	
	FVector CurrentScale = CurrentActor->GetActorScale3D();

	bool bScaled = false;
	
	switch (ScaleAxis)
	{
	case ESKGScaleAxis::ScaleX:
		{
			CurrentScale.X -= Difference.X;
			bScaled = true;
			break;
		}
	case ESKGScaleAxis::ScaleY:
		{
			CurrentScale.Y += Difference.Y;
			bScaled = true;
			break;
		}
	case ESKGScaleAxis::ScaleZ:
		{
			CurrentScale.Z -= Difference.Z;
			bScaled = true;
			break;
		}
	}

	if (CurrentScale.X < 0.0f)
	{
		CurrentScale.X = 0.01f;
	}
	if (CurrentScale.Y < 0.0f)
	{
		CurrentScale.Y = 0.01f;
	}
	if (CurrentScale.Z < 0.0f)
	{
		CurrentScale.Z = 0.01f;
	}
	
	if (bScaled)
	{
		CurrentActor->SetActorScale3D(CurrentScale);
		ClickedMousePos = GetMousePosition();
		ClickedMouseWorldPos = GetMouseWorldPosition();
	}
}

void ASKGMapEditorGizmo::SnapToActor(AActor* Actor)
{
	CurrentActor = Actor;
	if (CurrentActor)
	{
		SetActorLocation(CurrentActor->GetActorLocation());
		if (CurrentGizmo == ESKGGizmoType::Rotation)
		{
			//(CurrentActor->GetActorRotation());
		}
		else
		{
			//SetActorRotation(FRotator::ZeroRotator);
		}
		SetActorRotation(FRotator::ZeroRotator);
	}
}


ESKGMoveAxis ASKGMapEditorGizmo::GetMoveAxis(UPrimitiveComponent* HitComponent)
{
	if (HitComponent)
	{
		if (HitComponent == XAxis)
		{
			return ESKGMoveAxis::XAxis;
		}
		if (HitComponent == YAxis)
		{
			return ESKGMoveAxis::YAxis;
		}
		if (HitComponent == ZAxis)
		{
			return ESKGMoveAxis::ZAxis;
		}
	}

	return ESKGMoveAxis::None;
}

ESKGRotationAxis ASKGMapEditorGizmo::GetRotationAxis(UPrimitiveComponent* HitComponent)
{
	if (HitComponent)
	{
		if (HitComponent == Yaw)
		{
			return ESKGRotationAxis::Yaw;
		}
		if (HitComponent == Roll)
		{
			return ESKGRotationAxis::Roll;
		}
		if (HitComponent == Pitch)
		{
			return ESKGRotationAxis::Pitch;
		}
	}

	return ESKGRotationAxis::None;
}

ESKGScaleAxis ASKGMapEditorGizmo::GetScaleAxis(UPrimitiveComponent* HitComponent)
{
	if (HitComponent)
	{
		if (HitComponent == ScaleX)
		{
			return ESKGScaleAxis::ScaleX;
		}
		if (HitComponent == ScaleY)
		{
			return ESKGScaleAxis::ScaleY;
		}
		if (HitComponent == ScaleZ)
		{
			return ESKGScaleAxis::ScaleZ;
		}
	}

	return ESKGScaleAxis::None;
}

FVector ASKGMapEditorGizmo::GetMouseWorldPosition()
{
	FVector MouseLocation;
	FVector MouseDirection;
	OwningController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
	return MouseLocation;
}

FVector2D ASKGMapEditorGizmo::GetMousePosition()
{
	FVector2D MousePosition;
	if (OwningController.IsValid())
	{
		OwningController->GetMousePosition(MousePosition.X, MousePosition.Y);
	}
	return MousePosition;
}

void ASKGMapEditorGizmo::Replicate()
{
	HandlerComponent->ReplicateActor();
}

void ASKGMapEditorGizmo::HideGizmo(bool Hide)
{
	SetActorHiddenInGame(Hide);
	SetActorEnableCollision(!Hide);
}

void ASKGMapEditorGizmo::ClearGizmo()
{
	CurrentActor = nullptr;
}

void ASKGMapEditorGizmo::HitGizmo(FHitResult HitResult)
{
	if (OwningController.IsValid())
	{
		bGrabbed = true;
		CurrentMouseWorldPos = GetMouseWorldPosition();
		MoveAxis = GetMoveAxis(HitResult.GetComponent());
		switch (CurrentGizmo)
		{
			case ESKGGizmoType::Location: MoveAxis = GetMoveAxis(HitResult.GetComponent()); break;
			case ESKGGizmoType::Rotation: RotationAxis = GetRotationAxis(HitResult.GetComponent()); break;
			case ESKGGizmoType::Scale: ScaleAxis = GetScaleAxis(HitResult.GetComponent()); break;
		}
 
		ClickedMousePos = GetMousePosition();
		ClickedMouseWorldPos = GetMouseWorldPosition();
		float TimerReplicationRate = HandlerComponent && HandlerComponent->GetReplicationRate() > 0.01f ? TimerReplicationRate = HandlerComponent->GetReplicationRate() : TimerReplicationRate = 0.1f;
		GetWorld()->GetTimerManager().SetTimer(TReplicateHandle, this, &ASKGMapEditorGizmo::Replicate, TimerReplicationRate, true);
	}
}

void ASKGMapEditorGizmo::ReleaseGizmo()
{
	GetWorld()->GetTimerManager().ClearTimer(TReplicateHandle);
	bGrabbed = false;
	MoveAxis = ESKGMoveAxis::None;
	if (CurrentGizmo == ESKGGizmoType::Rotation && CurrentActor)
	{
		SetActorRotation(FRotator::ZeroRotator);
	}
	RotationAxis = ESKGRotationAxis::None;
	ScaleAxis = ESKGScaleAxis::None;
	if (HandlerComponent)
	{
		Replicate();
	}
}

void ASKGMapEditorGizmo::ShowMovement()
{
	CurrentGizmo = ESKGGizmoType::Location;
	
	XAxis->SetHiddenInGame(false);
	XAxis->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	YAxis->SetHiddenInGame(false);
	YAxis->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ZAxis->SetHiddenInGame(false);
	ZAxis->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	Yaw->SetHiddenInGame(true);
	Yaw->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Roll->SetHiddenInGame(true);
	Roll->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Pitch->SetHiddenInGame(true);
	Pitch->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ScaleX->SetHiddenInGame(true);
	ScaleX->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScaleY->SetHiddenInGame(true);
	ScaleY->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScaleZ->SetHiddenInGame(true);
	ScaleZ->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetActorRotation(FRotator::ZeroRotator);
}

void ASKGMapEditorGizmo::ShowRotation()
{
	CurrentGizmo = ESKGGizmoType::Rotation;
	
	XAxis->SetHiddenInGame(true);
	XAxis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	YAxis->SetHiddenInGame(true);
	YAxis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ZAxis->SetHiddenInGame(true);
	ZAxis->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Yaw->SetHiddenInGame(false);
	Yaw->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Roll->SetHiddenInGame(false);
	Roll->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Pitch->SetHiddenInGame(false);
	Pitch->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	ScaleX->SetHiddenInGame(true);
	ScaleX->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScaleY->SetHiddenInGame(true);
	ScaleY->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ScaleZ->SetHiddenInGame(true);
	ScaleZ->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (CurrentActor)
	{
		SetActorRotation(FRotator::ZeroRotator);
	}
}

void ASKGMapEditorGizmo::ShowScale()
{
	CurrentGizmo = ESKGGizmoType::Scale;
	
	XAxis->SetHiddenInGame(true);
	XAxis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	YAxis->SetHiddenInGame(true);
	YAxis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ZAxis->SetHiddenInGame(true);
	ZAxis->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Yaw->SetHiddenInGame(true);
	Yaw->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Roll->SetHiddenInGame(true);
	Roll->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Pitch->SetHiddenInGame(true);
	Pitch->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ScaleX->SetHiddenInGame(false);
	ScaleX->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ScaleY->SetHiddenInGame(false);
	ScaleY->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ScaleZ->SetHiddenInGame(false);
	ScaleZ->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (CurrentActor)
	{
		SetActorRotation(FRotator::ZeroRotator);
	}
}
