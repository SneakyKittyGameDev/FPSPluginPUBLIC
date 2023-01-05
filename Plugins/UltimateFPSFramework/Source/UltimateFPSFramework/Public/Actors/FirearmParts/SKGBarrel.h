// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Actors/FirearmParts/SKGPart.h"
#include "Interfaces/SKGMuzzleInterface.h"
#include "SKGBarrel.generated.h"

class ASKGProjectile;
class ASKGMuzzle;
class ASKGFirearm;
class UNiagaraSystem;
class UMaterialInstanceDynamic;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGBarrel : public ASKGPart, public ISKGMuzzleInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGBarrel();

protected:
	// Socket for the muzzle (end of barrel where projectile gets launched)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Sockets")
	FName MuzzleSocket;
	// Random niagara systems for this muzzle, to be used for shooting effects like muzzle flash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | Default")
	TArray<UNiagaraSystem*> FireNiagaraSystems;
	// Whether or not to use the muzzle temperature system as used in the example m4 barrel
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature")
	bool bUseMuzzleTemperature;
	// How much to increase the temperature by per shot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature", meta = (EditCondition = "bUseMuzzleTemperature", EditConditionHides))
	float TemperatureIncreasePerShot;
	// How much to decrease the temperature by on tick (how quick to cool barrel)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature", meta = (EditCondition = "bUseMuzzleTemperature", EditConditionHides))
	float TemperatureDecreasePerTick;
	// Min value for temperature to affect material
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature", meta = (EditCondition = "bUseMuzzleTemperature", EditConditionHides))
	float MinTemperatureEffect;
	// Max temperature for full effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature", meta = (EditCondition = "bUseMuzzleTemperature", EditConditionHides))
	float MaxTemperatureEffect;
	// Rate at which the tick runs to cool barrel
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature", meta = (EditCondition = "bUseMuzzleTemperature", EditConditionHides))
	float TemperatureCooldownTickRate;

	TWeakObjectPtr<UMaterialInstanceDynamic> MuzzleMaterial;
	float MuzzleTemperature;
	float MuzzleTemperatureNormalized;

	void NormalizeTemperature();
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | Effects")
	void MuzzleTemperatureChanged(float NewMuzzleTemperature);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Default")
	bool DoesMuzzleSocketExist() const;
	
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Effects")
	void FiredShot();

	// START OF MUZZLE INTERFACE
	virtual UNiagaraSystem* GetFireNiagaraSystem_Implementation() override;
	virtual FTransform GetMuzzleSocketTransform_Implementation() override;
};