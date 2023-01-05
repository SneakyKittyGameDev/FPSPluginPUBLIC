// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MapEditorDataTypes.h"
#include "SKGMapEditorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGMapEditorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SKGMAPEDITOR_API ISKGMapEditorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGMapEditor | Interface")
	void OnGrabbed();
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGMapEditor | Interface")
	void OnRelease();
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGMapEditor | Interface")
	void OnDeleted();
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGMapEditor | Interface")
	void OnUndo();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SKGMapEditor | Interface")
	void OnMaterialLoaded(FSKGMapEditorItemMaterial MapEditorItemMaterial);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SKGMapEditor | Interface")
	void OnScaleChanged(FVector NewScale);
};
