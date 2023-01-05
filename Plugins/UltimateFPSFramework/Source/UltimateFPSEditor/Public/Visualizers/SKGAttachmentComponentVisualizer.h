#pragma once

#include "InputCoreTypes.h"
#include "ComponentVisualizer.h"

class FSKGAttachmentComponentVisualizer : public FComponentVisualizer
{
public:
	FSKGAttachmentComponentVisualizer();

	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void OnRegister() override;

	//virtual bool HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
};
