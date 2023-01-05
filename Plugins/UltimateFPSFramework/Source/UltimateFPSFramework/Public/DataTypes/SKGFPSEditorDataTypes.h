//Copyright 2021, Dakota Dawe, All rights reserved

#pragma once


#include "SKGFPSEditorDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FSKGStabilizerComponentPreview
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	bool bShowArea = true;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework", meta = (EditCondition = "bShowArea"))
	bool bDrawOnTop = false;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework", meta = (EditCondition = "bShowArea"))
	FLinearColor Color = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework", meta = (EditCondition = "bShowArea"))
	float Thickness = 0.1f;
};