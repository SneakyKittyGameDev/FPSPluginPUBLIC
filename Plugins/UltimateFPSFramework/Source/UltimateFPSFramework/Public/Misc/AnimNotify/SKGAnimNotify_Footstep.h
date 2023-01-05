// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Runtime/Launch/Resources/Version.h"
#include "SKGAnimNotify_Footstep.generated.h"

/**
 * 
 */
UCLASS()
class ULTIMATEFPSFRAMEWORK_API USKGAnimNotify_Footstep : public UAnimNotify
{
	GENERATED_BODY()
public:
	USKGAnimNotify_Footstep();

protected:
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FSKGImpactEffects DefaultSound;
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FName FootName;
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	TEnumAsByte<ECollisionChannel> TraceChannel;
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	float TraceStartHeight;
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	float TraceEndHeight;

#if ENGINE_MAJOR_VERSION == 5
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
#else
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#endif
};
