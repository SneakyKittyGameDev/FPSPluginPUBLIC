// Copyright 2022, Dakota Dawe, All rights reserved


#include "Misc/SKGMeshComponentFunctionLibrary.h"

#include "GameFramework/PlayerController.h"

bool USKGMeshComponentFunctionLibrary::CalculateAdjustedMatrix(const UMeshComponent* Component,
	APlayerController* PlayerController, float DesiredFOV, FMatrix& OutMatrix)
{
	if (Component && PlayerController && PlayerController->PlayerCameraManager)
	{
		const float WorldFOV = PlayerController->PlayerCameraManager->GetFOVAngle();
		const float ViewmodelFOV = DesiredFOV;

		FVector ViewOrigin;
		FRotator ViewRotation;
		PlayerController->GetPlayerViewPoint(ViewOrigin, ViewRotation);

		const float WorldHalfFOVRadians = FMath::DegreesToRadians(FMath::Max(0.001f, WorldFOV)) / 2.0f;
		const float DesiredHalfFOVRadians = FMath::DegreesToRadians(FMath::Max(0.001f, ViewmodelFOV)) / 2.0f;
		const float FOVRatio = WorldHalfFOVRadians / DesiredHalfFOVRadians;

		const FMatrix PerspectiveAdjustmentMatrix = FMatrix(
			FPlane(FOVRatio, 0, 0, 0),
			FPlane(0, FOVRatio, 0, 0),
			FPlane(0, 0, 1, 0),
			FPlane(0, 0, 0, 1));

		FMatrix ViewRotationMatrix = FInverseRotationMatrix(ViewRotation) * FMatrix(
			   FPlane(0, 0, 1, 0),
			   FPlane(1, 0, 0, 0),
			   FPlane(0, 1, 0, 0),
			   FPlane(0, 0, 0, 1));
	
		if (!ViewRotationMatrix.GetOrigin().IsNearlyZero(0.0f))
		{
			ViewOrigin += ViewRotationMatrix.InverseTransformPosition(FVector::ZeroVector);
			ViewRotationMatrix = ViewRotationMatrix.RemoveTranslation();
		}

		const FMatrix ViewMatrix = FTranslationMatrix(-ViewOrigin) * ViewRotationMatrix;
		const FMatrix InverseViewMatrix = FTranslationMatrix(-ViewMatrix.GetOrigin()) * ViewMatrix.RemoveTranslation().GetTransposed();
		//Compensate for FOV Difference
		const float OffsetRange = FMath::GetMappedRangeValueUnclamped(FVector2D(45.f, 71.f), FVector2D(4.5f, -4.5f), DesiredFOV);
		
		const FVector CompensationOffset = ViewRotation.Vector() * OffsetRange;
		const FMatrix FOVCompensation = FTransform(FRotator::ZeroRotator, CompensationOffset, FVector::OneVector).ToMatrixNoScale();

		OutMatrix = Component->GetComponentToWorld().ToMatrixWithScale() * ViewMatrix * PerspectiveAdjustmentMatrix * InverseViewMatrix * FOVCompensation;

		return true;
	}
	return false;
}
