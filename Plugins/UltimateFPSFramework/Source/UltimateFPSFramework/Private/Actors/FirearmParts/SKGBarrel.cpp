// Copyright 2021, Dakota Dawe, All rights reserved


#include "Actors/FirearmParts/SKGBarrel.h"
#include "Actors/SKGFirearm.h"
#include "Actors/FirearmParts/SKGMuzzle.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/BlueprintFunctionsLibraries/SKGFPSStatics.h"

#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ASKGBarrel::ASKGBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	NetUpdateFrequency = 1.0f;

	bUseMuzzleTemperature = false;
	TemperatureIncreasePerShot = 4.0f;
	TemperatureDecreasePerTick = 15.0f;
	MinTemperatureEffect = 80.0f;
	MaxTemperatureEffect = 1000.0f;
	MuzzleTemperature = 0.0f;
	MuzzleTemperatureNormalized = 0.0f;
	TemperatureCooldownTickRate = 10.0f;

	bBeCheckedForOverlap = false;
	
	MuzzleSocket = FName("S_Muzzle");
	PartStats.Weight = 0.7f;
	PartStats.ErgonomicsChangePercentage = -5.0f;
	PartStats.VerticalRecoilChangePercentage = -2.0f;
	PartStats.HorizontalRecoilChangePercentage = -2.0f;

	PartType = ESKGPartType::Barrel;
}

// Called when the game starts or when spawned
void ASKGBarrel::BeginPlay()
{
	Super::BeginPlay();

	if (!bUseMuzzleTemperature)
	{
		PrimaryActorTick.bCanEverTick = false;
	}
	else
	{
		if (AttachmentMesh.IsValid())
		{
			MuzzleMaterial = AttachmentMesh->CreateDynamicMaterialInstance(0);
			if (MuzzleMaterial.IsValid())
			{
				AttachmentMesh->SetMaterial(0, MuzzleMaterial.Get());
				SetActorTickInterval(1.0f / TemperatureCooldownTickRate);
			}
		}
	}

	SetActorTickEnabled(false);
}

void ASKGBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASKGBarrel::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (MuzzleMaterial.IsValid())
	{
		MuzzleMaterial->SetScalarParameterValue(FName("MuzzleTemperature"), MuzzleTemperatureNormalized);
	}
	MuzzleTemperature -= TemperatureDecreasePerTick * DeltaSeconds;
	if (MuzzleTemperature < 0.0f)
	{
		MuzzleTemperature = 0.0f;
		SetActorTickEnabled(false);
	}
	NormalizeTemperature();
}

bool ASKGBarrel::DoesMuzzleSocketExist() const
{
	return AttachmentMesh.IsValid() ? AttachmentMesh->DoesSocketExist(MuzzleSocket) : false;
}

UNiagaraSystem* ASKGBarrel::GetFireNiagaraSystem_Implementation()
{
	const int32 RandomIndex = USKGFPSStatics::GetRandomIndexForArray(FireNiagaraSystems.Num());
	if (RandomIndex != INDEX_NONE)
	{
		return FireNiagaraSystems[RandomIndex];
	}
	return nullptr;
}

FTransform ASKGBarrel::GetMuzzleSocketTransform_Implementation()
{
	return AttachmentMesh.IsValid() ? AttachmentMesh->GetSocketTransform(MuzzleSocket) : FTransform();
}

void ASKGBarrel::NormalizeTemperature()
{
	MuzzleTemperatureNormalized = UKismetMathLibrary::NormalizeToRange(MuzzleTemperature, MinTemperatureEffect, MaxTemperatureEffect);
	MuzzleTemperatureNormalized = FMath::Clamp(MuzzleTemperatureNormalized, 0.0f, 1.0f);
	MuzzleTemperatureChanged(MuzzleTemperature);
	//UE_LOG(LogTemp, Warning, TEXT("Temperature: %f   Normalized: %f"), MuzzleTemperature, MuzzleTemperatureNormalized);
}

void ASKGBarrel::FiredShot()
{
	if (bUseMuzzleTemperature)
	{
		SetActorTickEnabled(true);
		MuzzleTemperature += TemperatureIncreasePerShot;
		NormalizeTemperature();
	}
}
