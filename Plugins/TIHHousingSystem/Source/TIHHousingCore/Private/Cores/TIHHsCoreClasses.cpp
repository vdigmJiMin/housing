// Fill out your copyright notice in the Description page of Project Settings.


#include "Cores/TIHHsCoreClasses.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void UTIHHsProbeTraceMouse2::StartProbe()
{
	UTIHHsProbeTraceMouse2 a;
	a.Init({[&](UObject* target)->void{}});

	
}

bool UTIHHsProbeTraceMouse2::IsProbeRunning()
{
	return false;
}

void UTIHHsProbeTraceMouse2::UpdateProbe(float deltaTime)
{
	Super::UpdateProbe(deltaTime);
}

void UTIHHsProbeTraceMouse2::StopProbe()
{
	Super::StopProbe();
}

class UTHsEnhancedActionSystem* UTHsDependencyBase::GetActionSystem() const
{
	return mEnhancedActionSystem;
}


