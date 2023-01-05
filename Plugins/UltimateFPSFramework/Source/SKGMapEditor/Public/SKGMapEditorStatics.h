// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MapEditorDataTypes.h"
#include "SKGMapEditorStatics.generated.h"

/**
 * 
 */
UCLASS()
class SKGMAPEDITOR_API USKGMapEditorStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static FString EncodeString(const FString& StringToEncode);
	static FString DecodeString(const FString& StringToEncode);
	
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Serialization")
	static FString SerializeLevel(AActor* WorldActor, bool& Success);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Serialization")
	static FSKGMapEditorItems DeSerializeLevel(const FString& JsonString, bool& Success);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Map")
	static void SpawnMapItems(AActor* WorldActor, FSKGMapEditorItems MapItems);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Map")
	static void SpawnMapItemsFromJson(AActor* WorldActor, const FString& JsonString);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Map")
	static void ClearMap(AActor* WorldActor);

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static bool LoadMapFromFile(AActor* WorldActor, const FString& MapDirectory, const FString& MapName, const FString& Extension, FString& OutString, FString& FullMapName);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static bool SaveMapToFile(AActor* WorldActor, const FString& MapDirectory, const FString& MapName, const FString& StringToSave, FString& FullMapName);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static bool DoesMapExist(AActor* WorldActor, const FString& MapDirectory, const FString& MapName);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static FString GetRealMapName(const FString& MapName);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static TArray<FString> GetMapList(AActor* WorldActor, const FString& MapDirectory, bool bCutLevelname = true, bool bShowAllMaps = false);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | FileHandling")
	static FString RemoveExtension(const FString& String);
	
	static void StripInvalidMaps(const FString& WorldName, TArray<FString>& MapList);

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Material")
	static void SetMaterials(const FSKGMapEditorItemMaterial& MapEditorItemMaterial);
};
