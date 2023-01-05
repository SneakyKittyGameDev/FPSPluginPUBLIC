// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "SKGGrenade.generated.h"

class UCapsuleComponent;

UCLASS()
class SKGGRENADE_API ASKGGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGGrenade();

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "SKGFPSFramework")
	UCapsuleComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	int32 MaxBounces;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	float FuseTime;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	bool bExplodeOnImpact;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	float ArmingTime;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	float DudDestroyTime;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	float ServerSyncIntervalPerSecond;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	TEnumAsByte<ECollisionChannel> PoseCollision;
	
	FTimerHandle TFuse;
	FTimerHandle TArmTime;
	FTimerHandle TSync;
	uint8 CurrentBounces;

	bool bIsArmed;
	bool bIsVisibleGrenade;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SuperExplode();
	
	UFUNCTION(BlueprintNativeEvent, Category = "SKGFPSFramework | Grenade")
	void Explode();
	virtual void Explode_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "SKGFPSFramework | Grenade")
	void Impact(float Velocity);
	virtual void Impact_Implementation(float Velocity);

	void EnablePhysics();
	void DisablePhysics();
	
	void ArmGrenade();

	UFUNCTION(NetMulticast, Unreliable)
	void Multi_UpdateLocation(FVector NewLocation, FRotator NewRotation);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_ReleaseGrenade(FVector Orientation, float Velocity);

	TWeakObjectPtr<ASKGGrenade> ClientGrenade;
	bool bClientGrenadeInterp;
	bool bSyncGrenadeInterp;
	
	void SyncLocation();

	FVector InterpToLocation;
	FRotator InterpToRotation;
	
public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Grenade")
	void ReleaseGrenade(FVector Orientation, float Velocity, bool IsClientGrenade = false);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Grenade")
	void CookGrenade();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Grenade")
	void InterpToNewLocation();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Grenade")
	void SetClientGrenade(ASKGGrenade* Grenade);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SmokeGrenade")
	FVector GetSmokeGrenadeParticleLocation(float SpriteRadius);

	void StartFuse(float Time);
};