// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameModeBase.h"
#include "TIHHsCore.generated.h"


UINTERFACE()
class UTIHHsCommonObject : public UInterface
{
	GENERATED_BODY()
};

class ITIHHsCommonObject
{
	GENERATED_BODY()

public:
	
};

UINTERFACE()
class UTIHHsComponentObject : public UInterface
{
	GENERATED_BODY()
};

class ITIHHsComponentObject
{
	GENERATED_BODY()

public:
	
};

UINTERFACE()
class UTIHHsActorObject : public UInterface
{
	GENERATED_BODY()
};

class ITIHHsActorObject
{
	GENERATED_BODY()
public:
	
};

UCLASS()
class UTIHHsBaseComponent : public USceneComponent, public ITIHHsCommonObject, public ITIHHsComponentObject
{
	GENERATED_BODY()
public:
	
};

UCLASS()
class ATIHHsBaseStuff : public AActor, public ITIHHsCommonObject, public ITIHHsActorObject
{
	GENERATED_BODY()
public:
	
};


UCLASS()
class ATIHHsStartPawn : public APawn
{
	GENERATED_BODY()
public:
};

UCLASS()
class ATIHHsGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
};

USTRUCT()
struct FTIHHsHousingNode
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 NodeID;

};

//	mouse system
USTRUCT()
struct FTIHHsHousingScene
{
	GENERATED_BODY()
	
};


UCLASS()
class UTIHHsMouseSubSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	
};


UCLASS()
class UTIHHsHousingSubSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
private:
	TArray<TObjectPtr<AActor>> mHousingActors;	//	소유권들을 위해서 
};



UCLASS()
class TIHHOUSINGCORE_API ATIHHsTestActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATIHHsTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
