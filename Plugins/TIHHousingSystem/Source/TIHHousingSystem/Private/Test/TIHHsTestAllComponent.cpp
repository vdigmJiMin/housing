// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/TIHHsTestAllComponent.h"

#include "TIHComponents/TIHHsTriggerComponent.h"


// Sets default values for this component's properties
UTIHHsTestAllComponent::UTIHHsTestAllComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTIHHsTestAllComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("UTIHHsTestAllComponent::BeginPlay"));
	// ...
	
}


// Called every frame
void UTIHHsTestAllComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int32 UTIHHsTestAllComponent::ProcessGeneration_Implementation(TArray<UActorComponent*>& others)
{
	// /Script/Engine.StaticMesh'/Engine/BasicShapes/Cone.Cone'
	UStaticMesh* loadedMesh = LoadObject<UStaticMesh>(
		this,TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
	if (loadedMesh)
	{
		SetStaticMesh(loadedMesh);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load mesh!"));
	}

	
	return 0;
}

void UTIHHsTestAllComponent::VisitCollectingTrigger(FTIHHsTriggerVisitor& triggerVisitor)
{
	triggerVisitor(TEXT("TestThema"),{},{},this)
	.SetThemaName(TEXT("TestThema"))
	.OnOverrideLocation()
	.OnOverrideRotation()
	.OnOverrideLocation()
	.AddOverrideInteractionFuncName(UTIHHsInteractionUtils::Name_ActionEventTap)
	.AddOverrideInteractionFuncName(UTIHHsInteractionUtils::Name_ActionEventDoubleTap)
	.AddOverrideGizmoTag(TEXT("gimochi"));
	
	
}

void UTIHHsTestAllComponent::VisitCollctingInputAction(FTIHHsInputActionVisitor& actionVisitor)
{
	actionVisitor.FunctionMap.Add({UTIHHsInteractionUtils::Name_ActionProcessHoldOngoing,this});
}

void UTIHHsTestAllComponent::ActionProcessHoldStart_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	UE_LOG(LogTemp, Log, TEXT("!!!!!!UTIHHsTestAllComponent::ActionProcessHoldStart: ActionName=%s"), *actionParamIn.ActionName);
}

void UTIHHsTestAllComponent::ActionProcessHoldOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	UE_LOG(LogTemp, Log, TEXT("!!!!!!UTIHHsTestAllComponent::ActionProcessHoldOngoing: ActionName=%s"), *actionParamIn.ActionName);
}

void UTIHHsTestAllComponent::ActionProcessHoldEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	UE_LOG(LogTemp, Log, TEXT("!!!!!!UTIHHsTestAllComponent::ActionProcessHoldEnd: ActionName=%s"), *actionParamIn.ActionName);
}

