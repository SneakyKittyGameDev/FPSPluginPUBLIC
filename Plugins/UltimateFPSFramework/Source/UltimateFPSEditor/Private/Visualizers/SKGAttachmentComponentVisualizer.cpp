#include "Visualizers/SKGAttachmentComponentVisualizer.h"
#include "Components/SKGAttachmentComponent.h"
#include "SKGAttachmentDataTypes.h"

#include "EditorViewportClient.h"
#include "Kismet/GameplayStatics.h"

FSKGAttachmentComponentVisualizer::FSKGAttachmentComponentVisualizer()
{
	
}

void FSKGAttachmentComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View,
	FPrimitiveDrawInterface* PDI)
{
	if (const USKGAttachmentComponent* PartComponent = Cast<USKGAttachmentComponent>(Component))
	{
		const FSKGAttachmentComponentPreview PreviewData = PartComponent->GetAttachmentComponentPreview();
		if (PartComponent->GetMinMaxOffset().X != 0.0f || PartComponent->GetMinMaxOffset().Y != 0.0f)
		{
			const FSKGAttachmentComponentMovementPreview LineData = PreviewData.LinePreview;
			if (LineData.bShowMinMaxLine || LineData.bShowMinMaxPoints || LineData.bShowSnapPoints)
			{
				FVector Start = PartComponent->GetComponentLocation();
				FVector End;
		
				bool bInverted = false;
				if (PartComponent->GetMinMaxOffset().X < 0.0f)
				{
					bInverted = true;
				}
		
				if (bInverted)
				{
					End = Start + PartComponent->GetDirectionVector() * PartComponent->GetMinMaxOffset().X;
				}
				else
				{
					End = Start + PartComponent->GetDirectionVector() * PartComponent->GetMinMaxOffset().Y;
				}

				Start.Z += LineData.HeightOffset;
				End.Z += LineData.HeightOffset;

				if (LineData.bShowMinMaxLine)
				{
					PDI->DrawLine(Start, End, LineData.LineColor, LineData.bDrawLineOnTop, LineData.LineThickness, 0);
				}
				
				if (LineData.bShowMinMaxPoints)
				{
					DrawWireSphere(PDI, Start, LineData.MinMaxPointColor, LineData.MinMaxPointRadius, 32, LineData.bDrawMinMaxPointsOnTop, LineData.MinMaxPointThickness, false);
					DrawWireSphere(PDI, End, LineData.MinMaxPointColor, LineData.MinMaxPointRadius, 32, LineData.bDrawMinMaxPointsOnTop, LineData.MinMaxPointThickness, false);
				}

				if (LineData.bShowSnapPoints)
				{
					TArray<FVector> SnapPoints = PartComponent->GetEditorSnapPoints();
					const int32 CurrentIndex = PartComponent->GetEditorAttachmentCurrentSnapPointIndex();
					for (uint8 i = 0; i < SnapPoints.Num(); ++i)
					{
						FVector& Location = SnapPoints[i];
						Location.Z += LineData.HeightOffset;
						const FLinearColor Color = CurrentIndex == i ? FLinearColor::Green : LineData.SnapPointColor;
						DrawWireSphere(PDI, Location, Color, LineData.SnapPointRadius, 32, LineData.bDrawSnapPointsOnTop, LineData.SnapPointThickness, false);
					}
				}
			}
		}
	}
}

void FSKGAttachmentComponentVisualizer::OnRegister()
{
	
}

/*bool FFPSTemplate_PartComponentVisualizer::HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport,
	FKey Key, EInputEvent Event)
{
	bool bHandled = false;
	UE_LOG(LogTemp, Warning, TEXT("HandleInputKey"));
	if (Key == EKeys::W)
	{
		if (UFPSTemplate_PartComponent* PartComponent = Cast<UFPSTemplate_PartComponent>(GetEditedComponent()))
		{
			UE_LOG(LogTemp, Warning, TEXT("HANDLED W EVENT"));
			PartComponent->AddRelativeLocation(FVector(0.0f, 0.0f, 1.0f));
			bHandled = true;
		}
	}

	return bHandled;
}*/
