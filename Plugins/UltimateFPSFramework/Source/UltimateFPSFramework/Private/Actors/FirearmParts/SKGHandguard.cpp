// Copyright 2021, Dakota Dawe, All rights reserved


#include "Actors/FirearmParts/SKGHandguard.h"
#include "Actors/SKGFirearm.h"
#include "Actors/FirearmParts/SKGForwardGrip.h"

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASKGHandguard::ASKGHandguard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	NetUpdateFrequency = 1.0f;

	PartStats.Weight = 1.0f;
	PartStats.ErgonomicsChangePercentage = 5.0f;

	PartType = ESKGPartType::Handguard;

	bBeCheckedForOverlap = false;
	
	HandGripSocket = FName("S_LeftHandIK");
}

// Called when the game starts or when spawned
void ASKGHandguard::BeginPlay()
{
	Super::BeginPlay();
}

void ASKGHandguard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

UAnimSequence* ASKGHandguard::GetGripAnimation() const
{
	return GripAnimation;
}

FTransform ASKGHandguard::GetGripTransform() const
{
	return AttachmentMesh.IsValid() ? AttachmentMesh->GetSocketTransform(HandGripSocket) : FTransform();
}