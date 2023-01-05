// Copyright 2021, Dakota Dawe, All rights reserved

#include "Actors/SKGRangeFinder.h"

#include "Components/TextRenderComponent.h"

ASKGRangeFinder::ASKGRangeFinder()
{
	PrimaryActorTick.bCanEverTick = true;
	MeasureRate = 2.0f;
	bAutoMeasure = false;
	MaxDistance = 999.0f;
	MinDistance = 8.0f;
	UnitType = ESKGMeasurementType::Metric;
	bAppendUnitType = true;
	bUseUpper = true;

	CachedRange = FString("");
	UnitTypeConverter = 100.0f;
}

void ASKGRangeFinder::BeginPlay()
{
	Super::BeginPlay();
	if (UnitType == ESKGMeasurementType::Imperial)
	{
		UnitTypeConverter = 91.44f;
	}
	SetRangeByValue(0.0f);
	SetActorTickEnabled(false);
	if (bAutoMeasure)
	{
		SetActorTickInterval(1.0f / MeasureRate);
		SetActorTickEnabled(true);
	}
}

void ASKGRangeFinder::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Measure();
}

void ASKGRangeFinder::SetupAttachmentMesh()
{
	Super::SetupAttachmentMesh();
	
	TArray<UTextRenderComponent*> ActorTextComponents;
	GetComponents<UTextRenderComponent>(ActorTextComponents);
	for (UTextRenderComponent* TextComponent : ActorTextComponents)
	{
		if (IsValid(TextComponent) && TextComponent->ComponentHasTag(SKGATTACHMENT_MESH_TAG_NAME))
		{
			TextRenderRange = TextComponent;
			ClearMeasure();
			break;
		}
	}
}

void ASKGRangeFinder::SetRangeByValue(float Range)
{
	SetRangeByString(FString::SanitizeFloat(Range));
}

void ASKGRangeFinder::SetRangeByString(FString RangeText)
{
	if (IsValid(TextRenderRange) && !RangeText.IsEmpty())
	{
		int32 DotIndex = -1;
		if (RangeText.FindChar('.', DotIndex))
		{
			FString NewString = RangeText.Left(DotIndex + 2);
			if (bAppendUnitType)
			{
				AppendUnitType(NewString);
			}
			TextRenderRange->SetText(FText::FromString(NewString));
		}
	}
}

void ASKGRangeFinder::AppendUnitType(FString& CurrentString)
{
	FString AppendingString = FString(" M");
	if (UnitType == ESKGMeasurementType::Imperial)
	{
		AppendingString[1] = 'Y';
	}
	if (!bUseUpper)
	{
		AppendingString = AppendingString.ToLower();
	}
	CurrentString.Append(AppendingString);
}

float ASKGRangeFinder::Measure()
{
	if (AttachmentMesh.IsValid() && AttachmentMesh->DoesSocketExist(RangeFinderLaserSocket))
	{	// Perform Line Trace for Distance
		FVector Start = AttachmentMesh->GetSocketLocation(RangeFinderLaserSocket);
		FVector End = Start + AttachmentMesh->GetSocketRotation(RangeFinderLaserSocket).Vector() * (MaxDistance * UnitTypeConverter);
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
		{
			if (HitResult.Distance > MinDistance * UnitTypeConverter)
			{
				SetRangeByValue(HitResult.Distance / UnitTypeConverter);
				return HitResult.Distance / UnitTypeConverter;
			}
		}
	}
	
	return -1.0f;
}

void ASKGRangeFinder::ClearMeasure()
{
	if (IsValid(TextRenderRange))
	{
		CachedRange = TextRenderRange->Text.ToString();
		TextRenderRange->SetText(FText());
	}
}

void ASKGRangeFinder::RestoreRange()
{
	if (IsValid(TextRenderRange))
	{
		SetRangeByString(CachedRange);
	}
}

void ASKGRangeFinder::DisableRenderTarget_Implementation(bool bDisable)
{
	Super::DisableRenderTarget_Implementation(bDisable);
	if (bDisable)
	{
		ClearMeasure();
	}
	else
	{
		RestoreRange();
	}
}
