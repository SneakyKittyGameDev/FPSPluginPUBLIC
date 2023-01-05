// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SKGHoleComponentDataTypes.h"
#include "SKGHoleComponent.generated.h"

class UMaterialInstanceDynamic;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKGHOLECOMPONENT_API USKGHoleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USKGHoleComponent();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	TArray<FSKGHoleMaterialSetting> MaterialSettings;

	TArray<FSKGHoleMaterial> HoleMaterials;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FVector> HoleLocations;
	uint8 HoleIndex;
	
	int32 GetFaceIndex(const FHitResult& HitLocation) const;

	UFUNCTION()
	void SetupMaterials();
public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Hole")
	void AddHole(const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Hole")
	bool DidImpactHitHole(const FHitResult& HitResult, float Tolerance = 1.0f);
};
