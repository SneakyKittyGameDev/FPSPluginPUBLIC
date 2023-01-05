// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Actors/FirearmParts/SKGSight.h"
#include "SKGMagnifier.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGMagnifier : public ASKGSight
{
	GENERATED_BODY()
public:
	ASKGMagnifier();

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FlippedOut, Category = "SKGFPSFramework | Magnifier")
	bool bFlippedOut;
	UFUNCTION()
	void OnRep_FlippedOut();
	
	bool bFullyFlippedOut;
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Magnifier")
	void SetFullyFlipped(bool bIsFullyFlippedOut) { bFullyFlippedOut = bIsFullyFlippedOut; }

	TWeakObjectPtr<ASKGSight> SightInfront;

	void SetupMagnifier();

	virtual void BeginPlay() override;
	virtual void OnRep_Owner() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_Flip(bool bFlip);
	
public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Default")
	void CycleMagnifier() { Use(); }
	virtual void Use_Implementation() override;
	ASKGSight* GetSightInfront() const { return SightInfront.Get(); }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Default")
	bool IsFlippedOut() const { return bFlippedOut; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Default")
	bool IsFullyFlipped() const { return bFullyFlippedOut; }
};
