//Copyright 2021, Dakota Dawe, All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "FPSTemplateEditorDataTypes.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "FPSTemplate_PartComponent.generated.h"

class AFPSTemplate_PartBase;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class UFPSTemplate_PreviewStatic;
class UFPSTemplate_PreviewSkeletal;

USTRUCT(BlueprintType)
struct FPartOverlap
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "FPSTemplate | Overlap")
	AFPSTemplate_PartBase* Part = nullptr;
	bool bIsOverlapped = false;
	
	FPartOverlap(){}
	FPartOverlap(AFPSTemplate_PartBase* INPart, bool bOverlapping)
	{
		Part = INPart;
		bIsOverlapped = bOverlapping;
	}

	bool operator== (const FPartOverlap& PartOverlap) const
	{
		return Part == PartOverlap.Part;
	}
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ULTIMATEFPSTEMPLATE_API UFPSTemplate_PartComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSTemplate_PartComponent();

protected:	
	// All the possible attachments this component can have. Is used for part compatibility and customization
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSTemplate | Customization")
	bool bAllowClientAuthorativePartChange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSTemplate | Customization")
	TArray<TSubclassOf<AFPSTemplate_PartBase>> PossibleParts;
	// If this is set this part will be spawned by default and attached to the firearm/attachment
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSTemplate | Customization")
	TSubclassOf<AFPSTemplate_PartBase> DefaultPart;
	// This is used for whatever, I use it for the firearm customizer. Use as you wish
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSTemplate | Customization")
	FText ComponentName;
	// The minimum distance you can move the part towards your camera (Up the rail). Recommended leave at 0 except for specific cases (look at the stock on the M4)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSTemplate | Customization")
	float Minimum;
	// The maximum distance you can move the part away from the camera (Down the rail)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSTemplate | Customization")
	float Maximum;
	// This will place the part upon its first spawning at the location in the editor (Display Part Min Max)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSTemplate | Customization")
	bool bSetPartInitialOffsetAtDisplayMinMax;
	// Snap distance. Picatinny rail = 1.0, M-LOK = 4.0, Keymod = 2.0, No Snap = 0.0
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSTemplate | Customization")
	float OffsetSnapDistance;
	UPROPERTY(BlueprintReadOnly, Category = "FPSTemplate | Customization")
	USceneComponent* PartTransformReference;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPSTemplate | Customization")
	bool bUseRightVector;
	
	// Slide the part up/down the rail so you can visualize and figure out the Minimum and Maximum values
	UPROPERTY(EditDefaultsOnly, Category = "FPSTemplate | Preview")
	float DisplayPartMinMax;
	// Sets the preview part to be the mesh of the index of Possible Parts (0 = first part in Possible Parts)
	UPROPERTY(EditDefaultsOnly, Category = "FPSTemplate | Preview")
	int32 DisplayPartIndex;

	UPROPERTY(EditDefaultsOnly, Category = "FPSTemplate | Preview")
	UFPSTemplate_PreviewStatic* PreviewStatic;
	UPROPERTY(EditDefaultsOnly, Category = "FPSTemplate | Preview")
	UFPSTemplate_PreviewSkeletal* PreviewSkeletal;

	UPROPERTY(ReplicatedUsing = OnRep_Part, BlueprintReadOnly, Category = "FPSTemplate")
	AFPSTemplate_PartBase* Part;
	UFUNCTION()
	void OnRep_Part() const;

	FTransform CurrentPartTransform;
	int OldAmount;
	bool bSetInitialOffset;
	TArray<float> CachedSnapPointOffsets;
	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "FPSTemplate | Preview")
	FPartComponentPreview PartComponentPreview;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	bool HasAuthority() const { return IsValid(GetOwner()) ? GetOwner()->HasAuthority() : false; }

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_AddPart(TSubclassOf<AFPSTemplate_PartBase> PartClass, bool bDestroyCurrentPart);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_AddExistingPart(AFPSTemplate_PartBase* INPart, bool bDestroyCurrentPart);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_DestroyCurrentPart();

	void HandlePartConstruction();

	void AddNewPart(AFPSTemplate_PartBase* INPart, bool bDestroyCurrentPart = true);

	UPROPERTY()
	TArray<FPartOverlap> OverlappedParts;

	bool IsMovementInverted() const;
	
public:
	void OverlappedWithPart(FPartOverlap OverlappedPart);
	
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	FTransform GetAttachmentTransform() const;
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	AFPSTemplate_PartBase* GetPart() const { return Part; }
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	TArray<TSubclassOf<AFPSTemplate_PartBase>>& GetPossibleParts() { return PossibleParts; }
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	FVector2D GetMinMaxOffset() const { return FVector2D(Minimum, Maximum); }
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	FVector GetMaxOffsetLocation() const;
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	TArray<float> GetSnapPoints();
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	TArray<FVector> GetWorldSnapPoints();
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	int32 GetPartCurrentSnapPointIndex() const;
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	float GetOffsetSnapDistance() const { return OffsetSnapDistance; }
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	TArray<FPartOverlap>& OverlappingWithParts() { return OverlappedParts; }
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	TArray<AFPSTemplate_PartBase*> GetOverlappedHiddenActors();
	UFUNCTION(BlueprintCallable, Category = "FPSTemplate | Part")
	void HideOverlappedAndUnhideUnoverlappedParts();
	UFUNCTION(BlueprintCallable, Category = "FPSTemplate | Part")
	void ClearOverlappedPartsArray() { OverlappedParts.Empty(); }
	
	template <class T>
	T* GetPart() const { return Cast<T>(Part); }

	UFUNCTION(BlueprintCallable, Category = "FPSTemplate | Part")
	void DestroyCurrentPart();
	/**
	 * If DestroyCurrentPart is false it will return the previously existing part
	 * if you had a holo sight and then added a thermal sight it will return the holo sight
	**/
	UFUNCTION(BlueprintCallable, Category = "FPSTemplate | Part")
	AFPSTemplate_PartBase* AddPart(TSubclassOf<AFPSTemplate_PartBase> PartClass, bool bDestroyCurrentPart = true);
	/**
	 * If DestroyCurrentPart is false it will return the previously existing part
	 * if you had a holo sight and then added a thermal sight it will return the holo sight
	**/
	UFUNCTION(BlueprintCallable, Category = "FPSTemplate | Part")
	AFPSTemplate_PartBase* AddExistingPart(AFPSTemplate_PartBase* INPart, bool bDestroyCurrentPart = true);
	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	bool IsPartCompatible(TSubclassOf<AFPSTemplate_PartBase> PartClass) const;

	UFUNCTION(BlueprintPure, Category = "FPSTemplate | Part")
	bool HasPart() const;

#if WITH_EDITOR
	const FPartComponentPreview& GetPartComponentPreview() const { return PartComponentPreview; }
	UActorComponent* GetMeshComponentFromPart() const;
	FString GetSelectedPartName() const;
	int32 GetEditorPartCurrentSnapPointIndex() const;
	TArray<float> GetEditorSnapDistancePoints() const;
	TArray<FVector> GetEditorSnapPoints() const;
	FVector GetDirectionVector() const  { return bUseRightVector ? GetRightVector() : GetForwardVector(); }
#endif
};
