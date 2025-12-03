// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TIHHsCore.h"
#include "Components/SceneComponent.h"
#include "TIHHsBaseRootComponent.generated.h"

//TIHHsBaseSceneComponent
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TIHHOUSINGCORE_API UTIHHsBaseRootComponent : public USceneComponent , public ITIHHsBaseObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTIHHsBaseRootComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual int32 ProcessGeneration_Implementation(TArray<UActorComponent*>& others) override;
};
