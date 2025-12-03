// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeComponent.h"
#include "Cores/TIHHsCoreInterface.h"

 #include "TIHHsStateTreeComponent.generated.h"


 UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
 class TIHHOUSINGCORE_API UTIHHsStateTreeComponent : public UStateTreeComponent , public ITIHHsBaseObject
 {
 	GENERATED_BODY()

 public:
 	// Sets default values for this component's properties
 	UTIHHsStateTreeComponent();

 	/*
 	 *	IsResourceLocked
 	 *	ForceUnlockResource
 	 *	ClearResourceLock
 	 *	LockResource
 	 *	HandleMessage
 	 *	DescribeSelfToVisLog
 	 *	OnRegister
 	 * 
 	 */
 protected:
 	// Called when the game starts
 	virtual void BeginPlay() override;

 public:
 	// Called every frame
 	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
 	                           FActorComponentTickFunction* ThisTickFunction) override;
 	virtual void DescribeSelfToVisLog(FVisualLogEntry* Snapshot) const override;
 	virtual void LockResource(EAIRequestPriority::Type LockSource) override;
 	virtual void ClearResourceLock(EAIRequestPriority::Type LockSource) override;
 	virtual void ForceUnlockResource() override;
 	virtual bool IsResourceLocked() const override;
 	virtual void HandleMessage(const FAIMessage& Message) override;
 	virtual void OnRegister() override;
 };
