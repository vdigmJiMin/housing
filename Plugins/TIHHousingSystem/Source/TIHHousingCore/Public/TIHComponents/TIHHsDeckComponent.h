// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TIHHsCore.h"
#include "Components/SceneComponent.h"
#include "TIHHsDeckComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TIHHOUSINGCORE_API UTIHHsDeckComponent : public USceneComponent,public ITIHHsBaseObject, public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTIHHsDeckComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
