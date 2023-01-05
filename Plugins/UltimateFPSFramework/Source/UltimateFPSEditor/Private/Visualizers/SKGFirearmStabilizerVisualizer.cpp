#include "Visualizers/SKGFirearmStabilizerVisualizer.h"
#include "DataTypes/SKGFPSEditorDataTypes.h"
#include "Components/SKGFirearmStabilizerComponent.h"

FSKGFirearmStabilizerVisualizer::FSKGFirearmStabilizerVisualizer()
{
	
}

void FSKGFirearmStabilizerVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View,
	FPrimitiveDrawInterface* PDI)
{
	if (const USKGFirearmStabilizerComponent* StabilizerComponent = Cast<USKGFirearmStabilizerComponent>(Component))
	{
		const FSKGStabilizerComponentPreview Preview = StabilizerComponent->GetStabilizerPreview();
		if (Preview.bShowArea)
		{
			FBox Box;
			Box.Min = StabilizerComponent->GetComponentLocation() + StabilizerComponent->GetSize();
			Box.Max = StabilizerComponent->GetComponentLocation() - StabilizerComponent->GetSize();
			DrawWireBox(PDI, Box, Preview.Color, Preview.bDrawOnTop, Preview.Thickness, 0, false);
		}
	}
}