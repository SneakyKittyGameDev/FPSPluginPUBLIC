// Copyright 2022, Dakota Dawe, All rights reserved


#include "Misc/AnimNotify/SKGAnimNotify_EquipFinished.h"

#include "SKGCharacterAnimInstance.h"

USKGAnimNotify_EquipFinished::USKGAnimNotify_EquipFinished()
{
	
}

#if ENGINE_MAJOR_VERSION == 5
void USKGAnimNotify_EquipFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
#else
void USKGAnimNotify_EquipFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
#endif

	if (GetCharacterComponent(MeshComp))
	{
		if (USKGCharacterAnimInstance* AnimInstance = Cast<USKGCharacterAnimInstance>(MeshComp->GetAnimInstance()))
		{
			AnimInstance->SetEquipped(true);
		}
		CharacterComponent->SetCanAim(true);
	}
}

USKGCharacterComponent* USKGAnimNotify_EquipFinished::GetCharacterComponent(const USkeletalMeshComponent* MeshComp)
{
	if (CharacterComponent.IsValid())
	{
		return CharacterComponent.Get();
	}

	if (MeshComp && MeshComp->GetOwner())
	{
		CharacterComponent = Cast<USKGCharacterComponent>(MeshComp->GetOwner()->GetComponentByClass(USKGCharacterComponent::StaticClass()));
		return CharacterComponent.Get();
	}
	return nullptr;
}
