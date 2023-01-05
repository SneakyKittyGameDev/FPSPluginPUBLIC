// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_BlendListBase.h"
#include "AnimGraphNodes/SKGAnimNode_GameplayTagBlend.h"
#include "SKGAnimGraphNode_GameplayTagBlend.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATEFPSEDITOR_API USKGAnimGraphNode_GameplayTagBlend : public UAnimGraphNode_BlendListBase
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FSKGAnimNode_GameplayTagBlend Node;

public:
	USKGAnimGraphNode_GameplayTagBlend();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual FText GetTooltipText() const override;

	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& PreviousPins) override;

	virtual FString GetNodeCategory() const override;

	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const override;

protected:
	static void GetBlendPinProperties(const UEdGraphPin* Pin, bool& bBlendPosePin, bool& bBlendTimePin);
};
