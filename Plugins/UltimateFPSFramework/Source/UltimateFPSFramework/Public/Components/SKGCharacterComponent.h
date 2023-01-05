// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "Actors/SKGFirearm.h"
#include "SKGCharacterComponent.generated.h"

class USKGCharacterAnimInstance;
class USkeletalMeshComponent;
class UCameraComponent;
class ISKGInfraredInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFreeLookEnd, FRotator, NewControlRotation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnEquipFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAimingActorChanged, AActor*, OldAimingActor, AActor*, NewAimingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPoseFinished);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ULTIMATEFPSFRAMEWORK_API USKGCharacterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USKGCharacterComponent();

protected:
	TWeakObjectPtr<USKGCharacterAnimInstance> AnimationInstance;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Readables")
	UCameraComponent* FPCameraComponent;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Readables")
	USkeletalMeshComponent* FPMesh;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Readables")
	USkeletalMeshComponent* TPMesh;
	
	// Sets to use third person mode by default (replacement first person poses with third person poses for local client)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	bool bIsThirdPersonDefault;
	// Will set the owner automatically when you call EquipHeldActor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	bool bAutoSetOwnerOnEquipHeldActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	bool bUseParentSocketForAiming;
	// The socket that the camera is to be attached to
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FName CameraSocket;
	// The camera sockets parent bone such as the *head* bone in the example character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	FName CameraSocketParentBone;
	// Default is for mannequin. If you find adjusting the Camera Distance in the Camera Settings (of firearm and sights) going the wrong direction, change it
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Default")
	TEnumAsByte<EAxis::Type> RightHandAxis;
	// Settings for leaning left/right
	UPROPERTY(EditAnywhere, Category = "SKGFPSFramework | Default")
	FSKGLeanSettings DefaultLeanSettings;
	// Used for firearm attaching ensural to prevent attaching issues, can be ignored
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	int32 MaxFirearmAttachmentAttempts;
	// Used for firearm attaching ensural to prevent attaching issues, can be ignored
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	float FirearmReAttachmentAttemptInterval;
	uint8 AttachmentAttempt;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Infrared")
	FSKGInfraredMaterialSettings InfraredMaterialSettings;
	// Set this to the max speed your character component allows for movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Animation")
	float MovementComponentSprintSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Animation")
	bool bAllowSuperSprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Animation", meta = (EditCondition = "bAllowSuperSprint", EditConditionHides))
	float MovementComponentSuperSprintSpeed;
	// The max angle you can look up (90 = straight up)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Animation")
	float MaxLookUpAngle;
	// The max angle you can look down (90 = stright down)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Animation")
	float MaxLookDownAngle;
	// How far you can look left/right in freelook
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | FreeLook")
	float MaxLookLeftRight;
	// How far you can look up/down in freelook
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | FreeLook")
	float MaxLookUpDown;
	
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Animation")
	bool bForceIntoSprintPose;

	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | FirearmCollision")
	bool bUseFirearmCollision;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | FirearmCollision")
	TEnumAsByte<ECollisionChannel> FirearmCollisionChannel;

	bool bInThirdPerson;

	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Animation")
	bool bCanAim;

	bool bHighPort;
	bool bLowPort;
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetFirearmPose(ESKGFirearmPose NewPortPose);
	UPROPERTY(ReplicatedUsing = OnRep_FirearmPose)
	ESKGFirearmPose FirearmPose;
	UFUNCTION()
	void OnRep_FirearmPose() const;
	
	UPROPERTY(Replicated)
	ESKGSprintType SprintType;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSprinting(ESKGSprintType NewSprintType);
	
	UPROPERTY(ReplicatedUsing = OnRep_IsAiming)
	bool bIsAiming;
	UFUNCTION()
	void OnRep_IsAiming();

	UPROPERTY(ReplicatedUsing = OnRep_HeldActor)
	AActor* HeldActor;
	UFUNCTION()
	void OnRep_HeldActor(AActor* OldActor = nullptr);

	UPROPERTY(ReplicatedUsing = OnRep_LeanSettings)
	FSKGLeanSettings LeanSettings;
	UFUNCTION()
	void OnRep_LeanSettings();
	bool bLeanLeftDown;
	bool bLeanRightDown;

	UPROPERTY(ReplicatedUsing = OnRep_UseLeftHandIK)
	bool bUseLeftHandIK;
	UFUNCTION()
	void OnRep_UseLeftHandIK() const;
	
	TWeakObjectPtr<APawn> OwningPawn;
	bool bIsInitialized;
	bool bLocallyControlled;
	float DefaultCameraFOV;
	
	TArray<TWeakObjectPtr<AActor>> InfraredInfraredDevices;
	bool bNightVisionOn;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void HandleInfraredMaterialCollection();
	
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetAiming(bool IsAiming);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetLean(float CurveTime, bool bIncremental);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetUseLeftHandIK(bool bUse);

	UPROPERTY(ReplicatedUsing = OnRep_FreeLook)
	bool bFreeLook;
	UFUNCTION()
	void OnRep_FreeLook();

	UPROPERTY(Replicated)
	float ControlYaw;

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_ClearCurrentHeldActor();

	void SetLeaning(bool bIncremental);
	UCurveFloat* GetLeanCurve() const { return LeanSettings.LeanCurves.LeanCurve; }
	float GetLeanCurveTime(bool bLeft) const { return bLeft ? LeanSettings.LeanCurveTimeLeft : LeanSettings.LeanCurveTimeRight; }

	bool HeldActorImplementsFirearmInterface() const { return IsValid(HeldActor) && HeldActor->GetClass()->ImplementsInterface(USKGFirearmPartsInterface::StaticClass()); }
	
public:
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	FSKGLeanSettings GetLeanSettings();

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Leaning")
	void LeanLeft();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Leaning")
	void LeanRight();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void StopLeaning();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void StopLeanLeft();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void StopLeanRight();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void IncrementalLean(ESKGLeaning LeanDirection);
	// Resets the characters lean angle to straight
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void ResetLeanAngle();
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Procedural")
	float GetLeanSpeed(bool bGetEndLeanSpeed = false) const;
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void SetMaxLookUpAngle(const float Angle);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void SetMaxLookDownAngle(const float Angle);
	
	void AddInfraredDevice(AActor* LightLaser);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | CharacterComponent")
	void SetNightVisionOn(bool bOn);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | CharacterComponent")
	bool NightVisionOn() const { return bNightVisionOn; }
	
	bool HasAuthority() const { return GetOwner() ? GetOwner()->HasAuthority() : false; }
	/**
	 * This function is required to be called by any character it is applied to as it handles everything
	 * @param CameraComponent Plug in your Camera here
	 * @param bAutoAttach If this is true it will auto attach the camera to the set socket in the Character Component settings
	 * @param FirstPersonMesh This is your first person mesh. If your using True First Person then plug in your full body mesh here, if using just Arms then plug that in here
	 * @param ThirdPersonMesh This is for your full body mesh.
	*/
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | CharacterComponent")
	void Init(UCameraComponent* CameraComponent, bool bAutoAttach, USkeletalMeshComponent* FirstPersonMesh, USkeletalMeshComponent* ThirdPersonMesh);
	/**
	 * Returns the current in use mesh
	 * @return This will return the first person mesh if you are locally controlled, and the third person mesh for remote clients
	*/
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	USkeletalMeshComponent* GetInUseMesh() const;
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	USKGCharacterAnimInstance* GetAnimationInstance() const  { return AnimationInstance.Get(); }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	APawn* GetOwningPawn() const { return OwningPawn.Get(); }
	template <typename Type>
	Type* GetOwningPawn() const { return Cast<Type>(OwningPawn.Get()); }
	
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	UPawnMovementComponent* GetMovementComponent() const;
	template <class T>
	T* GetMovementComponent() const;
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Character", meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsLocallyControlled() { return bLocallyControlled; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	FRotator GetBaseAimRotation()const;
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	FRotator GetControlRotation()const;
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Character")
	void AddControlRotation(const FRotator& Rotation);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	AController* GetOwningController() const;
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	const FName& GetCameraSocket()const;
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	UCameraComponent* GetCameraComponent() const { return FPCameraComponent; }
	// Will return the current lean angle of the character
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	float GetCurrentLeanCurveTime()const { return LeanSettings.CurrentLeanCurveTime; }
	
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	float GetMovementComponentSprintSpeed() const { return MovementComponentSprintSpeed; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	float GetMovementComponentSuperSprintSpeed() const { return MovementComponentSuperSprintSpeed; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Character", meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsAiming() { return bIsAiming; }
	// Simply returns the owning characters forward vector, just a simple wrapper around the function on AActor
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	FVector GetActorForwardVector() const;
	// Simply returns the owning characters right vector, just a simple wrapper around the function on AActor
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	FVector GetActorRightVector() const;
	// Makes the owning character ragdoll
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Character")
	void RagdollCharacter();
	// Makes the owning character ragdoll with force applied at the ImpactLocation
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Character")
	void RagdollCharacterWithForce(const FVector ImpactLocation, const float ImpactForce = 0.0f);
	/**
	 * Sets the system to utilize third person poses for local player if ThirdPerson is true
	 * @param bThirdPerson If true, change the system to use third person poses for local player.
	 * If false, change the system to use first person poses for local player
	*/
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Character")
	void SetThirdPersonView(bool bThirdPerson);
	// Returns true if the system is set to use third person poses
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Character")
	bool IsInThirdPerson() const { return bInThirdPerson; }

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void SetCanAim(bool AllowAim) { bCanAim = AllowAim; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Procedural")
	bool GetCanAim() const { return bCanAim; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void StartAiming();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void StopAiming();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void SetSprinting(ESKGSprintType NewSprintType);
	// Forces you into the sprinting pose to be used with the pose editor
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void SetForceIntoSprintPose(bool bForce) { bForceIntoSprintPose = bForce; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Procedural")
	bool GetIsSprinting() const { return SprintType == ESKGSprintType::Sprint || SprintType == ESKGSprintType::SuperSprint; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Procedural")
	ESKGSprintType GetSprintType() const { return SprintType; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Procedural")
	bool CanSuperSprint() const { return bAllowSuperSprint; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Procedural")
	bool GetForceIntoSprintPose() const { return bForceIntoSprintPose; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Procedural")
	bool GetIsAiming() const { return bIsAiming; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	bool IsUsingFirearmCollision() const { return bUseFirearmCollision; }
	// Whether or not to use the firearm collision system. True = use, False = dont use
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void SetFirearmCollision(bool EnableFirearmCollision) { bUseFirearmCollision = EnableFirearmCollision; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	ECollisionChannel GetFirearmCollisionChannel() const { return FirearmCollisionChannel; }

	/**
	 * Sets whether or not the system will use Left Hand IK at all
	 * @param bUse If true it will enable left hand ik, if false it will disable it
	*/
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Procedural")
	void SetUseLeftHandIK(bool bUse);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Procedural")
	bool GetUseLeftHandIK() const { return bUseLeftHandIK; }
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Procedural")
	EAxis::Type GetRightHandAxis() const { return RightHandAxis; }

	float GetDefaultFOV()const { return DefaultCameraFOV; }
	// Play camera shake to the clients camera
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | HeldActor")
	void PlayCameraShake(TSubclassOf<UCameraShakeBase> CameraShake, float Scale = 1.0f) const;
	// Gets the currently held firearm if it exists, returns nullptr if it doesnt
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | HeldActor")
	AActor* GetHeldActor() const { return HeldActor; }
	/**
	 * Will set the passed in firearm as the current firearm/aiming actor.
	 * Will auto attach the passed in firearm to a socket on the characters In Use Mesh at the given Grip Socket
 	*/
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Firearm")
	void EquipHeldActor(AActor* INHeldActor);
	// When aiming it takes the current optics magnification sensitivity and returns a multiplier to be used in Turn/Look up inputs
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Firearm")
	float GetMagnificationSensitivity() const;
	/**
	 * When aiming it takes the current optics magnification sensitivity and returns a multiplier to be used in Turn/Look up inputs.
	 * This only takes affect after the optics magnification exceeds StartAtMagnification
	*/
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Firearm")
	float GetMagnificationSensitivityStartValue(float StartAtMagnification = 4.0f) const;
	// Will attach an actor to your in use mesh at a given socket name
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Firearm")
	void AttachItem(AActor* Actor, const FName SocketName);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Firearm")
	void ClearCurrentHeldActor();

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | FirearmPose")
	void SetFirearmPose(ESKGFirearmPose Pose, bool bSkipNone, bool bGoToNoneIfEqual);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | FirearmPose")
	void StopFirearmPose();
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FirearmPose")
	ESKGFirearmPose GetFirearmPose() const { return FirearmPose; }

	float LookUpDownOffset;
	// Whether or not to freelook
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | FreeLook")
	void SetFreeLook(bool FreeLook);
	// Returns true if free looking
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FreeLook")
	bool GetFreeLook() const { return bFreeLook; }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | FreeLook", meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool ValidTurn(float AxisValue);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | FreeLook", meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool ValidLookUp(float AxisValue);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | FreeLook")
	void SetControlRotation(FRotator NewControlRotation);

	UPROPERTY(BlueprintAssignable, Category = "SKGFPSFramework | FreeLook")
	FOnFreeLookEnd OnFreeLookEnd;
	UPROPERTY(BlueprintAssignable, Category = "SKGFPSFramework | AimingActor")
	FOnUnEquipFinished OnUnEquipFinished;
	UPROPERTY(BlueprintAssignable, Category = "SKGFPSFramework | AimingActor")
	FOnAimingActorChanged OnAimingActorChanged;
	UPROPERTY(BlueprintAssignable, Category = "SKGFPSFramework | AimingActor")
	FOnPoseFinished OnFirearmPoseFinished;

	ASKGSight* GetCurrentSight() const;
};
