//Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "GameFramework/Actor.h"
#include "SKGAttachmentDataTypes.generated.h"

extern const FName SKGATTACHMENT_API SKGATTACHMENT_OVERLAP_TAG_NAME;
extern const FName SKGATTACHMENT_API SKGATTACHMENT_MESH_TAG_NAME;

USTRUCT(BlueprintType)
struct FSKGAttachmentAttachList
{
	GENERATED_BODY()
	UPROPERTY()
	FString ComponentName;
	UPROPERTY()
	TSubclassOf<AActor> ParentRootActor;
	UPROPERTY()
	TSubclassOf<AActor> ParentAttachment;
	UPROPERTY()
	TSubclassOf<AActor> Attachment;
	UPROPERTY()
	float AttachmentOffset = 0.0f;
	
	bool bHasBeenCreated = false;
};

USTRUCT(BlueprintType)
struct FSKGAttachmentParent
{
	GENERATED_BODY()
	UPROPERTY()
	TSubclassOf<AActor> AttachmentClass;
	UPROPERTY()
	TArray<FSKGAttachmentAttachList> AttachmentList;
};

USTRUCT(BlueprintType)
struct FSKGAttachmentOverlap
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGAttachment | Overlap")
	AActor* Attachment = nullptr;
	bool bIsOverlapped = false;
	
	FSKGAttachmentOverlap(){}
	FSKGAttachmentOverlap(AActor* INAttachment, bool bOverlapping)
	{
		Attachment = INAttachment;
		bIsOverlapped = bOverlapping;
	}

	bool operator == (const FSKGAttachmentOverlap& PartOverlap) const
	{
		return Attachment == PartOverlap.Attachment;
	}
};

USTRUCT(BlueprintType)
struct FSKGAttachmentComponentMovementPreview
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
	bool bShowMinMaxLine = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowMinMaxLine"))
	bool bDrawLineOnTop = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowMinMaxLine"))
	FLinearColor LineColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowMinMaxLine"))
	float LineThickness = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
	bool bShowMinMaxPoints = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowMinMaxPoints"))
	bool bDrawMinMaxPointsOnTop = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowMinMaxPoints"))
	FLinearColor MinMaxPointColor = FLinearColor::Green;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowMinMaxPoints"))
	float MinMaxPointRadius = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowMinMaxPoints"))
	float MinMaxPointThickness = 0.04f;

	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
	bool bShowSnapPoints = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowSnapPoints"))
	bool bDrawSnapPointsOnTop = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowSnapPoints"))
	FLinearColor SnapPointColor = FLinearColor::White;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowSnapPoints"))
	float SnapPointRadius = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowSnapPoints"))
	float SnapPointThickness = 0.04f;
	
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment", meta = (EditCondition = "bShowMinMaxLine || bShowMinMaxPoints || bShowSnapPoints"))
	float HeightOffset = 0.2f;
};

USTRUCT(BlueprintType)
struct FSKGAttachmentComponentPreview
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
	FSKGAttachmentComponentMovementPreview LinePreview = FSKGAttachmentComponentMovementPreview();
};

USTRUCT(BlueprintType)
struct FSKGAttachmentPartOverlap
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGAttachment | Overlap")
	AActor* Part = nullptr;
	bool bIsOverlapped = false;
	
	FSKGAttachmentPartOverlap(){}
	FSKGAttachmentPartOverlap(AActor* INPart, bool bOverlapping)
	{
		Part = INPart;
		bIsOverlapped = bOverlapping;
	}

	bool operator== (const FSKGAttachmentPartOverlap& PartOverlap) const
	{
		return Part == PartOverlap.Part;
	}
};

USTRUCT(BlueprintType)
struct FSKGAttachmentRandomDefault
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	bool bRandomAttachment = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment", meta = (EditCondition = "bRandomAttachment"))
	bool bAllowNoSpawn = false;
};

USTRUCT(BlueprintType)
struct FSKGDataAssetAttachment
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	TSubclassOf<AActor> ActorClass = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGAttachment")
	bool bEnabledForUse = true;

	bool operator == (const FSKGDataAssetAttachment& DataAssetAttachment) const
	{
		return ActorClass == DataAssetAttachment.ActorClass && bEnabledForUse == DataAssetAttachment.bEnabledForUse;
	}
};