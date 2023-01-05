// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/EngineTypes.h"
#include "SKGHitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ULTIMATEFPSFRAMEWORK_API ISKGHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SKGFPSFramework | HitInterface")
	void OnHit(const FHitResult& HitResult, AActor* Projectile);
	virtual void OnHit_Implementation(const FHitResult& HitResult, AActor* Projectile) {}
};
