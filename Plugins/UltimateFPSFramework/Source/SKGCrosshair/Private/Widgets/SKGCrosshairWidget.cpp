// Copyright 2022, Dakota Dawe, All rights reserved


#include "Widgets/SKGCrosshairWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

USKGCrosshairWidget::USKGCrosshairWidget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	TestVelocity = 0.0f;
	MaxVelocity = 350.0f;
	MaxOffset = 100.0f;

	UpdatesPerSecond = 60.0f;
	UpdatesFromOutsideSource = false;
	InterpolationRate = 30.0f;
	
	CrosshairScale = 0.0f;
	CircleScale = 0.0f;

	MaxCircleScale = 5.0f;
	CircleSize = FVector2D(100.0f, 100.0f);
}

void USKGCrosshairWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	SetCanvasPanelSlots();
	HandleCrosshairOffset(TestVelocity);
}

void USKGCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetCanvasPanelSlots();

	if (!UpdatesFromOutsideSource && UpdatesPerSecond > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(TCrosshair, this, &USKGCrosshairWidget::HandleCrosshairOffset, 1.0f / UpdatesPerSecond, true);
	}
}

void USKGCrosshairWidget::SetCanvasPanelSlots()
{
	CPS_Circle = GetCanvasPanelSlot(I_Circle);
	CPS_Top = GetCanvasPanelSlot(SB_Top);
	CPS_Bottom = GetCanvasPanelSlot(SB_Bottom);
	CPS_Left = GetCanvasPanelSlot(SB_Left);
	CPS_Right = GetCanvasPanelSlot(SB_Right);
}

UCanvasPanelSlot* USKGCrosshairWidget::GetCanvasPanelSlot(UWidget* Widget) const
{
	if (Widget)
	{
		return Cast<UCanvasPanelSlot>(Widget->Slot);
	}
	return nullptr;
}

void USKGCrosshairWidget::HandleCrosshairOffset()
{
	if (OwningPawn)
	{
		TestVelocity = OwningPawn->GetVelocity().Size();
	}
	HandleCrosshairOffset(TestVelocity);
}

void USKGCrosshairWidget::HandleCrosshairOffset(float Velocity)
{
	if (CPS_Circle.Get() || CPS_Top.Get() || CPS_Bottom.Get() || CPS_Left.Get() || CPS_Right.Get())
	{
		float FixedVelocity = UKismetMathLibrary::NormalizeToRange(Velocity, 0.0f, MaxVelocity);
		FixedVelocity = FMath::Clamp(FixedVelocity, 0.0f, 1.0f);

		CrosshairScale = UKismetMathLibrary::FInterpTo(CrosshairScale, FixedVelocity * MaxOffset, GetWorld()->DeltaTimeSeconds, InterpolationRate);
		
		FVector2D Position;

		if (CPS_Circle.Get())
		{
			CircleScale = UKismetMathLibrary::NormalizeToRange(CrosshairScale, 0.0f, MaxOffset);
			CircleScale *= MaxCircleScale;
			CircleScale += 1.0f;
			CircleScale = FMath::Clamp(CircleScale, 1.0f, MaxCircleScale + 1);
			
			CPS_Circle->SetSize(CircleSize * CircleScale);
		}
		
		if (CPS_Top.Get())
		{
			Position.Y = -CrosshairScale;
			Position.X = 0.0f;
			CPS_Top->SetPosition(Position);
		}

		if (CPS_Bottom.Get())
		{
			Position.Y = CrosshairScale;
			Position.X = 0.0f;
			CPS_Bottom->SetPosition(Position);
		}

		if (CPS_Left.Get())
		{
			Position.Y = 0.0f;
			Position.X = -CrosshairScale;
			CPS_Left->SetPosition(Position);
		}

		if (CPS_Right.Get())
		{
			Position.Y = 0.0f;
			Position.X = CrosshairScale;
			CPS_Right->SetPosition(Position);
		}
	}
}