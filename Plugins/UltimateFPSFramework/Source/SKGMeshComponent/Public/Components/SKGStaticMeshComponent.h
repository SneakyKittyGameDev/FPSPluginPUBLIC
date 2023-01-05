// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "SKGStaticMeshComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=(Rendering, Common), hidecategories=(Object,Activation,"Components|Activation"), ShowCategories=(Mobility), editinlinenew, meta=(BlueprintSpawnableComponent))
class SKGMESHCOMPONENT_API USKGStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
public:
	USKGStaticMeshComponent();

protected:
	uint8 MaxOwnerAttempts = 10;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	float DesiredFOV = 90.0f;
	
	TWeakObjectPtr<APlayerController> PlayerController;

	virtual void BeginPlay() override;
	virtual FMatrix GetRenderMatrix() const override;
};
