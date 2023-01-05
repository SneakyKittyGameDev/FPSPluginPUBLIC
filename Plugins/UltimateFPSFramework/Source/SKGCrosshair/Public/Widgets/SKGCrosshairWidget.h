// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SKGCrosshairWidget.generated.h"

class USizeBox;
class APawn;
class UCanvasPanelSlot;
class UImage;

UCLASS()
class SKGCROSSHAIR_API USKGCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	USKGCrosshairWidget(const FObjectInitializer& ObjectInitializer);
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFrameworkCrosshair", Meta = (BindWidget, OptionalWidget = true))
	UImage* I_Circle;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFrameworkCrosshair", Meta = (BindWidget, OptionalWidget = true))
	USizeBox* SB_Top;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFrameworkCrosshair", Meta = (BindWidget, OptionalWidget = true))
	USizeBox* SB_Bottom;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFrameworkCrosshair", Meta = (BindWidget, OptionalWidget = true))
	USizeBox* SB_Left;
	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFrameworkCrosshair", Meta = (BindWidget, OptionalWidget = true))
	USizeBox* SB_Right;

	UPROPERTY(EditAnywhere, Category = "SKGFPSFrameworkCrosshair")
	float TestVelocity;
	UPROPERTY(EditAnywhere, Category = "SKGFPSFrameworkCrosshair")
	float MaxVelocity;
	UPROPERTY(EditAnywhere, Category = "SKGFPSFrameworkCrosshair")
	FVector2D CircleSize;
	UPROPERTY(EditAnywhere, Category = "SKGFPSFrameworkCrosshair")
	float MaxCircleScale;
	UPROPERTY(EditAnywhere, Category = "SKGFPSFrameworkCrosshair")
	float MaxOffset;
	UPROPERTY(EditAnywhere, Category = "SKGFPSFrameworkCrosshair")
	float UpdatesPerSecond;
	UPROPERTY(EditAnywhere, Category = "SKGFPSFrameworkCrosshair")
	bool UpdatesFromOutsideSource;
	UPROPERTY(EditAnywhere, Category = "SKGFPSFrameworkCrosshair")
	float InterpolationRate;

	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFrameworkCrosshair", Meta = (ExposeOnSpawn))
	APawn* OwningPawn;

	FTimerHandle TCrosshair;
	float CrosshairScale;
	float CircleScale;

	TWeakObjectPtr<UCanvasPanelSlot> CPS_Circle;
	TWeakObjectPtr<UCanvasPanelSlot> CPS_Top;
	TWeakObjectPtr<UCanvasPanelSlot> CPS_Bottom;
	TWeakObjectPtr<UCanvasPanelSlot> CPS_Left;
	TWeakObjectPtr<UCanvasPanelSlot> CPS_Right;

	virtual void SynchronizeProperties() override;
	virtual void NativeConstruct() override;
	void SetCanvasPanelSlots();

	UCanvasPanelSlot* GetCanvasPanelSlot(UWidget* Widget) const;
	void HandleCrosshairOffset();

public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | Crosshair")
	void HandleCrosshairOffset(float Velocity);
};
