// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "SKGSceneCaptureDataTypes.h"
#include "Components/SceneCaptureComponent2D.h"
#include "SKGSceneCaptureComponent2D.generated.h"

class UTextureRenderTarget2D;

UCLASS(hidecategories=(Collision, Object, Physics, SceneComponent), ClassGroup=Rendering, editinlinenew, meta=(BlueprintSpawnableComponent))
class SKGSCENECAPTURE_API USKGSceneCaptureComponent2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:
	USKGSceneCaptureComponent2D();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	FSKGSceneCaptureOptimization Optimization;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	FSKGRenderTargetSize RenderTargetSize;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFPSFramework | Default")
	FName RenderTargetMaterialVarName;

	UPROPERTY(BlueprintReadOnly, Category = "SKGFPSFramework | SceneCaptureMaterial")
	UMaterialInstanceDynamic* DynamicMaterial;
	TWeakObjectPtr<UMeshComponent> OwningMesh;
	int32 MaterialIndex;
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetupRenderTarget();
	
public:
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	void StartCapture();
	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	void StopCapture();

	UFUNCTION(BlueprintCallable, Category = "SKGFPSFramework | SceneComponent")
	void SetupOwningMesh(int32 RTMaterialIndex, UMeshComponent* Mesh);
};
