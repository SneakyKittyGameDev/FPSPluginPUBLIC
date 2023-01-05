// Copyright 2022, Dakota Dawe, All rights reserved


#include "Misc/AnimNotify/SKGAnimNotify_UnEquipFinished.h"

#include "SKGCharacterAnimInstance.h"

USKGAnimNotify_UnEquipFinished::USKGAnimNotify_UnEquipFinished()
{
	
}

#if ENGINE_MAJOR_VERSION == 5
void USKGAnimNotify_UnEquipFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
#else
void USKGAnimNotify_UnEquipFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
#endif

	if (GetCharacterComponent(MeshComp))
	{
		if (USKGCharacterAnimInstance* AnimInstance = Cast<USKGCharacterAnimInstance>(MeshComp->GetAnimInstance()))
		{
			AnimInstance->SetEquipped(false);
		}
		CharacterComponent->SetCanAim(false);
		CharacterComponent->OnUnEquipFinished.Broadcast();
	}
}

USKGCharacterComponent* USKGAnimNotify_UnEquipFinished::GetCharacterComponent(const USkeletalMeshComponent* MeshComp)
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
