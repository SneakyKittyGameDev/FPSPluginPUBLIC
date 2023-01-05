// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Components/SKGCharacterComponent.h"
#include "Runtime/Launch/Resources/Version.h"
#include "SKGAnimNotify_UnEquipFinished.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATEFPSFRAMEWORK_API USKGAnimNotify_UnEquipFinished : public UAnimNotify
{
	GENERATED_BODY()

public:
	USKGAnimNotify_UnEquipFinished();

protected:
#if ENGINE_MAJOR_VERSION == 5
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
#else
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#endif

	TWeakObjectPtr<USKGCharacterComponent> CharacterComponent;
	USKGCharacterComponent* GetCharacterComponent(const USkeletalMeshComponent* MeshComp);
};
