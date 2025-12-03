// Fill out your copyright notice in the Description page of Project Settings.

#include "TIHHsCubeSpawner.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"

ATIHHsCubeSpawner::ATIHHsCubeSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	CubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	RootComponent = CubeMeshComponent;

	SetupCubeMesh();
}

void ATIHHsCubeSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ATIHHsCubeSpawner::SetupCubeMesh()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		CubeMeshComponent->SetStaticMesh(CubeMeshAsset.Object);
		CubeMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> DefaultMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
	if (DefaultMaterial.Succeeded())
	{
		CubeMeshComponent->SetMaterial(0, DefaultMaterial.Object);
	}

	CubeMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CubeMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
}

void ATIHHsCubeSpawner::ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	Super::ActionEventTap_Deprecated(actionParamIn);
	SpawnActor();
}

void ATIHHsCubeSpawner::SpawnActor()
{
	if (!ActorToSpawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATIHHsCubeSpawner: ActorToSpawn is not set!"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("ATIHHsCubeSpawner: Failed to get World!"));
		return;
	}

	FVector SpawnLocation = GetActorLocation() + SpawnOffset;
	FRotator SpawnRotation = GetActorRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AActor* SpawnedActor = World->SpawnActor<AActor>(ActorToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
	
	if (SpawnedActor)
	{
		UE_LOG(LogTemp, Log, TEXT("ATIHHsCubeSpawner: Successfully spawned actor at location: %s"), *SpawnLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ATIHHsCubeSpawner: Failed to spawn actor!"));
	}
}