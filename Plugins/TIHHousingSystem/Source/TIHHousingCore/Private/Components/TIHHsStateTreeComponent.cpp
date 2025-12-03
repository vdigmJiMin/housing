// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHousingCore/Public/TIHComponents/TIHHsStateTreeComponent.h"

#include "BehaviorTree/BlackboardComponent.h"


// Sets default values for this component's properties
UTIHHsStateTreeComponent::UTIHHsStateTreeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTIHHsStateTreeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//USceneComponent* comp;
	//USceneComponent* root;
	//comp->SetupAttachment(root);
}


// Called every frame
void UTIHHsStateTreeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTIHHsStateTreeComponent::DescribeSelfToVisLog(FVisualLogEntry* Snapshot) const
{
	//const static UEnum* PriorityEnum = StaticEnum<EAIRequestPriority::Type>();
//
	//if (!IsValid(this))
	//{
	//	return;
	//}
//
	//FVisualLogStatusCategory StatusCategory;
	//StatusCategory.Category = FString::Printf(TEXT("Resource lock: %s"), *ResourceLock.GetLockPriorityName());
	//for (int32 LockLevel = 0; LockLevel < int32(EAIRequestPriority::MAX); ++LockLevel)
	//{
	//	StatusCategory.Add(*PriorityEnum->GetNameStringByValue(LockLevel), ResourceLock.IsLockedBy(static_cast<EAIRequestPriority::Type>(LockLevel)) ? TEXT("Locked") : TEXT("Unlocked"));
	//}
	//Snapshot->Status.Add(StatusCategory);
//
	//if (BlackboardComp)
	//{
	//	BlackboardComp->DescribeSelfToVisLog(Snapshot);
	//}
	UStateTreeComponent::DescribeSelfToVisLog(Snapshot);
}

void UTIHHsStateTreeComponent::LockResource(EAIRequestPriority::Type LockSource)
{
	// const bool bWasLocked = ResourceLock.IsLocked();
	// ResourceLock.SetLock(LockSource);
	// if (bWasLocked == false)
	// {
	// 	PauseLogic(FString::Printf(TEXT("Locking Resource with source %s"), *ResourceLock.GetLockPriorityName()));
	// }
	UStateTreeComponent::LockResource(LockSource);
}

void UTIHHsStateTreeComponent::ClearResourceLock(EAIRequestPriority::Type LockSource)
{
	// ResourceLock.ClearLock(LockSource);
	//
	// if (ResourceLock.IsLocked() == false)
	// {
	// 	ResumeLogic(TEXT("unlocked"));
	// }
	UStateTreeComponent::ClearResourceLock(LockSource);
}

void UTIHHsStateTreeComponent::ForceUnlockResource()
{
	//ResourceLock.ForceClearAllLocks();
	//ResumeLogic(TEXT("unlocked: forced"));
	UStateTreeComponent::ForceUnlockResource();
	
}

bool UTIHHsStateTreeComponent::IsResourceLocked() const
{
	return UStateTreeComponent::IsResourceLocked();
}

void UTIHHsStateTreeComponent::HandleMessage(const FAIMessage& Message)
{
	//MessagesToProcess.Add(Message);
	UStateTreeComponent::HandleMessage(Message);
}

void UTIHHsStateTreeComponent::OnRegister()
{
	UStateTreeComponent::OnRegister();
}

