//Copyright 2021, Dakota Dawe, All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SKGCharacter.generated.h"

class UInputComponent;
class USKGCharacterComponent;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USKGCharacterComponent* CharacterComponent;

public:
	ASKGCharacter(const FObjectInitializer& ObjectInitializer);
};
