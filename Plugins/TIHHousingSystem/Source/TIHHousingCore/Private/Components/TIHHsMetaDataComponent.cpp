// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHousingCore/Public/TIHComponents/TIHHsMetaDataComponent.h"


// Sets default values for this component's properties
UTIHHsMetaDataComponent::UTIHHsMetaDataComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTIHHsMetaDataComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTIHHsMetaDataComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTIHHsMetaDataComponent::AnalysisBaseObject(AActor* target)
{
	const int32 baseObj = 0x00000001;	//	1 << 0
	const int32 interactionObj = 0x00000002;	//	1 << 1
	const int32 hasPlacementComp = 0x00000004;	//	1 << 2
	const int32 hasTriggerComp = 0x00000008;	//	1 << 3
	const int32 hasMetaObjComp = 0x00000010;	//	1 << 4
		
	int32 objectType = 0;

	const TSet<UActorComponent*>& components = target->GetComponents();	
	for (UActorComponent* comp :components)
	{
		//TScriptInterface<ITIHHsBaseObject> baseObj(comp);
		//baseObj->Execute_GetMetaData();
	}
	
}

