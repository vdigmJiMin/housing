// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHsCore.h"


void UTIHHsMouseSubSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}



// Sets default values
ATIHHsTestActor::ATIHHsTestActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATIHHsTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATIHHsTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

