// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AnimNodes/AnimNode_BlendListBase.h"
#include "SKGAnimNode_GameplayTagBlend.generated.h"

/**
 * 
 */
USTRUCT()
struct SKGANIMGRAPH_API FSKGAnimNode_GameplayTagBlend : public FAnimNode_BlendListBase
{
	GENERATED_BODY()

public:
	FSKGAnimNode_GameplayTagBlend();
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	FGameplayTagContainer GameplayTagContainer;
	
	UPROPERTY(EditAnywhere, Category = "Settings", Meta = (PinShownByDefault))
	FGameplayTag ActiveGameplayTag;

protected:
	virtual int32 GetActiveChildIndex() override;

public:
	const FGameplayTag& GetActiveTag() const;
	const FGameplayTagContainer& GetTags() const;

#if WITH_EDITOR
	void RefreshPoses();
#endif
};
