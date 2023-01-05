// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SKGAttachmentInterface.h"
#include "SKGAttachmentActor.generated.h"

class USKGAttachmentComponent;
class UShapeComponent;

UCLASS()
class SKGATTACHMENT_API ASKGAttachmentActor : public AActor, public ISKGAttachmentInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGAttachmentActor();

protected:
	TWeakObjectPtr<UMeshComponent> AttachmentMesh;
	
	// Used for the stock (example) which has a negative minimum offset because it slides backwards in reverse
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment | Default")
	bool bInvertMovingOffset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGAttachment | Default")
	bool bBeCheckedForOverlap;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment | Default")
	FTransform AttachmentAttachOffset;
	
	UPROPERTY(BlueprintReadOnly, Category = "SKGAttachment | Customization")
	float OffsetSnapDistance;
	TWeakObjectPtr<UShapeComponent> OverlapCheckComponent;
	
	float AccumulatedOffset;
	float OldAccumulatedOffset;
	float MinOffset;
	float MaxOffset;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentOffset)
	float CurrentOffset;
	UFUNCTION()
	void OnRep_CurrentOffset();
	void ApplyAttachmentAttachOffset();
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetOffset(float Offset);
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "SKGAttachment | Attachments")
	TArray<USKGAttachmentComponent*> AttachmentComponents;
	UPROPERTY(ReplicatedUsing = OnRep_OwningAttachmentComponent)
	USKGAttachmentComponent* OwningAttachmentComponent;
	UFUNCTION()
	virtual void OnRep_OwningAttachmentComponent();

	TArray<USKGAttachmentComponent*> CachedAttachmentComponents;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment | AssetID")
	FPrimaryAssetType AssetType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment | AssetID")
	FName AssetName;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId(AssetType, AssetName); }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetupAttachmentMesh();
	virtual void MarkComponentsAsPendingKill() override;
	virtual void OnRep_AttachmentReplication() override;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintPure, Category = "SKGAttachment | Customization")
	bool IsAttachmentAttached(const AActor* AttachmentToCheck);

	void CacheAttachments();
	
public:
	// ATTACHMENT INTERFACE
	virtual void SetOwningAttachmentComponent_Implementation(USKGAttachmentComponent* AttachmentComponent) override { OwningAttachmentComponent = AttachmentComponent; }
	virtual USKGAttachmentComponent* GetOwningAttachmentComponent_Implementation() override { return OwningAttachmentComponent; }
	virtual UMeshComponent* GetMesh_Implementation() override { return AttachmentMesh.Get(); }
	virtual TArray<USKGAttachmentComponent*> GetAttachmentComponents_Implementation() override { return AttachmentComponents; }
	virtual TArray<USKGAttachmentComponent*> GetAllAttachmentComponents_Implementation(bool bReCache) override;
	
	virtual void OnAttachmentUpdated_Implementation() override;
	virtual void OnAttachmentRemoved_Implementation() override;

	virtual bool AddOffset_Implementation(float Offset) override;
	virtual bool SetOffset_Implementation(float Offset) override;
	virtual FTransform GetAttachmentAttachOffset_Implementation() const override { return AttachmentAttachOffset; }
	virtual void FinishedMovingAttachment_Implementation() override;
	virtual bool IsMovementInverted_Implementation() const override { return bInvertMovingOffset; }
	virtual void SetMinMaxOffset_Implementation(float Min, float Max) override;
	virtual void SetSnapDistance_Implementation(float Distance) override;
	virtual float GetAttachmentOffset_Implementation() const override { return CurrentOffset; }

	virtual bool ShouldBeCheckedForOverlap_Implementation() override { return bBeCheckedForOverlap; }
	virtual void EnableGenerateOverlapEvents_Implementation(bool bEnable) override;
	virtual void OnAttachmentOverlapped_Implementation() override;
	virtual void OnAttachmentOverlapEnd_Implementation() override;
	// END OF ATTACHMENT INTERFACE
};
