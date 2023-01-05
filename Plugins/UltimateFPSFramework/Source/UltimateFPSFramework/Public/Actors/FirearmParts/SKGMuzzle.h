// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Actors/FirearmParts/SKGPart.h"
#include "Interfaces/SKGMuzzleInterface.h"
#include "SKGMuzzle.generated.h"

class UNiagaraSystem;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGMuzzle : public ASKGPart, public ISKGMuzzleInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGMuzzle();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	bool bIsSuppressor;
	// Random niagara systems for this muzzle, to be used for shooting effects like muzzle flash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFPSFramework | Default")
	TArray<UNiagaraSystem*> FireNiagaraSystems;
	// Whether or not to use the muzzle temperature system as used in the example Surefire muzzle devices
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature")
	bool bUseMuzzleTemperature;
	// How much to increase the temperature by per shot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature", meta = (EditCondition = "bUseMuzzleTemperature", EditConditionHides))
	float TemperatureIncreasePerShot;
	// How much to decrease the temperature by on tick (how quick to cool muzzle device)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature", meta = (EditCondition = "bUseMuzzleTemperature", EditConditionHides))
	float TemperatureDecreasePerTick;
	// Min value for temperature to affect material
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature", meta = (EditCondition = "bUseMuzzleTemperature", EditConditionHides))
	float MinTemperatureEffect;
	// Max temperature for full effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature", meta = (EditCondition = "bUseMuzzleTemperature", EditConditionHides))
	float MaxTemperatureEffect;
	// Rate at which the tick runs to cool muzzle
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Temperature", meta = (EditCondition = "bUseMuzzleTemperature", EditConditionHides))
	float TemperatureCooldownTickRate;
	// Socket for the muzzle (end of muzzle where projectile gets launched)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Sockets")
	FName MuzzleSocket;

	UPROPERTY(Replicated)
	ASKGMuzzle* MuzzleAttachment;

	TWeakObjectPtr<UMaterialInstanceDynamic> MuzzleMaterial;
	float MuzzleTemperature;
	float MuzzleTemperatureNormalized;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void NormalizeTemperature();

	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | Effects")
	void MuzzleTemperatureChanged(float NewMuzzleTemperature);
	
public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Default")
	bool DoesMuzzleSocketExist();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Default")
	bool IsSuppressor() const;
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Ammo")
	ASKGMuzzle* GetMuzzleAttachment() { return IsValid(MuzzleAttachment) ? MuzzleAttachment : this; }
	
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Effects")
	void FiredShot();

	// START OF MUZZLE INTERFACE
	virtual UNiagaraSystem* GetFireNiagaraSystem_Implementation() override;
	virtual FTransform GetMuzzleSocketTransform_Implementation() override;
};
