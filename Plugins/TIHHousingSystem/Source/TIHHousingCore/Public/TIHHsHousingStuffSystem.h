// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TIHHsCore.h"
#include "TIHHsHousingStuffSystem.generated.h"

/**
 * 
 */
class UTIHHsHousingStuffSystem;
UCLASS()
class ATIHHsHousingStuffSystemRootActor : public AActor
{
	GENERATED_BODY()
	friend class UTIHHsHousingStuffSystem;
public:

private:
	TArray<AActor*> mManagedActors;
	TArray<AActor*> mSystemActors;
};

USTRUCT()
struct FTIHHsStuffNode
{
	GENERATED_BODY()

	FTransform RelateTransform;

	TWeakPtr<FTIHHsStuffNode> ParentNode;
};

UCLASS()
class TIHHOUSINGCORE_API UTIHHsHousingStuffSystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTIHHsAfterCrawlingWorld);
	FTIHHsAfterCrawlingWorld mAfterCrawlingWorld;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	FTIHHsStuffNode& CreateHsStuffNode();
	
private:
	void CrawlingWorld();
	bool CheckManagedTags(AActor* actor,int32& findIdx);
	
	TObjectPtr<ATIHHsHousingStuffSystemRootActor> mStuffRootActor;

	TArray<AActor*> mOtherActors;
	TArray<AActor*> mStatEnvironmentActors;
	
	TArray<ITIHHsCommonObject*> mCommonObjects;	
	TArray<ITIHHsActorObject*> mActorObjects;//reserve capacity
	
	UWorld* mWorld;
	//	for generating the housing stuff
	TMap<FName,UClass*> mGenSourceClasses;
	TMap<UClass*,FName> mGenSourceClassesReverse;

	TSet<FName> mManagedActorTags;
};
/*
 *	컴포넌트 테스트
 *		모든것의 기본은 그리드임.
 *		그 그리드가 마우스 클릭 호버 드래그등이 모두 되어야함.
 */


