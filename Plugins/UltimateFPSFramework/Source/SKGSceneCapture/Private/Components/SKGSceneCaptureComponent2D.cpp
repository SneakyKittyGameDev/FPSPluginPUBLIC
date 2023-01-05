// Copyright 2022, Dakota Dawe, All rights reserved


#include "Components/SKGSceneCaptureComponent2D.h"

#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

USKGSceneCaptureComponent2D::USKGSceneCaptureComponent2D()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(false);
	bUseAttachParentBound = true;
	
	bHiddenInGame = false;
	bCaptureEveryFrame = true;
	bCaptureOnMovement = false;
	bAlwaysPersistRenderingState = true;
	PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	CaptureSource = ESceneCaptureSource::SCS_SceneColorHDRNoAlpha;
	
	ShowFlags.DynamicShadows = true;
	ShowFlags.AmbientOcclusion = false;
	ShowFlags.AmbientCubemap = false;
	ShowFlags.DistanceFieldAO = false;
	ShowFlags.LightFunctions = false;
	ShowFlags.LightShafts = false;
	ShowFlags.ReflectionEnvironment = true;
	ShowFlags.ScreenSpaceReflections = false;
	ShowFlags.TexturedLightProfiles = false;
	ShowFlags.VolumetricFog = false;
	ShowFlags.MotionBlur = 0;

	RenderTargetMaterialVarName = FName("RenderTarget");
}

void USKGSceneCaptureComponent2D::BeginPlay()
{
	Super::BeginPlay();
	
	StopCapture();
	
	if (!Optimization.bOverrideCaptureEveryFrame)
	{
		SetComponentTickInterval(1.0f / Optimization.RefreshRate);
	}
	SetupRenderTarget();
}

void USKGSceneCaptureComponent2D::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USKGSceneCaptureComponent2D::SetupRenderTarget()
{
	TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), RenderTargetSize.Width, RenderTargetSize.Height, ETextureRenderTargetFormat::RTF_RGBA16f);
}

void USKGSceneCaptureComponent2D::StartCapture()
{
	SetComponentTickEnabled(true);
}

void USKGSceneCaptureComponent2D::StopCapture()
{
	SetComponentTickEnabled(false);
}

void USKGSceneCaptureComponent2D::SetupOwningMesh(int32 RTMaterialIndex, UMeshComponent* Mesh)
{
	OwningMesh = Mesh;
	MaterialIndex = RTMaterialIndex;
	if (OwningMesh.IsValid() && OwningMesh->GetNumMaterials() >= MaterialIndex)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(OwningMesh->GetMaterial(MaterialIndex), this);
		DynamicMaterial->SetTextureParameterValue(RenderTargetMaterialVarName, TextureTarget);
		OwningMesh->SetMaterial(MaterialIndex, DynamicMaterial);
	}
}
