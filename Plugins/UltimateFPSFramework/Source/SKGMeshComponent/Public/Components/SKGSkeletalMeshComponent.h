// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "SKGSkeletalMeshComponent.generated.h"

struct FMatrices
{
	FMatrix ViewMatrix;
	FMatrix InverseViewMatrix;
	FMatrix ProjectionMatrix;
	FMatrix InverseProjectionMatrix;
	FMatrix InverseViewProjectionMatrix;
	float NearClippingPlaneDistance;
};

UCLASS(Blueprintable, ClassGroup=(Rendering, Common), hidecategories=Object, config=Engine, editinlinenew, meta=(BlueprintSpawnableComponent))
class SKGMESHCOMPONENT_API USKGSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
public:
	USKGSkeletalMeshComponent();

protected:
	uint8 MaxOwnerAttempts = 10;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	float DesiredFOV = 90.0f;
	
	TWeakObjectPtr<APlayerController> PlayerController;
	
	virtual void BeginPlay() override;
	virtual FMatrix GetRenderMatrix() const override;
};
