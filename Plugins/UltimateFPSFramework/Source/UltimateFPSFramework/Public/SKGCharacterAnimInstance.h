//Copyright 2021, Dakota Dawe, All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "GameplayTagContainer.h"
#include "SKGCharacterAnimInstance.generated.h"

class ASKGFirearm;
class USKGCharacterComponent;
class UAnimSequence;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API USKGCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
		
public:
	USKGCharacterAnimInstance();

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Toggles")
	bool bUseProceduralSpine;
	// MAKE SURE IT ADDS UP TO 1
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Toggles", meta = (EditCondition = "bUseProceduralSpine", EditConditionHides))
	TArray<float> SpineBlendPercents;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Toggles")
	bool bUseLeftHandIK;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Collision")
	FName FirearmCollisionVirtualBoneName;
	
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	USKGCharacterComponent* CharacterComponent;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	float CharacterDirection;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	float CharacterVelocity;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	bool bEquipped;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Firearm")
	int32 AnimationIndex;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Firearm")
	FGameplayTag AnimationGameplayTag;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	FGameplayTag DefaultGameplayTag;
	bool bIndexOrTagForced;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Firearm")
	AActor* HeldActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	UAnimSequence* LeftHandPose;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	bool bValidLeftHandPose;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	float AimInterpolationSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	float CycleSightsInterpolationSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	float RotationLagResetInterpolationSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	float MotionLagResetInterpolationSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	FName RightHandBone;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	FVector RelativeToHandLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	FRotator RelativeToHandRotation;
	FTransform RelativeToHandTransform;
	FTransform FinalRelativeHand;
	bool bInterpRelativeToHand;

	bool bFirstRun;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	FVector SightLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	FRotator SightRotation;

	float SightDistance;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	float AimingAlpha;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	bool bIsAiming;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Aiming")
	FRotator HeadAimingRotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Sway")
	bool bInvertRotationLag;

	void SetSightTransform();
	void SetRelativeToHand();
	void InterpRelativeToHand(float DeltaSeconds);

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | LeftHandIK")
	FTransform LeftHandIKTransform;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | LeftHandIK")
	float LeftHandIKAlpha;
	void SetLeftHandIK();

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Actions")
	float RotationAlpha;
	
	bool bInterpAiming;
	bool bInterpCameraZoom;
	void InterpCameraZoom(float DeltaSeconds);
	float DefaultCameraFOV;

	void InterpAimingAlpha(float DeltaSeconds);

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Lag")
	FVector RotationLagLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Lag")
	FRotator RotationLagRotation;
	FTransform RotationLagTransform;
	FRotator UnmodifiedRotationLag;
	FRotator OldRotation;
	void SetRotationLag(float DeltaSeconds);

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Lag")
	FVector MovementLagLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Lag")
	FRotator MovementLagRotation;

	void SetMovementLag(float DeltaSeconds);
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	bool bIsLocallyControlled;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FSKGFirearmStats FirearmStats;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator Spine0Rotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator Spine1Rotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator Spine2Rotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator Spine3Rotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator Spine4Rotation;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Recoil")
	FVector RecoilLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Recoil")
	FRotator RecoilRotation;
	FTransform RecoilTransform;
	FTransform FinalRecoilTransform;
	bool bApplyForwardVector;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Recoil")
	FVector BasePoseOffsetLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Recoil")
	FRotator BasePoseOffsetRotation;

	bool bInterpRecoil;
	float RecoilStartTime;
	float RecoilMultiplier;
	float ShotCounterStartTime;
	uint8 ShotCounter;

	void RecoilInterpToZero(float DeltaSeconds);
	void RecoilInterpTo(float DeltaSeconds);

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator LeanRotation;
	bool bInterpLeaning;
	float CurrentGraphTime;
	float PressedGraphTime;
	float LeanStartTime;
	float GraphTimeToGoTo;
	void InterpLeaning(float DeltaSeconds);
	
	float CurveTimer;
	float VelocityMultiplier;
	void HandleMovementSway(float DeltaSeconds);
	void HandleSprinting();
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FVector AdditiveLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator AdditiveRotation;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FVector SwayLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator SwayRotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FVector SprintPoseLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator SprintPoseRotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FVector SuperSprintPoseLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator SuperSprintPoseRotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	float SprintAlpha;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	float SuperSprintAlpha;
	bool bSprinting;

	float SwayMultiplier;

	bool bCustomizingFirearm;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FTransform WeaponCustomizingTransform;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	float WeaponCustomizingAlpha;

	bool bCanAim;

	bool bInterpFirearmPose;
	ESKGFirearmPose FirearmPose;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	float FirearmPoseAlpha;
	FTransform FirearmPoseToInterpTo;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Poses")
	FVector CurrentFirearmPoseLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Poses")
	FRotator CurrentFirearmPoseRotation;
	float FirearmPoseStartTime;
	float FirearmPoseCurrentTime;
	FSKGFirearmPoseCurveSettings FirearmPoseGraphSettings;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Poses")
	FVector CollisionPoseLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Poses")
	FRotator CollisionPoseRotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Poses")
	float CollisionPoseAlpha;
	FSKGCollisionSettings CurrentCollisionSettings;
	float ShortStockBlend;
	float FirearmMovedDistance;
	bool bInCollisionPose;
	FTransform CollisionPose;

	FTransform DefaultRelativeToHand;

	void InterpFirearmPose(float DeltaSeconds);
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator FreeLookRotation;
	FRotator FreeLookStartRotation;
	bool bFreeLook;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FRotator ShakeRotation;
	bool bInterpShakeCurve;
	void InterpShakeCurve(float DeltaSeconds);
	float ShakeCurveStartTime;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	float ShakeCurveAlpha;
	bool bCanPlayShakeCurve;
	
	FRotator SpineToInterpTo;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Poses")
	FVector CustomPoseLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Poses")
	FRotator CustomPoseRotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Poses")
	float CustomPoseAlpha;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Curve")
	FVector CustomCurveLocation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Curve")
	FRotator CustomCurveRotation;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Curve")
	float CustomCurveAlpha;
	FSKGCurveData CustomCurveData;
	float CustomCurveStartTime;
	bool bInterpCustomCurve;
	void InterpCustomCurve(float DeltaSeconds);

	// Returns true if you are the one controlling the actor of this AnimBP
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Extra")
	bool IsLocallyControlled();

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Curve")
	FVector FirearmCollisionBoneLocationOffset;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "SKGFPSFramework | Curve")
	FRotator FirearmCollisionBoneRotationOffset;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | FirearmCollision")
	TEnumAsByte<ECollisionChannel> FirearmCollisionChannel;
	void HandleFirearm(float DeltaSeconds);
	void HandleAimingActor(float DeltaSeconds);
	void HandleSpine(float DeltaSeconds);
	void HandleFirearmCollision(float DeltaSeconds);
	
public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Aiming")
	void SetIsAiming(bool IsAiming);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Aiming")
	void CycledSights();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Aiming")
	void PlayFirearmShakeCurve(bool ManuallyPlay = false);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Aiming")
	void PlayCustomCurve(FSKGCurveData INCurveData);
	
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Animation")
	void EnterCustomPose(const FTransform& Pose);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Animation")
	void ClearCustomPose();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Animation")
	void SetAnimationIndex(int32 NewAnimationIndex, bool bForceIndex) { AnimationIndex = NewAnimationIndex; bIndexOrTagForced = bForceIndex; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Animation")
	void SetAnimationGameplayTag(FGameplayTag GameplayTag, bool bForceTag) { AnimationGameplayTag = GameplayTag; bIndexOrTagForced = bForceTag; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Animation")
	void ClearForcedIndexAndTag() { bIndexOrTagForced = false; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Animation")
	FTransform GetRecoilTransform() const { return FinalRecoilTransform; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Animation")
	FTransform GetSwayTransform() const { return FTransform(SwayRotation, SwayLocation); }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Animation")
	FTransform GetRotationLagTransform() const { return RotationLagTransform; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Animation")
	FTransform GetMovementLagTransform() const { return FTransform(MovementLagRotation, MovementLagLocation); }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Animation")
	void SetLeaning(float TargetGraphTime);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Animation")
	void SetFirearmPose(ESKGFirearmPose Pose);
	
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Actions")
	void SetEquipped(bool Equipped) { bEquipped = Equipped; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Actions")
	bool GetEquipped() const { return bEquipped; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Actions")
	void SetIsReloading(bool IsReloading, float BlendAlpha = 0.35f);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Actions")
	void SetCanAim(bool CanAim);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Actions")
	void PerformRecoil(float Multiplier = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Actions")
	void EnableLeftHandIK(bool Enable) { Enable ? LeftHandIKAlpha = 1.0f : LeftHandIKAlpha = 0.0f; }

	void SetCharacterComponent(USKGCharacterComponent* INCharacterComponent) { CharacterComponent = INCharacterComponent;}

	void SetFreeLook(bool FreeLook);
};
