// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Actors/FirearmParts/SKGPart.h"
#include "Interfaces/SKGInfraredInterface.h"
#include "Interfaces/SKGLightLaserInterface.h"
#include "Interfaces/SKGAimInterface.h"
#include "Interfaces/SKGProceduralAnimationInterface.h"
#include "SKGLightLaser.generated.h"

class USpotLightComponent;
class UDecalComponent;
class UStaticMeshComponent;
class UMaterialInstance;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGLightLaser : public ASKGPart, public ISKGInfraredInterface, public ISKGLightLaserInterface,
	public ISKGProceduralAnimationInterface, public ISKGAimInterface
{
	GENERATED_BODY()
	
public:
	ASKGLightLaser();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	USceneComponent* Root;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	UStaticMeshComponent* LaserMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	USpotLightComponent* SpotLightComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	UDecalComponent* LaserDecalComponent;
	// Socket for the laser to be projected from (use for collision/line tracing)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Sockets")
	FName LaserSocket;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Aim")
	bool bIsAimable;
	// Socket that is used for aiming such as S_Aim that is on optics and the lightlaser mesh
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Aim")
	FName AimSocket;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_LightLaserSettings, Category = "SKGFPSFramework | Default")
	FSKGLightLaserSettings LightLaserSettings;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	TArray<ESKGAllLightLaser> LightLaserModeCycle;

	UPROPERTY(ReplicatedUsing = OnRep_InfraredMode)
	bool bInfraredMode;
	UFUNCTION()
	void OnRep_InfraredMode();
	
	UPROPERTY(ReplicatedUsing = OnRep_LightLaserState)
	ESKGLightLaserState LightLaserState;
	UFUNCTION()
	void OnRep_LightLaserState();
	ESKGLightLaserState PreviousBeforeOffLightLaserState;

	UFUNCTION()
	void OnRep_LightLaserSettings();

	uint8 LightLaserModeCycleIndex;
	
	TWeakObjectPtr<USKGCharacterComponent> CharacterComponent;

	virtual void PostInitProperties() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void SetupAttachmentMesh() override;

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetLightLaserState(ESKGLightLaserState State);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetLaserColorIndex(uint8 Index);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetLightIntensityIndex(uint8 Index);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetInfraredMode(bool bInfrared);

	/* Gets called anytime the light visibility is toggled.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | LightLaser")
	void OnLightToggled(bool bOn);
	/* Gets called anytime the light power mode changes.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | LightLaser")
	void OnLightPowerChanged(float NewPower);
	/* Gets called when you change the Infrared mode on your device or by enabling night vision on your CharacterComponent.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | LightLaser")
	void OnInfraredModeChanged(bool bInInfrared, bool bIsNightVisionOn);

	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Infrared")
	bool DoIHaveNightVisionOn() const;
	
	void SetLightOn(bool bOn);
	void SetLaserOn(bool bOn);
	
public:
	virtual void HandleIRDevice_Implementation() override;
	virtual void SetInfraredMode_Implementation(bool bInfrared) override;

	// LIGHT LASER INTERFACE
	virtual void SetLightLaserState_Implementation(const ESKGLightLaserState State) override;
	virtual void RestorePreviousBeforeOffState_Implementation() override;
	virtual void ToggleLight_Implementation() override;
	virtual void ToggleLaser_Implementation() override;
	/* If bDownArray is true it will go 0,1,2,etc, if it is false it will go 0,2,1,etc. If ForceIndex is within the count of LightLaserModeCycle's it will use that index*/
	virtual void CycleLightLaserModes_Implementation(const bool bDownArray = true, const uint8 ForceIndex = 254) override;
	virtual ESKGLightLaserState GetLightLaserState_Implementation() const override { return LightLaserState; }
	/* If bDownArray is true it will go 0,1,2,etc, if it is false it will go 0,2,1,etc.*/
	virtual void CycleLaserColor_Implementation(bool bDownArray = true) override;
	/* If bDownArray is true it will go 0,1,2,etc, if it is false it will go 0,2,1,etc.*/
	virtual void CyclePowerModes_Implementation(bool bDownArray = true) override;
	// END OF LIGHT LASER INTERFACE
	
	// AIM INTERFACE
	virtual bool IsAimable_Implementation() override { return bIsAimable && (AttachmentMesh.IsValid() && AttachmentMesh.Get()->DoesSocketExist(AimSocket)); }
	virtual void EnableAiming_Implementation() override { bIsAimable = true; }
	virtual void DisableAiming_Implementation() override { bIsAimable = false; }
	// END OF AIM INTERFACE
	
	// PROCEDURAL ANIMATION INTERFACE
	virtual FTransform GetAimSocketTransform_Implementation() override;
	// END OF PROCEDURAL ANIMATION INTERFACE
};
