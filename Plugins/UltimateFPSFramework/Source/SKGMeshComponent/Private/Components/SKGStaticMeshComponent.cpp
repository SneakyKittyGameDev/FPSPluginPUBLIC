// Copyright 2022, Dakota Dawe, All rights reserved


#include "Components/SKGStaticMeshComponent.h"
#include "Misc/SKGMeshComponentFunctionLibrary.h"

#include "GameFramework/PlayerController.h"

USKGStaticMeshComponent::USKGStaticMeshComponent()
{
}

void USKGStaticMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = GetWorld()->GetFirstPlayerController();
}

FMatrix USKGStaticMeshComponent::GetRenderMatrix() const
{
	FMatrix Matrix = Super::GetRenderMatrix();
	if (!PlayerController.IsValid() || !PlayerController->GetPawn())
	{
		return Matrix;
	}
	
	if (GetOwner())
	{
		AActor* OwningActor = GetOwner();
		for (uint8 i = 0; i < MaxOwnerAttempts; ++i)
		{
			if (APawn* OwningPawn = Cast<APawn>(OwningActor))
			{
				OwningActor = OwningPawn;
				break;
			}
			
			if (OwningActor)
			{
				OwningActor = OwningActor->GetOwner();
			}
			else
			{
				return Matrix;
			}
		}
		if (PlayerController->GetPawn() != OwningActor)
		{
			return Matrix;
		}
	}

	USKGMeshComponentFunctionLibrary::CalculateAdjustedMatrix(this, PlayerController.Get(), DesiredFOV, Matrix);

	return Matrix;
}
