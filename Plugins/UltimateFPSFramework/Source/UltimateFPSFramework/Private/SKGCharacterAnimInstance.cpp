//Copyright 2021, Dakota Dawe, All rights reserved

#include "SKGCharacterAnimInstance.h"

#include "DrawDebugHelpers.h"
#include "Actors/SKGFirearm.h"
#include "Components/SKGCharacterComponent.h"
#include "Interfaces/SKGFirearmInterface.h"

#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveVector.h"
#include "GameFramework/PawnMovementComponent.h"

#if ENGINE_MAJOR_VERSION == 5
#include "KismetAnimationLibrary.h"
#endif

USKGCharacterAnimInstance::USKGCharacterAnimInstance()
{
	AimInterpolationSpeed = 20.0f;
	CycleSightsInterpolationSpeed = 20.0f;
	RotationLagResetInterpolationSpeed = 20.0f;
	MotionLagResetInterpolationSpeed = 20.0f;

	FirearmCollisionVirtualBoneName = FName("VB FirearmCollisionGrip");

	bUseProceduralSpine = true;
	bUseLeftHandIK = true;
	SpineBlendPercents = { 1.0f };
	
	bInterpRelativeToHand = false;
	bFirstRun = true;
	bIsAiming = false;
	AimingAlpha = 0.0f;
	LeftHandIKAlpha = 1.0f;
	RotationAlpha = false;
	bInterpAiming = false;
	RightHandBone = FName("hand_r");
	
	HeadAimingRotation = FRotator(45.0f, 0.0f, 0.0f);

	SightDistance = 0.0f;
	CurveTimer = 1.0f;
	bCustomizingFirearm = false;
	bCanAim = true;
	bInterpFirearmPose = false;
	CollisionPoseAlpha = 0.0f;
	bValidLeftHandPose = false;
	bInvertRotationLag = false;
	bSprinting = false;
	
	SwayMultiplier = 1.0f;

	bIsLocallyControlled = false;

	FreeLookRotation = FRotator(90.0f, 0.0f, 0.0f);

	bInterpRecoil = false;
	bInterpShakeCurve = false;
	bCanPlayShakeCurve = true;
	
	ShakeCurveAlpha = 0.0f;

	CustomPoseAlpha = 0.0f;

	CustomCurveStartTime = 0.0f;
	CustomCurveAlpha = 0.0f;
	bInterpCustomCurve = false;

	bEquipped = false;

	bIndexOrTagForced = false;

	bApplyForwardVector = false;
}

void USKGCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	if (const AActor* OwningActor = GetOwningActor())
	{
		CharacterComponent = Cast<USKGCharacterComponent>(OwningActor->GetComponentByClass(TSubclassOf<USKGCharacterComponent>()));
		if (IsValid(CharacterComponent))
		{
			bIsLocallyControlled = CharacterComponent->IsLocallyControlled();
		}
	}
}

void USKGCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!CharacterComponent || bCustomizingFirearm) { return; }

	FVector FPSVelocity = CharacterComponent->GetOwner()->GetVelocity();
	FPSVelocity.Z = 0.0f;
	CharacterVelocity = FPSVelocity.Size();

#if ENGINE_MAJOR_VERSION == 5
	CharacterDirection = UKismetAnimationLibrary::CalculateDirection(FPSVelocity, CharacterComponent->GetOwner()->GetActorRotation());
#else
	CharacterDirection = CalculateDirection(FPSVelocity, CharacterComponent->GetOwner()->GetActorRotation());
#endif
	
	HandleAimingActor(DeltaSeconds);
	HandleSpine(DeltaSeconds);
	HandleFirearmCollision(DeltaSeconds);
	
	if (bInterpLeaning)
	{
		InterpLeaning(DeltaSeconds);
	}

	AdditiveLocation = SwayLocation + RotationLagLocation;
	AdditiveRotation = SwayRotation + RotationLagRotation + MovementLagRotation;
}

void USKGCharacterAnimInstance::InterpLeaning(float DeltaSeconds)
{
	const FSKGLeanCurves LeanGraphs = CharacterComponent->GetLeanSettings().LeanCurves;
	if (LeanGraphs.IsValid())
	{
		const UCurveFloat* GraphToUse = LeanGraphs.LeanCurve;
		bool bEndingLean = false;
		if (!CharacterComponent->GetLeanSettings().bIncrementalLeaning && LeanGraphs.LeanEndCurve)
		{
			if ((GraphTimeToGoTo <= 0.0f && CurrentGraphTime < GraphTimeToGoTo) || (GraphTimeToGoTo >= 0.0f && CurrentGraphTime > GraphTimeToGoTo))
			{
				GraphToUse = LeanGraphs.LeanEndCurve;
				bEndingLean = true;
			}
		}
		
		float InterpSpeed = CharacterComponent->GetLeanSpeed(bEndingLean);
		if (HeldActor && HeldActor->GetClass()->ImplementsInterface(USKGProceduralAnimationInterface::StaticClass()))
		{
			InterpSpeed *= ISKGProceduralAnimationInterface::Execute_GetLeaningSpeedMultiplier(HeldActor);
		}
		CurrentGraphTime = UKismetMathLibrary::FInterpTo_Constant(CurrentGraphTime, GraphTimeToGoTo, DeltaSeconds, InterpSpeed);
		
		float GraphValue = GraphToUse->GetFloatValue(CurrentGraphTime);
		GraphValue = (CurrentGraphTime < 0.0f ? -GraphValue : GraphValue);
		
		GraphValue = UKismetMathLibrary::FInterpTo_Constant(LeanRotation.Pitch, GraphValue, DeltaSeconds, CharacterComponent->GetLeanSettings().LeaningSmoothInterpSpeed);
		
		LeanRotation.Pitch = GraphValue;
		
		//UE_LOG(LogTemp, Warning, TEXT("CurrentGraphTime: %f  GraphTimeToGoTo: %f"), CurrentGraphTime, GraphTimeToGoTo);
		if (CurrentGraphTime == GraphTimeToGoTo)
		{
			bInterpLeaning = false;
		}
	}
}

void USKGCharacterAnimInstance::SetLeaning(float TargetGraphTime)
{
	PressedGraphTime = CurrentGraphTime;
	GraphTimeToGoTo = TargetGraphTime;
	//UE_LOG(LogTemp, Warning, TEXT("GraphTimeToGoTo: %f"), GraphTimeToGoTo);
	LeanStartTime = GetWorld()->GetTimeSeconds();
	bInterpLeaning = true;
}

void USKGCharacterAnimInstance::HandleAimingActor(float DeltaSeconds)
{
	HeldActor = CharacterComponent->GetHeldActor();
	// check if aiming actor implements interface only here
	if (IsValid(HeldActor) && HeldActor->GetClass()->ImplementsInterface(USKGProceduralAnimationInterface::StaticClass()))
	{
		if (HeldActor->GetClass()->ImplementsInterface(USKGFirearmInterface::StaticClass()))
		{
			HandleFirearm(DeltaSeconds);
		}
		
		if (FirearmPose == ESKGFirearmPose::None)
		{
			SetRelativeToHand();
			InterpRelativeToHand(DeltaSeconds);
		}
		
		if (bInterpAiming)
		{
			InterpAimingAlpha(DeltaSeconds);
		}
		
		if (CharacterComponent->IsLocallyControlled())
		{
			if (bInterpCameraZoom)
			{
				InterpCameraZoom(DeltaSeconds);
			}
			
			if (CharacterComponent->GetMovementComponent()->Velocity.Size() || !MovementLagRotation.Equals(FRotator::ZeroRotator))
			{
				SetMovementLag(DeltaSeconds);
			}
		}

		if (bInterpCustomCurve)
		{
			InterpCustomCurve(DeltaSeconds);
		}
	}
	else
	{
		LeftHandIKAlpha = 0.0f;
	}
}

void USKGCharacterAnimInstance::HandleFirearm(float DeltaSeconds)
{
	BasePoseOffsetLocation = ISKGProceduralAnimationInterface::Execute_GetBasePoseOffset(HeldActor).GetLocation();
	BasePoseOffsetRotation = ISKGProceduralAnimationInterface::Execute_GetBasePoseOffset(HeldActor).Rotator();

	if (bUseLeftHandIK)
	{	// Handles Left Hand IK and Pose
		LeftHandPose = ISKGProceduralAnimationInterface::Execute_GetGripAnimation(HeldActor);
		if (LeftHandPose)
		{
			bValidLeftHandPose = true;
		}
		else
		{
			bValidLeftHandPose = false;
		}
		
		if (CharacterComponent->GetUseLeftHandIK() && ISKGProceduralAnimationInterface::Execute_UseLeftHandIK(HeldActor))
		{
			SetLeftHandIK();
		}
		else
		{
			LeftHandIKAlpha = 0.0f;
		}
	}

	if (CharacterComponent->IsLocallyControlled())
	{
		SetRotationLag(DeltaSeconds);// firearm
		HandleMovementSway(DeltaSeconds);// firearm
	}
	
	HandleSprinting();// firearm

	if (bInterpRecoil)
	{
		RecoilInterpTo(DeltaSeconds);// firearm
	}
	
	if (bInterpFirearmPose)
	{
		InterpFirearmPose(DeltaSeconds);// firearm
	}

	if (bInterpShakeCurve)
	{
		InterpShakeCurve(DeltaSeconds);// firearm
	}

	if (!IsValid(HeldActor))
	{
		BasePoseOffsetLocation = FVector::ZeroVector;
		BasePoseOffsetRotation = FRotator::ZeroRotator;
	}
}

void USKGCharacterAnimInstance::HandleSpine(float DeltaSeconds)
{
	if (bUseProceduralSpine && !bFreeLook)
	{
		if (!IsLocallyControlled())
		{
			SpineToInterpTo = CharacterComponent->GetBaseAimRotation();
			SpineToInterpTo = UKismetMathLibrary::NormalizedDeltaRotator(SpineToInterpTo, TryGetPawnOwner()->GetActorRotation());
			SpineToInterpTo.Roll = (SpineToInterpTo.Pitch * -1.0f);
			SpineToInterpTo.Pitch = 0.0f;
			SpineToInterpTo.Yaw = 0.0f;
			FRotator TempRotator = SpineToInterpTo;
			for (uint8 i = 0; i < SpineBlendPercents.Num(); ++i)
			{
				TempRotator.Roll = SpineToInterpTo.Roll * SpineBlendPercents[i];
				switch (i)
				{
				case 0: Spine0Rotation = UKismetMathLibrary::RInterpTo(Spine0Rotation, TempRotator, DeltaSeconds, 10.0f); break;
				case 1: Spine1Rotation = UKismetMathLibrary::RInterpTo(Spine1Rotation, TempRotator, DeltaSeconds, 10.0f); break;
				case 2: Spine2Rotation = UKismetMathLibrary::RInterpTo(Spine2Rotation, TempRotator, DeltaSeconds, 10.0f); break;
				case 3: Spine3Rotation = UKismetMathLibrary::RInterpTo(Spine3Rotation, TempRotator, DeltaSeconds, 10.0f); break;
				case 4: Spine4Rotation = UKismetMathLibrary::RInterpTo(Spine4Rotation, TempRotator, DeltaSeconds, 10.0f); break;
				default: checkNoEntry(); break;
				}
			}
		}
		else
		{
			SpineToInterpTo = CharacterComponent->GetControlRotation();
			SpineToInterpTo = UKismetMathLibrary::NormalizedDeltaRotator(SpineToInterpTo, TryGetPawnOwner()->GetActorRotation());
			SpineToInterpTo.Roll = (SpineToInterpTo.Pitch * -1.0f);
			SpineToInterpTo.Pitch = 0.0f;
			SpineToInterpTo.Yaw = 0.0f;
			FRotator TempRotator = SpineToInterpTo;
			for (uint8 i = 0; i < SpineBlendPercents.Num(); ++i)
			{
				TempRotator.Roll = SpineToInterpTo.Roll * SpineBlendPercents[i];
				switch (i)
				{
				case 0: Spine0Rotation = TempRotator; break;
				case 1: Spine1Rotation = TempRotator; break;
				case 2: Spine2Rotation = TempRotator; break;
				case 3: Spine3Rotation = TempRotator; break;
				case 4: Spine4Rotation = TempRotator; break;
				default: checkNoEntry(); break;
				}
			}
		}

		if (bInterpLeaning)
		{
			//InterpLeaning(DeltaSeconds);
		}
	}
}

void USKGCharacterAnimInstance::HandleFirearmCollision(float DeltaSeconds)
{
	if (IsValid(HeldActor) && IsValid(CharacterComponent) && CharacterComponent->IsUsingFirearmCollision())
	{
		if (HeldActor->GetClass()->ImplementsInterface(USKGFirearmCollisionInterface::StaticClass()))
		{
			// UPDATE THIS IN CHARACTER COMPONENT UPON AIMING ACTOR CHANGED
			FTransform FirearmGripOffset = CharacterComponent->GetInUseMesh()->GetSocketTransform(ISKGFirearmCollisionInterface::Execute_GetFirearmGripSocket(HeldActor), RTS_ParentBoneSpace);
			FirearmCollisionBoneLocationOffset = FirearmGripOffset.GetLocation();
			FirearmCollisionBoneRotationOffset = FirearmGripOffset.Rotator();
			CurrentCollisionSettings = ISKGFirearmCollisionInterface::Execute_GetCollisionSettings(HeldActor);
		
			const FTransform MuzzleTransform = ISKGFirearmCollisionInterface::Execute_GetCollisionMuzzleSocketTransform(HeldActor);
			FTransform RelativeMuzzle = MuzzleTransform.GetRelativeTransform(HeldActor->GetActorTransform());
			RelativeMuzzle.SetLocation(RelativeMuzzle.GetLocation() + ISKGFirearmCollisionInterface::Execute_GetCollisionSettings(HeldActor).MuzzlePositionOffset);
		
			float MuzzleDistanceToRoot = RelativeMuzzle.GetLocation().Size();

			FTransform EndTransform = RelativeMuzzle *  CharacterComponent->GetInUseMesh()->GetSocketTransform(FirearmCollisionVirtualBoneName, RTS_World);
		
			FVector End = (EndTransform).GetTranslation();
			FVector Start = End + (EndTransform.Rotator().Vector() * -1.0f) * MuzzleDistanceToRoot;
		
			FHitResult HitResult;
			TArray<AActor*> IgnoredActors;
			IgnoredActors.Append(ISKGFirearmCollisionInterface::Execute_GetPartsToIgnore(HeldActor));
			IgnoredActors.Add(CharacterComponent->GetOwner());
			IgnoredActors.Add(HeldActor);
		
			bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, CurrentCollisionSettings.TraceRadius, UEngineTypes::ConvertToTraceType(CharacterComponent->GetFirearmCollisionChannel()), false, IgnoredActors,
				CurrentCollisionSettings.DebugTrace, HitResult, true);

			if (bHit)
			{
#if WITH_EDITOR
				if (CurrentCollisionSettings.bPrintTraceHit)
				{
					if (AActor* HitActor = HitResult.GetActor())
					{
						FString DebugString = FString("Hit Actor: " + HitActor->GetName());
						if (UPrimitiveComponent* HitComponent = HitResult.GetComponent())
						{
							DebugString.Append(FString("    Hit Component: ") + HitComponent->GetName());
						}
						UKismetSystemLibrary::PrintString(GetWorld(), DebugString);
					}
				}
#endif
			
				FirearmMovedDistance = FVector::Distance(End, HitResult.Location);

				if (FirearmMovedDistance < CurrentCollisionSettings.DistanceBeforePush && ShortStockBlend == 0.0f)
				{
					return;
				}
				CollisionPoseAlpha = 1.0f;
				if (FirearmPose != ESKGFirearmPose::None) // CHECK IF IN SHORT STOCK POSE???
					{
					if (bIsAiming && FirearmMovedDistance > CurrentCollisionSettings.PushDistanceToStopAiming)
					{
						SetFirearmPose(ESKGFirearmPose::None);
						SetIsAiming(false);
					}
					else
					{
						if (!bIsAiming && CharacterComponent->IsAiming())
						{
							SetIsAiming(true);
						}
					}
					CollisionPose = FTransform();
					const FVector Pose = FVector (0.0f, -FirearmMovedDistance, 0.0f);

					FTransform CurrentTransform = FTransform(CollisionPoseRotation, CollisionPoseLocation);
					const FTransform InterpToTransform = FTransform(FRotator::ZeroRotator, Pose);
					CurrentTransform = UKismetMathLibrary::TInterpTo(CurrentTransform, InterpToTransform, DeltaSeconds, CurrentCollisionSettings.InterpolationSpeed);

					CollisionPoseLocation = CurrentTransform.GetLocation();
					CollisionPoseRotation = CurrentTransform.Rotator();
					}
				else
				{
					SetFirearmPose(ESKGFirearmPose::None);
					const FTransform ShortStockPose = ISKGFirearmCollisionInterface::Execute_GetCollisionShortStockPose(HeldActor);
					float MaxPush = -ShortStockPose.GetLocation().Y;
					ShortStockBlend = UKismetMathLibrary::NormalizeToRange(FirearmMovedDistance, 0.0f, MaxPush);
				
					if (FirearmMovedDistance > CurrentCollisionSettings.PushDistanceToStopAiming)
					{
						if (bIsAiming)
						{
							SetIsAiming(false);
						}
					}
					else
					{
						if (!bIsAiming && CharacterComponent->IsAiming())
						{
							SetIsAiming(true);
						}
					}
				
					if (ShortStockBlend < 1.0f)
					{					
						const FTransform ShortStockLerpped = UKismetMathLibrary::TLerp(FTransform(), ShortStockPose, ShortStockBlend);
						CollisionPose = UKismetMathLibrary::TInterpTo(CollisionPose, ShortStockLerpped, DeltaSeconds, CurrentCollisionSettings.InterpolationSpeed);
					}
					else
					{
						CollisionPose = ISKGFirearmCollisionInterface::Execute_GetCollisionPose(HeldActor);
						const FTransform CurrentPoseTransform = FTransform(CollisionPoseRotation, CollisionPoseLocation);
						CollisionPose = UKismetMathLibrary::TInterpTo(CurrentPoseTransform, CollisionPose, DeltaSeconds, CurrentCollisionSettings.InterpolationSpeed);

						if (CollisionPose.GetLocation().Y < FirearmMovedDistance)
						{
							FVector Correct = CollisionPose.GetLocation();
							Correct.Y = -FirearmMovedDistance - (CollisionPose.GetLocation().Y / 2.0f);
							CollisionPose.SetLocation(Correct);
						}
					}

					CollisionPoseLocation = CollisionPose.GetLocation();
					CollisionPoseRotation = CollisionPose.Rotator();
				}
			}
			else
			{
				CollisionPoseAlpha = 0.0f;
				ShortStockBlend = 0.0f;
				FirearmMovedDistance = 0.0f;
				if (CharacterComponent->IsAiming())
				{
					SetIsAiming(true);
				}
				else
				{
					SetFirearmPose(CharacterComponent->GetFirearmPose());
				}
			}
		}
	}
}

void USKGCharacterAnimInstance::SetFreeLook(bool FreeLook)
{
	if (IsValid(CharacterComponent))
	{
		bFreeLook = FreeLook;
		if (bFreeLook)
		{
			FreeLookStartRotation = CharacterComponent->GetControlRotation();
		}
		else
		{
			CharacterComponent->OnFreeLookEnd.Broadcast(FreeLookStartRotation);
			FreeLookStartRotation = FRotator::ZeroRotator;
		}
	}
}

void USKGCharacterAnimInstance::InterpRelativeToHand(float DeltaSeconds)
{
	// Change InterpSpeed to weight of firearm
	if (!bIndexOrTagForced)
	{
		AnimationIndex = ISKGProceduralAnimationInterface::Execute_GetAnimationIndex(HeldActor);
		AnimationGameplayTag = ISKGProceduralAnimationInterface::Execute_GetAnimationGameplayTag(HeldActor);
	}
	
	float InterpSpeed = AimInterpolationSpeed;
	float Multiplier = ISKGProceduralAnimationInterface::Execute_GetAimInterpolationMultiplier(HeldActor);
	Multiplier = UKismetMathLibrary::NormalizeToRange(Multiplier, -40.0f, 150.0f);
	Multiplier = FMath::Clamp(Multiplier, 0.0f, 1.0f);
	InterpSpeed *= Multiplier;

	// Change InterpSpeed to be modified by firearm in hand
	RelativeToHandTransform = UKismetMathLibrary::TInterpTo(RelativeToHandTransform, FinalRelativeHand, DeltaSeconds, InterpSpeed);
	RelativeToHandLocation = RelativeToHandTransform.GetLocation();
	RelativeToHandRotation = RelativeToHandTransform.Rotator();
	
	float HandToSightDistance = FinalRelativeHand.GetLocation().X;
	bool UseFixedCameraDistance = false;
	if (CharacterComponent->IsLocallyControlled() && !CharacterComponent->IsInThirdPerson())
	{
		FSKGAimCameraSettings CameraSettings;
		ISKGProceduralAnimationInterface::Execute_GetCameraSettings(HeldActor, CameraSettings);
		UseFixedCameraDistance = CameraSettings.bUsedFixedCameraDistance;
		if (CameraSettings.bUsedFixedCameraDistance)
		{
			HandToSightDistance = CameraSettings.CameraDistance;
		}
		else
		{
			HandToSightDistance -= CameraSettings.CameraDistance;
		}
	}
////////////////// ADD LOGIC HERE FOR GET STOCK LENGTH OF PULL IN THE INTERFACE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (!UseFixedCameraDistance && HeldActor->GetClass()->ImplementsInterface(USKGFirearmPartsInterface::StaticClass()))
	{
		HandToSightDistance += ISKGFirearmPartsInterface::Execute_GetStockLengthOfPull(HeldActor) / 2.0f;
	}
	SightDistance = UKismetMathLibrary::FInterpTo(SightDistance, HandToSightDistance * -1.0f, DeltaSeconds, InterpSpeed);
	SetSightTransform();

	if (RelativeToHandTransform.Equals(FinalRelativeHand))
	{
		bInterpRelativeToHand = false;
		SightDistance = HandToSightDistance * -1.0f;
		SetSightTransform();
	}
	
	if (!IsValid(HeldActor))
	{
		AnimationIndex = 0;
		AnimationGameplayTag = DefaultGameplayTag;
	}
}

void USKGCharacterAnimInstance::SetSightTransform()
{
	FTransform CameraTransform = CharacterComponent->GetInUseMesh()->GetSocketTransform(CharacterComponent->GetCameraSocket(), RTS_ParentBoneSpace);

	FRotator NewRot = FRotator::ZeroRotator;
	NewRot.Roll += -90.0f;
	NewRot.Yaw += 90.0f;

	CameraTransform.SetRotation(NewRot.Quaternion());
	
	FVector CameraVector = CameraTransform.GetLocation();
	const float AdditiveDistance = SightDistance + 10.0f;
	switch (CharacterComponent->GetRightHandAxis())
	{
	case EAxis::Type::Y : CameraVector.Y += AdditiveDistance; break;
	case EAxis::Type::Z : CameraVector.Z += AdditiveDistance; break;
	case EAxis::Type::X : CameraVector.X += AdditiveDistance; break;
	case EAxis::Type::None : break;
	}

	if (HeldActor->GetClass()->ImplementsInterface(USKGFirearmPartsInterface::StaticClass()) && !CharacterComponent->IsLocallyControlled() || CharacterComponent->IsInThirdPerson())
	{
		FVector StockOffset = ISKGFirearmPartsInterface::Execute_GetStockOffset(HeldActor) * -1.0f;
		StockOffset.Y += ISKGFirearmPartsInterface::Execute_GetStockLengthOfPull(HeldActor);
		
		CameraVector.X += StockOffset.Z * -1.0f;
		CameraVector.Z += StockOffset.X;
		CameraVector.Y += StockOffset.Y;
	}

	CameraTransform.SetLocation(CameraVector);
	
	SightLocation = CameraTransform.GetLocation();
	SightRotation = CameraTransform.Rotator();
}

void USKGCharacterAnimInstance::SetRelativeToHand()
{
	if (IsValid(HeldActor) && HeldActor->GetClass()->ImplementsInterface(USKGProceduralAnimationInterface::StaticClass()))
	{
		HeadAimingRotation = ISKGProceduralAnimationInterface::Execute_GetHeadRotation(HeldActor);

		const FTransform DefaultTransform = ISKGProceduralAnimationInterface::Execute_GetDefaultAimSocketTransform(HeldActor);
		FTransform SightSocketTransform = ISKGProceduralAnimationInterface::Execute_GetAimSocketTransform(HeldActor);
	
		if (SprintAlpha > 0.0f)
		{
			SightSocketTransform = DefaultTransform;
		}
		else
		{
			if (ISKGProceduralAnimationInterface::Execute_GetUseBasePoseCorrection(HeldActor))
			{
				SightSocketTransform = UKismetMathLibrary::TLerp(DefaultTransform, SightSocketTransform, AimingAlpha);
			}
		}

		const FTransform Hand_RTransform = CharacterComponent->GetInUseMesh()->GetSocketTransform(RightHandBone);
		FinalRelativeHand = UKismetMathLibrary::MakeRelativeTransform(SightSocketTransform, Hand_RTransform);
	
		DefaultRelativeToHand = UKismetMathLibrary::MakeRelativeTransform(DefaultTransform, Hand_RTransform);
		bInterpRelativeToHand = true;
	}
}

void USKGCharacterAnimInstance::SetLeftHandIK()
{
	const FTransform LeftHandIK = ISKGProceduralAnimationInterface::Execute_GetLeftHandIKTransform(HeldActor);
	if (LeftHandIK.GetLocation().Equals(FVector::ZeroVector))
	{
		LeftHandIKAlpha = 0.0f; return;
	}
	LeftHandIKAlpha = 1.0f;
	
	FVector OutPosition;
	FRotator OutRotation;

	CharacterComponent->GetInUseMesh()->TransformToBoneSpace(RightHandBone, LeftHandIK.GetLocation(), LeftHandIK.Rotator(), OutPosition, OutRotation);

	LeftHandIKTransform.SetLocation(OutPosition);
	LeftHandIKTransform.SetRotation(OutRotation.Quaternion());
	LeftHandIKTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
}

void USKGCharacterAnimInstance::InterpFirearmPose(float DeltaSeconds)
{
	if (FirearmPoseGraphSettings.bUseLegacySystem)
	{
		FTransform ToInterpFrom = FTransform(CurrentFirearmPoseRotation, CurrentFirearmPoseLocation);
		ToInterpFrom = UKismetMathLibrary::TInterpTo(ToInterpFrom, FirearmPoseToInterpTo, DeltaSeconds, ISKGProceduralAnimationInterface::Execute_GetHighLowPortPoseInterpolationSpeed(HeldActor));
		CurrentFirearmPoseLocation = ToInterpFrom.GetLocation();
		CurrentFirearmPoseRotation = ToInterpFrom.Rotator();
		
		if (CurrentFirearmPoseLocation.Equals(FirearmPoseToInterpTo.GetLocation()) && CurrentFirearmPoseRotation.Equals(FirearmPoseToInterpTo.Rotator()))
		{
			CharacterComponent->OnFirearmPoseFinished.Broadcast();
			bInterpFirearmPose = false;
		}
	}
	else
	{
		FirearmPoseCurrentTime = (GetWorld()->GetTimeSeconds() - FirearmPoseStartTime) * FirearmPoseGraphSettings.GetActiveCurveSetting().CurveSpeedMultiplier;
	
		const FTransform PoseTransform = FirearmPoseGraphSettings.GetTransform(FirearmPoseCurrentTime, CharacterComponent->IsLocallyControlled());

		CurrentFirearmPoseLocation = PoseTransform.GetLocation();
		CurrentFirearmPoseRotation = PoseTransform.Rotator();
		
		if (FirearmPoseCurrentTime >= FirearmPoseGraphSettings.GetActiveCurveSetting().CurveLength)
		{
			CharacterComponent->OnFirearmPoseFinished.Broadcast();
			FirearmPoseCurrentTime = 0.0f;
			bInterpFirearmPose = false;
		}
	}

	if (!IsValid(HeldActor))
	{
		bInterpFirearmPose = false;
	}
}

void USKGCharacterAnimInstance::SetFirearmPose(ESKGFirearmPose Pose)
{
	if (IsValid(HeldActor) && FirearmPose != Pose)
	{
		FirearmPoseStartTime = GetWorld()->GetTimeSeconds();
		
		FirearmPose = Pose;
		if (FirearmPose != ESKGFirearmPose::None)
		{
			switch (FirearmPose)
			{
			case ESKGFirearmPose::High: FirearmPoseToInterpTo = ISKGProceduralAnimationInterface::Execute_GetHighPortPose(HeldActor); FirearmPoseGraphSettings = ISKGProceduralAnimationInterface::Execute_GetHighPortCurveSettings(HeldActor); FirearmPoseGraphSettings.bGoIntoPose = true; break;
			case ESKGFirearmPose::Low: FirearmPoseToInterpTo = ISKGProceduralAnimationInterface::Execute_GetLowPortPose(HeldActor); FirearmPoseGraphSettings = ISKGProceduralAnimationInterface::Execute_GetLowPortCurveSettings(HeldActor); FirearmPoseGraphSettings.bGoIntoPose = true; break;
			case ESKGFirearmPose::ShortStock: FirearmPoseToInterpTo = ISKGProceduralAnimationInterface::Execute_GetShortStockPose(HeldActor); FirearmPoseGraphSettings = ISKGProceduralAnimationInterface::Execute_GetShortStockCurveSettings(HeldActor); FirearmPoseGraphSettings.bGoIntoPose = true; break;
			case ESKGFirearmPose::OppositeShoulder: FirearmPoseToInterpTo = ISKGProceduralAnimationInterface::Execute_GetOppositeShoulderPose(HeldActor); FirearmPoseGraphSettings = ISKGProceduralAnimationInterface::Execute_GetOppositeShoulderCurveSettings(HeldActor); FirearmPoseGraphSettings.bGoIntoPose = true; break;
			case ESKGFirearmPose::BlindFireLeft: FirearmPoseToInterpTo = ISKGProceduralAnimationInterface::Execute_GetBlindFireLeftPose(HeldActor); FirearmPoseGraphSettings = ISKGProceduralAnimationInterface::Execute_GetBlindFireLeftCurveSettings(HeldActor); FirearmPoseGraphSettings.bGoIntoPose = true; break;
			case ESKGFirearmPose::BlindFireUp: FirearmPoseToInterpTo = ISKGProceduralAnimationInterface::Execute_GetBlindFireUpPose(HeldActor); FirearmPoseGraphSettings = ISKGProceduralAnimationInterface::Execute_GetBlindFireUpCurveSettings(HeldActor); FirearmPoseGraphSettings.bGoIntoPose = true; break;
			default: FirearmPoseToInterpTo = FTransform(); FirearmPoseGraphSettings.bGoIntoPose = false;
			}
			SetIsAiming(false);
		}
		else
		{
			FirearmPoseToInterpTo = FTransform();
			FirearmPoseGraphSettings.bGoIntoPose = false;
			if (FirearmPoseGraphSettings.bUseLegacySystem && ISKGProceduralAnimationInterface::Execute_GetPerformShakeAfterPortPose(HeldActor))
			{
				PlayFirearmShakeCurve(true);
			}
		}

		if (!CharacterComponent->IsLocallyControlled())
		{
			RelativeToHandTransform = DefaultRelativeToHand;
			RelativeToHandLocation = RelativeToHandTransform.GetLocation();
			RelativeToHandRotation = RelativeToHandTransform.Rotator();
		}
		FirearmPoseAlpha = 1.0f;
		bInterpFirearmPose = true;
	}
}

void USKGCharacterAnimInstance::InterpShakeCurve(float DeltaSeconds)
{
	if (const UCurveVector* Curve = ISKGProceduralAnimationInterface::Execute_GetShakeCurve(HeldActor))
	{
		const float CurrentTime = (GetWorld()->GetTimeSeconds() - ShakeCurveStartTime) * ISKGProceduralAnimationInterface::Execute_GetShakeCurveSpeed(HeldActor);
		const FVector Shake = Curve->GetVectorValue(CurrentTime);
		ShakeRotation = FRotator(Shake.X, Shake.Y, Shake.Z);

		if (CurrentTime > ISKGProceduralAnimationInterface::Execute_GetShakeCurveDuration(HeldActor))
		{
			bInterpShakeCurve = false;
			ShakeCurveAlpha = 0.0f;
		}
	}
	else
	{
		ShakeCurveAlpha = 0.0f;
		bInterpShakeCurve = false;
	}

	if (!IsValid(HeldActor))
	{
		ShakeCurveAlpha = 0.0f;
		bInterpShakeCurve = false;
	}
}

void USKGCharacterAnimInstance::PlayFirearmShakeCurve(bool ManuallyPlay)
{
	if (IsValid(HeldActor) && bCanPlayShakeCurve)
	{
		bCanPlayShakeCurve = ManuallyPlay;
		ShakeRotation = FRotator::ZeroRotator;
		bInterpShakeCurve = true;
		ShakeCurveAlpha = 1.0f;
		ShakeCurveStartTime = GetWorld()->GetTimeSeconds();
	}
}

void USKGCharacterAnimInstance::InterpCustomCurve(float DeltaSeconds)
{
	if (IsValid(CustomCurveData.RotationCurve) || IsValid(CustomCurveData.LocationCurve))
	{
		const float CurrentTime = (GetWorld()->GetTimeSeconds() - CustomCurveStartTime) * CustomCurveData.CurveSpeed;
		if (CustomCurveData.LocationCurve)
		{
			CustomCurveLocation = CustomCurveData.LocationCurve->GetVectorValue(CurrentTime);
		}
		
		if (CustomCurveData.RotationCurve)
		{
			const FVector Shake = CustomCurveData.RotationCurve->GetVectorValue(CurrentTime);
			CustomCurveRotation = FRotator(Shake.X, Shake.Y, Shake.Z);
		}
		
		if (CurrentTime > CustomCurveData.CurveDuration)
		{
			bInterpCustomCurve = false;
			CustomCurveAlpha = 0.0f;
		}
	}
	else
	{
		bInterpCustomCurve = false;
		CustomCurveAlpha = 0.0f;
	}
}

void USKGCharacterAnimInstance::PlayCustomCurve(FSKGCurveData INCurveData)
{
	if (IsValid(INCurveData.RotationCurve) || IsValid(INCurveData.LocationCurve))
	{
		CustomCurveAlpha = 1.0f;
		CustomCurveData = INCurveData;
		CustomCurveStartTime = GetWorld()->GetTimeSeconds();
		bInterpCustomCurve = true;
	}
}

void USKGCharacterAnimInstance::InterpCameraZoom(float DeltaSeconds)
{
	float CurrentFOV = CharacterComponent->GetCameraComponent()->FieldOfView;
	float TargetFOV = CharacterComponent->GetDefaultFOV();
	float InterpSpeed = 10.0f;
	if (bIsAiming)
	{
		FSKGAimCameraSettings CameraSettings;
		ISKGProceduralAnimationInterface::Execute_GetCameraSettings(HeldActor, CameraSettings);
		TargetFOV -= CameraSettings.CameraFOVZoom;
		InterpSpeed = CameraSettings.CameraFOVZoomSpeed;
	}
	CurrentFOV = UKismetMathLibrary::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, InterpSpeed);
	CharacterComponent->GetCameraComponent()->SetFieldOfView(CurrentFOV);
	if (CurrentFOV == TargetFOV)
	{
		bInterpCameraZoom = false;
	}
}

void USKGCharacterAnimInstance::InterpAimingAlpha(float DeltaSeconds)
{
	float InterpSpeed = AimInterpolationSpeed;
	float Multiplier;
	if (bIsAiming)
	{
		Multiplier = ISKGProceduralAnimationInterface::Execute_GetAimInterpolationMultiplier(HeldActor);
	}
	else
	{
		Multiplier = ISKGProceduralAnimationInterface::Execute_GetUnAimInterpolationMultiplier(HeldActor);
	}
	
	Multiplier = UKismetMathLibrary::NormalizeToRange(Multiplier, -40.0f, 150.0f);
	InterpSpeed *= Multiplier;

	AimingAlpha = UKismetMathLibrary::FInterpTo(AimingAlpha, bIsAiming, DeltaSeconds, InterpSpeed);
	
	if ((bIsAiming && AimingAlpha >= 1.0f) || (!bIsAiming && AimingAlpha <= 0.0f))
	{
		bInterpAiming = false;
	}
}

void USKGCharacterAnimInstance::SetRotationLag(float DeltaSeconds)
{
	float InterpSpeed = RotationLagResetInterpolationSpeed;
	float Multiplier = ISKGProceduralAnimationInterface::Execute_GetRotationLagInterpolationMultiplier(HeldActor);
	Multiplier = UKismetMathLibrary::NormalizeToRange(Multiplier, -40.0f, 150.0f);
	InterpSpeed *= Multiplier;
	// Temp Workaround for consistency
	InterpSpeed *= 1.5f;
	const float Delta = (1.0f - (DeltaSeconds * 35.0f));
	
	const FRotator CurrentRotation = CharacterComponent->GetControlRotation();
	const FQuat Difference = (CurrentRotation - OldRotation).Quaternion() * Delta;

	FRotator Rotation = UKismetMathLibrary::RInterpTo(UnmodifiedRotationLag, Difference.Rotator(), DeltaSeconds, InterpSpeed);
	UnmodifiedRotationLag = Rotation;

	float FirearmWeightMultiplier = 1.0f;
	if (IsValid(HeldActor))
	{
		// Modify on HELD weapon weight	
		FirearmWeightMultiplier = UKismetMathLibrary::NormalizeToRange(ISKGFirearmInterface::Execute_GetFirearmStats(HeldActor).Weight, 0.0f, 25.0f);
		FirearmWeightMultiplier = FirearmWeightMultiplier * (0.3f / Multiplier);
	}
	
	Rotation *= 3.0f;
	Rotation *= FirearmWeightMultiplier;

	float InvertMultiplier = -1.0f;
	Rotation.Roll = Rotation.Pitch;
	Rotation.Pitch = 0.0f;
	if (bInvertRotationLag)
	{
		InvertMultiplier = 1.0f;
	}
	Rotation.Yaw = FMath::Clamp(Rotation.Yaw, -7.0f, 7.0f) * InvertMultiplier;	
	Rotation.Roll = FMath::Clamp(Rotation.Roll, -3.0f, 3.0f) * -InvertMultiplier;

	RotationLagRotation = Rotation;
	RotationLagLocation = FVector(Rotation.Yaw / 4.0f, 0.0f, Rotation.Roll / 1.5);
	RotationLagTransform.SetRotation(Rotation.Quaternion());
	RotationLagTransform.SetLocation(FVector(Rotation.Yaw / 4.0f, 0.0f, Rotation.Roll / 1.5));

	OldRotation = CurrentRotation;
}

void USKGCharacterAnimInstance::SetMovementLag(float DeltaSeconds)
{
	const FVector FPSVelocity = CharacterComponent->GetMovementComponent()->Velocity;
	float RightSpeed = FVector::DotProduct(FPSVelocity, CharacterComponent->GetActorRightVector());
	float VerticalSpeed = FPSVelocity.Z;
	RightSpeed = UKismetMathLibrary::NormalizeToRange(RightSpeed, 0.0f, 75.0f);
	VerticalSpeed = UKismetMathLibrary::NormalizeToRange(VerticalSpeed, 0.0f, 75.0f);

	const FSKGSwayMultipliers SwayMultipliers = ISKGProceduralAnimationInterface::Execute_GetSwayMultipliers(HeldActor);
	
	FRotator NewRot = MovementLagRotation;
	NewRot.Pitch = UKismetMathLibrary::FInterpTo(NewRot.Pitch, RightSpeed * SwayMultipliers.MovementRollMultiplier, DeltaSeconds, 10.0f);
	NewRot.Yaw = UKismetMathLibrary::FInterpTo(NewRot.Yaw, (RightSpeed * SwayMultipliers.MovementYawMultiplier) * -1.0f, DeltaSeconds, 10.0f);
	NewRot.Roll = UKismetMathLibrary::FInterpTo(NewRot.Roll, VerticalSpeed * SwayMultipliers.MovementPitchMultiplier, DeltaSeconds, 10.0f);
	
	MovementLagRotation = NewRot;
}

void USKGCharacterAnimInstance::HandleMovementSway(float DeltaSeconds)
{	// REWRITE FOR USE WITH AIMING ACTOR AS WELL???
	if (const UCurveVector* Curve = ISKGProceduralAnimationInterface::Execute_GetMovementSwayCurve(HeldActor))
	{
		const float OldVelocityMultiplier = VelocityMultiplier;
		VelocityMultiplier = UKismetMathLibrary::NormalizeToRange(CharacterVelocity, 0.0f, CharacterComponent->GetMovementComponentSprintSpeed());
		if (VelocityMultiplier < OldVelocityMultiplier)
		{
			VelocityMultiplier = UKismetMathLibrary::FInterpTo(OldVelocityMultiplier, VelocityMultiplier, DeltaSeconds, 3.2f);
		}
		if (VelocityMultiplier < 0.25f)
		{
			VelocityMultiplier = 0.25f;
		}
		float Multiplier = 1.1f;
		if (ISKGProceduralAnimationInterface::Execute_UseStatsForMovementSway(HeldActor))
		{
			const FSKGFirearmStats Stats = ISKGFirearmInterface::Execute_GetFirearmStats(HeldActor);
			Multiplier = Stats.Weight * (Stats.Weight * 3.0f / (Stats.Ergonomics * 1.5f)) + 1.0f;
			Multiplier = FMath::Clamp(Multiplier, 0.5f, 1.0f);
		}

		SwayMultiplier = UKismetMathLibrary::FInterpTo(SwayMultiplier, ISKGProceduralAnimationInterface::Execute_GetSwayMultiplier(HeldActor), DeltaSeconds, 2.0f);
		
		CurveTimer += (DeltaSeconds * VelocityMultiplier);
		FVector Graph = Curve->GetVectorValue(CurveTimer);
		Graph *= VelocityMultiplier * Multiplier;
		const FRotator Rotation = FRotator(Graph.Y, Graph.X, Graph.Z);
		SwayLocation = Graph * SwayMultiplier;
		SwayRotation = Rotation * SwayMultiplier;
	}
}

void USKGCharacterAnimInstance::HandleSprinting()
{
	const bool bForcedIntoSprint = CharacterComponent->GetForceIntoSprintPose();
	if (CharacterComponent->GetIsSprinting() && (CharacterVelocity > CharacterComponent->GetMovementComponentSprintSpeed() / 2.0f || bForcedIntoSprint))
	{
		if (FirearmPose != ESKGFirearmPose::None)
		{
			CharacterComponent->StopFirearmPose();
		}
		SprintAlpha = 1.0f;
		if (bSprinting != true)
		{
			bSprinting = true;
			SetIsAiming(false);
		}
		
		SprintPoseLocation = ISKGProceduralAnimationInterface::Execute_GetSprintPose(HeldActor).GetLocation();
		SprintPoseRotation = ISKGProceduralAnimationInterface::Execute_GetSprintPose(HeldActor).Rotator();
		SuperSprintPoseLocation = ISKGProceduralAnimationInterface::Execute_GetSuperSprintPose(HeldActor).GetLocation();
		SuperSprintPoseRotation = ISKGProceduralAnimationInterface::Execute_GetSuperSprintPose(HeldActor).Rotator();
		
		if (bForcedIntoSprint)
		{
			if (CharacterComponent->GetSprintType() == ESKGSprintType::SuperSprint)
			{
				SuperSprintAlpha = 1.0f;
				SprintAlpha = 0.0f;
			}
			else
			{
				SuperSprintAlpha = 0.0f;
				SprintAlpha = 1.0f;
			}
		}
		else if (CharacterComponent->CanSuperSprint())
		{
			if (CharacterVelocity > CharacterComponent->GetMovementComponentSprintSpeed() + 25.0f)
			{
				SprintAlpha = 0.0f;
				SuperSprintAlpha = 1.0f;
			}
			else if (CharacterComponent->GetSprintType() == ESKGSprintType::SuperSprint)
			{
				SprintAlpha = 0.5f;
				SuperSprintAlpha = 0.0f;
			}
			else
			{
				SuperSprintAlpha = 0.0f;
				SprintAlpha = 1.0f;
			}
		}
	}
	else if (bSprinting != false)
	{
		bSprinting = false;
		if (CharacterComponent->IsAiming())
		{
			SetIsAiming(true);
		}
		SprintAlpha = 0.0f;
		SuperSprintAlpha = 0.0f;
	}
}

bool USKGCharacterAnimInstance::IsLocallyControlled()
{
	if (IsValid(CharacterComponent))
	{
		bIsLocallyControlled = CharacterComponent->IsLocallyControlled();
	}
	return bIsLocallyControlled;
}

void USKGCharacterAnimInstance::SetIsAiming(bool IsAiming)
{
	if (!(FirearmMovedDistance > CurrentCollisionSettings.PushDistanceToStopAiming) || !IsAiming)
	{
		if ((IsAiming && !bCanAim) || bInCollisionPose || bSprinting)
		{
			bIsAiming = false;
			bInterpAiming = true;
			bInterpCameraZoom = true;
			return;
		}

		if (bIsAiming != IsAiming)
		{
			SetFirearmPose(ESKGFirearmPose::None);
			bIsAiming = IsAiming;
			bInterpAiming = true;
			bInterpCameraZoom = true;
		}
	}
}

void USKGCharacterAnimInstance::SetIsReloading(bool IsReloading, float BlendAlpha)
{
	if (IsReloading)
	{
		RotationAlpha = BlendAlpha;
		LeftHandIKAlpha = 0.0f;
	}
	else
	{
		RotationAlpha = 0.0f;
		LeftHandIKAlpha = 1.0f;
	}
}

void USKGCharacterAnimInstance::SetCanAim(bool CanAim)
{
	bCanAim = CanAim;
}

void USKGCharacterAnimInstance::CycledSights()
{
	SetRelativeToHand();
	bInterpCameraZoom = true;
}

void USKGCharacterAnimInstance::EnterCustomPose(const FTransform& Pose)
{
	CustomPoseLocation = Pose.GetLocation();
	CustomPoseRotation = Pose.Rotator();
	CustomPoseAlpha = 1.0f;
}

void USKGCharacterAnimInstance::ClearCustomPose()
{
	CustomPoseAlpha = 0.0f;
}

void USKGCharacterAnimInstance::RecoilInterpToZero(float DeltaSeconds)
{
	FinalRecoilTransform = UKismetMathLibrary::TInterpTo(FinalRecoilTransform, FTransform(), DeltaSeconds, 8.0f); // def = 6
	RecoilLocation = FinalRecoilTransform.GetLocation();
	RecoilRotation = FinalRecoilTransform.Rotator();
	if (RecoilLocation.Equals(FVector::ZeroVector, 0.1f) && RecoilRotation.Equals(FRotator::ZeroRotator, 0.1f))
	{
		//bInterpRecoil = false;
	}
}

void USKGCharacterAnimInstance::RecoilInterpTo(float DeltaSeconds)
{
	const float CurrentTime = GetWorld()->GetTimeSeconds() - RecoilStartTime;
	const FSKGRecoilData RecoilData = ISKGFirearmInterface::Execute_GetRecoilData(HeldActor);
	const float VerticalRecoilMultiplier = ISKGFirearmInterface::Execute_GetFirearmStats(HeldActor).VertialRecoilMultiplier * RecoilMultiplier;
	const float HorizontalRecoilMultiplier = ISKGFirearmInterface::Execute_GetFirearmStats(HeldActor).HorizontalRecoilMultiplier * RecoilMultiplier;
	
	if (RecoilData.RecoilLocationCurve)
	{
		const float Randomess = FMath::RandRange(RecoilData.RecoilLocationRandomness.Min, RecoilData.RecoilLocationRandomness.Max);
		RecoilLocation += ((RecoilData.RecoilLocationCurve->GetVectorValue(CurrentTime) * Randomess) * RecoilMultiplier) * (100.0f * DeltaSeconds);
		FinalRecoilTransform.SetLocation(RecoilLocation);
	}
	if (RecoilData.RecoilRotationCurve)
	{
		const FVector CurveData = RecoilData.RecoilRotationCurve->GetVectorValue(CurrentTime);
		const float PitchRandom = (CurveData.X * FMath::RandRange(RecoilData.RecoilPitchRandomness.Min, RecoilData.RecoilPitchRandomness.Max)) * VerticalRecoilMultiplier;
		const float YawRandom = (CurveData.Y * FMath::RandRange(RecoilData.RecoilYawRandomness.Min, RecoilData.RecoilYawRandomness.Max)) * HorizontalRecoilMultiplier;
		const float RollRandom = (CurveData.Z * FMath::RandRange(RecoilData.RecoilRollRandomness.Min, RecoilData.RecoilRollRandomness.Max)) * HorizontalRecoilMultiplier;
		
		RecoilRotation += FRotator(RollRandom, PitchRandom, YawRandom) * (100.0f * DeltaSeconds);
		FinalRecoilTransform.SetRotation(RecoilRotation.Quaternion());
	}
	if (RecoilData.bUseControlRotation && RecoilData.ControlRotationCurve && CharacterComponent && CharacterComponent->IsLocallyControlled())
	{
		const FVector CurveData = RecoilData.ControlRotationCurve->GetVectorValue(CurrentTime);
		const float PitchRandom = ((CurveData.X * FMath::RandRange(RecoilData.ControlRotationPitchRandomness.Min, RecoilData.ControlRotationPitchRandomness.Max)) * VerticalRecoilMultiplier) * RecoilData.ControlRotationScale;
		const float YawRandom = ((CurveData.Y * FMath::RandRange(RecoilData.ControlRotationYawRandomness.Min, RecoilData.ControlRotationYawRandomness.Max)) * HorizontalRecoilMultiplier) * RecoilData.ControlRotationScale;
		
		CharacterComponent->AddControlRotation(FRotator(PitchRandom, YawRandom, 0.0f) * (100.0f * DeltaSeconds));
	}

	/*if (bApplyForwardVector && IsValid(HeldActor) && HeldActor->GetClass()->ImplementsInterface(USKGFirearmPartsInterface::StaticClass()))
	{
		bApplyForwardVector = false;
		FTransform MuzzleTransform = ISKGFirearmPartsInterface::Execute_GetMuzzleSocketTransform(HeldActor);
		MuzzleTransform = MuzzleTransform.GetRelativeTransform(CharacterComponent->GetOwner()->GetActorTransform());
		FVector MuzzleForward = MuzzleTransform.GetRotation().GetForwardVector();
		
		RecoilLocation *= MuzzleForward;
		FinalRecoilTransform.SetLocation(RecoilLocation);
		FVector End = MuzzleTransform.GetLocation() + RecoilLocation * 300.0f;
		DrawDebugLine(GetWorld(), MuzzleTransform.GetLocation(), End, FColor::Red, false, 5.0f, 0, 2.0f);
	}*/

	RecoilInterpToZero(DeltaSeconds);
}

void USKGCharacterAnimInstance::PerformRecoil(float Multiplier)
{
	if (HeldActor)
	{
		RecoilStartTime = GetWorld()->GetTimeSeconds();
		RecoilMultiplier = Multiplier;

		if (ShotCounter == 0)
		{
			ShotCounterStartTime = RecoilStartTime;
		}
		++ShotCounter;
		
		bApplyForwardVector = true;
		bInterpRecoil = true;
	}
}