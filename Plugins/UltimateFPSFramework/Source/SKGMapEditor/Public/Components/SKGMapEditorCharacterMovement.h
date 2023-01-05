// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SKGMapEditorCharacterMovement.generated.h"

class FSavedMove_MapEditor;
class FNetworkPredictionData_Client;

UCLASS()
class SKGMAPEDITOR_API USKGMapEditorCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	USKGMapEditorCharacterMovement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SKGMapEditor | Default")
	bool bCenterMouseOnStopMovementMode;
	
	bool bInMovementMode;
	int32 ScreenCenterX, ScreenCenterY;

	float MouseStartX, MouseStartY;

	virtual void BeginPlay() override;
	
	friend FSavedMove_MapEditor;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual float GetMaxSpeed() const override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	UPROPERTY(EditAnywhere, Category = "SKGMapEditor | PawnMovement")
	float MaxSpeedMultiplier;
	UPROPERTY(EditAnywhere, Category = "SKGMapEditor | PawnMovement")
	float MinSpeedMultiplier;
	
	float RequestedSpeedMultiplier;
	
public:
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | PawnMovement")
	void MoveForward(float Value = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | PawnMovement")
	void MoveRight(float Value = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | PawnMovement")
	void MoveUp(float Value = 1.0f);
	
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | PawnMovement")
	void LookUp(float Value = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | PawnMovement")
	void Turn(float Value = 1.0f);
	
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | PawnMovement")
	void EnterMovementMode(bool Enter);
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | PawnMovement")
	bool InMovementMode() const { return bInMovementMode; }
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | PawnMovement")
	void IncreaseSpeedMultiplier(float IncreaseAmount = 0.2f);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | PawnMovement")
	void DecreaseSpeedMultiplier(float DecreaseAmount = 0.2f);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSpeedMultiplier(float SpeedMultiplier);

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Init")
	void Init();
};

class FSavedMove_MapEditor : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;
	virtual void Clear() override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
	virtual void PrepMoveFor(ACharacter* Character) override;
	
	float SavedRequestedSpeedMultiplier = 1.0f;
};

class FNetworkPredictionData_Client_MapEditor : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_MapEditor(const UCharacterMovementComponent& ClientMovement):Super(ClientMovement){}
	typedef FNetworkPredictionData_Client_Character Super;
	virtual FSavedMovePtr AllocateNewMove() override;
};