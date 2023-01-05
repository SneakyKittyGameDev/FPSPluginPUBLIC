// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SKGCharacterMovementComponent.generated.h"

class FSavedMove_FPSTemplate;
class FNetworkPredictionData_Client;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API USKGCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	USKGCharacterMovementComponent();

	virtual void BeginPlay() override;
	
	friend FSavedMove_FPSTemplate;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual float GetMaxSpeed() const override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Standing")
	float SprintSpeed;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Standing")
	float SuperSprintSpeed;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Standing")
	float MoveDirectionTolerance;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Standing")
	bool bCanOnlySprintForwards;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Standing", meta = (EditCondition = "!bCanOnlySprintForwards", EditConditionHides))
	bool bSprintSideways;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Standing", meta = (EditCondition = "!bCanOnlySprintForwards && bSprintSideways", EditConditionHides))
	float SprintSidewaysSpeed;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Standing", meta = (EditCondition = "!bCanOnlySprintForwards", EditConditionHides))
	bool bSprintBackwards;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Standing", meta = (EditCondition = "!bCanOnlySprintForwards && bSprintBackwards", EditConditionHides))
	float SprintBackwardsSpeed;

	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Crouched")
	bool bCanSprintWhileCrouched;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Crouched", meta = (EditCondition = "bCanSprintWhileCrouched", EditConditionHides))
	float CrouchSprintSpeed;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Crouched", meta = (EditCondition = "bCanSprintWhileCrouched", EditConditionHides))
	bool bCrouchCanOnlySprintForwards;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Crouched", meta = (EditCondition = "bCanSprintWhileCrouched && !bCrouchCanOnlySprintForwards", EditConditionHides))
	bool bCrouchSprintSideways;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Crouched", meta = (EditCondition = "bCanSprintWhileCrouched && !bCrouchCanOnlySprintForwards && bCrouchSprintSideways", EditConditionHides))
	float CrouchSprintSidewaysSpeed;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Crouched", meta = (EditCondition = "bCanSprintWhileCrouched && !bCrouchCanOnlySprintForwards", EditConditionHides))
	bool bCrouchSprintBackwards;
	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Sprinting | Crouched", meta = (EditCondition = "bCanSprintWhileCrouched && !bCrouchCanOnlySprintForwards && bCrouchSprintBackwards", EditConditionHides))
	float CrouchSprintBackwardsSpeed;

	UPROPERTY(EditAnywhere, Category = "Character Movement: SKGFPSFramework | Walking")
	FSKGMinMax MinMaxWalkSpeedMultiplier;
	
	float DefaultMaxWalkSpeed;
	uint8 bWantsToSprint : 1;
	uint8 bWantsToSuperSprint : 1;
	float RequestedWalkSpeedMultiplier;

	bool IsMovingForwards() const;
	bool IsMovingBackwards() const;
	bool IsMovingSideways() const;
	
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | CharacterMovement")
	void SetSprinting(ESKGSprintType SprintType);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | CharacterMovement")
	bool GetIsSprinting() const { return bWantsToSprint; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | CharacterMovement")
	void SetWalkSpeedMultiplier(float WalkSpeedMultiplier);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | CharacterMovement")
	void IncreaseWalkSpeedMultiplier(float IncreaseAmount);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | CharacterMovement")
	void DecreaseWalkSpeedMultiplier(float DecreaseAmount);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetWalkSpeedMultiplier(float WalkSpeedMultiplier);
	
	float HandleSprintSpeed() const;
};

class FSavedMove_FPSTemplate : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
	virtual void PrepMoveFor(ACharacter* Character) override;

	uint8 bSavedWantsToSprint : 1;
	uint8 bSavedWantsToSuperSprint : 1;
	float SavedRequestedWalkSpeedMultiplier = 1.0f;
};

class FNetworkPredictionData_Client_FPSTemplate : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_FPSTemplate(const UCharacterMovementComponent& ClientMovement):Super(ClientMovement){}
	typedef FNetworkPredictionData_Client_Character Super;
	virtual FSavedMovePtr AllocateNewMove() override;
};