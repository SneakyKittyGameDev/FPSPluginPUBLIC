// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Actors/FirearmParts/SKGPart.h"
#include "SKGHandguard.generated.h"

class ASKGForwardGrip;
class ASKGFirearm;
class AFPSTemplateFirearm_Sight;
class UAnimSequence;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGHandguard : public ASKGPart
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGHandguard();

protected:
	// Socket for the Left Hand IK (to position the left hand)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Sockets")
	FName HandGripSocket;
	// Grip animation/pose to play for the left hand when this grip is in use. This is optional
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	UAnimSequence* GripAnimation;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Animation")
	UAnimSequence* GetGripAnimation() const;
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Animation")
	FTransform GetGripTransform() const;
};