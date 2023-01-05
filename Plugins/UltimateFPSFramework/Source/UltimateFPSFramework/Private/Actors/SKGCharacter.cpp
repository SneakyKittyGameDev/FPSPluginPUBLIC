//Copyright 2021, Dakota Dawe, All rights reserved

#include "Actors/SKGCharacter.h"
#include "Actors/SKGFirearm.h"
#include "Blueprint/UserWidget.h"
#include "Components/SKGCharacterComponent.h"
#include "Components/SKGCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ASKGCharacter::ASKGCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<USKGCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	GetMesh()->bCastHiddenShadow = true;

	CharacterComponent = CreateDefaultSubobject<USKGCharacterComponent>(TEXT("CharacterComponent"));
}