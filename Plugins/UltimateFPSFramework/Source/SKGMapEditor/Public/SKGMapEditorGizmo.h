// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapEditorDataTypes.h"
#include "SKGMapEditorGizmo.generated.h"

class UStaticMeshComponent;
class USKGMapEditorHandlerComponent;

UCLASS()
class SKGMAPEDITOR_API ASKGMapEditorGizmo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASKGMapEditorGizmo();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	UStaticMeshComponent* Origin;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	UStaticMeshComponent* ZAxis;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	UStaticMeshComponent* XAxis;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	UStaticMeshComponent* YAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	UStaticMeshComponent* Yaw;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	UStaticMeshComponent* Roll;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	UStaticMeshComponent* Pitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	UStaticMeshComponent* ScaleZ;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	UStaticMeshComponent* ScaleX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	UStaticMeshComponent* ScaleY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	float MovementSlowdown;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ESKGMoveAxis GetMoveAxis(UPrimitiveComponent* HitComponent);
	ESKGRotationAxis GetRotationAxis(UPrimitiveComponent* HitComponent);
	ESKGScaleAxis GetScaleAxis(UPrimitiveComponent* HitComponent);
	FVector GetMouseWorldPosition();
	FVector2D GetMousePosition();
	//FVector GetMousePosition3D();

	void HandleMovement();
	void HandleRotation();
	void HandleScale();

	ESKGMoveAxis MoveAxis;
	ESKGRotationAxis RotationAxis;
	ESKGScaleAxis ScaleAxis;

	ESKGGizmoType CurrentGizmo;
	
	FVector CurrentMouseWorldPos;
	FVector2D ClickedMousePos;
	FVector ClickedMouseWorldPos;

	TWeakObjectPtr<APlayerController> OwningController;
	UPROPERTY()
	USKGMapEditorHandlerComponent* HandlerComponent;
	UPROPERTY()
	AActor* CurrentActor;

	bool bGrabbed;

	FTimerHandle TReplicateHandle;
	void Replicate();

	bool IsRightOfActor();

	bool GreaterThanSnapAmount(float Current, float New);
	
public:
	virtual void Tick(float DeltaSeconds) override;

	void SetHandler(USKGMapEditorHandlerComponent* Handler) { HandlerComponent = Handler; }
	
	void SnapToActor(AActor* Actor);
	void HideGizmo(bool Hide);
	void ClearGizmo();

	void HitGizmo(FHitResult HitResult);
	void ReleaseGizmo();

	void ShowMovement();
	void ShowRotation();
	void ShowScale();

	ESKGGizmoType GetGizmoType() const { return CurrentGizmo; }
};