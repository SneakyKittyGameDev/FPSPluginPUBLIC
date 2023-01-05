// Copyright 2022, Dakota Dawe, All rights reserved


#include "Misc/AnimNotify/SKGAnimNotify_Footstep.h"
#include "Misc/BlueprintFunctionsLibraries/SKGFPSStatics.h"
#include "Misc/SKGPhysicalMaterial.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

USKGAnimNotify_Footstep::USKGAnimNotify_Footstep()
{
	FootName = FName("foot_r");
	TraceChannel = ECollisionChannel::ECC_Visibility;
	TraceStartHeight = 20.0f;
	TraceEndHeight = 40.0f;
}

#if ENGINE_MAJOR_VERSION == 5
void USKGAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
#else
void USKGAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
#endif
	if (MeshComp && MeshComp->GetWorld() && MeshComp->GetOwner() && MeshComp->DoesSocketExist(FootName))
	{
		const FVector FootLocation = MeshComp->GetSocketLocation(FootName);
		const FVector Start = FVector(FootLocation.X, FootLocation.Y, FootLocation.Z + TraceStartHeight);
		const FVector End = FVector(FootLocation.X, FootLocation.Y, FootLocation.Z - TraceEndHeight);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.bReturnPhysicalMaterial = true;
		QueryParams.AddIgnoredActor(MeshComp->GetOwner());
		
		if (MeshComp->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, TraceChannel, QueryParams))
		{
			HitResult.Location = MeshComp->GetOwner()->GetActorLocation();
			if (USKGPhysicalMaterial* PhysMat = Cast<USKGPhysicalMaterial>(HitResult.PhysMaterial))
			{
				USKGFPSStatics::SpawnImpactEffect(HitResult, PhysMat->FootstepImpactEffect);
			}
			else
			{
				USKGFPSStatics::SpawnImpactEffect(HitResult, DefaultSound);
			}
		}
	}
}
