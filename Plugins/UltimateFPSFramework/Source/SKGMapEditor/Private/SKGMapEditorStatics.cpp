// Copyright 2021, Dakota Dawe, All rights reserved


#include "SKGMapEditorStatics.h"
#include "SKGMapEditorInterface.h"

#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/Base64.h"
#include "Materials/MaterialInterface.h"

FString USKGMapEditorStatics::SerializeLevel(AActor* WorldActor, bool& Success)
{
	if (!WorldActor)
	{
		Success = false;
		return FString("World Actor Invalid");
	}

	if (const UWorld* World = WorldActor->GetWorld())
	{
		FString SerializedString;
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsWithInterface(World, USKGMapEditorInterface::StaticClass(), Actors);
		FSKGMapEditorItems MapItems;
		for (const AActor* Actor : Actors)
		{
			if (IsValid(Actor))
			{
				TArray<UMaterialInterface*> Materials;
				
				TArray<UActorComponent*> MeshComponents = Actor->GetComponentsByTag(UMeshComponent::StaticClass(), FName("MapEditor"));
				for (UActorComponent* Component : MeshComponents)
				{
					if (const UMeshComponent* MeshComponent = Cast<UMeshComponent>(Component))
					{
						const int32 MaterialCount = MeshComponent->GetNumMaterials();
						for (uint8 i = 0; i < MaterialCount; ++i)
						{
							Materials.Add((MeshComponent->GetMaterial(i)));
						}
						UE_LOG(LogTemp, Warning, TEXT("Mesh Component Found: %s"), *MeshComponent->GetName());
						//Materials.Add((MeshComponent->GetMaterial(0)));
					}
				}
				FSKGMapEditorItem Item;
				Item.ActorToSpawn = Actor->GetClass();
				Item.ItemTransform = Actor->GetTransform();
				Item.Materials = Materials;
				MapItems.Items.Add(Item);
			}
		}
		Success = FJsonObjectConverter::UStructToJsonObjectString(MapItems, SerializedString);
		return SerializedString;
	}
	return FString("Failed");
}

FSKGMapEditorItems USKGMapEditorStatics::DeSerializeLevel(const FString& JsonString, bool& Success)
{
	FSKGMapEditorItems MapItems;
	Success = false;
	if (!JsonString.IsEmpty())
	{
		Success = FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &MapItems, 0, 0);
	}
	return MapItems;
}

void USKGMapEditorStatics::SpawnMapItems(AActor* WorldActor, FSKGMapEditorItems MapItems)
{
	if (!WorldActor || (WorldActor && !WorldActor->HasAuthority())) return;
	ClearMap(WorldActor);
	
	if (UWorld* World = WorldActor->GetWorld())
	{
		for (FSKGMapEditorItem& Item : MapItems.Items)
		{
			if (Item.ActorToSpawn)
			{
				if (AActor* Actor = World->SpawnActor<AActor>(Item.ActorToSpawn))
				{
					Actor->SetActorTransform(Item.ItemTransform);
					
					if (Actor->GetClass()->ImplementsInterface(USKGMapEditorInterface::StaticClass()))
					{
						if (Item.Materials.Num())
						{
							FSKGMapEditorItemMaterial MapEditorItemMaterial;
							
							uint8 MaterialIndex = 0;
							TArray<UActorComponent*> MeshComponents = Actor->GetComponentsByTag(UMeshComponent::StaticClass(), FName("MapEditor"));
							MapEditorItemMaterial.MeshComponents.Reserve(MeshComponents.Num());
							for (UActorComponent* Component : MeshComponents)
							{
								if (UMeshComponent* MeshComponent = Cast<UMeshComponent>(Component))
								{
									MapEditorItemMaterial.MeshComponents.Add(MeshComponent);
									MapEditorItemMaterial.Materials = Item.Materials;
								}
							}
							ISKGMapEditorInterface::Execute_OnMaterialLoaded(Actor, MapEditorItemMaterial);
						}
					}
				}
			}
		}
	}
}

void USKGMapEditorStatics::SpawnMapItemsFromJson(AActor* WorldActor, const FString& JsonString)
{
	if (!WorldActor || (WorldActor && !WorldActor->HasAuthority()) && !JsonString.IsEmpty()) return;
	ClearMap(WorldActor);
	
	bool bSuccess = false;
	const FSKGMapEditorItems MapItems = DeSerializeLevel(JsonString, bSuccess);
	SpawnMapItems(WorldActor, MapItems);
}

void USKGMapEditorStatics::ClearMap(AActor* WorldActor)
{
	if (!WorldActor || (WorldActor && !WorldActor->HasAuthority())) return;
	if (const UWorld* World = WorldActor->GetWorld())
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsWithInterface(World, USKGMapEditorInterface::StaticClass(), Actors);

		for (AActor* Actor : Actors)
		{
			if (Actor)
			{
				Actor->Destroy();
			}
		}
	}
}

FString USKGMapEditorStatics::EncodeString(const FString& StringToEncode)
{
	return FBase64::Encode(StringToEncode);
}

FString USKGMapEditorStatics::DecodeString(const FString& StringToEncode)
{
	FString Dest;
	FBase64::Decode(StringToEncode, Dest);
	return Dest;
}

bool USKGMapEditorStatics::LoadMapFromFile(AActor* WorldActor, const FString& MapDirectory, const FString& MapName, const FString& Extension, FString& OutString, FString& FullMapName)
{
	if (!WorldActor || MapName.IsEmpty()) return false;
	
	if (const UWorld* World = WorldActor->GetWorld())
	{
		const FString LevelName = UGameplayStatics::GetCurrentLevelName(World);
		const FString FileName = FString(LevelName + "&" + MapName);
		const FString FilePath = FString(MapDirectory + "/" + FileName + Extension);
		FString Dest;
		FullMapName = RemoveExtension(FileName);
		const bool Successful = FFileHelper::LoadFileToString(Dest, *FilePath);
		OutString = DecodeString(Dest);
		return Successful;
	}
	return false;
}

bool USKGMapEditorStatics::SaveMapToFile(AActor* WorldActor, const FString& MapDirectory, const FString& MapName, const FString& StringToSave, FString& FullMapName)
{
	if (!WorldActor || MapName.IsEmpty() || StringToSave.IsEmpty()) return false;
	
	if (const UWorld* World = WorldActor->GetWorld())
	{
		const FString LevelName = UGameplayStatics::GetCurrentLevelName(World);
		const FString FileName = FString(LevelName + "&" + MapName + ".skmap");
		const FString FilePath = FString(MapDirectory + "/" + FileName);
		FullMapName = RemoveExtension(FileName);
		return FFileHelper::SaveStringToFile(EncodeString(StringToSave), *FilePath);
	}
	return false;
}

bool USKGMapEditorStatics::DoesMapExist(AActor* WorldActor, const FString& MapDirectory, const FString& MapName)
{
	if (!WorldActor || MapName.IsEmpty()) return false;
	
	if (const UWorld* World = WorldActor->GetWorld())
	{
		const FString LevelName = UGameplayStatics::GetCurrentLevelName(World);
		const FString FileName = FString(LevelName + "&" + MapName + ".skmap");
		const FString FilePath = FString(MapDirectory + "/" + FileName);
		return FPaths::FileExists(*FilePath);
	}
	return false;
}

FString USKGMapEditorStatics::GetRealMapName(const FString& MapName)
{
	int32 Index = MapName.Find("&");
	if (Index > -1)
	{
		++Index;
		Index = MapName.Len() - Index;
		return MapName.Right(Index);
	}
	return FString("Could Not Get Map Name");
}

TArray<FString> USKGMapEditorStatics::GetMapList(AActor* WorldActor, const FString& MapDirectory, bool bCutLevelname, bool bShowAllMaps)
{
	FString FilesDirectory = *(MapDirectory + "/");
	if (FPaths::DirectoryExists(FilesDirectory))
	{
		TArray<FString> FileNames;
		FilesDirectory += "*.skmap";
		FFileManagerGeneric::Get().FindFiles(FileNames, *FilesDirectory, true, false);
		if (WorldActor && !bShowAllMaps)
		{
			if (UWorld* World = WorldActor->GetWorld())
			{
				StripInvalidMaps(UGameplayStatics::GetCurrentLevelName(World), FileNames);
			}
		}
		if (bCutLevelname)
		{
			for (FString& MapName : FileNames)
			{
				MapName = GetRealMapName(MapName);
			}
		}
		return FileNames;
	}
	return TArray<FString>();
}

FString USKGMapEditorStatics::RemoveExtension(const FString& String)
{
	int32 Index = String.Find(".");
	if (Index > -1)
	{
		int32 Difference = String.Len() - Index;
		Index = String.Len() - Difference;
		return String.Left(Index);
	}
	return String;
}

void USKGMapEditorStatics::StripInvalidMaps(const FString& WorldName, TArray<FString>& MapList)
{
	for (uint8 i = 0; i < MapList.Num(); ++i)
	{
		if (!MapList[i].Contains(WorldName))
		{
			MapList.RemoveSingle(MapList[i]);
		}
	}
	MapList.Shrink();
}

void USKGMapEditorStatics::SetMaterials(const FSKGMapEditorItemMaterial& MapEditorItemMaterial)
{
	if (MapEditorItemMaterial.MeshComponents.Num() && MapEditorItemMaterial.Materials.Num())
	{
		int32 MaterialIndex = 0;
		for (UMeshComponent* MeshComponent : MapEditorItemMaterial.MeshComponents)
		{
			if (IsValid(MeshComponent))
			{
				const int32 MaterialNum = MeshComponent->GetNumMaterials();
				for (uint8 i = 0; i < MaterialNum; ++i)
				{
					if (MaterialIndex < MapEditorItemMaterial.Materials.Num())
					{
						MeshComponent->SetMaterial(i, MapEditorItemMaterial.Materials[MaterialIndex]);
					}
					else
					{
						break;
					}
					++MaterialIndex;
				}
			}
		}
	}
}
