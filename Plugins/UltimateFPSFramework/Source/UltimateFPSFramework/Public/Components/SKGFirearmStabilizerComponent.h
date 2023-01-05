// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "DataTypes/SKGFPSEditorDataTypes.h"
#include "SKGFirearmStabilizerComponent.generated.h"

class ASKGFirearm;
class USKGCharacterComponent;
class UCurveVector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFirearmStabilized);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFirearmUnStabilized);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ULTIMATEFPSFRAMEWORK_API USKGFirearmStabilizerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	USKGFirearmStabilizerComponent();

protected:
	// What Trace Channel to use for detection
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Stabilizer")
	TEnumAsByte<ETraceTypeQuery> CollisionChannel;
	// Whether or not to draw debug lines and what type of debugging
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Stabilizer")
	TEnumAsByte<EDrawDebugTrace::Type> DebugType;
	// Only attempt to stabilize with a key press, does not automatically stabilize
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Stabilizer")
	bool bOnlyWithKeyPress;
	// Only run the stabilization logic on the owner. I recommend leaving to true
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Stabilizer")
	bool bOnlyRunOnOwner;
	// Dimensions of the stabilizer box
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Stabilizer")
	FVector Size;
	// How many times per second to check for a surface to stabilize on
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Stabilizer")
	float UpdateRate;
	// Curve to play upon stabilized
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Stabilizer")
	FSKGCurveData StabilizedCurveData;
	// Curve to play upon unstabilized
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Stabilizer")
	FSKGCurveData UnStabilizedCurveData;

	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Stabilizer")
	ASKGFirearm* Firearm;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | Stabilizer")
	USKGCharacterComponent* CharacterComponent;

	bool bIsStabilized;
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFPSFramework | Stabilizer")
	FSKGStabilizerComponentPreview StabilizerPreview;
#endif

	void StabilizeFirearm();
	
	UPROPERTY(BlueprintAssignable, Category = "SKGFPSFramework | Stabilizer")
	FFirearmStabilized FirearmStabilized;
	UPROPERTY(BlueprintAssignable, Category = "SKGFPSFramework | Stabilizer")
	FFirearmUnStabilized FirearmUnStabilized;
	
public:	
	// Gets called on Begin Play by default, sets Firearm and CharacterComponent reference.
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Stabilizer")
	void CacheEssentials();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Stabilizer")
	void Enable();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Stabilizer")
	void Disable();
	// True if firearm resting area found
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Stabilizer")
	bool FindStabilizingSurface();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Stabilizer")
	void AttemptStabilizeFirearm();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Stabilizer")
	void UnStabilizeFirearm();
	UFUNCTION(BlueprintPure, Category = "SKGFPSFramework | Stabilizer")
	bool IsStabilized() const { return bIsStabilized; }

#if WITH_EDITOR
	FVector GetSize() const { return Size; }
	const FSKGStabilizerComponentPreview& GetStabilizerPreview() const { return StabilizerPreview; }
#endif
};
