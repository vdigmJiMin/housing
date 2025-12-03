// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TIHHsCoreInterface.h"
#include "TIHHsCoreStructures.h"
#include "TIHHsCoreEnums.h"
#include "TIHHsCoreClasses.generated.h"

/**
 * 
 */
UCLASS()
class TIHHOUSINGCORE_API UTIHHsCoreClasses : public UObject
{
	GENERATED_BODY()
};




USTRUCT()
struct FTIHHsProbeInitData
{
	GENERATED_BODY()

	UPROPERTY()
	ETIHHsFunctionType FunctionType;

	UPROPERTY()
	TArray<UObject*> Objects;
	
	TFunction<void(UObject*)> TFunctionFunc;

	FTIHHsProbeInitData()
		: FunctionType(ETIHHsFunctionType::EInvalid)
		, TFunctionFunc(nullptr)
	{
	}
	FTIHHsProbeInitData(const TFunction<void(UObject*)>& InTFunctionFunc)
		: TFunctionFunc(InTFunctionFunc)
	{
		FunctionType = ETIHHsFunctionType::ETFunction;
	}
	
	bool IsValid() const
	{
		bool result = false;
		switch (FunctionType) {
		case ETIHHsFunctionType::EInvalid:
			break;
		case ETIHHsFunctionType::EJustOrder:
			break;
		case ETIHHsFunctionType::EJustOrderDebugLog:
			break;
		case ETIHHsFunctionType::EDynamicDelegate:
			break;
		case ETIHHsFunctionType::EUObject:
			break;
		case ETIHHsFunctionType::ETFunction:
			if (TFunctionFunc != nullptr)
			{
				result = true;
			}
			break;
		case ETIHHsFunctionType::EDelegate:
			break;
		case ETIHHsFunctionType::EStaticFunction:
			break;
		case ETIHHsFunctionType::EMemberFunction:
			break;
		}
		return result;
	}

	void Execute(UObject* paramIn)
	{
		if (not IsValid())
		{
			return;
		}
		//(*this)(paramIn);
		this->operator()(paramIn);
	}

	void operator()(UObject* paramIn) const
	{
		switch (FunctionType) {
		case ETIHHsFunctionType::EInvalid:
			break;
		case ETIHHsFunctionType::EJustOrder:
			break;
		case ETIHHsFunctionType::EJustOrderDebugLog:
			break;
		case ETIHHsFunctionType::EDynamicDelegate:
			break;
		case ETIHHsFunctionType::EUObject:
			break;
		case ETIHHsFunctionType::ETFunction:
			TFunctionFunc(paramIn);
			break;
		case ETIHHsFunctionType::EDelegate:
			break;
		case ETIHHsFunctionType::EStaticFunction:
			break;
		case ETIHHsFunctionType::EMemberFunction:
			break;
		}
	}
};

UCLASS(BlueprintType,Abstract)
class UTIHHsProbeBase : public UObject
{
	GENERATED_BODY()
public:
	virtual void Init(FTIHHsProbeInitData initData)
	{
		initData.Execute(this);
	}

	virtual void StartProbe(){}
	
	virtual bool IsProbeRunning(){return false;}
	virtual void UpdateProbe(float deltaTime){}
	
	virtual void StopProbe(){}
};

UCLASS()
class UTIHHsProbeTraceMouse2 final : public UTIHHsProbeBase
{
	GENERATED_BODY()
public:
	virtual void Init(FTIHHsProbeInitData initData) final
	{
		initData.Execute(this);
	}
	virtual void StartProbe() override final;
	virtual bool IsProbeRunning() override final;
	virtual void UpdateProbe(float deltaTime) override final;
	virtual void StopProbe() override final;

	void SetWorldContext(UWorld* worldContext)
	{
		mWorldContext = worldContext;
	}
	void SetTimerManager(FTimerManager* timerManager)
	{
		mTimerMgr = timerManager;
	}
	void SetTraceTarget(TScriptInterface<ITIHHsInteraction> target)
	{
		mTraceTarget = target;
	}
private:
	UWorld* mWorldContext = nullptr;	//	월드 컨텍스트, 트레이스가 시작될 때 월드 컨텍스트를 저장함
	FTimerManager* mTimerMgr = nullptr;	//	타이머 매니저, 트레이스 중일 때 타이머를 사용하여 트랜스폼을 업데이트함
	TScriptInterface<ITIHHsInteraction> mTraceTarget;	//	트레이스 대상 인터페이스, 트레이스가 시작될 때 대상 인터페이스를 저장함
	TArray<TScriptInterface<ITIHHsInteraction>> mTraceTargets;	//	집단으로 움직일때 사용하는거.
	
	bool mIsTracing = false;	//	트레이스 중인지 여부
	FVector2D mLastMousePosition;	//	마지막 마우스 위치, 마우스가 움직일 때마다 업데이트됨
};

UCLASS()
class UTHsFilter : public UObject
{
	GENERATED_BODY()

public:
	void InitFilter(std::initializer_list<FGameplayTag> filterList)
	{
		for (const FGameplayTag& tag : filterList)
		{
			mGameplayFilterTag.AddTag(tag);
		}
	}
	void InitFilter(const FGameplayTagContainer& tags)
	{
		mGameplayFilterTag = tags;
	}
	
	bool IsMatch(TScriptInterface<ITIHHsInteraction> target) const
	{
		if (not target)
		{
			return false;
		}
		if (mGameplayFilterTag.Num() == 0)
		{
			return true;
		}
		FGameplayTagContainer targetTags = target->GetInteractionTags();
		return targetTags.HasAny(mGameplayFilterTag);
	}
	
	void AddFilterTag(const FGameplayTag& tag)
	{
		mGameplayFilterTag.AddTag(tag);
	}
	

private:
	FGameplayTagContainer mGameplayFilterTag;
};

UCLASS(BlueprintType)
class TIHHOUSINGCORE_API UTHsDependencyBase : public UObject
{
	GENERATED_BODY()

public:
	void SetWorldContext(UWorld* worldContext)
	{
		mWorldContext = worldContext;
	}
	void SetActionSystem(class UTHsEnhancedActionSystem* actionSystem)
	{
		mEnhancedActionSystem = actionSystem;
	}
	void SetGizmoManager(class UTHsGizmoManagerWrapper* gizmoManager)
	{
		mGizmoManager = gizmoManager;
	}
	UTHsGizmoManagerWrapper* GetGizmoManager() const{return mGizmoManager;}
	UTHsEnhancedActionSystem* GetActionSystem() const;
	UWorld* GetWorld() const
	{
		return mWorldContext;
	}
	bool IsValidDependency() const
	{
		return mWorldContext != nullptr && mEnhancedActionSystem != nullptr && mGizmoManager != nullptr;
	}
	
protected:
	class UTHsGizmoManagerWrapper* mGizmoManager = nullptr;
	class UTHsEnhancedActionSystem* mEnhancedActionSystem = nullptr;
	UWorld* mWorldContext = nullptr;
};
namespace TIHUtils
{
	inline UWorld* GetTHsWorldSimple()
	{
		UWorld* world = nullptr;
		if (GEngine && GEngine->GetWorldContexts().Num() > 0)
		{
			world = GEngine->GetWorldContexts()[0].World();
		}
		return world;
	}
	
}

/*
if (mTraceMouseProbe == nullptr)
	{
		mTraceMouseProbe = NewObject<UTIHHsProbeTraceMouse2>();
		mTraceMouseProbe->Init({
			[targetActor = this](UObject* probe)->void
			{
				if (probe)
				{
					if (UTIHHsProbeTraceMouse2* mouseTrace = Cast<UTIHHsProbeTraceMouse2>(probe))
					{
						UWorld* world = targetActor->GetWorld();
						if (world == nullptr)
						{
							UE_LOG(LogTemp, Error, TEXT("UTIHHsProbeTraceMouse2::Init: World is null."));
							return;
						}
						FTimerManager* timerManager = &world->GetTimerManager();
						
						mouseTrace->SetWorldContext(world);
						mouseTrace->SetTimerManager(timerManager);
						mouseTrace->SetTraceTarget(targetActor);
					}
				}
				
			}});
	}
	
	mTraceMouseProbe->StartProbe();
 */