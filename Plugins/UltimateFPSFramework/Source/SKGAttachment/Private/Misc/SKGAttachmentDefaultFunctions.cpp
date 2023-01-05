// Copyright 2022, Dakota Dawe, All rights reserved


#include "Misc/SKGAttachmentDefaultFunctions.h"
#include "Components/SKGAttachmentComponent.h"
#include "Components/SKGAttachmentManager.h"

void USKGAttachmentDefaultFunctions::OnAttachmentUpdated(USKGAttachmentComponent* OwningAttachmentComponent)
{
	if (OwningAttachmentComponent && OwningAttachmentComponent->GetAttachmentManager())
	{
		OwningAttachmentComponent->GetAttachmentManager()->CacheAttachmentComponents();
	}
}

void USKGAttachmentDefaultFunctions::OnAttachmentRemoved(TArray<USKGAttachmentComponent*>& CachedAttachmentComponents)
{
	TArray<AActor*> ActorsToDestroy;
	ActorsToDestroy.Reserve(CachedAttachmentComponents.Num());
	for (int i = 0; i < CachedAttachmentComponents.Num(); ++i)
	{
		const USKGAttachmentComponent* AttachmentComponent = CachedAttachmentComponents[i];
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

void USKGAttachmentDefaultFunctions::OnAttachmentOverlapped(AActor* Actor, TArray<USKGAttachmentComponent*>& CachedAttachmentComponents)
{
	if (IsValid(Actor))
	{
		for (const USKGAttachmentComponent* AttachmentComponent : CachedAttachmentComponents)
		{
			if (AttachmentComponent && IsValid(AttachmentComponent->GetAttachment()))
			{
				AttachmentComponent->GetAttachment()->SetActorHiddenInGame(true);
			}
		}
		Actor->SetActorHiddenInGame(true);
	}
}

void USKGAttachmentDefaultFunctions::OnAttachmentOverlapEnd(AActor* Actor, TArray<USKGAttachmentComponent*>& CachedAttachmentComponents)
{
	for (const USKGAttachmentComponent* AttachmentComponent : CachedAttachmentComponents)
	{
		if (AttachmentComponent && IsValid(AttachmentComponent->GetAttachment()))
		{
			AttachmentComponent->GetAttachment()->SetActorHiddenInGame(false);
		}
	}
	Actor->SetActorHiddenInGame(false);
}
