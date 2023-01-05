// Copyright 2022, Dakota Dawe, All rights reserved


#include "SKGAttachmentActor.h"
#include "Components/SKGAttachmentComponent.h"
#include "Components/SKGAttachmentManager.h"
#include "Misc/SKGAttachmentFunctionLibrary.h"
#include "Misc/SKGAttachmentDefaultFunctions.h"

#include "Net/UnrealNetwork.h"
#include "Components/ShapeComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASKGAttachmentActor::ASKGAttachmentActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ASKGAttachmentActor::OnRep_CurrentOffset()
{
	ApplyAttachmentAttachOffset();
}

void ASKGAttachmentActor::ApplyAttachmentAttachOffset()
{
	const FVector RelativeLocation = FVector(AttachmentAttachOffset.GetLocation().X, CurrentOffset + AttachmentAttachOffset.GetLocation().Y, AttachmentAttachOffset.GetLocation().Z);
	const FTransform RelativeTransform = FTransform(AttachmentAttachOffset.GetRotation(), RelativeLocation, AttachmentAttachOffset.GetScale3D());
	SetActorRelativeTransform(RelativeTransform);
}

void ASKGAttachmentActor::OnRep_OwningAttachmentComponent()
{
	if (IsValid(OwningAttachmentComponent))
	{
		OffsetSnapDistance = OwningAttachmentComponent->GetOffsetSnapDistance();
	}
}

// Called when the game starts or when spawned
void ASKGAttachmentActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetComponents<USKGAttachmentComponent>(AttachmentComponents);
	}

	TArray<UActorComponent*> ShapeComponents = GetComponentsByTag(UShapeComponent::StaticClass(), SKGATTACHMENT_OVERLAP_TAG_NAME);
	if (ShapeComponents.Num())
	{
		OverlapCheckComponent = Cast<UShapeComponent>(ShapeComponents[0]);
		Execute_EnableGenerateOverlapEvents(this, false);
		OverlapCheckComponent->OnComponentBeginOverlap.AddDynamic(this, &ASKGAttachmentActor::OnOverlapBegin);
		OverlapCheckComponent->OnComponentEndOverlap.AddDynamic(this, &ASKGAttachmentActor::OnOverlapEnd);
	}
	
	SetupAttachmentMesh();
}

void ASKGAttachmentActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if (IsValid(OtherActor) && OtherActor != this && OtherActor->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()) && IsValid(Execute_GetOwningAttachmentComponent(this)))
	{
		if (Execute_ShouldBeCheckedForOverlap(OtherActor) && !IsAttachmentAttached(OtherActor))
		{
			const FSKGAttachmentOverlap AttachmentOverlap = FSKGAttachmentOverlap(OtherActor, true);
			OwningAttachmentComponent->OverlappedWithAttachment(AttachmentOverlap);
		}
	}
}

void ASKGAttachmentActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor) && OtherActor != this && OtherActor->GetClass()->ImplementsInterface(USKGAttachmentInterface::StaticClass()) && IsValid(Execute_GetOwningAttachmentComponent(this)))
	{
		if (Execute_ShouldBeCheckedForOverlap(OtherActor) && !IsAttachmentAttached(OtherActor))
		{
			const FSKGAttachmentOverlap AttachmentOverlap = FSKGAttachmentOverlap(OtherActor, false);
			OwningAttachmentComponent->OverlappedWithAttachment(AttachmentOverlap);
		}
	}
}

bool ASKGAttachmentActor::IsAttachmentAttached(const AActor* AttachmentToCheck)
{
	if (IsValid(AttachmentToCheck))
	{
		for (const USKGAttachmentComponent* AttachmentComponent : Execute_GetAttachmentComponents(this))
		{
			if (IsValid(AttachmentComponent) && IsValid(AttachmentComponent->GetAttachment()))
			{
				if (AttachmentComponent->GetAttachment() == AttachmentToCheck)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void ASKGAttachmentActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASKGAttachmentActor, AttachmentComponents);
	DOREPLIFETIME(ASKGAttachmentActor, CurrentOffset);
	DOREPLIFETIME_CONDITION(ASKGAttachmentActor, OwningAttachmentComponent, COND_InitialOnly);
}

void ASKGAttachmentActor::SetupAttachmentMesh()
{
	AttachmentMesh = USKGAttachmentFunctionLibrary::SetupAttachmentMesh(this);
}

void ASKGAttachmentActor::MarkComponentsAsPendingKill()
{
	Super::MarkComponentsAsPendingKill();
	
	if (OwningAttachmentComponent && OwningAttachmentComponent->GetAttachmentManager())
	{
		OwningAttachmentComponent->GetAttachmentManager()->CacheAttachmentComponents();
	}
}

void ASKGAttachmentActor::OnRep_AttachmentReplication()
{
	Super::OnRep_AttachmentReplication();
	ApplyAttachmentAttachOffset();
}

TArray<USKGAttachmentComponent*> ASKGAttachmentActor::GetAllAttachmentComponents_Implementation(bool bReCache)
{
	if (bReCache)
	{
		CacheAttachments();
	}
	
	return CachedAttachmentComponents;
}

void ASKGAttachmentActor::CacheAttachments()
{
	CachedAttachmentComponents = USKGAttachmentFunctionLibrary::CreateCacheFromAttachmentComponents(AttachmentComponents);
}

void ASKGAttachmentActor::OnAttachmentUpdated_Implementation()
{
	USKGAttachmentDefaultFunctions::OnAttachmentUpdated(OwningAttachmentComponent);
}

void ASKGAttachmentActor::OnAttachmentRemoved_Implementation()
{
	USKGAttachmentDefaultFunctions::OnAttachmentRemoved(CachedAttachmentComponents);
	Destroy();
}

bool ASKGAttachmentActor::AddOffset_Implementation(float Offset)
{
	if (bInvertMovingOffset)
	{
		Offset *= -1.0f;
	}

	if (OffsetSnapDistance > 0.0f)
	{
		bool bIncrease = false;
		if (Offset > 0.0f)
		{
			bIncrease = true;
		}
		else if (Offset < 0.0f)
		{
			bIncrease = false;
		}
		else
		{
			return false;
		}

		AccumulatedOffset += Offset;
		if (bIncrease && CurrentOffset < MaxOffset && AccumulatedOffset >= OffsetSnapDistance)
		{
			bIncrease = true;
			AccumulatedOffset = 0.0f;
		}
		else if (!bIncrease && CurrentOffset > MinOffset && AccumulatedOffset <= -OffsetSnapDistance)
		{
			bIncrease = false;
			AccumulatedOffset = 0.0f;
		}
		else
		{
			return false;
		}
	
		bIncrease ? CurrentOffset += OffsetSnapDistance : CurrentOffset -= OffsetSnapDistance;
	}
	else
	{
		CurrentOffset += Offset;
	}
	bool MaxedOffset = false;

	if (CurrentOffset > MaxOffset)
	{
		CurrentOffset = MaxOffset;
		MaxedOffset = true;
	}
	if (CurrentOffset < MinOffset)
	{
		CurrentOffset = MinOffset;
		MaxedOffset = true;
	}

	SetActorRelativeLocation(FVector(AttachmentAttachOffset.GetLocation().X, CurrentOffset + AttachmentAttachOffset.GetLocation().Y, AttachmentAttachOffset.GetLocation().Z));
	return MaxedOffset;
}

bool ASKGAttachmentActor::SetOffset_Implementation(float Offset)
{
	if (bInvertMovingOffset)
	{
		Offset *= -1.0f;
	}
	bool MaxedOffset = false;
	if (Offset > MaxOffset || Offset < MinOffset)
	{
		MaxedOffset = true;
	}
	else
	{
		CurrentOffset = Offset;
	}
	
	SetActorRelativeLocation(FVector(AttachmentAttachOffset.GetLocation().X, CurrentOffset + AttachmentAttachOffset.GetLocation().Y, AttachmentAttachOffset.GetLocation().Z));
	return MaxedOffset;
}

bool ASKGAttachmentActor::Server_SetOffset_Validate(float Offset)
{
	return true;
}

void ASKGAttachmentActor::Server_SetOffset_Implementation(float Offset)
{
	CurrentOffset = Offset;
	OnRep_CurrentOffset();
}

void ASKGAttachmentActor::FinishedMovingAttachment_Implementation()
{
	AccumulatedOffset = 0.0f;
	OldAccumulatedOffset = 0.0f;
	if (!HasAuthority())
	{
		Server_SetOffset(CurrentOffset);
	}
	OnRep_CurrentOffset();
}

void ASKGAttachmentActor::SetMinMaxOffset_Implementation(float Min, float Max)
{
	MinOffset = Min;
	MaxOffset = Max;
}

void ASKGAttachmentActor::SetSnapDistance_Implementation(float Distance)
{
	OffsetSnapDistance = Distance;

	if (OffsetSnapDistance > 0.0f)
	{
		MaxOffset -= fmod(MaxOffset, OffsetSnapDistance);
	}
}

void ASKGAttachmentActor::EnableGenerateOverlapEvents_Implementation(bool bEnable)
{
	if (OverlapCheckComponent.IsValid())
	{
		OverlapCheckComponent->SetGenerateOverlapEvents(bEnable);
	}
}

void ASKGAttachmentActor::OnAttachmentOverlapped_Implementation()
{
	USKGAttachmentDefaultFunctions::OnAttachmentOverlapped(this, CachedAttachmentComponents);
}

void ASKGAttachmentActor::OnAttachmentOverlapEnd_Implementation()
{
	USKGAttachmentDefaultFunctions::OnAttachmentOverlapEnd(this, CachedAttachmentComponents);
}
