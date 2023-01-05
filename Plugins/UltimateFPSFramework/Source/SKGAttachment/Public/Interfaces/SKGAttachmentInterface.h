// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SKGAttachmentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class USKGAttachmentInterface : public UInterface
{
	GENERATED_BODY()
};

class USKGAttachmentComponent;
class UMeshComponent;
class USKGAttachmentManager;

class SKGATTACHMENT_API ISKGAttachmentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void OnAttachmentRemoved();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void OnAttachmentUpdated();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void OnNewAttachmentAdded(USKGAttachmentComponent* AttachmentComponent);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	USKGAttachmentManager* GetAttachmentManager() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	TArray<USKGAttachmentComponent*> GetAttachmentComponents();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	TArray<USKGAttachmentComponent*> GetAllAttachmentComponents(bool bReCache);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void SetOwningAttachmentComponent(USKGAttachmentComponent* AttachmentComponent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	USKGAttachmentComponent* GetOwningAttachmentComponent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	UMeshComponent* GetMesh();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void SetMinMaxOffset(float Min, float Max);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void SetSnapDistance(float Distance);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	bool SetOffset(float Offset);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	bool AddOffset(float Offset);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void FinishedMovingAttachment();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	bool IsMovementInverted();
	virtual bool IsMovementInverted_Implementation() const { return false; }
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	float GetAttachmentOffset() const;
	virtual float GetAttachmentOffset_Implementation() const { return 0.0f; }
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	FTransform GetAttachmentAttachOffset() const;
	virtual FTransform GetAttachmentAttachOffset_Implementation() const { return FTransform(); }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	bool ShouldBeCheckedForOverlap();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void EnableGenerateOverlapEvents(bool bEnable);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void OnAttachmentOverlapped();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void OnAttachmentOverlapEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	void SetIsLoadedByPreset();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGAttachment | AttachmentInterface")
	bool GetShouldSpawnDefaultOnPreset();
	virtual bool GetShouldSpawnDefaultOnPreset_Implementation() { return true; }
};
