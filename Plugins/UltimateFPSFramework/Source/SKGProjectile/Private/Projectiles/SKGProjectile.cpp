//Copyright 2021, Dakota Dawe, All rights reserved

#include "Projectiles/SKGProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/WindDirectionalSource.h"
#include "Components/WindDirectionalSourceComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASKGProjectile::ASKGProjectile()
{
	//create our collision component (a sphere) along with its default values
 	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(15.0f);
	RootComponent = CollisionComponent;
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Add a mesh to the projectile to allow for an effect such as a tracer being a simple shaped mesh
	//with a bright emissive material to give the tracer effect
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	Mesh->SetupAttachment(CollisionComponent);

	//This sets our drag curve. This curve controls how our bullets drag gets affected.
	DragCurve = CreateDefaultSubobject<UCurveFloat>("DragCurveFloat");

	//Create and setup our projectile movement component. Because we are using a hybrid system (line
	//traces for hit detection) we disable some things such as bouncing.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	//Disable auto activation in order to set custom velocity specified by the user in blueprint.
	ProjectileMovementComponent->bAutoActivate = false;

	//After 10 seconds the bullet is destroyed
	InitialLifeSpan = 10.0f;

	//Set tick to true as the tick function controls our simulation for drag/wind ect.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//VelocityFPS is our velocity in feet per second
	VelocityFPS = 3200.0f;
	//Set our inital velocity to 0
	ProjectileMovementComponent->InitialSpeed = 0.0f;
	ProjectileMovementComponent->MaxSpeed = VelocityFPS * 100.f;

	//To visually see your hits for debug purposes we have a debug box that we draw when the bullet
	//impacts with an object.
	DebugBoxSize = 0.02f;
	DrawDebugBoxOnImpact = false;

	//Dictates whether or not the bullet is affected by wind.
	AffectedByWind = true;

	//Bullet weight to allow for doing calculations with bullet weight and velocity for damage
	//in your own system.
	BulletWeightGrains = 55;

	bReplicates = false;

	CollisionChannel = ECollisionChannel::ECC_Visibility;
}

// Called when the game starts or when spawned
void ASKGProjectile::BeginPlay()
{
	Super::BeginPlay();

	//Setup our velocity to be in cm/s and set our last location to the spawn location
	VelocityFPS *= 30.48f;
	//Calculate and set the velocity based on the velocity set by the user, then activate the projectile movement.
	/*ProjectileMovementComponent->Velocity = GetActorForwardVector() * VelocityFPS;
	ProjectileMovementComponent->Activate();*/

	LastPosition = GetActorLocation();

	//find and set our bullet to utilize a wind source in the world to be effected by wind
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWindDirectionalSource::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		if (AWindDirectionalSource* Wind = Cast<AWindDirectionalSource>(Actor))
		{
			WindSource = Wind;
			if (const UWindDirectionalSourceComponent* WindComponent = WindSource->GetComponent())
			{
				float Weight = 0.1f;
				WindComponent->GetWindParameters(GetActorLocation(), WindData, Weight);
				break;
			}
		}
	}
}

float ASKGProjectile::CalculateDrag() const
{
	if (DragCurve)
	{	//get the information from our graph based on our projectiles velocity
		const float ProjectileVelocity = ProjectileMovementComponent->Velocity.Size();
		const float DragGraphValue = DragCurve->GetFloatValue(GetGameTimeSinceCreation());

		float Drag = -0.5f * DragGraphValue * 1.225f * 0.0000571f * (ProjectileVelocity * 0.01f) * (ProjectileVelocity * 0.01f);
		Drag /= 3.56394f;
		Drag = Drag * GetWorld()->DeltaTimeSeconds * -100.0f / ProjectileVelocity;
		Drag = 1.0f - Drag;
		//return our calculated drag
		return Drag;
	}
	return 1.0f;
}

// Called every frame
void ASKGProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult HitResult;
	FCollisionQueryParams Params;//Setup settings for line trace collision.
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	Params.bReturnPhysicalMaterial = true;

	//Perform the line trace starting at the projectiles last tick position and ending at its current position.
	if (GetWorld()->LineTraceSingleByChannel(OUT HitResult, LastPosition, GetActorLocation(), CollisionChannel, Params, FCollisionResponseParams::DefaultResponseParam))
	{	//Handle bullet hits, setup for C++ code as well as exposed to blueprint in the form of an event
		//called OnBulletHit which passes the HitResults as a parameter.
		OnProjectileImpact(HitResult);
		Destroy();
	}
	LastPosition = GetActorLocation();
	ProjectileMovementComponent->Velocity *= CalculateDrag();
	
	if (AffectedByWind)//If the bullet is allowed to be affected by wind, apply the corresponding force.
		ProjectileMovementComponent->AddForce(WindData.Direction * (WindData.Speed / 4.2f));
}

float ASKGProjectile::CalculateImpactForce() const
{
	return UKismetMathLibrary::NormalizeToRange(FMath::Sqrt(GetVelocity().Size() * BulletWeightGrains), 0.0f, 5000.0f);
}

void ASKGProjectile::ActivateProjectile(float VelocityMultiplier)
{
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * (VelocityFPS * VelocityMultiplier);
	ProjectileMovementComponent->Activate();
}
