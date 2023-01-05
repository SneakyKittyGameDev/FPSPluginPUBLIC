// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "SKGAttachmentActor.h"
#include "GameFramework/Actor.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "Components/MeshComponent.h"
#include "Interfaces/SKGFirearmAttachmentsInterface.h"
#include "SKGPart.generated.h"

class USKGCharacterComponent;

#define MAX_PartStack 10

UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGPart : public ASKGAttachmentActor, public ISKGFirearmAttachmentsInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGPart();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | Default")
	FSKGFirearmPartStats PartStats;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | Default")
	FSKGFirearmPartData PartData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | Default")
	ESKGPartType PartType;
	
	// Trace channel to use for firearm collision, Highly recommended you create your own in your project.
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	TEnumAsByte<ECollisionChannel> FirearmCollisionChannel;
	
	
	TWeakObjectPtr<AActor> OwningActor;
	TWeakObjectPtr<USKGCharacterComponent> OwningCharacterComponent;
	TWeakObjectPtr<ASKGFirearm> OwningFirearm;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void PostInitProperties() override;
	virtual void Destroyed() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | Events")
	void OnUse();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Use();
	virtual void Server_Use_Implementation();

	void CacheCharacterAndFirearm();
	
public:
	// FIREARM ATTACHMENTS INTERFACE
	virtual FSKGFirearmPartStats GetPartStats_Implementation() override;
	virtual FSKGFirearmPartData GetPartData_Implementation() const override { return PartData; }
	virtual ESKGPartType GetPartType_Implementation() const override { return PartType; }
	virtual AActor* GetOwningActor_Implementation() override;
	virtual USKGCharacterComponent* GetOwningCharacterComponent_Implementation() override;
	virtual UMeshComponent* GetPartMesh_Implementation() const override { return AttachmentMesh.Get(); }
	// END OF FIREARM ATTACHMENTS INTERFACE

	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Default")
	ASKGFirearm* GetOwningFirearm();
	UFUNCTION(BlueprintNativeEvent, Category = "SKGFPSFramework | Use")
	void Use();
	virtual void Use_Implementation();
};
