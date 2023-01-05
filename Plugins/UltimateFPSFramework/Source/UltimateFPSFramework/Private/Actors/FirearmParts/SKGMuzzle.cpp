// Copyright 2021, Dakota Dawe, All rights reserved


#include "Actors/FirearmParts/SKGMuzzle.h"
#include "Actors/FirearmParts/SKGBarrel.h"
#include "Actors/SKGFirearm.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/BlueprintFunctionsLibraries/SKGFPSStatics.h"

#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstanceDynamic.h"

ASKGMuzzle::ASKGMuzzle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	NetUpdateFrequency = 1.0f;
	
	bIsSuppressor = false;
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
	
	PartStats.Weight = 1.6f;
	PartStats.VerticalRecoilChangePercentage = -10.0f;
	PartStats.HorizontalRecoilChangePercentage = -10.0f;

	PartType = ESKGPartType::MuzzleDevice;
}

void ASKGMuzzle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASKGMuzzle, MuzzleAttachment);
}

void ASKGMuzzle::BeginPlay()
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

void ASKGMuzzle::Tick(float DeltaSeconds)
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

bool ASKGMuzzle::DoesMuzzleSocketExist()
{
	return AttachmentMesh.IsValid() ? AttachmentMesh->DoesSocketExist(MuzzleSocket) : false;
}

bool ASKGMuzzle::IsSuppressor() const
{
	if (IsValid(MuzzleAttachment))
	{
		return MuzzleAttachment->bIsSuppressor;
	}
	return bIsSuppressor;
}

void ASKGMuzzle::NormalizeTemperature()
{
	MuzzleTemperatureNormalized = UKismetMathLibrary::NormalizeToRange(MuzzleTemperature, MinTemperatureEffect, MaxTemperatureEffect);
	MuzzleTemperatureNormalized = FMath::Clamp(MuzzleTemperatureNormalized, 0.0f, 1.0f);
	MuzzleTemperatureChanged(MuzzleTemperature);
	//UE_LOG(LogTemp, Warning, TEXT("Temperature: %f   Normalized: %f"), MuzzleTemperature, MuzzleTemperatureNormalized);
}

void ASKGMuzzle::FiredShot()
{
	if (bUseMuzzleTemperature)
	{
		SetActorTickEnabled(true);
		MuzzleTemperature += TemperatureIncreasePerShot;
		NormalizeTemperature();
	}
}

UNiagaraSystem* ASKGMuzzle::GetFireNiagaraSystem_Implementation()
{
	const int32 RandomIndex = USKGFPSStatics::GetRandomIndexForArray(FireNiagaraSystems.Num());
	if (RandomIndex != INDEX_NONE)
	{
		return FireNiagaraSystems[RandomIndex];
	}
	return nullptr;
}

FTransform ASKGMuzzle::GetMuzzleSocketTransform_Implementation()
{
	if (IsValid(MuzzleAttachment) && MuzzleAttachment->AttachmentMesh.IsValid())
	{
		return MuzzleAttachment->AttachmentMesh->GetSocketTransform(MuzzleSocket);
	}
	return AttachmentMesh.IsValid() ? AttachmentMesh->GetSocketTransform(MuzzleSocket) : FTransform();
}
