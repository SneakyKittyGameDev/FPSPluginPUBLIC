//Copyright 2021, Dakota Dawe, All rights reserved

#include "Components/FPSTemplate_PartComponent.h"
#include "Components/FPSTemplate_PreviewStatic.h"
#include "Actors/FPSTemplateFirearm.h"
#include "Components/FPSTemplate_PreviewSkeletal.h"
#include "FirearmParts/BaseClasses/FPSTemplate_PartBase.h"
#include "Misc/FPSTemplateStatics.h"
#include "FirearmParts/BaseClasses/FPSTemplate_PartBase.h"

#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/ShapeComponent.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Net/UnrealNetwork.h"

#define COMPONENT_TAG_NAME FName("FPSPart")

// Sets default values for this component's properties
UFPSTemplate_PartComponent::UFPSTemplate_PartComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bUseAttachParentBound = true;

	bAllowClientAuthorativePartChange = true;
	
	Minimum = 0.0f;
	Maximum = 0.0f;
	DisplayPartIndex = INDEX_NONE;
	DisplayPartMinMax = 0.0f;
	bSetPartInitialOffsetAtDisplayMinMax = false;
	bSetInitialOffset = false;
	OffsetSnapDistance = 0.0f;

	PreviewStatic = CreateDefaultSubobject<UFPSTemplate_PreviewStatic>(TEXT("PreviewStaticMesh"));
	PreviewStatic->SetupAttachment(GetAttachmentRoot());
	PreviewStatic->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewStatic->SetHiddenInGame(true);
	PreviewStatic->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	
	PartTransformReference = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	PartTransformReference->SetupAttachment(PreviewStatic);
	
	PreviewSkeletal = CreateDefaultSubobject<UFPSTemplate_PreviewSkeletal>(TEXT("PreviewSkeletalMesh"));
	PreviewSkeletal->SetupAttachment(PreviewStatic);
	PreviewSkeletal->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewSkeletal->SetHiddenInGame(true);
	PreviewSkeletal->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	
	SetIsReplicatedByDefault(true);

	bUseRightVector = true;
}

void UFPSTemplate_PartComponent::OnRep_Part() const
{
	if (Part)
	{
		Part->SetMinMaxOffset(Minimum, Maximum);
		Part->PartsUpdated();		
	}
}

// Called when the game starts
void UFPSTemplate_PartComponent::BeginPlay()
{
	Super::BeginPlay();
	PreviewSkeletal->DestroyComponent();
	PreviewStatic->DestroyComponent();
	
	HandlePartConstruction();
}

void UFPSTemplate_PartComponent::HandlePartConstruction()
{
	AActor* CurrentOwner = GetOwner();
	if (CurrentOwner && CurrentOwner->HasAuthority())
	{
		bool bSpawnDefaultParts = true;
		if (const AFPSTemplateFirearm* Firearm = Cast<AFPSTemplateFirearm>(CurrentOwner))
		{
			bSpawnDefaultParts = Firearm->GetShouldSpawnDefaultOnPreset();
		}
		else
		{
			for (uint8 i = 0; i < 10; ++i)
			{
				if (IsValid(CurrentOwner))
				{
					CurrentOwner = CurrentOwner->GetOwner();
					if (const AFPSTemplateFirearm* OwningFirearm = Cast<AFPSTemplateFirearm>(CurrentOwner))
					{
						bSpawnDefaultParts = OwningFirearm->GetShouldSpawnDefaultOnPreset();
						break;
					}
				}
			}
		}
		
		if (bSpawnDefaultParts && IsValid(DefaultPart))
		{
			AddPart(DefaultPart);
		}
	}
}

void UFPSTemplate_PartComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFPSTemplate_PartComponent, Part);
}

#if WITH_EDITOR
void UFPSTemplate_PartComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const FString PropertyName = PropertyChangedEvent.GetPropertyName().ToString();

	if (PropertyName.Equals(TEXT("bUseRightVector")))
	{
		DisplayPartMinMax = FMath::Clamp(DisplayPartMinMax, Minimum, Maximum);
		int Amount = (DisplayPartMinMax / OffsetSnapDistance);
		Amount *= OffsetSnapDistance;
		
		if (bUseRightVector)
		{
			PreviewStatic->SetRelativeLocation(FVector(0.0f, Amount, 0.0f));
		}
		else
		{
			PreviewStatic->SetRelativeLocation(FVector(Amount, 0.0f, 0.0f));
		}
	}
	else if (PropertyName.Equals(TEXT("DisplayPartMinMax")))
	{
		DisplayPartMinMax = FMath::Clamp(DisplayPartMinMax, Minimum, Maximum);
		int Amount = (DisplayPartMinMax / OffsetSnapDistance);
		Amount *= OffsetSnapDistance;
		if (Amount <= Maximum && Amount >= Minimum && Amount != OldAmount)
		{
			OldAmount = Amount;
			if (bUseRightVector)
			{
				PreviewStatic->SetRelativeLocation(FVector(0.0f, Amount, 0.0f));
			}
			else
			{
				PreviewStatic->SetRelativeLocation(FVector(Amount, 0.0f, 0.0f));
			}
		}
	}
	else if (PropertyName.Contains(TEXT("DisplayPartIndex")))
	{
		if (PreviewStatic && PreviewSkeletal)
		{
			UE_LOG(LogTemp, Warning, TEXT("DisplayPartIndex"));
			if (UActorComponent* ActorComponent = GetMeshComponentFromPart())
			{
				if (const UStaticMeshComponent* PartStaticMesh = Cast<UStaticMeshComponent>(ActorComponent))
				{
					PreviewStatic->SetStaticMesh(PartStaticMesh->GetStaticMesh());
					PreviewStatic->SetHiddenInGame(true);
					PreviewSkeletal->SetSkeletalMesh(nullptr);
				}
				else if (const USkeletalMeshComponent* PartSkeletalMesh = Cast<USkeletalMeshComponent>(ActorComponent))
				{
					PreviewSkeletal->SetSkeletalMesh(PartSkeletalMesh->SkeletalMesh);
					PreviewSkeletal->SetHiddenInGame(true);
					PreviewStatic->SetStaticMesh(nullptr);
				}
			}
			else
			{
				PreviewStatic->SetStaticMesh(nullptr);
				PreviewStatic->SetStaticMesh(nullptr);
			}
		}
	}
	else if (PropertyName.Contains(TEXT("Preview")))
	{
		DisplayPartIndex = INDEX_NONE;
		if (PropertyName.Contains(TEXT("Static")))
		{
			if (PreviewStatic)
			{
				PreviewSkeletal->SetSkeletalMesh(nullptr);
			}
		}
		else if (PropertyName.Contains(TEXT("Skeletal")))
		{
			if (PreviewSkeletal)
			{
				PreviewStatic->SetStaticMesh(nullptr);
			}
		}
	}
}

UActorComponent* UFPSTemplate_PartComponent::GetMeshComponentFromPart() const
{
	if (DisplayPartIndex > -1 && DisplayPartIndex < PossibleParts.Num())
	{
		if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(PossibleParts[DisplayPartIndex]))
		{
			const TArray<USCS_Node*>& ActorBlueprintNodes = BPClass->SimpleConstructionScript->GetAllNodes();
		
			for (const USCS_Node* Node : ActorBlueprintNodes)
			{
				if (Node && Node->ComponentTemplate)
				{
					if (Node->ComponentClass == UStaticMeshComponent::StaticClass() || Node->ComponentClass == USkeletalMeshComponent::StaticClass())
					{
						if (Node->ComponentTemplate->ComponentHasTag(COMPONENT_TAG_NAME))
						{
							return Node->ComponentTemplate;
						}
					}
				}
			}
		}
	}
	return nullptr;
}

FString UFPSTemplate_PartComponent::GetSelectedPartName() const
{
	FString PartName = "IS EMPTY";
	if (DisplayPartIndex > -1 && DisplayPartIndex < PossibleParts.Num())
	{
		if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(PossibleParts[DisplayPartIndex]))
		{
			const TArray<USCS_Node*>& ActorBlueprintNodes = BPClass->SimpleConstructionScript->GetAllNodes();
		
			PartName = BPClass->GetDisplayNameText().ToString();
		}
	}
	else if (DefaultPart)
	{
		PartName = DefaultPart->GetDisplayNameText().ToString();
	}
	return PartName;
}

int32 UFPSTemplate_PartComponent::GetEditorPartCurrentSnapPointIndex() const
{
	const float Clamped = FMath::Clamp(DisplayPartMinMax, Minimum, Maximum);
	const int Amount = Clamped / OffsetSnapDistance;
	return IsMovementInverted() ? -Amount : Amount;
}

TArray<float> UFPSTemplate_PartComponent::GetEditorSnapDistancePoints() const
{
	TArray<float> SnapPoints;
	if (OffsetSnapDistance > 0.0f)
	{
		bool bInverted = false;
		if (Minimum < 0.0f)
		{
			bInverted = true;
		}
		const uint8 ElementCount = (bInverted ? -Minimum : Maximum) / OffsetSnapDistance;

		SnapPoints.Reserve(ElementCount);
		SnapPoints.Add(0.0f);
		
		float Offset = 0.0f;
		for (uint8 i = 0; i < ElementCount; ++i)
		{
			Offset += bInverted ? -OffsetSnapDistance : OffsetSnapDistance;
			SnapPoints.Add(Offset);
		}
	}

	return SnapPoints;
}

TArray<FVector> UFPSTemplate_PartComponent::GetEditorSnapPoints() const
{
	TArray<FVector> WorldSnapPointOffsets;
	
	if (OffsetSnapDistance > 0.0f)
	{
		TArray<float> SnapDistancePoints = GetEditorSnapDistancePoints();

		WorldSnapPointOffsets.Reserve(SnapDistancePoints.Num());
		const FVector DirectionalVector = bUseRightVector ? GetRightVector() : GetForwardVector();

		for (const float Offset : SnapDistancePoints)
		{
			const FVector PointLocation = GetComponentLocation() + DirectionalVector * Offset;
			WorldSnapPointOffsets.Add(PointLocation);
		}
	}
	
	return WorldSnapPointOffsets;
}

#endif

FTransform UFPSTemplate_PartComponent::GetAttachmentTransform() const
{
	return PartTransformReference->GetComponentTransform();
}

bool UFPSTemplate_PartComponent::Server_DestroyCurrentPart_Validate()
{
	return bAllowClientAuthorativePartChange ? true : false;
}

void UFPSTemplate_PartComponent::Server_DestroyCurrentPart_Implementation()
{
	DestroyCurrentPart();
}

TArray<AFPSTemplate_PartBase*> UFPSTemplate_PartComponent::GetOverlappedHiddenActors()
{
	TArray<AFPSTemplate_PartBase*> HiddenParts;
	HiddenParts.Reserve(OverlappedParts.Num());

	for (const FPartOverlap& PartOverlap : OverlappedParts)
	{
		if (IsValid(PartOverlap.Part))
		{
			if (PartOverlap.Part->IsHidden())
			{
				HiddenParts.Add(PartOverlap.Part);
			}
		}
	}
	
	return HiddenParts;
}

void UFPSTemplate_PartComponent::HideOverlappedAndUnhideUnoverlappedParts()
{
	for (const FPartOverlap PartOverlap : OverlappedParts)
	{
		if (IsValid(PartOverlap.Part))
		{
			if (PartOverlap.bIsOverlapped)
			{
				PartOverlap.Part->SetActorHiddenInGame(true);
			}
			else
			{
				PartOverlap.Part->SetActorHiddenInGame(false);
			}
		}
	}
}

void UFPSTemplate_PartComponent::DestroyCurrentPart()
{
	if (!HasAuthority())
	{
		if (bAllowClientAuthorativePartChange)
		{
			Server_DestroyCurrentPart();
		}
	}
	else
	{
		if (IsValid(GetPart()))
		{
			/*for (const UFPSTemplate_PartComponent* PartComponent : Part->GetPartComponents())
			{
				if (PartComponent && PartComponent->GetPart())
				{
					PartComponent->GetPart()->Destroy();
				}
			}
			Part->OnPartRemoved(this);*/
		}
	}
}

FVector UFPSTemplate_PartComponent::GetMaxOffsetLocation() const
{
	FVector EndLocation = GetComponentLocation() + GetComponentRotation().Vector() * Maximum;
	if (IsValid(GetPart()))
	{
		const float Distance = GetPart()->GetPartOffset();
		const FVector PartRightVector = GetPart()->GetActorRightVector();
		
		if (IsMovementInverted())
		{
			EndLocation = GetComponentLocation() + PartRightVector * (Minimum);
		}
		else
		{
			FVector PartLocation = GetPart()->GetActorLocation();
			PartLocation.Z -= GetPart()->GetPartAttachOffset().GetLocation().Z;
			EndLocation = PartLocation + PartRightVector * (Maximum - Distance);
		}
		//DrawDebugSphere(GetWorld(), EndLocation, 2.0f, 14.0f, FColor::Red);
	}
	return EndLocation;
}

TArray<float> UFPSTemplate_PartComponent::GetSnapPoints()
{
	if (CachedSnapPointOffsets.Num() == 0 && OffsetSnapDistance > 0.0f)
	{
		if (IsValid(GetPart()))
		{
			const uint8 ElementCount = (IsMovementInverted() ? -Minimum : Maximum) / OffsetSnapDistance;

			CachedSnapPointOffsets.Reserve(ElementCount);
			CachedSnapPointOffsets.Add(0.0f);
			
			float Offset = 0.0f;
			for (uint8 i = 0; i < ElementCount; ++i)
			{
				Offset += IsMovementInverted() ? -OffsetSnapDistance : OffsetSnapDistance;
				CachedSnapPointOffsets.Add(Offset);
			}
		}
	}

	return CachedSnapPointOffsets;
}

TArray<FVector> UFPSTemplate_PartComponent::GetWorldSnapPoints()
{
	TArray<FVector> WorldSnapPointOffsets;
	
	if (OffsetSnapDistance > 0.0f)
	{
		GetSnapPoints();
		WorldSnapPointOffsets.Reserve(CachedSnapPointOffsets.Num());
		const FVector DirectionalVector = bUseRightVector ? GetRightVector() : GetForwardVector();

		for (const float Offset : CachedSnapPointOffsets)
		{
			const FVector PointLocation = GetComponentLocation() + DirectionalVector * Offset;
			WorldSnapPointOffsets.Add(PointLocation);
		}
	}
	
	return WorldSnapPointOffsets;
}

int32 UFPSTemplate_PartComponent::GetPartCurrentSnapPointIndex() const
{
	return IsValid(GetPart()) ? (IsMovementInverted() ? -1 : 1) * GetPart()->GetPartOffset() / OffsetSnapDistance : -1;
}

bool UFPSTemplate_PartComponent::Server_AddPart_Validate(TSubclassOf<AFPSTemplate_PartBase> PartClass, bool bDestroyCurrentPart)
{
	return bAllowClientAuthorativePartChange ? true : false;
}

void UFPSTemplate_PartComponent::Server_AddPart_Implementation(TSubclassOf<AFPSTemplate_PartBase> PartClass, bool bDestroyCurrentPart)
{
	AddPart(PartClass, bDestroyCurrentPart);
}

AFPSTemplate_PartBase* UFPSTemplate_PartComponent::AddPart(TSubclassOf<AFPSTemplate_PartBase> PartClass, bool bDestroyCurrentPart)
{
	if (!IsPartCompatible(PartClass) || !GetOwner())
	{
		return GetPart();
	}

	AFPSTemplate_PartBase* CurrentPart = GetPart();

	if (!HasAuthority())
	{
		if (bAllowClientAuthorativePartChange)
		{
			Server_AddPart(PartClass, bDestroyCurrentPart);
		}
	}
	else
	{
		if (PartClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();

			AFPSTemplate_PartBase* SpawnedPart = GetWorld()->SpawnActor<AFPSTemplate_PartBase>(PartClass, SpawnParams);
			AddNewPart(SpawnedPart, bDestroyCurrentPart);
		}
	}
	return bDestroyCurrentPart ? Part : CurrentPart;
}

bool UFPSTemplate_PartComponent::Server_AddExistingPart_Validate(AFPSTemplate_PartBase* INPart, bool bDestroyCurrentPart)
{
	return bAllowClientAuthorativePartChange ? true : false;
}

void UFPSTemplate_PartComponent::Server_AddExistingPart_Implementation(AFPSTemplate_PartBase* INPart, bool bDestroyCurrentPart)
{
	AddExistingPart(INPart, bDestroyCurrentPart);
}

AFPSTemplate_PartBase* UFPSTemplate_PartComponent::AddExistingPart(AFPSTemplate_PartBase* INPart, bool bDestroyCurrentPart)
{
	if (IsValid(INPart) && !IsPartCompatible(INPart->GetClass()) || !GetOwner())
	{
		return GetPart();
	}

	AFPSTemplate_PartBase* CurrentPart = GetPart();
	
	if (!HasAuthority())
	{
		if (bAllowClientAuthorativePartChange)
		{
			Server_AddExistingPart(INPart, bDestroyCurrentPart);
		}
	}
	else
	{
		AddNewPart(INPart, bDestroyCurrentPart);
	}
	return bDestroyCurrentPart ? Part : CurrentPart;
}

void UFPSTemplate_PartComponent::AddNewPart(AFPSTemplate_PartBase* INPart, bool bDestroyCurrentPart)
{
	if (HasAuthority() && IsValid(INPart))
	{
		float PartOffset = 0.0f;
		if (bDestroyCurrentPart && IsValid(Part))
		{
			PartOffset = Part->GetPartOffset();
			DestroyCurrentPart();
		}
		INPart->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		INPart->SetActorTransform(GetAttachmentTransform());
		Part = INPart;
		Part->SetMinMaxOffset(Minimum, Maximum);
		//Part->SetOwningPartComponent(this);
		
		if (!bSetInitialOffset && bSetPartInitialOffsetAtDisplayMinMax && DisplayPartMinMax <= Maximum && DisplayPartMinMax >= Minimum)
		{
			bSetInitialOffset = true;
			Part->SetOffset(DisplayPartMinMax);
		}
		else
		{
			Part->SetOffset(PartOffset);
		}
		Part->SetSnapDistance(OffsetSnapDistance);
		Part->PartsUpdated();
	}
}

bool UFPSTemplate_PartComponent::IsMovementInverted() const
{
	if (IsValid(GetPart()))
	{
		return GetPart()->IsMovementInverted();
	}

	if (Minimum < 0.0f)
	{
		return true;
	}
	
	return false;
}

void UFPSTemplate_PartComponent::OverlappedWithPart(FPartOverlap OverlappedPart)
{
	bool bMatchedPart = false;
	bool bRemovedElement = false;
	for (uint8 i = 0; i < OverlappedParts.Num(); ++i)
	{
		FPartOverlap& PartOverlap = OverlappedParts[i];
		if (PartOverlap == OverlappedPart)
		{
			bMatchedPart = true;
			PartOverlap.bIsOverlapped = OverlappedPart.bIsOverlapped;
		}
		else if (!IsValid(PartOverlap.Part))
		{
			bRemovedElement = true;
			OverlappedParts.RemoveAt(i, 1, false);
		}
	}

	if (bRemovedElement)
	{
		OverlappedParts.Shrink();
	}
	
	if (!bMatchedPart)
	{
		OverlappedParts.Add(OverlappedPart);
	}
}

bool UFPSTemplate_PartComponent::IsPartCompatible(TSubclassOf<AFPSTemplate_PartBase> PartClass) const
{
	if (!PartClass)
	{
		return false;
	}
	
	for (TSubclassOf<AFPSTemplate_PartBase> PossiblePart : PossibleParts)
	{
		if (PossiblePart && PossiblePart == PartClass)
		{
			return true;
		}
	}
	return false;
}

bool UFPSTemplate_PartComponent::HasPart() const
{
	return IsValid(GetPart());
}