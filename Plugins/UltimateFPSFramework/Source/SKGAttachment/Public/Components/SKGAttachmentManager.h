// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SKGAttachmentManager.generated.h"

class USKGAttachmentComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKGATTACHMENT_API USKGAttachmentManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USKGAttachmentManager();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment | Default")
	bool bSpawnDefaultPartsFromPreset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment | Default")
	float InitalOnAttachmentUpdatedDelay;
	bool bInitDone;
	
	UPROPERTY(ReplicatedUsing = OnRep_AttachmentComponents)
	TArray<USKGAttachmentComponent*> AttachmentComponents;
	UFUNCTION()
	void OnRep_AttachmentComponents();
	TArray<USKGAttachmentComponent*> CachedAttachmentComponents;

	FTimerHandle TCacheTimerHandle;
	void CallOnAttachmentUpdated();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority() ? true : false; }
	
public:
	bool GetShouldSpawnDefaultOnPreset() const { return bSpawnDefaultPartsFromPreset; }
	
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | Attachment")
	void DestroyAllAttachments();
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | Attachment")
	void AddAttachment(USKGAttachmentComponent* AttachmentComponent);
	UFUNCTION(BlueprintPure, Category = "SKGAttachment | Attachment")
	TArray<USKGAttachmentComponent*> GetAllAttachmentComponents(bool bReCache);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | Attachment")
	void CacheAttachmentComponents(float OverrideDelay = 0.0f);
	UFUNCTION(BlueprintPure, Category = "SKGAttachment | Attachment")
	TArray<USKGAttachmentComponent*> GetAttachmentComponents() const { return AttachmentComponents; }
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment | Attachment")
	bool AttachmentExists(USKGAttachmentComponent* AttachmentComponent);

	void SetMasterPoseComponent(USkeletalMeshComponent* SkeletalMeshComponent) const;
	
	template <typename Type>
	TArray<Type*> GetAttachmentsOfType();
};
