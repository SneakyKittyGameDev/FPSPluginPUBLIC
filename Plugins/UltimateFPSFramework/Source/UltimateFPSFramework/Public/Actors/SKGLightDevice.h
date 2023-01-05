// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SKGInfraredInterface.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "SKGLightDevice.generated.h"

class ULightComponent;

class USKGCharacterComponent;
UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGLightDevice : public AActor, public ISKGInfraredInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGLightDevice();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	TArray<ESKGLightMode> LightModes;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	FSKGLightDeviceSettings LightDeviceSettings;
	
	TWeakObjectPtr<USKGCharacterComponent> CharacterComponent;
	TArray<TWeakObjectPtr<ULightComponent>> LightComponents;

	UPROPERTY(ReplicatedUsing = OnRep_InfraredMode)
	bool bInfraredMode;
	UFUNCTION()
	void OnRep_InfraredMode();
	
	uint8 LightModeIndex;
	UPROPERTY(ReplicatedUsing = OnRep_LightMode)
	ESKGLightMode LightMode;
	UFUNCTION()
	void OnRep_LightMode();

	UFUNCTION(BlueprintImplementableEvent)
	void LightModeChanged(ESKGLightMode NewMode);

	void SetLightsOn(bool bOn);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	void SetupCharacterComponent();
	void SetupLightComponents();
	bool DoIHaveNightVisionOn() const;

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetLightMode(ESKGLightMode Mode);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetInfraredMode(bool bInfrared);

public:
	virtual void HandleIRDevice_Implementation() override;
	virtual void SetInfraredMode_Implementation(bool bInfrared) override;
	
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | LightDevice")
	void CycleModes(const bool bDownArray = true, const uint8 ForceIndex = 254);
};
