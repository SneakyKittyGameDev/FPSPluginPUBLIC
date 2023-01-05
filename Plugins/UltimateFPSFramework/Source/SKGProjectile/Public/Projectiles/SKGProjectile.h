//Copyright 2021, Dakota Dawe, All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneManagement.h"
#include "SKGProjectile.generated.h"

class AWindDirectionalSource;
class UProjectileMovementComponent;
class UStaticMeshComponent;
class USphereComponent;
class UCurveFloat;

UCLASS()
class SKGPROJECTILE_API ASKGProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGProjectile();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SKGFPSFramework")
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	USphereComponent* CollisionComponent;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework")
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere, Category = "SKGFPSFramework | Debug")
	bool DrawDebugBoxOnImpact;
	UPROPERTY(EditAnywhere, Category = "SKGFPSFramework | Debug")
	float DebugBoxSize;

	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Physics")
	UCurveFloat* DragCurve;//The drag curve used for this projectile (air resistance)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Physics")
	bool AffectedByWind;//Whether or not this bullet is affected by wind
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Physics")
	AWindDirectionalSource* WindSource;
	FWindData WindData;//store our wind data to allow for wind to push projectile on tick

	FVector LastPosition;

	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	float VelocityFPS;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	uint16 BulletWeightGrains;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float CalculateDrag() const;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGFPSFramework | Events")
	void OnProjectileImpact(const FHitResult& HitResult);

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Default")
	UProjectileMovementComponent* GetProjectileMovement() const {return ProjectileMovementComponent;}
	//Get the wind source that is affecting our projectile
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Default")
	AWindDirectionalSource* GetWindSource() const {return WindSource;}

	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Ammo")
	float CalculateImpactForce() const;
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Ammo")
	float GetProjectilePower() const { return FMath::Sqrt(VelocityFPS * BulletWeightGrains); }
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Ammo")
	void ActivateProjectile(float VelocityMultiplier);
};
