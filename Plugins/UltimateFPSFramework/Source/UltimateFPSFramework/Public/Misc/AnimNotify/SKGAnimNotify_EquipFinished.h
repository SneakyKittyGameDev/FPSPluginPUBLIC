// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Components/SKGCharacterComponent.h"
#include "Runtime/Launch/Resources/Version.h"
#include "SKGAnimNotify_EquipFinished.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATEFPSFRAMEWORK_API USKGAnimNotify_EquipFinished : public UAnimNotify
{
	GENERATED_BODY()

public:
	USKGAnimNotify_EquipFinished();

protected:
#if ENGINE_MAJOR_VERSION == 5
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
#else
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#endif

	TWeakObjectPtr<USKGCharacterComponent> CharacterComponent;
	USKGCharacterComponent* GetCharacterComponent(const USkeletalMeshComponent* MeshComp);
};
