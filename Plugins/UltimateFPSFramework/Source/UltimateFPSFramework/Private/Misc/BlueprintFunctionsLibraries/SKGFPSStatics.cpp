// Copyright 2021, Dakota Dawe, All rights reserved


#include "Misc/BlueprintFunctionsLibraries/SKGFPSStatics.h"
#include "Misc/SKGPhysicalMaterial.h"
#include "Actors/SKGFirearm.h"
#include "Actors/FirearmParts/SKGPart.h"
#include "Actors/FirearmParts/SKGSight.h"
#include "Actors/FirearmParts/SKGLightLaser.h"
#include "Actors/FirearmParts/SKGMagnifier.h"
#include "Actors/FirearmParts/SKGBarrel.h"
#include "Actors/FirearmParts/SKGHandguard.h"
#include "Actors/FirearmParts/SKGForwardGrip.h"
#include "Actors/FirearmParts/SKGMuzzle.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystem.h"
#include "JsonObjectConverter.h"
#include "Components/DecalComponent.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/FileHelper.h"
#include "Sound/SoundBase.h"
#include "Misc/Paths.h"

constexpr float SpeedOfSoundMs = 343.0f;

const float MOA_TO_MRAD_MULTIPLIER = (5.0 * PI) / 54.0;
const float MRAD_TO_MOA_MULTIPLIER = 54.0 / (5.0 * PI);

void USKGFPSStatics::SpawnImpactEffect(const FHitResult& HitResult, const FSKGImpactEffects& ImpactEffect)
{
	SpawnEffects(HitResult, ImpactEffect);
}

void USKGFPSStatics::SpawnImpactEffectSoundDelayed(const FHitResult& HitResult, const FSKGImpactEffects& ImpactEffect)
{
	SpawnEffects(HitResult, ImpactEffect, true);
}

void USKGFPSStatics::SpawnEffects(const FHitResult& HitResult, const FSKGImpactEffects& ImpactEffects, bool bDelayed)
{
	if (const AActor* WorldActor = HitResult.GetActor())
	{
		if (const UWorld* World = WorldActor->GetWorld())
		{
			const FSKGSoundSettings SoundSettings = ImpactEffects.SoundSettings;
			const float Delay = GetSpeedOfSoundDelayActor(HitResult.GetActor(), UGameplayStatics::GetPlayerCharacter(World, 0));

			if (!bDelayed || Delay < 0.15f)
			{
				if (USoundBase* ImpactSound = ImpactEffects.SoundSettings.Sound)
				{
					UGameplayStatics::SpawnSoundAtLocation(World, ImpactSound, HitResult.Location, FRotator::ZeroRotator, SoundSettings.VolumeMultiplier, SoundSettings.PitchMultiplier, 0.0f, ImpactSound->AttenuationSettings);
				}
			}
			else
			{
				PlaySoundDelayed(WorldActor, HitResult.Location, SoundSettings);
			}
			
			const FSKGEffectImpactSettings DecalImpact = ImpactEffects.EffectSettings;
			FRotator Rotation = FRotator::ZeroRotator;
			if (!DecalImpact.bUseParticlesRotation)
			{
				Rotation = UKismetMathLibrary::MakeRotFromX(HitResult.Normal);
			}
			
			if (UFXSystemAsset* ImpactEffect = ImpactEffects.EffectSettings.ImpactEffect)
			{
				if (UNiagaraSystem* NEffect = Cast<UNiagaraSystem>(ImpactEffect))
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, NEffect, HitResult.Location, Rotation);
				}
				else if (UParticleSystem* PEffect = Cast<UParticleSystem>(ImpactEffect))
				{
					UGameplayStatics::SpawnEmitterAtLocation(World, PEffect, HitResult.Location, Rotation);
				}
			}
			
			if (UMaterialInstance* ImpactDecal = ImpactEffects.EffectSettings.ImpactDecal)
			{
				if (USceneComponent* HitComponent = HitResult.GetComponent())
				{
					Rotation.Pitch += 180.0f;
					if (UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAttached(ImpactDecal, DecalImpact.DecalSize, HitComponent, NAME_None,
						HitResult.Location, Rotation, EAttachLocation::KeepWorldPosition, DecalImpact.DecalLifeTime))
					{
						DecalComponent->SetFadeScreenSize(ImpactEffects.EffectSettings.DecalFadeScreenSize);
					}
				}
			}
		}
	}
}

void USKGFPSStatics::PlaySoundDelayed(const AActor* WorldActor, const FVector& Location, FSKGSoundSettings SoundSettings)
{
	if (WorldActor && WorldActor->GetWorld())
	{
		if (const ACharacter* LocalCharacter = UGameplayStatics::GetPlayerCharacter(WorldActor->GetWorld(), 0))
		{
			const float Delay = GetSpeedOfSoundDelayLocation(Location, LocalCharacter->GetActorLocation());
			if (Delay < 0.15f)
			{
				if (USoundBase* ImpactSound = SoundSettings.Sound)
				{
					UGameplayStatics::SpawnSoundAtLocation(WorldActor->GetWorld(), ImpactSound, Location, FRotator::ZeroRotator, SoundSettings.VolumeMultiplier, SoundSettings.PitchMultiplier, 0.0f, ImpactSound->AttenuationSettings);
				}
			}
			else
			{
				FTimerHandle FSoundDelay;
				const FTimerDelegate SoundDelayDelegate = FTimerDelegate::CreateStatic(&USKGFPSStatics::PlaySoundDelayed_Implementation, WorldActor, Location, SoundSettings);
				WorldActor->GetWorld()->GetTimerManager().SetTimer(FSoundDelay, SoundDelayDelegate, Delay, false);
			}
		}
	}
}

void USKGFPSStatics::PlaySoundDelayed_Implementation(const AActor* WorldActor, const FVector Location,
	FSKGSoundSettings SoundSettings)
{
	if (WorldActor && WorldActor->GetWorld())
	{
		if (USoundBase* ImpactSound = SoundSettings.Sound)
		{
			UGameplayStatics::SpawnSoundAtLocation(WorldActor->GetWorld(), ImpactSound, Location, FRotator::ZeroRotator, SoundSettings.VolumeMultiplier, SoundSettings.PitchMultiplier, 0.0f, ImpactSound->AttenuationSettings);
		}
	}
}

float USKGFPSStatics::GetSpeedOfSoundDelayLocation(const FVector& LocationA, const FVector& LocationB)
{
	const float Distance = FVector::Distance(LocationA, LocationB) / 100.0f;
	return Distance / SpeedOfSoundMs;
}

float USKGFPSStatics::GetSpeedOfSoundDelayActor(const AActor* ActorA, const AActor* ActorB)
{
	if (ActorA && ActorB)
	{
		return GetSpeedOfSoundDelayLocation(ActorA->GetActorLocation(), ActorB->GetActorLocation());
	}
	return 0.1f;
}

FRotator USKGFPSStatics::SetMuzzleMOA(FRotator MuzzleRotation, float MOA)
{
	MOA *= 0.016668f; // Sets MOA to be in exact adjustments. 1 inch at 100 yards = 0.016668
	float MOAChange = (FMath::RandRange(-MOA / 2.0f, MOA / 2.0f));
	MuzzleRotation.Yaw += MOAChange;
	
	MOAChange = (FMath::RandRange(-MOA / 2.0f, MOA / 2.0f));
	MuzzleRotation.Pitch += MOAChange * 0.6f; // reduce vertical MOA shift for consistency

	return MuzzleRotation;
}

FRotator USKGFPSStatics::GetEstimatedMuzzleToScopeZero(const FTransform& MuzzleTransform,
	const FTransform& SightTransform, const float RangeMeters)
{
	const FVector EndLocation = SightTransform.GetLocation() + SightTransform.GetRotation().Vector() * RangeMeters;
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleTransform.GetLocation(), EndLocation);
	LookAtRotation.Pitch += 0.04f; // Increase vertical projectile launch angle (thanks gravity)
	return LookAtRotation;
}

FTransform USKGFPSStatics::FixTransform(const FTransform& Transform)
{
	FVector Vec = Transform.GetLocation();
	const float TempVal = Vec.Z;
	Vec.Z = Vec.X;
	Vec.X = TempVal;

	return FTransform(Transform.Rotator(), Vec, Transform.GetScale3D());
}

bool USKGFPSStatics::Ragdoll(UPrimitiveComponent* MeshComponent)
{
	if (Cast<USkeletalMeshComponent>(MeshComponent))
	{
		if (ACharacter* Character = Cast<ACharacter>(MeshComponent->GetOwner()))
		{
			Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComponent->SetSimulatePhysics(true);
		return true;
	}
	return false;
}

void USKGFPSStatics::RagdollWithImpact(UPrimitiveComponent* MeshComponent, FVector ImpactLocation, float ImpactForce)
{
	if (Ragdoll(MeshComponent) && ImpactLocation != FVector::ZeroVector && ImpactForce != 0.0f)
	{
		ImpactForce *= MeshComponent->GetMass();
		MeshComponent->AddRadialImpulse(ImpactLocation, 100.0f, ImpactForce, ERadialImpulseFalloff::RIF_Linear);
	}
}

FSKGProjectileTransform USKGFPSStatics::CreateProjectileTransform(const FTransform& MuzzleTransform)
{
	return FSKGProjectileTransform(MuzzleTransform);
}

FTransform USKGFPSStatics::CreateTransformForProjectile(const FSKGProjectileTransform& ProjectileTransform)
{
	return FSKGProjectileTransform::GetTransformFromProjectile(ProjectileTransform);
}

bool USKGFPSStatics::SaveStringToFile(const FString& Path, const FString& FileName, const FString& FileContent)
{
	return FFileHelper::SaveStringToFile(FileContent, *(Path + FileName));
}

bool USKGFPSStatics::LoadFileToString(const FString& Path, const FString& FileName, FString& OutString)
{
	return FFileHelper::LoadFileToString(OutString, *(Path + FileName));
}

bool USKGFPSStatics::DeleteFile(const FString& Path, const FString& FileName)
{
	IPlatformFile& PlatformFile = IPlatformFile::GetPlatformPhysical();
	return PlatformFile.DeleteFile(*(Path + FileName));
}

bool USKGFPSStatics::GetAllFiles(FString Path, TArray<FString>& OutFiles)
{
	bool ValidFiles = false;
	if (FPaths::DirectoryExists(Path))
	{
		ValidFiles = true;
		Path.Append("*");
		FFileManagerGeneric::Get().FindFiles(OutFiles, *Path, true, false);
	}

	return ValidFiles;
}

FString USKGFPSStatics::RoundFloat2Pos(float Value)
{
	return FString::Printf(TEXT("%.2f"), Value);
}

FString USKGFPSStatics::RoundFloat3Pos(float Value)
{
return FString::Printf(TEXT("%.3f"), Value);
}

FString USKGFPSStatics::GetFormattedCopyPasteTransform(const FTransform& Transform)
{
	const FQuat Rot = Transform.GetRotation();
	const FVector Loc = Transform.GetLocation();
	FString RotStr = FString::Printf(TEXT("(Rotation=(X=%f,Y=%f,Z=%f,W=%f),Translation=(X=%f,Y=%f,Z=%f),Scale3D=(X=1.000000,Y=1.000000,Z=1.000000))"), Rot.X, Rot.Y, Rot.Z, Rot.W, Loc.X, Loc.Y, Loc.Z);

	return RotStr;
}

int32 USKGFPSStatics::GetRandomIndexForArray(uint8 ArrayLength)
{
	if (ArrayLength)
	{
		if (ArrayLength == 1)
		{
			return 0;
		}
		return FMath::RandRange(0, ArrayLength - 1);
	}
	return INDEX_NONE;
}

bool USKGFPSStatics::ChangeTransformProperty(UObject* Object, const FTransform& NewTransform, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FProperty* Property = Class->FindPropertyByName(NameOfProperty))
				{
					if (FTransform* Transform = Property->ContainerPtrToValuePtr<FTransform>(ClassDefault))
					{
						Transform->SetLocation(NewTransform.GetLocation());
						Transform->SetRotation(NewTransform.GetRotation());
						Transform->SetScale3D(NewTransform.GetScale3D());
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool USKGFPSStatics::ChangeVectorProperty(UObject* Object, const FVector& NewVector, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FProperty* Property = Class->FindPropertyByName(NameOfProperty))
				{
					if (FVector* Vector = Property->ContainerPtrToValuePtr<FVector>(ClassDefault))
					{
						Vector->X = NewVector.X;
						Vector->Y = NewVector.Y;
						Vector->Z = NewVector.Z;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool USKGFPSStatics::ChangeRotatorProperty(UObject* Object, const FRotator& NewRotator, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FProperty* Property = Class->FindPropertyByName(NameOfProperty))
				{
					if (FRotator* Rotator = Property->ContainerPtrToValuePtr<FRotator>(ClassDefault))
					{
						Rotator->Roll = NewRotator.Roll;
						Rotator->Yaw = NewRotator.Yaw;
						Rotator->Pitch = NewRotator.Pitch;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool USKGFPSStatics::ChangeFloatProperty(UObject* Object, const float NewFloat, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FFloatProperty* Property = CastField<FFloatProperty>(Class->FindPropertyByName(NameOfProperty)))
				{
					Property->SetPropertyValue_InContainer(ClassDefault, NewFloat);
					return true;
				}
			}
		}
	}

	return false;
}

bool USKGFPSStatics::ChangeIntProperty(UObject* Object, const int32 NewInt, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FIntProperty* Property = CastField<FIntProperty>(Class->FindPropertyByName(NameOfProperty)))
				{
					Property->SetPropertyValue_InContainer(ClassDefault, NewInt);
					return true;
				}
			}
		}
	}

	return false;
}

bool USKGFPSStatics::ChangeInt64Property(UObject* Object, const int64 NewInt, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FInt64Property* Property = CastField<FInt64Property>(Class->FindPropertyByName(NameOfProperty)))
				{
					Property->SetPropertyValue_InContainer(ClassDefault, NewInt);
					return true;
				}
			}
		}
	}

	return false;
}

bool USKGFPSStatics::ChangeByteProperty(UObject* Object, const uint8 NewByte, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FByteProperty* Property = CastField<FByteProperty>(Class->FindPropertyByName(NameOfProperty)))
				{
					Property->SetPropertyValue_InContainer(ClassDefault, NewByte);
					return true;
				}
			}
		}
	}

	return false;
}

bool USKGFPSStatics::ChangeBoolProperty(UObject* Object, const bool NewBool, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FBoolProperty* Property = CastField<FBoolProperty>(Class->FindPropertyByName(NameOfProperty)))
				{
					Property->SetPropertyValue_InContainer(ClassDefault, NewBool);
					return true;
				}
			}
		}
	}

	return false;
}

bool USKGFPSStatics::ChangeStringProperty(UObject* Object, const FString& NewString, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FStrProperty* Property = CastField<FStrProperty>(Class->FindPropertyByName(NameOfProperty)))
				{
					Property->SetPropertyValue_InContainer(ClassDefault, NewString);
					return true;
				}
			}
		}
	}

	return false;
}

bool USKGFPSStatics::ChangeNameProperty(UObject* Object, const FName& NewName, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FNameProperty* Property = CastField<FNameProperty>(Class->FindPropertyByName(NameOfProperty)))
				{
					Property->SetPropertyValue_InContainer(ClassDefault, NewName);
					return true;
				}
			}
		}
	}

	return false;
}

bool USKGFPSStatics::ChangeTextProperty(UObject* Object, const FText& NewText, const FName NameOfProperty)
{
	if (Object)
	{
		if (const UClass* Class = Object->GetClass())
		{
			if (UObject* ClassDefault = Class->GetDefaultObject())
			{
				if (const FTextProperty* Property = CastField<FTextProperty>(Class->FindPropertyByName(NameOfProperty)))
				{
					Property->SetPropertyValue_InContainer(ClassDefault, NewText);
					return true;
				}
			}
		}
	}

	return false;
}

FTransform USKGFPSStatics::ProjectileTransformToTransform(const FSKGProjectileTransform& ProjectileTransform)
{
	return ProjectileTransform.GetTransformFromProjectile();
}

FString USKGFPSStatics::GetTextAfterPeriod(const FString& INString)
{
	int32 LastPeriod;
	INString.FindLastChar('.', LastPeriod);
	if (LastPeriod == INDEX_NONE)
	{
		return INString;
	}

	return INString.RightChop(LastPeriod + 1);
}

TArray<ASKGLightLaser*> USKGFPSStatics::GetLightsLasers(ASKGFirearm* Firearm)
{
	TArray<ASKGLightLaser*> LightsLasers;
	if (IsValid(Firearm))
	{
		TArray<USKGAttachmentComponent*> PartComponents = Firearm->GetCachedComponents().LightLasers;
		LightsLasers.Reserve(PartComponents.Num());
		
		for (const USKGAttachmentComponent* PartComponent : PartComponents)
		{
			if (IsValid(PartComponent) && IsValid(PartComponent->GetAttachment<ASKGLightLaser>()))
			{
				LightsLasers.AddUnique(PartComponent->GetAttachment<ASKGLightLaser>());
			}
		}
	}

	return LightsLasers;
}

TArray<ASKGSight*> USKGFPSStatics::GetSights(ASKGFirearm* Firearm)
{
	TArray<ASKGSight*> Sights;
	if (IsValid(Firearm))
	{
		TArray<USKGAttachmentComponent*> PartComponents = Firearm->GetCachedComponents().LightLasers;
		Sights.Reserve(PartComponents.Num());
		
		for (const USKGAttachmentComponent* PartComponent : PartComponents)
		{
			if (IsValid(PartComponent) && IsValid(PartComponent->GetAttachment<ASKGSight>()))
			{
				Sights.AddUnique(PartComponent->GetAttachment<ASKGSight>());
			}
		}
	}

	return Sights;
}

TArray<ASKGMagnifier*> USKGFPSStatics::GetMagnifiers(ASKGFirearm* Firearm)
{
	TArray<ASKGMagnifier*> Magnifiers;
	if (IsValid(Firearm))
	{
		TArray<USKGAttachmentComponent*> PartComponents = Firearm->GetCachedComponents().Magnifiers;
		Magnifiers.Reserve(PartComponents.Num());
		
		for (const USKGAttachmentComponent* PartComponent : PartComponents)
		{
			if (IsValid(PartComponent) && IsValid(PartComponent->GetAttachment<ASKGMagnifier>()))
			{
				Magnifiers.AddUnique(PartComponent->GetAttachment<ASKGMagnifier>());
			}
		}
	}

	return Magnifiers;
}

TArray<ASKGPart*> USKGFPSStatics::GetRenderTargets(ASKGFirearm* Firearm)
{
	TArray<ASKGPart*> RenderTargets;
	if (IsValid(Firearm))
	{
		TArray<USKGAttachmentComponent*> PartComponents = Firearm->GetCachedComponents().RenderTargets;
		RenderTargets.Reserve(PartComponents.Num());
		
		for (const USKGAttachmentComponent* PartComponent : PartComponents)
		{
			if (IsValid(PartComponent) && IsValid(PartComponent->GetAttachment<ASKGPart>()))
			{
				RenderTargets.AddUnique(PartComponent->GetAttachment<ASKGPart>());
			}
		}
	}

	return RenderTargets;
}

ASKGBarrel* USKGFPSStatics::GetBarrel(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		const USKGAttachmentComponent* PartComponent = Firearm->GetCachedComponents().Barrel;
		if (IsValid(PartComponent))
		{
			return PartComponent->GetAttachment<ASKGBarrel>();
		}
	}
	return nullptr;
}

ASKGHandguard* USKGFPSStatics::GetHandguard(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		const USKGAttachmentComponent* PartComponent = Firearm->GetCachedComponents().Handguard;
		if (IsValid(PartComponent))
		{
			return PartComponent->GetAttachment<ASKGHandguard>();
		}
	}
	return nullptr;
}

ASKGPart* USKGFPSStatics::GetStock(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		const USKGAttachmentComponent* PartComponent = Firearm->GetCachedComponents().Stock;
		if (IsValid(PartComponent))
		{
			return PartComponent->GetAttachment<ASKGPart>();
		}
	}
	return nullptr;
}

ASKGForwardGrip* USKGFPSStatics::GetForwardGrip(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		const USKGAttachmentComponent* PartComponent = Firearm->GetCachedComponents().ForwardGrip;
		if (IsValid(PartComponent))
		{
			return PartComponent->GetAttachment<ASKGForwardGrip>();
		}
	}
	return nullptr;
}

ASKGMuzzle* USKGFPSStatics::GetMuzzle(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		const USKGAttachmentComponent* PartComponent = Firearm->GetCachedComponents().Muzzle;
		if (IsValid(PartComponent))
		{
			return PartComponent->GetAttachment<ASKGMuzzle>();
		}
	}
	return nullptr;
}

FTransform USKGFPSStatics::GetFixedBasePoseOffset(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		return FixTransform(ISKGProceduralAnimationInterface::Execute_GetBasePoseOffset(Firearm));
	}
	return FTransform();
}

FTransform USKGFPSStatics::GetFixedShortStockPose(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		return FixTransform(ISKGProceduralAnimationInterface::Execute_GetShortStockPose(Firearm));
	}
	return FTransform();
}

FTransform USKGFPSStatics::GetFixedOppositeShoulderPose(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		return FixTransform(ISKGProceduralAnimationInterface::Execute_GetOppositeShoulderPose(Firearm));
	}
	return FTransform();
}

FTransform USKGFPSStatics::GetFixedBlindFireLeftPose(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		return FixTransform(ISKGProceduralAnimationInterface::Execute_GetBlindFireLeftPose(Firearm));
	}
	return FTransform();
}

FTransform USKGFPSStatics::GetFixedBlindFireUpPose(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		return FixTransform(ISKGProceduralAnimationInterface::Execute_GetBlindFireUpPose(Firearm));
	}
	return FTransform();
}

FTransform USKGFPSStatics::GetFixedSprintPose(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		return FixTransform(ISKGProceduralAnimationInterface::Execute_GetSprintPose(Firearm));
	}
	return FTransform();
}

FTransform USKGFPSStatics::GetFixedSuperSprintPose(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		return FixTransform(ISKGProceduralAnimationInterface::Execute_GetSuperSprintPose(Firearm));
	}
	return FTransform();
}

FTransform USKGFPSStatics::GetFixedHighPortPose(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		return FixTransform(ISKGProceduralAnimationInterface::Execute_GetHighPortPose(Firearm));
	}
	return FTransform();
}

FTransform USKGFPSStatics::GetFixedLowPortPose(ASKGFirearm* Firearm)
{
	if (IsValid(Firearm))
	{
		return FixTransform(ISKGProceduralAnimationInterface::Execute_GetLowPortPose(Firearm));
	}
	return FTransform();
}

float USKGFPSStatics::MOAtoMrad(float Angle)
{
	return MOA_TO_MRAD_MULTIPLIER * Angle;
}

float USKGFPSStatics::MradtoMOA(float Angle)
{
	return MRAD_TO_MOA_MULTIPLIER * Angle;
}

float USKGFPSStatics::MradtoDeg(float Angle)
{
	return FMath::RadiansToDegrees(Angle / 1000.0);
}

float USKGFPSStatics::MOAtoDeg(float Angle)
{
	return Angle / 60.0;
}

float USKGFPSStatics::MOAfromDeg(float Angle)
{
	return Angle * 60.0;
}

float USKGFPSStatics::MradFromDeg(float Angle)
{
	return FMath::DegreesToRadians(Angle * 1000.0);
}
