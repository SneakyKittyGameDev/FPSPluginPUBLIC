// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Actors/FirearmParts/SKGSight.h"
#include "SKGSceneCaptureDataTypes.h"
#include "SKGRangeFinder.generated.h"

class UTextRenderComponent;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API ASKGRangeFinder : public ASKGSight
{
	GENERATED_BODY()
public:
	ASKGRangeFinder();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | RangeFinder")
	FName RangeFinderLaserSocket;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | RangeFinder")
	bool bAutoMeasure;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | RangeFinder", meta = (EditCondition = "bAutoMeasure", EditConditionHides))
	float MeasureRate;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | RangeFinder")
	float MaxDistance;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | RangeFinder")
	float MinDistance;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | RangeFinder")
	ESKGMeasurementType UnitType;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | RangeFinder")
	bool bAppendUnitType;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | RangeFinder", meta = (EditCondition = "bAppendUnitType", EditConditionHides))
	bool bUseUpper;
	
	UPROPERTY(BlueprintReadWrite, Category = "SKGFPSFramework")
	UTextRenderComponent* TextRenderRange;

	FString CachedRange;

	float UnitTypeConverter;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupAttachmentMesh() override;
	
	void RestoreRange();
	void AppendUnitType(FString& CurrentString);

public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | RangeFinder")
	void SetRangeByValue(float Range);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | RangeFinder")
	void SetRangeByString(FString RangeText);

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | RangeFinder")
	float Measure();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | RangeFinder")
	void ClearMeasure();

	virtual void DisableRenderTarget_Implementation(bool Disable) override;
};
