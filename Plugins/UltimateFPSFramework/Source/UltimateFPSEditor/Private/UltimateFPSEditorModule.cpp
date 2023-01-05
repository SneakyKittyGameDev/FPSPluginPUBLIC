// Copyright Epic Games, Inc. All Rights Reserved.

#include "UltimateFPSEditorModule.h"

#include "UnrealEdGlobals.h"
#include "Components/SKGAttachmentComponent.h"
#include "Editor/UnrealEdEngine.h"
#include "Visualizers/SKGAttachmentComponentVisualizer.h"
#include "Visualizers/SKGFirearmStabilizerVisualizer.h"
#include "Components/SKGFirearmStabilizerComponent.h"

#define LOCTEXT_NAMESPACE "FUltimateFPSEditorModule"

void FUltimateFPSEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	if (GUnrealEd)
	{
		const TSharedPtr<FSKGAttachmentComponentVisualizer> PartComponentVisualizer = MakeShareable(new FSKGAttachmentComponentVisualizer());
		if (PartComponentVisualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(USKGAttachmentComponent::StaticClass()->GetFName(), PartComponentVisualizer);
			PartComponentVisualizer->OnRegister();
		}

		const TSharedPtr<FSKGFirearmStabilizerVisualizer> StabilizerComponentVisualizer = MakeShareable(new FSKGFirearmStabilizerVisualizer());
		if (StabilizerComponentVisualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(USKGFirearmStabilizerComponent::StaticClass()->GetFName(), StabilizerComponentVisualizer);
			StabilizerComponentVisualizer->OnRegister();
		}
	}
}

void FUltimateFPSEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(USKGAttachmentComponent::StaticClass()->GetFName());
		GUnrealEd->UnregisterComponentVisualizer(USKGFirearmStabilizerComponent::StaticClass()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUltimateFPSEditorModule, UltimateFPSEditor)