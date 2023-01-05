// Copyright 2021, Dakota Dawe, All rights reserved


#include "Components/SKGMapEditorHandlerComponent.h"
#include "SKGMapEditorGizmo.h"
#include "SKGMapEditorInterface.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

USKGMapEditorHandlerComponent::USKGMapEditorHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	MapDirectory = FString("Maps");

	ReplicationRate = 1.0f;
}


void USKGMapEditorHandlerComponent::OnRep_CurrentActor()
{
	SetActor(CurrentActor);
}

void USKGMapEditorHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (APawn* OwningPawn = GetOwner<APawn>())
	{
		if (OwningPawn->IsLocallyControlled() && GizmoClass)
		{
			Gizmo = GetWorld()->SpawnActor<ASKGMapEditorGizmo>(GizmoClass);
			Gizmo->SetHandler(this);
		}
	}
}

void USKGMapEditorHandlerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(USKGMapEditorHandlerComponent, CurrentActor, COND_OwnerOnly);
}

FHitResult USKGMapEditorHandlerComponent::MouseTraceSingle(const float Distance, bool& bHitGizmo, const ECollisionChannel CollisionChannel, const bool bDrawDebugLine)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FVector WorldLocation;
		FVector WorldDirection;
		if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			FVector End = WorldLocation + WorldDirection * Distance;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetOwner());
			FHitResult HitResult;
			bool bBlockingHit = false;
			if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, End, CollisionChannel, Params))
			{
				bBlockingHit = true;
			}

			if (bDrawDebugLine)
			{
				DrawDebugLine(GetWorld(), WorldLocation, End, FColor::Red, false, 5.0f, 0, 3.0f);
			}

			if (bBlockingHit)
			{
				return HitResult;
			}
		}
	}
	return FHitResult();
}

FHitResult USKGMapEditorHandlerComponent::MouseTraceMulti(const float Distance, bool& bHitGizmo, const ECollisionChannel CollisionChannel, const bool bDrawDebugLine)
{
	FHitResult HitResult;
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FVector WorldLocation;
		FVector WorldDirection;
		if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			FVector End = WorldLocation + WorldDirection * Distance;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetOwner());
			TArray<FHitResult> HitResults;
			if (GetWorld()->LineTraceMultiByChannel(HitResults, WorldLocation, End, CollisionChannel, Params))
			{
				if (Gizmo.IsValid())
				{
					for (FHitResult& Result : HitResults)
					{
						if (Result.GetActor() == Gizmo)
						{
							bHitGizmo = true;
							HitResult = Result;
						}
					}
				}
			}
			else
			{
				if (HitResults.Num())
				{
					return HitResults[0];
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Mouse Trace"));
			if (bDrawDebugLine)
			{
				DrawDebugLine(GetWorld(), WorldLocation, End, FColor::Red, false, 5.0f, 0, 3.0f);
			}
		}
	}
	return HitResult;
}

void USKGMapEditorHandlerComponent::Init()
{
	if (APawn* OwningPawn = GetOwner<APawn>())
	{
		if (OwningPawn->IsLocallyControlled() && GizmoClass)
		{UE_LOG(LogTemp, Warning, TEXT("CREATING GIZMO Init"));
			Gizmo = GetWorld()->SpawnActor<ASKGMapEditorGizmo>(GizmoClass);
			Gizmo->SetHandler(this);
		}
	}
}

void USKGMapEditorHandlerComponent::SetReturnPawn(APawn* Pawn)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		ReturnPawn = Pawn;
	}
}

bool USKGMapEditorHandlerComponent::Server_UnpossessToReturnPawn_Validate()
{
	return true;
}

void USKGMapEditorHandlerComponent::Server_UnpossessToReturnPawn_Implementation()
{
	UnpossessToReturnPawn();
}

void USKGMapEditorHandlerComponent::UnpossessToReturnPawn()
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		if (GetOwner() && ReturnPawn.IsValid())
		{
			GetOwner<APawn>()->GetController()->Possess(ReturnPawn.Get());
			GetOwner()->Destroy();
		}
	}
	else
	{
		Server_UnpossessToReturnPawn();
	}
	if (Gizmo.IsValid())
	{
		Gizmo->HideGizmo(true);
		Gizmo->ClearGizmo();
	}
}

void USKGMapEditorHandlerComponent::SetGizmo()
{
	if (Gizmo.IsValid())
	{
		
	}
}

bool USKGMapEditorHandlerComponent::Server_ReplicateTransform_Validate(AActor* Actor, FTransform Transform)
{
	return true;
}

void USKGMapEditorHandlerComponent::Server_ReplicateTransform_Implementation(AActor* Actor, FTransform Transform)
{
	if (Actor)
	{
		Actor->SetActorTransform(Transform);
	}
}

void USKGMapEditorHandlerComponent::SetActor(AActor* Actor)
{
	if (Actor)
	{
		if (!Actor->IsRootComponentMovable())
		{
			return;
		}
		if (Actor->Implements<USKGMapEditorInterface>())
		{
			ISKGMapEditorInterface::Execute_OnGrabbed(Actor);
		}
	}
	
	CurrentActor = Actor;

	if (CurrentActor)
	{
		FSKGMapEditorSteps UndoStep;
		UndoStep.Actor = CurrentActor;
		UndoSteps.Add(UndoStep);
	}
	
	if (Gizmo.IsValid())
	{
		if (CurrentActor)
		{
			Gizmo->SnapToActor(CurrentActor);
			Gizmo->HideGizmo(false);
		}
		else
		{
			Gizmo->HideGizmo(true);
			Gizmo->ClearGizmo();
		}
	}
}

FHitResult USKGMapEditorHandlerComponent::MouseTrace(float Distance, bool& bHitGizmo, bool bDrawDebugLine)
{
	return MouseTraceMulti(Distance, bHitGizmo, TraceCollisionChannel, bDrawDebugLine);
}

void USKGMapEditorHandlerComponent::Grab()
{
	bool bHitGizmo = false;
	FHitResult HitResult = MouseTrace(100000.0f, bHitGizmo, false);

	if (bHitGizmo && Gizmo.IsValid())
	{
		Gizmo->HitGizmo(HitResult);
	}
	else
	{
		SetActor(HitResult.GetActor());
	}
}

void USKGMapEditorHandlerComponent::Release()
{
	if (Gizmo.IsValid())
	{
		Gizmo->ReleaseGizmo();
		if (CurrentActor)
		{
			if (CurrentActor->Implements<USKGMapEditorInterface>())
			{
				ISKGMapEditorInterface::Execute_OnRelease(CurrentActor);
			}
			int32 Index = UndoSteps.Num() - 1;
			if (Index >= 0)
			{
				UndoSteps[Index].Transforms.Add(CurrentActor->GetActorTransform());
			}
		}
	}
}

void USKGMapEditorHandlerComponent::ReplicateActor()
{
	if (CurrentActor)
	{
		if (!CurrentActor->GetActorTransform().Equals(CurrentActorTransform))
		{
			CurrentActorTransform = CurrentActor->GetActorTransform();
			if (!HasAuthority())
			{
				Server_ReplicateTransform(CurrentActor, CurrentActor->GetActorTransform());
			}
			else
			{
				if (CurrentActor->Implements<USKGMapEditorInterface>())
				{
					ISKGMapEditorInterface::Execute_OnScaleChanged(CurrentActor, CurrentActor->GetActorTransform().GetScale3D());
				}
			}
		}
	}
}

void USKGMapEditorHandlerComponent::ShowMovement()
{
	if (Gizmo.IsValid())
	{
		Gizmo->ShowMovement();
	}
}

void USKGMapEditorHandlerComponent::ShowRotation()
{
	if (Gizmo.IsValid())
	{
		Gizmo->ShowRotation();
	}
}

void USKGMapEditorHandlerComponent::ShowScale()
{
	if (Gizmo.IsValid())
	{
		Gizmo->ShowScale();
	}
}

void USKGMapEditorHandlerComponent::SetActorTransform(const FTransform NewTransform)
{
	if (CurrentActor)
	{
		CurrentActor->SetActorTransform(NewTransform);
		if (Gizmo.IsValid())
		{
			Gizmo->SnapToActor(CurrentActor);
		}
		ReplicateActor();
	}
}

void USKGMapEditorHandlerComponent::DuplicateActor(bool bSpawnInPlace)
{
	if (IsValid(CurrentActor))
	{
		bool bHitGizmo;
		const FHitResult HitResult = MouseTraceSingle(10000.0f, bHitGizmo, ECC_Visibility, false);

		if (!HitResult.Location.Equals(FVector::ZeroVector))
		{
			if (bSpawnInPlace)
			{
				SpawnActorAtTransform(CurrentActor->GetClass(), CurrentActor->GetTransform());
			}
			else
			{
				SpawnActorAtTransform(CurrentActor->GetClass(), FTransform(HitResult.Location));
			}
		}
	}
}

void USKGMapEditorHandlerComponent::DeselectActor()
{
	CurrentActor = nullptr;
}

void USKGMapEditorHandlerComponent::DeleteActor()
{
	if (CurrentActor)
	{
		if (CurrentActor->Implements<USKGMapEditorInterface>())
		{
			ISKGMapEditorInterface::Execute_OnGrabbed(CurrentActor);
		}
		if (HasAuthority())
		{
			CurrentActor->Destroy();
		}
		else
		{
			Server_DeleteActor(CurrentActor);
		}
		if (Gizmo.IsValid())
		{
			Gizmo->ClearGizmo();
			Gizmo->HideGizmo(true);
		}
		CurrentActor = nullptr;
	}
}

void USKGMapEditorHandlerComponent::Undo()
{
	int32 Index = UndoSteps.Num() - 1;
	if (Index >= 0)
	{
		FSKGMapEditorSteps UndoStep = UndoSteps[Index];
		if (UndoStep.Actor && UndoStep.Transforms.Num() - 1)
		{
			const int32 TransformIndex = UndoStep.Transforms.Num() - 1;
			if (TransformIndex - 1 >= 0)
			{
				UndoStep.Actor->SetActorTransform(UndoStep.Transforms[TransformIndex - 1]);
				UndoStep.Transforms.RemoveAt(TransformIndex);
				UndoSteps[Index] = UndoStep;
				if (Gizmo.IsValid())
				{
					CurrentActor = UndoStep.Actor;
					if (CurrentActor->Implements<USKGMapEditorInterface>())
					{
						ISKGMapEditorInterface::Execute_OnUndo(CurrentActor);
					}
					Gizmo->SnapToActor(CurrentActor);
					ReplicateActor();
				}
			}
			else
			{
				UndoSteps.RemoveAt(Index);
				Undo();
			}
		}
		else
		{
			UndoSteps.RemoveAt(Index);
			Undo();
		}
	}
}

void USKGMapEditorHandlerComponent::SetSnapAmount(FSKGMapEditorSnapping SnappingAmounts)
{
	SnapAmount.Location = SnappingAmounts.Location >= 0 ? SnappingAmounts.Location : 0.0f;
	SnapAmount.Rotation = SnappingAmounts.Rotation >= 0 ? SnappingAmounts.Rotation : 0.0f;
	SnapAmount.Scale = SnappingAmounts.Scale >= 0 ? SnappingAmounts.Scale : 0.0f;
}

bool USKGMapEditorHandlerComponent::Server_SpawnActor_Validate(TSubclassOf<AActor> ActorClass)
{
	return true;
}

void USKGMapEditorHandlerComponent::Server_SpawnActor_Implementation(TSubclassOf<AActor> ActorClass)
{
	SpawnActor(ActorClass);
}

void USKGMapEditorHandlerComponent::SpawnActor(TSubclassOf<AActor> ActorClass)
{
	if (ActorClass)
	{
		if (HasAuthority())
		{
			if (GetOwner())
			{
				FTransform Transform = GetOwner()->GetActorTransform();
				FVector SpawnLocation = Transform.GetLocation() + Transform.Rotator().Vector() * 500.0f;
				Transform.SetLocation(SpawnLocation);

				FVector End = SpawnLocation;
				End.Z -= 10000.0f;
				FHitResult HitResult;
				if (GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation, End, ECollisionChannel::ECC_Visibility))
				{
					SpawnLocation = HitResult.Location;
				}

				Transform.SetRotation(FRotator::ZeroRotator.Quaternion());
				
				CurrentActor = GetWorld()->SpawnActor<AActor>(ActorClass, Transform);
				OnRep_CurrentActor();
			}
		}
		else
		{
			Server_SpawnActor(ActorClass);
		}
	}
}

bool USKGMapEditorHandlerComponent::Server_SpawnActorAtTransform_Validate(TSubclassOf<AActor> ActorClass, const FTransform& Transform)
{
	return true;
}

void USKGMapEditorHandlerComponent::Server_SpawnActorAtTransform_Implementation(TSubclassOf<AActor> ActorClass, const FTransform& Transform)
{
	SpawnActorAtTransform(ActorClass, Transform);
}

void USKGMapEditorHandlerComponent::SpawnActorAtTransform(TSubclassOf<AActor> ActorClass, const FTransform SpawnTransform)
{
	if (ActorClass)
	{
		if (HasAuthority())
		{
			if (GetOwner())
			{
				CurrentActor = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnTransform);
				OnRep_CurrentActor();
			}
		}
		else
		{
			Server_SpawnActorAtTransform(ActorClass, SpawnTransform);
		}
	}
}

bool USKGMapEditorHandlerComponent::Server_DeleteActor_Validate(AActor* Actor)
{
	return true;
}

void USKGMapEditorHandlerComponent::Server_DeleteActor_Implementation(AActor* Actor)
{
	if (Actor)
	{
		Actor->Destroy();
	}
}