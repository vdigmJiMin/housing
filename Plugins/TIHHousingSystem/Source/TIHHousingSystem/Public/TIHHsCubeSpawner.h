// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TIHHsStuffBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "TIHHsCubeSpawner.generated.h"

UCLASS()
class TIHHOUSINGSYSTEM_API ATIHHsCubeSpawner : public ATIHHsStuffBase
{
	GENERATED_BODY()

public:
	ATIHHsCubeSpawner();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CubeMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	FVector SpawnOffset = FVector(0.0f, 0.0f, 100.0f);

public:
	virtual void ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnActor();

private:
	void SetupCubeMesh();
};