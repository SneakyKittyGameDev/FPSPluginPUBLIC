// Copyright 2021, Dakota Dawe, All rights reserved


#include "Actors/SKGGrenade.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

// Sets default values
ASKGGrenade::ASKGGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SphereComponent"));
	//CollisionComponent->InitSphereRadius(1.6f);
	CollisionComponent->InitCapsuleSize(4.8f, 6.8f);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetLinearDamping(0.35f);
	CollisionComponent->SetAngularDamping(1.0f);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	RootComponent = CollisionComponent;

	PoseCollision = ECC_GameTraceChannel2;

	DudDestroyTime = 5.0f;
	ServerSyncIntervalPerSecond = 10.0f;
	
	MaxBounces = 2;
	CurrentBounces = 0;
	FuseTime = 4.5f;

	bReplicates = true;

	bExplodeOnImpact = false;
	bIsArmed = false;

	ClientGrenade = nullptr;
	bClientGrenadeInterp = true;
	bSyncGrenadeInterp = false;

	bIsVisibleGrenade = true;
}

// Called when the game starts or when spawned
void ASKGGrenade::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		//SetActorHiddenInGame(true);
		SetActorTickEnabled(false);
		SetActorTickInterval(1 / 60);
	}
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ASKGGrenade::OnComponentHit);
}


void ASKGGrenade::PostInitProperties()
{
	Super::PostInitProperties();
	CollisionComponent->SetCollisionResponseToChannel(PoseCollision, ECR_Ignore);
}

void ASKGGrenade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	InterpToNewLocation();
}

void ASKGGrenade::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!HasAuthority() && IsValid(this))
	{
		SuperExplode();
	}
	Super::EndPlay(EndPlayReason);
}

void ASKGGrenade::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	++CurrentBounces;
	if (bExplodeOnImpact && CurrentBounces >= MaxBounces)
	{
		CollisionComponent->SetGenerateOverlapEvents(false);
		CollisionComponent->OnComponentHit.RemoveAll(this);
		SetLifeSpan(DudDestroyTime);
	}
	
	if (bIsArmed)
	{
		SuperExplode();
		CollisionComponent->SetGenerateOverlapEvents(false);
		CollisionComponent->OnComponentHit.RemoveAll(this);
	}
	else
	{
		Impact(GetVelocity().Size());
	}
	
	if (GetWorldTimerManager().IsTimerActive(TArmTime))
	{
		GetWorldTimerManager().ClearTimer(TArmTime);
	}
}

void ASKGGrenade::SuperExplode()
{
	if (!bIsVisibleGrenade)
	{
		return;
	}

	Explode();
}

void ASKGGrenade::Explode_Implementation()
{
}

void ASKGGrenade::Impact_Implementation(float Velocity)
{
}

void ASKGGrenade::EnablePhysics()
{
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASKGGrenade::DisablePhysics()
{
	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASKGGrenade::ArmGrenade()
{
	bIsArmed = true;
	UE_LOG(LogTemp, Warning, TEXT("Armed"));
}

void ASKGGrenade::Multi_UpdateLocation_Implementation(FVector NewLocation, FRotator NewRotation)
{
	if (!HasAuthority())
	{
		bSyncGrenadeInterp = true;
		InterpToLocation = NewLocation;
		InterpToRotation = NewRotation;
		SetActorTickEnabled(true);
	}
}

void ASKGGrenade::Multi_ReleaseGrenade_Implementation(FVector Orientation, float Velocity)
{
	if (!HasAuthority() && GetVelocity().Size() < 10.0f)
	{
		ReleaseGrenade(Orientation, Velocity);
	}
}

void ASKGGrenade::SyncLocation()
{
	Multi_UpdateLocation(GetActorLocation(), GetActorRotation());
	if (GetVelocity().Equals(FVector::ZeroVector, 1.0f))
	{
		GetWorldTimerManager().ClearTimer(TSync);
	}
}

void ASKGGrenade::ReleaseGrenade(FVector Orientation, float Velocity, bool IsClientGrenade)
{
	EnablePhysics();
	CollisionComponent->SetPhysicsLinearVelocity(Orientation * Velocity);

	if (ServerSyncIntervalPerSecond > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TSync, this, &ASKGGrenade::SyncLocation, 1.0f / ServerSyncIntervalPerSecond, true);
	}
	else
	{
		SetActorTickEnabled(false);
	}
	Multi_ReleaseGrenade(Orientation, Velocity);
	
	if (bExplodeOnImpact)
	{
		if (ArmingTime > 0.0f)
		{
			GetWorldTimerManager().SetTimer(TArmTime, this, &ASKGGrenade::ArmGrenade, ArmingTime, false);
		}
		else
		{
			ArmGrenade();
		}
	}
	else if (!GetWorldTimerManager().IsTimerActive(TFuse) && !IsClientGrenade)
	{
		StartFuse(FuseTime);
	}
}

void ASKGGrenade::CookGrenade()
{
	if (!bExplodeOnImpact && !GetWorldTimerManager().IsTimerActive(TFuse))
	{
		StartFuse(FuseTime);
	}
}

void ASKGGrenade::InterpToNewLocation()
{
	if (bClientGrenadeInterp)
	{
		if (ClientGrenade.IsValid())
		{
			ClientGrenade->SetActorLocation(UKismetMathLibrary::VInterpTo(ClientGrenade->GetActorLocation(), InterpToLocation, GetWorld()->GetDeltaSeconds(), 1.0f));
			ClientGrenade->SetActorRotation(UKismetMathLibrary::RInterpTo(ClientGrenade->GetActorRotation(), InterpToRotation, GetWorld()->GetDeltaSeconds(), 1.0f));
		}
	}
}

void ASKGGrenade::SetClientGrenade(ASKGGrenade* Grenade)
{
	if (IsValid(Grenade))
	{
		Destroy();
		ClientGrenade = Grenade;
		ClientGrenade->SetActorHiddenInGame(false);
	
		FuseTime += (CreationTime - ClientGrenade->CreationTime) / 2.0f;
		ClientGrenade->StartFuse(FuseTime);
		
		SetActorHiddenInGame(true);
		bIsVisibleGrenade = false;
	}
}

FVector ASKGGrenade::GetSmokeGrenadeParticleLocation(float SpriteRadius)
{
	if (SpriteRadius <= 0.0f)
	{
		SpriteRadius = 1.0f;
	}
	FVector Location = GetActorLocation();
	Location.Z += SpriteRadius / 2.0f;
	return Location;
}

void ASKGGrenade::StartFuse(float Time)
{
	GetWorldTimerManager().SetTimer(TFuse, this, &ASKGGrenade::SuperExplode, Time, false);
}
