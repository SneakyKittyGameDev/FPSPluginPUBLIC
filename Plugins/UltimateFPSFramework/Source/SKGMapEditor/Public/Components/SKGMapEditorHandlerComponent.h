// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "MapEditorDataTypes.h"
#include "SKGMapEditorGizmo.h"
#include "Components/ActorComponent.h"
#include "SKGMapEditorHandlerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class SKGMAPEDITOR_API USKGMapEditorHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USKGMapEditorHandlerComponent();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SKGMapEditor")
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	FString MapDirectory;
	UPROPERTY(EditDefaultsOnly, Category = "SKGMapEditor")
	TSubclassOf<ASKGMapEditorGizmo> GizmoClass;
	UPROPERTY(EditDefaultsOnly, Category = "SKGMapEditor")
	float ReplicationRate;
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentActor, Category = "SKGMapEditor | Selection")
	AActor* CurrentActor;
	UFUNCTION()
	void OnRep_CurrentActor();
	FTransform CurrentActorTransform;
	
	TWeakObjectPtr<APawn> ReturnPawn;

	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor | Edit")
	FSKGMapEditorSnapping SnapAmount;

	TArray<FSKGMapEditorSteps> UndoSteps;

	TWeakObjectPtr<ASKGMapEditorGizmo> Gizmo;
	
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetGizmo();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_ReplicateTransform(AActor* Actor, FTransform Transform);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SpawnActor(TSubclassOf<AActor> ActorClass);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SpawnActorAtTransform(TSubclassOf<AActor> ActorClass, const FTransform& Transform);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_DeleteActor(AActor* Actor);
	
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_UnpossessToReturnPawn();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FHitResult MouseTraceSingle(const float Distance, bool& bHitGizmo, const ECollisionChannel CollisionChannel, const bool bDrawDebugLine = false);
	FHitResult MouseTraceMulti(const float Distance, bool& bHitGizmo, const ECollisionChannel CollisionChannel, const bool bDrawDebugLine = false);
	
public:
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Initilization")
	void Init();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Initilization")
	void SetReturnPawn(APawn* Pawn);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Initilization")
	void UnpossessToReturnPawn();
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Initilization")
	bool HasValidReturnPawn() const { return ReturnPawn != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Edit")
	void DuplicateActor(bool bSpawnInPlace);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Edit")
	void DeselectActor();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Edit")
	void DeleteActor();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Edit")
	void Undo();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Edit")
	void SetSnapAmount(FSKGMapEditorSnapping SnappingAmounts);
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Edit")
	FSKGMapEditorSnapping GetSnapAmount() const { return SnapAmount; }

	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Edit")
	float GetReplicationRate() const { return ReplicationRate > 0.001 ? ReplicationRate : 0.1f; }
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Spawn")
	void SpawnActor(TSubclassOf<AActor> ActorClass);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Spawn")
	void SpawnActorAtTransform(TSubclassOf<AActor> ActorClass, const FTransform SpawnTransform);
	
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Select")
	void SetActor(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Select")
	FHitResult MouseTrace(float Distance, bool& bHitGizmo, bool bDrawDebugLine = false);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Select")
	void Grab();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Select")
	void Release();

	void ReplicateActor();

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Gizmo")
	void ShowMovement();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Gizmo")
	void ShowRotation();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Gizmo")
	void ShowScale();
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Gizmo")
	ESKGGizmoType GetGizmoType() const { return Gizmo.IsValid() ? Gizmo->GetGizmoType() : ESKGGizmoType::Location; }

	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Selected Actor")
	FTransform GetActorTransform() const { return CurrentActor ? CurrentActor->GetActorTransform() : FTransform(); }
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Selected Actor")
	FString GetActorName() const { return CurrentActor ? CurrentActor->GetName() : FString(); }
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selected Actor")
	void SetActorTransform(const FTransform NewTransform);

	bool HasAuthority() const { return GetOwner() ? GetOwner()->HasAuthority() : false; }
};
