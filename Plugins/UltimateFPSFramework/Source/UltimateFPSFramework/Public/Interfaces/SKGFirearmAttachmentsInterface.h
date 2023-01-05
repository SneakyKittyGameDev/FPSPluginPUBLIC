// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGFPSDataTypes.h"
#include "UObject/Interface.h"
#include "SKGFirearmAttachmentsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGFirearmAttachmentsInterface : public UInterface
{
	GENERATED_BODY()
};

class USKGCharacterComponent;
class UMeshComponent;

class ULTIMATEFPSFRAMEWORK_API ISKGFirearmAttachmentsInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmAttachmentsInterface")
	FSKGFirearmPartStats GetPartStats();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmAttachmentsInterface")
	FSKGFirearmPartData GetPartData() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmAttachmentsInterface")
	ESKGPartType GetPartType() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmAttachmentsInterface")
	USKGCharacterComponent* GetOwningCharacterComponent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmAttachmentsInterface")
	UMeshComponent* GetPartMesh() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFPSFramework | FirearmAttachmentsInterface")
	AActor* GetOwningActor();
};
