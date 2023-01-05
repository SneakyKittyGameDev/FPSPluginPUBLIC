// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SKGAttachmentFunctionLibrary.generated.h"

class USKGAttachmentManager;
class USKGAttachmentComponent;
class UMeshComponent;

UCLASS()
class SKGATTACHMENT_API USKGAttachmentFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static USKGAttachmentManager* GetOwningAttachmentManager(AActor* Actor);
	static USKGAttachmentManager* GetDirectOwnersAttachmentManager(AActor* Actor);
	static TArray<USKGAttachmentComponent*> CreateCacheFromAttachmentComponents(TArray<USKGAttachmentComponent*>& AttachmentComponents);
	static UMeshComponent* SetupAttachmentMesh(AActor* Actor);

	static FSKGAttachmentParent GetAttachmentStruct(AActor* AttachmentParent, FString& Error);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | Serialize")
	static FString SerializeAttachmentParent(AActor* AttachmentParent, FString& Error);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | Serialize")
	static FSKGAttachmentParent DeserializeAttachmentString(const FString& JsonString, FString& Error);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | Serialize")
	static AActor* ConstructAttachmentParent(AActor* WorldActor, FSKGAttachmentParent AttachmentStruct, FString& Error);
};
