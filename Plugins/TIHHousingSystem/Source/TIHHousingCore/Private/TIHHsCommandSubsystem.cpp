// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHsCommandSubsystem.h"

#include "TIHBlackBoardSubsystem.h"
#include "TIHHsBlackboard.inl"

void UTIHHsCommandSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

bool UTIHHsCommandSubsystem::IsTickable() const
{
	return Super::IsTickable();
}

bool UTIHHsCommandSubsystem::IsTickableInEditor() const
{
	return Super::IsTickableInEditor();
}

bool UTIHHsCommandSubsystem::IsTickableWhenPaused() const
{
	return Super::IsTickableWhenPaused();
}

void UTIHHsCommandSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}



void UTIHHsCommandSubsystem::test()
{
	TArray<FString> arg = {TEXT("test"),TEXT("test2")}; 
	

	
}
