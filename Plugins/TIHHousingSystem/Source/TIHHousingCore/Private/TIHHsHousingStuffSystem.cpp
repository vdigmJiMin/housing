// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHsHousingStuffSystem.h"

#include "DSP/AudioDebuggingUtilities.h"
#include "Kismet/GameplayStatics.h"

void UTIHHsHousingStuffSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTIHHsHousingStuffSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	mWorld = &InWorld;
	FTimerHandle startTIHWorld;
	mWorld->GetTimerManager().SetTimer(startTIHWorld,this,&UTIHHsHousingStuffSystem::CrawlingWorld,0.1f,false);
}

void UTIHHsHousingStuffSystem::CrawlingWorld()
{
	TArray<AActor*> allActors;
	UGameplayStatics::GetAllActorsOfClass(mWorld,AActor::StaticClass(),allActors);

	if (not allActors.IsEmpty())
	{
		int32 lastIdx = allActors.Num()-1;
		int32 searchIdx = 0;
		for (int32 i = 0; i < allActors.Num(); ++i)
		{
			AActor* topActor = allActors.Top();
			if (topActor == nullptr || lastIdx <= 0)
			{
				break;
			}
			if (topActor->Implements<UTIHHsActorObject>())
			{
				mActorObjects.Add(Cast<ITIHHsActorObject>(topActor));
				mStuffRootActor->mManagedActors.Add(topActor);
			}
			else if (topActor->Implements<UTIHHsCommonObject>())
			{
				mCommonObjects.Add(Cast<ITIHHsCommonObject>(topActor));
				mStuffRootActor->mManagedActors.Add(topActor);
			}
			else
			{
				int32 idx = INDEX_NONE;
				if (CheckManagedTags(topActor,idx))
				{
					allActors[0] = nullptr;
					allActors.Swap(0,lastIdx);
					--lastIdx;
				}
			}
		}
	}
	mAfterCrawlingWorld.Broadcast();
}

bool UTIHHsHousingStuffSystem::CheckManagedTags(AActor* actor,int32& findIdx) 
{
	if (actor == nullptr)
	{
		return false;
	}
	
	for (int32 i =0; i < actor->Tags.Num(); ++i)
	{
		if (mManagedActorTags.Contains(actor->Tags[i]))
		{
			findIdx = i;
			bool specialTag =false;
			if (actor->Tags[i] == TEXT("system"))
			{
				mStuffRootActor->mSystemActors.Add(actor);
				specialTag = true;
			}
			if (actor->Tags[i] == TEXT("environment"))
			{
				mStatEnvironmentActors.Add(actor);
				specialTag = true;
			}
			if (actor->Tags[i] == TEXT("don-touch"))
			{
				mOtherActors.Add(actor);
				specialTag = true;
			}
			if (not specialTag)
			{
				mOtherActors.Add(actor);
			}
			//else
			//{
			//	//	신경쓸 필요없는것들임.
			//}
			return true;
		}
	}
	return false;
}
