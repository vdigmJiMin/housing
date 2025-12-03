// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHousingCore/Public/TIHComponents/TIHHsBaseRootComponent.h"


// Sets default values for this component's properties
UTIHHsBaseRootComponent::UTIHHsBaseRootComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTIHHsBaseRootComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTIHHsBaseRootComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// ...
}

int32 UTIHHsBaseRootComponent::ProcessGeneration_Implementation(TArray<UActorComponent*>& others)
{
	TArray<USceneComponent*> childrenComponents;
	GetChildrenComponents(false,childrenComponents);
	int32 processedChildren = 0;
	for (USceneComponent* child :childrenComponents)
	{
		if (
			child != nullptr
			&& child->GetClass()->ImplementsInterface(UTIHHsBaseObject::StaticClass())
			)
		{
			++processedChildren;
			Execute_ProcessGeneration(child,others);
		}
	}
	
	return processedChildren;
}

