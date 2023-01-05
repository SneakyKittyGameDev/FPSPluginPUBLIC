// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Actors/FirearmParts/SKGPart.h"
#include "SKGForwardGrip.generated.h"

class UAnimSequence;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGForwardGrip : public ASKGPart
{
	GENERATED_BODY()
public:
	ASKGForwardGrip();
	
protected:
	// Socket for the Left Hand IK (to position the left hand)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Sockets")
	FName HandGripSocket;
	// Grip animation/pose to play for the left hand when this grip is in use. This is optional
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	UAnimSequence* GripAnimation;

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Animation")
	FTransform GetGripTransform() const;

	UAnimSequence* GetGripAnimation() const { return GripAnimation; }
};