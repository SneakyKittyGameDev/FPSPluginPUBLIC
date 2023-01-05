// Copyright 2021, Dakota Dawe, All rights reserved


#include "Components/SKGHoleComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values for this component's properties
USKGHoleComponent::USKGHoleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

// Called when the game starts or when spawned
void USKGHoleComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetupMaterials();
}

void USKGHoleComponent::SetupMaterials()
{
	if (GetOwner())
	{
		HoleMaterials.Reserve(MaterialSettings.Num() - 1);
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (StaticMeshComponent)
		{
			for (const FSKGHoleMaterialSetting MaterialSetting : MaterialSettings)
			{
				FSKGHoleMaterial HoleMaterial;
				
				HoleMaterial.MaterialInstance = StaticMeshComponent->CreateDynamicMaterialInstance(MaterialSetting.MaterialIndex);
				//StaticMeshComponent->SetMaterial(MaterialSetting.MaterialIndex, MaterialInstance);

				HoleMaterial.HoleLocations.Reserve(MaterialSetting.MaxHoleCount);
				for (uint8 i = 0; i < MaterialSetting.MaxHoleCount; ++i)
				{
					HoleMaterial.HoleLocations.Add(FVector::ZeroVector);
				}

				HoleMaterial.MaxHoleCount = MaterialSetting.MaxHoleCount;				
				HoleMaterials.Add(HoleMaterial);
			}
		}
	}
}

int32 USKGHoleComponent::GetFaceIndex(const FHitResult& HitLocation) const
{
	FHitResult NewHitResult;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	Params.bReturnFaceIndex = true;
	
	if (GetWorld()->LineTraceSingleByChannel(NewHitResult, HitLocation.TraceStart, HitLocation.TraceEnd, ECC_Visibility, Params))
	{
		return NewHitResult.FaceIndex;
	}
	return -1;
}

void USKGHoleComponent::AddHole(const FHitResult& HitResult)
{
	//HitResult.FaceIndex
	int32 SectionIndex = 0;
	const int32 FaceIndex = GetFaceIndex(HitResult);
	if (UMaterialInterface* MI = HitResult.Component->GetMaterialFromCollisionFaceIndex(FaceIndex, SectionIndex))
	{
		for (FSKGHoleMaterial& Material : HoleMaterials)
		{
			if (Material.MaterialInstance.IsValid() && MI == Material.MaterialInstance)
			{
				FString ParamName = FString("Impact" + FString::FromInt(Material.HoleIndex));
				
				Material.MaterialInstance->SetVectorParameterValue(FName(ParamName), HitResult.ImpactPoint);
				if (Material.HoleIndex < Material.HoleLocations.Num())
				{
					Material.HoleLocations[Material.HoleIndex] = HitResult.ImpactPoint;
				}
				
				if (++Material.HoleIndex > Material.MaxHoleCount - 1)
				{
					Material.HoleIndex = 0;
				}
				
				break;
			}
		}
	}
}

bool USKGHoleComponent::DidImpactHitHole(const FHitResult& HitResult, float Tolerance)
{
	int32 SectionIndex = 0;
	const int32 FaceIndex = GetFaceIndex(HitResult);
	if (UMaterialInterface* MI = HitResult.Component->GetMaterialFromCollisionFaceIndex(FaceIndex, SectionIndex))
	{
		for (FSKGHoleMaterial& Material : HoleMaterials)
		{
			if (Material.MaterialInstance.IsValid() && MI == Material.MaterialInstance)
			{
				for (const FVector& Location : Material.HoleLocations)
				{
					if (FVector::Distance(Location, HitResult.Location) <= Tolerance)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}
