// Copyright 2022, Dakota Dawe, All rights reserved


#include "AnimGraphNodes/SKGAnimGraphNode_GameplayTagBlend.h"
#include "Misc/BlueprintFunctionsLibraries/SKGFPSStatics.h"

USKGAnimGraphNode_GameplayTagBlend::USKGAnimGraphNode_GameplayTagBlend()
{
	Node.AddPose();
}

void USKGAnimGraphNode_GameplayTagBlend::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FSKGAnimNode_GameplayTagBlend, GameplayTagContainer))
	{
		ReconstructNode();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

FText USKGAnimGraphNode_GameplayTagBlend::GetNodeTitle(const ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Blend Poses by Gameplay Tag");
}

FText USKGAnimGraphNode_GameplayTagBlend::GetTooltipText() const
{
	return FText::FromString("Blend Poses by Gameplay Tag");
}

void USKGAnimGraphNode_GameplayTagBlend::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& PreviousPins)
{
#if WITH_EDITOR
	Node.RefreshPoses();
#endif
	Super::ReallocatePinsDuringReconstruction(PreviousPins);
}

FString USKGAnimGraphNode_GameplayTagBlend::GetNodeCategory() const
{
	return FString("FPSTemplate");
}

void USKGAnimGraphNode_GameplayTagBlend::CustomizePinData(UEdGraphPin* Pin, const FName SourcePropertyName, const int32 ArrayIndex) const
{
	bool bBlendPosePin;
	bool bBlendTimePin;
	GetBlendPinProperties(Pin, bBlendPosePin, bBlendTimePin);

	if (!bBlendPosePin && !bBlendTimePin)
	{
		return;
	}

	const FGameplayTag GameplayTag = Node.GetTags().GetByIndex(ArrayIndex - 1);
	const FString BaseName = FString(USKGFPSStatics::GetTextAfterPeriod(GameplayTag.ToString()) + ":");
	const FString Index = FString(" " + FString::FromInt(ArrayIndex));
	
	Pin->PinFriendlyName = FText::FromString(BaseName);

	if (bBlendPosePin)
	{
		Pin->PinFriendlyName = FText::FromString(BaseName + Index);
	}
	else if (bBlendTimePin)
	{
		Pin->PinFriendlyName = FText::FromString(BaseName + " Blend" + Index);
	}
}

void USKGAnimGraphNode_GameplayTagBlend::GetBlendPinProperties(const UEdGraphPin* Pin, bool& bBlendPosePin, bool& bBlendTimePin)
{
	const auto PinFullName{Pin->PinName.ToString()};
	const auto SeparatorIndex{PinFullName.Find(TEXT("_"), ESearchCase::CaseSensitive)};

	if (SeparatorIndex <= 0)
	{
		bBlendPosePin = false;
		bBlendTimePin = false;
		return;
	}

	const auto PinName{PinFullName.Left(SeparatorIndex)};
	bBlendPosePin = PinName == TEXT("BlendPose");
	bBlendTimePin = PinName == TEXT("BlendTime");
}