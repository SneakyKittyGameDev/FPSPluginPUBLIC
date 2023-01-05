// Copyright 2022, Dakota Dawe, All rights reserved


#include "Misc/AnimNotify/SKGAnimNotify_CameraShake.h"

USKGAnimNotify_CameraShake::USKGAnimNotify_CameraShake()
{
	ShakeScale = 1.0f;
}

#if ENGINE_MAJOR_VERSION == 5
void USKGAnimNotify_CameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
#else
void USKGAnimNotify_CameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
#endif
	if (CameraShake && GetCharacterComponent(MeshComp))
	{
		CharacterComponent->PlayCameraShake(CameraShake, ShakeScale);
	}
}

USKGCharacterComponent* USKGAnimNotify_CameraShake::GetCharacterComponent(const USkeletalMeshComponent* MeshComp)
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
