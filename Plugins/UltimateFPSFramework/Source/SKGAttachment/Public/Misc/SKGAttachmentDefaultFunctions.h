// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SKGAttachmentDefaultFunctions.generated.h"

class USKGAttachmentComponent;
/**
 * 
 */
UCLASS()
class SKGATTACHMENT_API USKGAttachmentDefaultFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | DefaultImplementations")
	static void OnAttachmentUpdated(USKGAttachmentComponent* OwningAttachmentComponent);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | DefaultImplementations")
	static void OnAttachmentRemoved(TArray<USKGAttachmentComponent*>& CachedAttachmentComponents);
	/*UFUNCTION(BlueprintCallable, Category = "SKGAttachment | DefaultImplementations")
	static bool AddOffset(float Offset);*/
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | DefaultImplementations")
	static void OnAttachmentOverlapped(AActor* Actor, TArray<USKGAttachmentComponent*>& CachedAttachmentComponents);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | DefaultImplementations")
	static void OnAttachmentOverlapEnd(AActor* Actor, TArray<USKGAttachmentComponent*>& CachedAttachmentComponents);
};
