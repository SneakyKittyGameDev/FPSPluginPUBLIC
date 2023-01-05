// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "Components/MeshComponent.h"
#include "MapEditorDataTypes.generated.h"

UENUM(BlueprintType)
enum class ESKGMoveAxis : uint8
{
	None	UMETA(DisplayName = "None"),
	XAxis	UMETA(DisplayName = "XAxis"),
	YAxis	UMETA(DisplayName = "Yxis"),
	ZAxis	UMETA(DisplayName = "ZAxis")
};

UENUM(BlueprintType)
enum class ESKGRotationAxis : uint8
{
	None	UMETA(DisplayName = "None"),
	Yaw		UMETA(DisplayName = "Yaw"),
	Roll	UMETA(DisplayName = "Roll"),
	Pitch	UMETA(DisplayName = "Pitch")
};

UENUM(BlueprintType)
enum class ESKGScaleAxis : uint8
{
	None	UMETA(DisplayName = "None"),
	ScaleX	UMETA(DisplayName = "ScaleX"),
	ScaleY	UMETA(DisplayName = "ScaleY"),
	ScaleZ	UMETA(DisplayName = "ScaleZ")
};

UENUM(BlueprintType)
enum class ESKGGizmoType : uint8
{
	None		UMETA(DisplayName = "None"),
	Location	UMETA(DisplayName = "Location"),
	Rotation	UMETA(DisplayName = "Rotation"),
	Scale		UMETA(DisplayName = "Scale")
};

USTRUCT(BlueprintType)
struct FSKGMapEditorSnapping
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "SKGMapEditor")
	float Location;
	UPROPERTY(BlueprintReadWrite, Category = "SKGMapEditor")
	float Rotation;
	UPROPERTY(BlueprintReadWrite, Category = "SKGMapEditor")
	float Scale;

	FSKGMapEditorSnapping()
	{
		Location = 5.0f;
		Rotation = 5.0f;
		Scale = 0.0f;
	}
	FSKGMapEditorSnapping(float DefaultSnapValue)
	{
		Location = DefaultSnapValue;
		Rotation = DefaultSnapValue;
		Scale = 0.0f;
	}
};

USTRUCT()
struct FSKGMapEditorSteps
{
	GENERATED_BODY()
	AActor* Actor;
	TArray<FTransform> Transforms;
};

USTRUCT(BlueprintType)
struct FSKGMapEditorItemMaterial
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<UMeshComponent*> MeshComponents;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<UMaterialInterface*> Materials;
};

USTRUCT(BlueprintType)
struct FSKGMapEditorItem
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TSubclassOf<AActor> ActorToSpawn;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	FTransform ItemTransform;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<UMaterialInterface*> Materials;
};

USTRUCT(BlueprintType)
struct FSKGMapEditorItems
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<FSKGMapEditorItem> Items;
};