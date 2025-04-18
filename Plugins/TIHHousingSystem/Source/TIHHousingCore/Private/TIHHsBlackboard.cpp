// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHsBlackboard.h"

#include "TIHBlackBoardSubsystem.h"

void UTIHHsBlackBoardSession::PostCreateSession()
{
	UE_LOG(LogTemp, Warning, TEXT("PostCreateSession %s"), *GetBlackBoardName());
}

void UTIHHsBlackBoardSession::PreDestroySession()
{
	UE_LOG(LogTemp, Warning, TEXT("PreDestroySession %s"), *GetBlackBoardName());
}

void UTIHHsBlackBoardSession::OnTimerExpired()
{
	UTIHHsBlackBoardSubsystem* bbSystem =  UTIHHsBlackBoardSubsystem::GetBBSystem();
	bbSystem->SessionTimerExpired(this);
}
