// Copyright 2021, Dakota Dawe, All rights reserved


#include "SKGMapEditorCharacter.h"
#include "Components/SKGMapEditorCharacterMovement.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ASKGMapEditorCharacter::ASKGMapEditorCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USKGMapEditorCharacterMovement>(CharacterMovementComponentName))
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = true;
	GetMesh()->SetupAttachment(CameraComponent);
}

// Called when the game starts or when spawned
void ASKGMapEditorCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void ASKGMapEditorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}