// Fill out your copyright notice in the Description page of Project Settings.


#include "Cores/TIHHsCoreInterface.h"

const FName UTIHHsInteractionUtils::Name_ActionEventDown = TEXT("ActionEventDown");
const FName UTIHHsInteractionUtils::Name_ActionEventUp = TEXT("ActionEventUp");
const FName UTIHHsInteractionUtils::Name_ActionEventTap = TEXT("ActionEventTap");
const FName UTIHHsInteractionUtils::Name_ActionEventDoubleTap = TEXT("ActionEventDoubleTap");
const FName UTIHHsInteractionUtils::Name_ActionProcessHoldStart = TEXT("ActionProcessHoldStart");
const FName UTIHHsInteractionUtils::Name_ActionProcessHoldOngoing = TEXT("ActionProcessHoldOngoing");
const FName UTIHHsInteractionUtils::Name_ActionProcessHoldEnd = TEXT("ActionProcessHoldEnd");
const FName UTIHHsInteractionUtils::Name_ActionProcessMoveStart = TEXT("ActionProcessMoveStart");
const FName UTIHHsInteractionUtils::Name_ActionProcessMoveOngoing = TEXT("ActionProcessMoveOngoing");
const FName UTIHHsInteractionUtils::Name_ActionProcessMoveEnd = TEXT("ActionProcessMoveEnd");
const FName UTIHHsInteractionUtils::Name_ActionProcessMouseMove = TEXT("ActionProcessMouseMove");
const FName UTIHHsInteractionUtils::Name_ActionEventMoveAny = TEXT("ActionEventMoveAny");
const FName UTIHHsInteractionUtils::Name_ActionProcessHoverStart = TEXT("ActionProcessHoverStart");
const FName UTIHHsInteractionUtils::Name_ActionProcessHoverOngoing = TEXT("ActionProcessHoverOngoing");
const FName UTIHHsInteractionUtils::Name_ActionProcessHoverEnd = TEXT("ActionProcessHoverEnd");
const FName UTIHHsInteractionUtils::Name_CommandAction = TEXT("CommandAction");
const FName UTIHHsInteractionUtils::Name_InteractionInit = TEXT("InteractionInit");

void UTHsGlobalDependencyCaller::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}
