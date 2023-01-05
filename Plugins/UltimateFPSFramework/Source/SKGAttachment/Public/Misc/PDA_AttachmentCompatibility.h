// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGAttachmentDataTypes.h"
#include "PDA_AttachmentCompatibility.generated.h"


class USKGAttachmentInterface;
class USkeletalMesh;
class UStaticMesh;

UCLASS()
class SKGATTACHMENT_API UPDA_AttachmentCompatibility : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPDA_AttachmentCompatibility();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment | Default")
	TArray<FSKGDataAssetAttachment> Attachments;

	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | DataAssets")
	bool EnableAttachment(FSKGDataAssetAttachment& Attachment, const bool bEnable);
};
