// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TIHHsCommandSubsystem.generated.h"

/**
 * 
 */


struct FTIHHsBBTicket;

UCLASS()
class TIHHOUSINGCORE_API UTIHHsCommandSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;	//	UWorld::BeginPlay()에서 호출된다.
	
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UTIHHsCommandSubsystem, STATGROUP_Tickables);
	}

	
	
	int32 SendCommand(const FString& inCommandName, const TArray<FString>& inCommandArgs);
	
private:
	bool IsTickEnable() const;

	void test();
};
