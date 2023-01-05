// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "Engine/EngineTypes.h"
#include "SKGFPSStatics.generated.h"

class UPrimitiveComponent;
class UMeshComponent;
class APlayerController;
class USKGPhysicalMaterial;
class ASKGFirearm;

extern const float ULTIMATEFPSFRAMEWORK_API MOA_TO_MRAD_MULTIPLIER;
extern const float ULTIMATEFPSFRAMEWORK_API MOA_TO_DEG_MULTIPLIER;

UCLASS()
class ULTIMATEFPSFRAMEWORK_API USKGFPSStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
protected:
	static float GetSpeedOfSoundDelayLocation(const FVector& LocationA, const FVector& LocationB);
	static float GetSpeedOfSoundDelayActor(const AActor* ActorA, const AActor* ActorB);
	static void PlaySoundDelayed_Implementation(const AActor* WorldActor, const FVector Location, FSKGSoundSettings SoundSettings);
public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Effects")
	static void SpawnImpactEffect(const FHitResult& HitResult, const FSKGImpactEffects& ImpactEffect);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Effects")
	static void SpawnImpactEffectSoundDelayed(const FHitResult& HitResult, const FSKGImpactEffects& ImpactEffect);
	static void SpawnEffects(const FHitResult& HitResult, const FSKGImpactEffects& ImpactEffects, bool bDelayed = false);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Effects")
	static void PlaySoundDelayed(const AActor* WorldActor, const FVector& Location, FSKGSoundSettings SoundSettings);
	
	// Applies MOA to the muzzles rotation for accuracy. 1 MOA = 1 inch at 100 yards
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Firearm")
	static FRotator SetMuzzleMOA(FRotator MuzzleRotation, float MOA = 1.0f);
	// Gets the estimated zero for magnified scopes based on the sight and muzzle
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Firearm")
	static FRotator GetEstimatedMuzzleToScopeZero(const FTransform& MuzzleTransform, const FTransform& SightTransform, const float RangeMeters);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Firearm")
	static FTransform FixTransform(const FTransform& Transform);
	
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Character")
	static bool Ragdoll(UPrimitiveComponent* MeshComponent);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Character")
	static void RagdollWithImpact(UPrimitiveComponent* MeshComponent, FVector ImpactLocation = FVector::ZeroVector, float ImpactForce = 0.0f);	
	
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Projectile")
	static FSKGProjectileTransform CreateProjectileTransform(const FTransform& MuzzleTransform);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Projectile")
	static FTransform CreateTransformForProjectile(const FSKGProjectileTransform& ProjectileTransform);

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | File")
	static bool SaveStringToFile(const FString& Path, const FString& FileName, const FString& FileContent);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | File")
	static bool LoadFileToString(const FString& Path, const FString& FileName, FString& OutString);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | File")
	static bool DeleteFile(const FString& Path, const FString& FileName);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | File")
	static bool GetAllFiles(FString Path, TArray<FString>& OutFiles);
	
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Conversion", meta = (BlueprintAutocast))
	static FTransform ProjectileTransformToTransform(const FSKGProjectileTransform& ProjectileTransform);

	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | String")
	static FString GetTextAfterPeriod(const FString& INString);
	// Rounds a float to the 2nd decimal place and returns in string format
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | String")
	static FString RoundFloat2Pos(float Value);
	// Rounds a float to the 3rd decimal place and returns in string format
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | String")
	static FString RoundFloat3Pos(float Value);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | String")
	static FString GetFormattedCopyPasteTransform(const FTransform& Transform);
	
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Array")
	static int32 GetRandomIndexForArray(uint8 ArrayLength);

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeBoolProperty(UObject* Object, const bool NewBool, const FName NameOfProperty);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeByteProperty(UObject* Object, const uint8 NewByte, const FName NameOfProperty);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeIntProperty(UObject* Object, const int32 NewInt, const FName NameOfProperty);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeInt64Property(UObject* Object, const int64 NewInt, const FName NameOfProperty);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeFloatProperty(UObject* Object, const float NewFloat, const FName NameOfProperty);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeNameProperty(UObject* Object, const FName& NewName, const FName NameOfProperty);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeStringProperty(UObject* Object, const FString& NewString, const FName NameOfProperty);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeTextProperty(UObject* Object, const FText& NewText, const FName NameOfProperty);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeVectorProperty(UObject* Object, const FVector& NewVector, const FName NameOfProperty);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeRotatorProperty(UObject* Object, const FRotator& NewRotator, const FName NameOfProperty);
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Properties")
	static bool ChangeTransformProperty(UObject* Object, const FTransform& NewTransform, const FName NameOfProperty);

	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FirearmParts")
	static TArray<class ASKGLightLaser*> GetLightsLasers(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FirearmParts")
	static TArray<ASKGSight*> GetSights(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FirearmParts")
	static TArray<class ASKGMagnifier*> GetMagnifiers(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FirearmParts")
	static TArray<ASKGPart*> GetRenderTargets(ASKGFirearm* Firearm);
	
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FirearmParts")
	static class ASKGBarrel* GetBarrel(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FirearmParts")
	static class ASKGHandguard* GetHandguard(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FirearmParts")
	static ASKGPart* GetStock(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FirearmParts")
	static class ASKGForwardGrip* GetForwardGrip(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | FirearmParts")
	static class ASKGMuzzle* GetMuzzle(ASKGFirearm* Firearm);
	
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | PoseOffsetFix")
	static FTransform GetFixedBasePoseOffset(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | PoseOffsetFix")
	static FTransform GetFixedSprintPose(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | PoseOffsetFix")
	static FTransform GetFixedSuperSprintPose(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | PoseOffsetFix")
	static FTransform GetFixedHighPortPose(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | PoseOffsetFix")
	static FTransform GetFixedLowPortPose(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | PoseOffsetFix")
	static FTransform GetFixedShortStockPose(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | PoseOffsetFix")
	static FTransform GetFixedOppositeShoulderPose(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | PoseOffsetFix")
	static FTransform GetFixedBlindFireLeftPose(ASKGFirearm* Firearm);
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | PoseOffsetFix")
	static FTransform GetFixedBlindFireUpPose(ASKGFirearm* Firearm);
	
	/**
	* Converts an angle from MOA to milliradians
	*/
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | OpticConversions", meta = (DisplayName = "MOA to mrad", CompactNodeTitle = "MOA->mrad"))
	static float MOAtoMrad(float Angle);

	/**
	* Converts an angle from milliradians to MOA
	*/
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | OpticConversions", meta = (DisplayName = "mrad to MOA", CompactNodeTitle = "mrad->MOA"))
	static float MradtoMOA(float Angle);

	/**
	* Converts an angle from milliradians to Degrees
	*/
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | OpticConversions", meta = (DisplayName = "mrad to Deg", CompactNodeTitle = "mrad->Deg"))
	static float MradtoDeg(float Angle);
	/**
	* Converts an angle from MOA to Degrees
	*/
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | OpticConversions", meta = (DisplayName = "MOA to Deg", CompactNodeTitle = "MOA->Deg"))
	static float MOAtoDeg(float Angle);
	/**
	* Converts an angle from Degrees to MOA
	*/
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | OpticConversions", meta = (DisplayName = "MOA from Deg", CompactNodeTitle = "Deg->MOA"))
	static float MOAfromDeg(float Angle);
	/**
	* Converts an angle from Degrees to milliradians
	*/
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | OpticConversions", meta = (DisplayName = "mrad from Deg", CompactNodeTitle = "Deg->mrad"))
	static float MradFromDeg(float Angle);
};
