// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TIHHsCore.h"
#include "Components/SceneComponent.h"
#include "TIHHsTestAllComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TIHHOUSINGSYSTEM_API UTIHHsTestAllComponent : public UStaticMeshComponent
	,public ITIHHsInteraction
	,public ITIHHsBaseObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTIHHsTestAllComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual int32 ProcessGeneration_Implementation(TArray<UActorComponent*>& others) override;
	virtual void VisitCollectingTrigger(FTIHHsTriggerVisitor& triggerVisitor) override;
	virtual void VisitCollctingInputAction(FTIHHsInputActionVisitor& actionVisitor) override;
	virtual void ActionProcessHoldStart_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoldOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoldEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
};
