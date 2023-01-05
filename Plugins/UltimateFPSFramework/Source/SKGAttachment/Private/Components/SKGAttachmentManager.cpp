// Copyright 2022, Dakota Dawe, All rights reserved


#include "Components/SKGAttachmentManager.h"
#include "Components/SKGAttachmentComponent.h"
#include "Interfaces/SKGAttachmentInterface.h"

#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

// Sets default values for this component's properties
USKGAttachmentManager::USKGAttachmentManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	bSpawnDefaultPartsFromPreset = true;
	InitalOnAttachmentUpdatedDelay = 0.25f;
	bInitDone = false;
}

// Called when the game starts
void USKGAttachmentManager::BeginPlay()
{
	Super::BeginPlay();
}

void USKGAttachmentManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USKGAttachmentManager, AttachmentComponents);
}

void USKGAttachmentManager::SetMasterPoseComponent(USkeletalMeshComponent* SkeletalMeshComponent) const
{
	if (SkeletalMeshComponent)
	{
		AActor* OwningActor = GetOwner();
		if (OwningActor && OwningActor->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()))
		{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 1
			SkeletalMeshComponent->SetLeaderPoseComponent(Cast<USkeletalMeshComponent>(Cast<USkeletalMeshComponent>(ISKGAttachmentInterface::Execute_GetMesh(OwningActor))));
#else
			SkeletalMeshComponent->SetMasterPoseComponent(Cast<USkeletalMeshComponent>(Cast<USkeletalMeshComponent>(ISKGAttachmentInterface::Execute_GetMesh(OwningActor))));
#endif
		}
	}
}

bool USKGAttachmentManager::AttachmentExists(USKGAttachmentComponent* AttachmentComponent)
{
	for (const USKGAttachmentComponent* Component : AttachmentComponents)
	{
		if (AttachmentComponent == Component)
		{
			return true;
		}
	}

	return false;
}

void USKGAttachmentManager::OnRep_AttachmentComponents()
{
	CacheAttachmentComponents();
}

void USKGAttachmentManager::DestroyAllAttachments()
{
	if (HasAuthority())
	{
		TArray<AActor*> ActorsToDestroy;
		ActorsToDestroy.Reserve(CachedAttachmentComponents.Num());
		for (int i = 0; i < CachedAttachmentComponents.Num(); ++i)
		{
			USKGAttachmentComponent* AttachmentComponent = CachedAttachmentComponents[i];
			if (AttachmentComponent)
			{
				AActor* Attachment = AttachmentComponent->GetAttachment();
				if (IsValid(Attachment))
				{
					ActorsToDestroy.Emplace(Attachment);
				}
			}
		}
		CachedAttachmentComponents.Empty();
		for (AActor* Actor : ActorsToDestroy)
		{
			Actor->Destroy();
		}
	}
}

void USKGAttachmentManager::AddAttachment(USKGAttachmentComponent* AttachmentComponent)
{
	if (IsValid(AttachmentComponent))
	{
		AttachmentComponents.Add(AttachmentComponent);
		OnRep_AttachmentComponents();
	}
}

TArray<USKGAttachmentComponent*> USKGAttachmentManager::GetAllAttachmentComponents(bool bReCache)
{
	if (bReCache)
	{
		CacheAttachmentComponents();
	}
	
	return CachedAttachmentComponents;
}

void USKGAttachmentManager::CacheAttachmentComponents(float OverrideDelay)
{
	CachedAttachmentComponents.Empty();
	CachedAttachmentComponents.Append(AttachmentComponents);
	for (USKGAttachmentComponent* AttachmentComponent : AttachmentComponents)
	{
		if (AttachmentComponent)
		{
			AActor* Actor = AttachmentComponent->GetAttachment();
			if (IsValid(Actor) && Actor->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()))
			{
				CachedAttachmentComponents.Append(ISKGAttachmentInterface::Execute_GetAllAttachmentComponents(Actor, true));
			}
		}
	}

	const AActor* OwningActor = GetOwner();
	if (OwningActor && OwningActor->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()))
	{
		if (!bInitDone && (OverrideDelay > 0.0f || InitalOnAttachmentUpdatedDelay > 0.0f))
		{
			if (OverrideDelay > InitalOnAttachmentUpdatedDelay)
			{
				InitalOnAttachmentUpdatedDelay = OverrideDelay;
			}
			GetWorld()->GetTimerManager().SetTimer(TCacheTimerHandle, this, &USKGAttachmentManager::CallOnAttachmentUpdated, InitalOnAttachmentUpdatedDelay, false);
		}
		else
		{
			CallOnAttachmentUpdated();
		}
	}
}

void USKGAttachmentManager::CallOnAttachmentUpdated()
{
	bInitDone = true;
	if (GetOwner())
	{
		ISKGAttachmentInterface::Execute_OnAttachmentUpdated(GetOwner());
	}
}

template <typename Type>
TArray<Type*> USKGAttachmentManager::GetAttachmentsOfType()
{
	TArray<Type*> Attachments;
	
	for (const USKGAttachmentComponent* Component : AttachmentComponents)
	{
		if (Component)
		{
			if (Type* Casted = Component->GetAttachment<Type>())
			{
				Attachments.Add(Casted);
			}
		}
	}

	return Attachments;
}
