// Copyright 2022, Dakota Dawe, All rights reserved


#include "Misc/SKGAttachmentFunctionLibrary.h"
#include "Components/SKGAttachmentManager.h"
#include "Interfaces/SKGAttachmentInterface.h"
#include "Components/SKGAttachmentComponent.h"

#include "JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"

USKGAttachmentManager* USKGAttachmentFunctionLibrary::GetOwningAttachmentManager(AActor* Actor)
{
	USKGAttachmentManager* AttachmentManager = nullptr;
	if (IsValid(Actor))
	{
		TArray<USKGAttachmentManager*> AttachmentManagers;
		Actor->GetComponents<USKGAttachmentManager>(AttachmentManagers);
		if (AttachmentManagers.Num() && IsValid(AttachmentManagers[0]))
		{
			AttachmentManager = AttachmentManagers[0];
		}
		else
		{
			AActor* CurrentOwner = Actor;
			for (uint8 i = 0; i < 10; ++i)
			{
				if (IsValid(CurrentOwner))
				{
					CurrentOwner = CurrentOwner->GetOwner();
					if (IsValid(CurrentOwner))
					{
						CurrentOwner->GetComponents<USKGAttachmentManager>(AttachmentManagers);
						if (AttachmentManagers.Num() && IsValid(AttachmentManagers[0]))
						{
							AttachmentManager = AttachmentManagers[0];
							break;
						}
					}
				}
			}	
		}
	}
	
	return AttachmentManager;
}

USKGAttachmentManager* USKGAttachmentFunctionLibrary::GetDirectOwnersAttachmentManager(AActor* Actor)
{
	USKGAttachmentManager* AttachmentManager = nullptr;
	if (IsValid(Actor))
	{
		TArray<USKGAttachmentManager*> AttachmentManagers;
		Actor->GetComponents<USKGAttachmentManager>(AttachmentManagers);
		if (AttachmentManagers.Num() && IsValid(AttachmentManagers[0]))
		{
			AttachmentManager = AttachmentManagers[0];
		}
	}
	
	return AttachmentManager;
}

TArray<USKGAttachmentComponent*> USKGAttachmentFunctionLibrary::CreateCacheFromAttachmentComponents(TArray<USKGAttachmentComponent*>& AttachmentComponents)
{
	TArray<USKGAttachmentComponent*> CachedAttachmentComponents;
	CachedAttachmentComponents.Empty();
	CachedAttachmentComponents.Append(AttachmentComponents);
	for (const USKGAttachmentComponent* Component : AttachmentComponents)
	{
		if (Component)
		{
			AActor* Attachment = Component->GetAttachment();
			if (IsValid(Attachment) && Attachment->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()))
			{
				CachedAttachmentComponents.Append(ISKGAttachmentInterface::Execute_GetAllAttachmentComponents(Attachment, true));
			}
		}
	}
	return CachedAttachmentComponents;
}

UMeshComponent* USKGAttachmentFunctionLibrary::SetupAttachmentMesh(AActor* Actor)
{
	if (IsValid(Actor))
	{
		TArray<UMeshComponent*> ActorMeshComponents;
		Actor->GetComponents<UMeshComponent>(ActorMeshComponents);
		bool bFoundWithTag = false;
		for (UMeshComponent* MeshComponent : ActorMeshComponents)
		{
			if (IsValid(MeshComponent) && MeshComponent->ComponentHasTag(SKGATTACHMENT_MESH_TAG_NAME))
			{
				bFoundWithTag = true;
				//MeshComponent->bUseAttachParentBound = true;
				MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
				MeshComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
				MeshComponent->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
				return MeshComponent;
			}
		}
#if WITH_EDITOR
		if (!bFoundWithTag)
		{
			const FString ErrorString = FString::Printf(TEXT("Attachment: %s has NO valid component with tag %s"), *Actor->GetName(), *SKGATTACHMENT_MESH_TAG_NAME.ToString());
			for (int32 i = 0; i < 10; ++i)
			{
				UKismetSystemLibrary::PrintString(Actor->GetWorld(), ErrorString, true, true, FLinearColor::Red, 10.0f);
			}
		}
		//checkf(bFoundWithTag, TEXT("Attachment: %s has NO valid component with tag %s"), *Actor->GetName(), *SKGATTACHMENT_MESH_TAG_NAME.ToString());
#endif
	}

	return nullptr;
}

FSKGAttachmentParent USKGAttachmentFunctionLibrary::GetAttachmentStruct(AActor* AttachmentParent, FString& Error)
{
	if (IsValid(AttachmentParent))
	{
		if (AttachmentParent->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()))
		{
			USKGAttachmentManager* AttachmentManager = ISKGAttachmentInterface::Execute_GetAttachmentManager(AttachmentParent);
			if (AttachmentManager)
			{
				FSKGAttachmentParent AttachmentStruct;
				AttachmentStruct.AttachmentClass = AttachmentParent->GetClass();
				for (USKGAttachmentComponent* AttachmentComponent : AttachmentManager->GetAllAttachmentComponents(false))
				{
					if (AttachmentComponent)
					{
						AActor* Attachment = AttachmentComponent->GetAttachment();
						if (IsValid(Attachment) && Attachment->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()))
						{
							FSKGAttachmentAttachList AttachmentList;
							AttachmentList.ComponentName = AttachmentComponent->GetName();
							AttachmentList.Attachment = Attachment->GetClass();
							AttachmentList.AttachmentOffset = ISKGAttachmentInterface::Execute_GetAttachmentOffset(Attachment);
							if (Attachment->GetOwner() == AttachmentParent)
							{
								AttachmentList.ParentRootActor = Attachment->GetOwner()->GetClass();
							}
							else
							{
								AttachmentList.ParentAttachment = Attachment->GetOwner()->GetClass();
							}
							AttachmentStruct.AttachmentList.Add(AttachmentList);
						}
					}
				}
				return AttachmentStruct;
			}
			
			Error = FString::Printf(TEXT("AttachmentParent: %s does NOT implement the GetAttachmentManager function"), *AttachmentParent->GetName());
		}
		else
		{
			Error = "AttachmentParent does not implement the SKGAttachmentInterface";	
		}
	}
	else
	{
		Error = "Invalid AttachmentParent";	
	}
	
	return FSKGAttachmentParent();
}

FString USKGAttachmentFunctionLibrary::SerializeAttachmentParent(AActor* AttachmentParent, FString& Error)
{
	if (!IsValid(AttachmentParent))
	{
		Error = "AttachmentParent INVALID";
		return "";
	}
	FString SerializedString;
	if (AttachmentParent)
	{
		FSKGAttachmentParent AttachmentStruct = GetAttachmentStruct(AttachmentParent, Error);
		
		if (Error.IsEmpty() && !FJsonObjectConverter::UStructToJsonObjectString(AttachmentStruct, SerializedString))
		{
			Error = "Could Not Serialize String";
			return "";
		}
	}

	//FPlatformMisc::ClipboardCopy(ToCStr(SerializedString));
	return SerializedString;
}

FSKGAttachmentParent USKGAttachmentFunctionLibrary::DeserializeAttachmentString(const FString& JsonString, FString& Error)
{
	FSKGAttachmentParent AttachmentStruct;
	if (FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &AttachmentStruct, 0, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS DESERIALIZED"));
	}
	else
	{
		Error = "Could NOT Deserialize Json String";
	}

	return AttachmentStruct;
}

AActor* USKGAttachmentFunctionLibrary::ConstructAttachmentParent(AActor* WorldActor, FSKGAttachmentParent AttachmentStruct, FString& Error)
{
	if (!IsValid(WorldActor))
	{
		Error = "WorldActor INVALID";
		return nullptr;
	}
	if (!WorldActor->HasAuthority())
	{
		Error = "NO AUTHORITY";
		return nullptr;
	}
	
	UWorld* World = WorldActor->GetWorld();
	if (!World)
	{
		Error = "Could NOT Get World";
		return nullptr;
	}

	if (AttachmentStruct.AttachmentClass->ImplementsInterface(USKGAttachmentInterface::StaticClass()))
	{
		if (AActor* AttachmentParent = World->SpawnActorDeferred<AActor>(AttachmentStruct.AttachmentClass, FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn))
		{
			ISKGAttachmentInterface::Execute_SetIsLoadedByPreset(AttachmentParent);
			//AttachmentParent->SetIsLoadedFromPreset();
			UGameplayStatics::FinishSpawningActor(AttachmentParent, FTransform());
			USKGAttachmentManager* AttachmentManager = ISKGAttachmentInterface::Execute_GetAttachmentManager(AttachmentParent);
			if (AttachmentManager)
			{
				for (FSKGAttachmentAttachList& AttachmentList : AttachmentStruct.AttachmentList)
				{
					TArray<USKGAttachmentComponent*> AttachmentComponentsComponents = AttachmentManager->GetAllAttachmentComponents(false);
					AttachmentList.bHasBeenCreated = true;
					if (AttachmentList.ParentRootActor)
					{
						for (USKGAttachmentComponent* PartComponent : AttachmentComponentsComponents)
						{
							//UE_LOG(LogTemp, Warning, TEXT("PartComp: %s"), *PartComponent->GetName());
							if (PartComponent && PartComponent->GetName().Equals(AttachmentList.ComponentName))
							{
								if (PartComponent->AddAttachment(AttachmentList.Attachment, true))
								{
									AActor* Attachment = PartComponent->GetAttachment();
									if (Attachment->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()))
									{
										if (ISKGAttachmentInterface::Execute_IsMovementInverted(Attachment))
										{
											AttachmentList.AttachmentOffset *= -1.0f;
										}
										ISKGAttachmentInterface::Execute_SetOffset(Attachment, AttachmentList.AttachmentOffset);
									}
								}
							}
						}
					}
					else
					{
						for (USKGAttachmentComponent* PartComponent : AttachmentComponentsComponents)
						{
							if (PartComponent && PartComponent->GetName().Equals(AttachmentList.ComponentName))
							{
								if (PartComponent->AddAttachment(AttachmentList.Attachment, true))
								{
									AActor* Attachment = PartComponent->GetAttachment();
									if (Attachment && Attachment->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()))
									{
										if (ISKGAttachmentInterface::Execute_IsMovementInverted(Attachment))
										{
											AttachmentList.AttachmentOffset *= -1.0f;
										}
										ISKGAttachmentInterface::Execute_SetOffset(Attachment, AttachmentList.AttachmentOffset);
									}
								}
							}
						}
					}
				}
			}
			return AttachmentParent;
		}
	}
	else
	{
		Error = "AttachmentClass does not implement FPSAttachmentInterface";
		return nullptr;
	}

	return nullptr;
}
