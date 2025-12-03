// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TIHHsCoreEnums.h"
#include "TIHHsCoreStructures.h"
#include "GameplayTagContainer.h"
#include "GameplayTags.h"
#include "InstancedStruct.h"
#include "TIHHsCoreInterface.generated.h"


class ITHsActionPointAPI;
class UTHsActionPoint;

UENUM(Blueprintable,meta = (Bitflags))
enum class ETHsInteractionAgentType : uint8
{
	ENone	= 0		UMETA(DisplayName="None",Hidden),
	EActor	= 1		UMETA(DisplayName="Actor"),
	EComponent = 2	UMETA(DisplayName="Component"),
	ECondition = 4	UMETA(DisplayName="Condition"),
};

USTRUCT()
struct FTHsActionPointAnyParam
{
	GENERATED_BODY()

	UPROPERTY()
	ETHsActionFunctionType FunctionType = ETHsActionFunctionType::EZCustom0;
	UPROPERTY()
	class ATHsActionCursor* ActionCursor = nullptr;
	UPROPERTY()
	UTHsActionPoint* ActionPoint = nullptr;
	UPROPERTY()
	float DeltaTime = 0.0f;
	UPROPERTY()
	UObject* UserParam = nullptr;

	FTHsActionPointAnyParam() = default;
	FTHsActionPointAnyParam(const FTHsActionPointAnyParam& other) = default;
	FTHsActionPointAnyParam(FTHsActionPointAnyParam&& other) noexcept = default;
	FTHsActionPointAnyParam& operator=(const FTHsActionPointAnyParam& other) = default;
	FTHsActionPointAnyParam& operator=(FTHsActionPointAnyParam&& other) noexcept = default;
	FTHsActionPointAnyParam(ETHsActionFunctionType functionType)
		: FunctionType(functionType)
	{
	}
	FTHsActionPointAnyParam(ETHsActionFunctionType functionType,class ATHsActionCursor* actionCursor)
		: FunctionType(functionType), ActionCursor(actionCursor)
	{
	}
	FTHsActionPointAnyParam(ETHsActionFunctionType functionType,UTHsActionPoint* actionPoint )
		: FunctionType(functionType), ActionPoint(actionPoint)
	{
	}
	FTHsActionPointAnyParam(ETHsActionFunctionType functionType,float deltaTime  )
		: FunctionType(functionType), DeltaTime(deltaTime)
	{
	}

	void Clear()
	{
		FunctionType = ETHsActionFunctionType::EZCustom0;
		ActionCursor = nullptr;
		ActionPoint = nullptr;
		DeltaTime = 0.0f;
	}
};


DECLARE_MULTICAST_DELEGATE_OneParam(
	FTIHActionGestureDelegate,      // 이름
	const FTIHHsActionParamIn&		// 매개변수: 액션 파라미터
);

class ITIHHsBaseObject;

UCLASS()
class UTIHHsBaseObjectLinkedList : public UObject
{
	GENERATED_BODY()

public:
	
};

USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsDeckSurfaceData
{
	GENERATED_BODY()
};
USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsAttachmentStepQuery
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Query;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Result;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PrevQueryIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NextQueryIndex = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SelfQueryIdx = -1;	//	자기 자신을 가리키는 인덱스, 자기 자신이 없으면 -1

	FTIHHsAttachmentStepQuery() = default;
	FTIHHsAttachmentStepQuery(FName InQuery, int32 InResult = 0)
		: Query(InQuery), Result(InResult) {}
	FTIHHsAttachmentStepQuery(const FTIHHsAttachmentStepQuery& other)
		: Query(other.Query), Result(other.Result) {}
	FTIHHsAttachmentStepQuery(FTIHHsAttachmentStepQuery&& other) noexcept
		: Query(MoveTemp(other.Query)), Result(other.Result) {}
	FTIHHsAttachmentStepQuery& operator=(const FTIHHsAttachmentStepQuery& other)
	{
		if (this != &other)
		{
			Query = other.Query;
			Result = other.Result;
		}
		return *this;
	}
	FTIHHsAttachmentStepQuery& operator=(FTIHHsAttachmentStepQuery&& other) noexcept
	{
		if (this != &other)
		{
			Query = MoveTemp(other.Query);
			Result = other.Result;
		}
		return *this;
	}
};


USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsAttachmentStepQueryList
{
	GENERATED_BODY()


	struct FTIHHsAttachmentStepQueryIterator
	{
		using value_type = FTIHHsAttachmentStepQuery;
		using reference = FTIHHsAttachmentStepQuery&;
		using pointer = FTIHHsAttachmentStepQuery*;

		TArray<value_type>* QueriesPtr;
		int32 CurrentIndex;

		FTIHHsAttachmentStepQueryIterator(TArray<value_type>* InQueriesPtr, int32 InCurrentIndex)
			: QueriesPtr(InQueriesPtr), CurrentIndex(InCurrentIndex) {}

		reference operator*() const
		{
			return (*QueriesPtr)[CurrentIndex];
		}
		pointer operator->() const
		{
			return &(*QueriesPtr)[CurrentIndex];
		}
		FTIHHsAttachmentStepQueryIterator& operator++()
		{
			if (QueriesPtr && QueriesPtr->IsValidIndex(CurrentIndex))
			{
				CurrentIndex = (*QueriesPtr)[CurrentIndex].NextQueryIndex;
			}
			else
			{
				CurrentIndex = -1; // End of iteration
			}
			return *this;
		}
		bool operator!=(const FTIHHsAttachmentStepQueryIterator& Other) const
		{
			return CurrentIndex != Other.CurrentIndex || QueriesPtr != Other.QueriesPtr;
		}
	};
	FTIHHsAttachmentStepQueryIterator begin()
	{
		return FTIHHsAttachmentStepQueryIterator(&Queries, RootQueryIndex);
	}
	FTIHHsAttachmentStepQueryIterator end()
	{
		return FTIHHsAttachmentStepQueryIterator(&Queries, -1);
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTIHHsAttachmentStepQuery> Queries;

	TMap<FName, int32> QueryTable;

	int32 RootQueryIndex = -1;
	int32 LastQueryIndex = -1;

	//	TODO: 이름앞에 둘지. 뒤에 둘지 같은거 만들어야하고 반복자 만들어야함.
	
	void PushBack(const FTIHHsAttachmentStepQuery& query)
	{
		if (QueryTable.Contains(query.Query))
		{
			FTIHHsAttachmentStepQuery& existingQuery = Queries[QueryTable[query.Query]];
			int32 existingQueryPrev = existingQuery.PrevQueryIndex;
			int32 existingQueryNext = existingQuery.NextQueryIndex;

			if (Queries.IsValidIndex(existingQueryPrev) )
			{
				Queries[existingQueryPrev].NextQueryIndex = existingQueryNext;
			}
			if (Queries.IsValidIndex(existingQueryNext) )
			{
				Queries[existingQueryNext].PrevQueryIndex = existingQueryPrev;
			}
			if (Queries.IsValidIndex(LastQueryIndex) && existingQuery.SelfQueryIdx != LastQueryIndex)
			{
				Queries[LastQueryIndex].NextQueryIndex = existingQuery.SelfQueryIdx;
				existingQuery.PrevQueryIndex = Queries[LastQueryIndex].SelfQueryIdx;
			}
			existingQuery.NextQueryIndex = -1;
			LastQueryIndex = existingQuery.SelfQueryIdx;
			
		}
		else
		{
			FTIHHsAttachmentStepQuery& newQuery = AddNewQuery(query);
			if (RootQueryIndex < 0)
			{
				RootQueryIndex = newQuery.SelfQueryIdx;
			}
			else
			{
				Queries[LastQueryIndex].NextQueryIndex = newQuery.SelfQueryIdx;
				newQuery.PrevQueryIndex = Queries[LastQueryIndex].SelfQueryIdx;
			}
			LastQueryIndex = newQuery.SelfQueryIdx;
		}
	}
	
	void PushFront(const FTIHHsAttachmentStepQuery& query)
	{
		if (QueryTable.Contains(query.Query))
		{
			FTIHHsAttachmentStepQuery& existingQuery = Queries[QueryTable[query.Query]];
			int32 existingQueryPrev = existingQuery.PrevQueryIndex;
			int32 existingQueryNext = existingQuery.NextQueryIndex;

			if (Queries.IsValidIndex(existingQueryPrev) )
			{
				Queries[existingQueryPrev].NextQueryIndex = existingQueryNext;
			}
			if (Queries.IsValidIndex(existingQueryNext) )
			{
				Queries[existingQueryNext].PrevQueryIndex = existingQueryPrev;
			}
			
			if (Queries.IsValidIndex(RootQueryIndex) && existingQuery.SelfQueryIdx != RootQueryIndex)
			{
				Queries[RootQueryIndex].NextQueryIndex = existingQuery.SelfQueryIdx;
				existingQuery.PrevQueryIndex = Queries[RootQueryIndex].SelfQueryIdx;
			}
			existingQuery.PrevQueryIndex = -1;
			RootQueryIndex = existingQuery.SelfQueryIdx;
			
		}
		else
		{
			FTIHHsAttachmentStepQuery& newQuery = AddNewQuery(query);
			if (RootQueryIndex < 0)
			{
				LastQueryIndex = newQuery.SelfQueryIdx;
			}
			else
			{
				Queries[RootQueryIndex].PrevQueryIndex = newQuery.SelfQueryIdx;
				newQuery.NextQueryIndex = Queries[RootQueryIndex].SelfQueryIdx;
			}
			RootQueryIndex = newQuery.SelfQueryIdx;
		}
	}
	
	FTIHHsAttachmentStepQuery* FindQuery(const FName& queryName)
	{
		if (QueryTable.Contains(queryName))
		{
			return &Queries[QueryTable[queryName]];
		}
		return nullptr;
	}
	
	void InsertBefore(const FName& queryName, const FTIHHsAttachmentStepQuery& inNewQuery)
	{
		if (QueryTable.Contains(queryName))
		{
			FTIHHsAttachmentStepQuery& newQuery = AddNewQuery(inNewQuery);
			
			int32 targetIdx = QueryTable[queryName];
			FTIHHsAttachmentStepQuery& targetQuery = Queries[targetIdx];

			if (Queries.IsValidIndex( targetQuery.PrevQueryIndex))
			{
				FTIHHsAttachmentStepQuery& prevQuery = Queries[targetQuery.PrevQueryIndex];
				prevQuery.NextQueryIndex = newQuery.SelfQueryIdx;
				newQuery.PrevQueryIndex = prevQuery.SelfQueryIdx;
			}
			targetQuery.PrevQueryIndex = newQuery.SelfQueryIdx;
			newQuery.NextQueryIndex = targetQuery.SelfQueryIdx;
			
			if (GetRootQuery())
			{
				RootQueryIndex = newQuery.SelfQueryIdx;
			}
		}
		else
		{
			PushFront(inNewQuery);
		}
	}
	void InsertAfter(const FName& queryName, const FTIHHsAttachmentStepQuery& inNewQuery)
	{
		if (QueryTable.Contains(queryName))
		{
			FTIHHsAttachmentStepQuery& newQuery = AddNewQuery(inNewQuery);
			
			int32 targetIdx = QueryTable[queryName];
			FTIHHsAttachmentStepQuery& targetQuery = Queries[targetIdx];

			if (Queries.IsValidIndex(targetQuery.NextQueryIndex))
			{
				FTIHHsAttachmentStepQuery& nextQuery = Queries[targetQuery.NextQueryIndex];
				nextQuery.PrevQueryIndex = newQuery.SelfQueryIdx;
				newQuery.NextQueryIndex = nextQuery.SelfQueryIdx;
			}
			targetQuery.NextQueryIndex = newQuery.SelfQueryIdx;
			newQuery.PrevQueryIndex = targetQuery.SelfQueryIdx;
			
			if (GetLastQuery())
			{
				LastQueryIndex = newQuery.SelfQueryIdx;
			}
		}
		else
		{
			PushBack(inNewQuery);
		}
	}
	
	FTIHHsAttachmentStepQuery& AddNewQuery(const FTIHHsAttachmentStepQuery& newQuery)
	{
		int32 newQueryIdx = Queries.Add(newQuery);
		Queries[newQueryIdx].SelfQueryIdx = newQueryIdx;
		Queries[newQueryIdx].NextQueryIndex = -1;
		Queries[newQueryIdx].PrevQueryIndex = -1;
		QueryTable.Add(newQuery.Query, newQueryIdx);
		return Queries[newQueryIdx];
	}
	
	FTIHHsAttachmentStepQuery* GetRootQuery()
	{
		FTIHHsAttachmentStepQuery* rootQuery = nullptr;
		if (Queries.IsValidIndex(RootQueryIndex))
		{
			rootQuery = &Queries[RootQueryIndex];
		}
		return rootQuery;
	}
	
	FTIHHsAttachmentStepQuery* GetLastQuery()
	{
		FTIHHsAttachmentStepQuery* lastQuery = nullptr;
		if (Queries.IsValidIndex(LastQueryIndex))
		{
			lastQuery = &Queries[LastQueryIndex];
		}
		return lastQuery;
	}
	
	void Clear()
	{
		Queries.Empty();
		QueryTable.Empty();
		RootQueryIndex = -1;
		LastQueryIndex = -1;
	}
};
USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsTriggerVisitorNode
{
	GENERATED_BODY()

	UPROPERTY()
	USceneComponent* AttachedComponent = nullptr;	//	nullptr이면 그냥 루트에 붙여라.
	
	UPROPERTY()
	FTIHHsGizmoTriggerSpawnBody TriggerGizmoSpawnPrefab;	//	트리거 기즈모 스폰 프리팹

	UPROPERTY()
	FTIHHsGizmoTriggerSpawnRule TriggerGizmoSpawnRule;
};


USTRUCT()
struct TIHHOUSINGCORE_API FTIHHsInputActionVisitor
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FName, TScriptInterface<ITIHHsInteraction>> FunctionMap;	//	함수 이름과 함수 객체를 매핑하는 맵

	void operator()(const FName& actionName,TScriptInterface<ITIHHsInteraction> actionObject)
	{
		if (FunctionMap.Contains(actionName))
		{
			// 이미 존재하는 액션 이름이면 덮어쓰기
			FunctionMap[actionName] = actionObject;
		}
		else
		{
			// 새로운 액션 이름이면 추가
			FunctionMap.Add(actionName, actionObject);
		}
	}
};
USTRUCT()
struct TIHHOUSINGCORE_API FTIHHsMetaReport
{
	GENERATED_BODY()

	UPROPERTY()
	bool HasBaseRootComponent = false;	//	베이스 루트 컴포넌트가 있는지 여부
	UPROPERTY()
	bool HasDeckComponent = false;	//	데크 컴포넌트가 있는지 여부
	UPROPERTY()
	bool HasTriggerComponent = false;	//	트리거 컴포넌트가 있는지 여부
	UPROPERTY()
	bool HasPlacementComponent = false;	//	서피스 컴포넌트가 있는지 여부
	UPROPERTY()
	bool HasTriggerGizmo = false;	//	트리거 기즈모가 있는지 여부
	UPROPERTY()
	bool HasMetaDataComponent = false;	//	메타데이터 컴포넌트가 있는지 여부
	UPROPERTY()
	bool HasStateTreeComponent = false;	//	상태 트리 컴포넌트가 있는지 여부
	UPROPERTY()
	bool ProcessedGeneration = false;	//	생성 프로세스가 완료되었는지 여부
	UPROPERTY()
	bool ProcessedWorldInteraction = false;	//	월드 상호작용 프로세스가 완료되었는지 여부
	UPROPERTY()
	bool CollectedInputAction = false;	//	입력 액션이 수집되었는지 여부
	UPROPERTY()
	bool CollectedFunctionTable = false;	//	함수 테이블이 수집되었는지 여부
	UPROPERTY()
	bool CollectedInteractionTable = false;	//	인터랙션 테이블이 수집되었는지 여부
	UPROPERTY()
	bool CompleteAnalyzedMetaData = false;	//	분석이 완료되었는지 여부
	
};

UINTERFACE(Blueprintable)
class TIHHOUSINGCORE_API UTIHHsBaseObject : public UInterface
{
	GENERATED_BODY()
};
	
class TIHHOUSINGCORE_API ITIHHsBaseObject
{
	GENERATED_BODY()
public:

	/*
	 *	기본 컴포넌트들의 허브기능만 넣어놓자.
	 *	베이스 루트: 계층구조를 담당
	 *		Attach, Detach,CheckAttach
*		데크: 
	 * 
	 */
	
	//	Commons
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category = "TIHBase")
	int32 ProcessGeneration(TArray<UActorComponent*>& others);

	UFUNCTION( Category = "TIHBase")
	virtual int32 GetProcessGenPriority() const	
	{return 1;}
	
	UFUNCTION(Category = "TIHBase")				
	virtual int32 ProcessWorldInteraction(TArray<TScriptInterface<ITIHHsBaseObject>>& others)
	{return 0;}
	
	UFUNCTION( Category = "TIHBase")			
	virtual int32 GetObjectValidation()
	{return 0;}
	
	UFUNCTION( Category = "TIHBase")
	virtual bool IsDeckAcceptable(const TScriptInterface<ITIHHsBaseObject>& Child){return true;}
	
	UFUNCTION( Category ="TIHBase")				
	virtual bool CheckImplementFunction(FName functionName) const
	{return false;}

	UFUNCTION( Category ="TIHBase")
	virtual void RegistFunction(const FName& functionNameKey,TScriptInterface<ITIHHsBaseObject> targetValue,ETIHHsFunctionRegistOvrrideOption overrideOption = ETIHHsFunctionRegistOvrrideOption::EAddOrOverride)
	{}
	
	UFUNCTION( Category ="TIHBase")
	virtual int32 GetInitPriority(FTIHHsPriorityQueryList& list)const
	{
		return 0;
	}
	UFUNCTION( Category ="TIHBase")
	virtual void VisitCollctingInputAction(FTIHHsInputActionVisitor& actionVisitor)
	{
		// 기본적으로는 빈 함수, 액션을 수집하는 함수
		// actionVisitor.FunctionMap.Add(FName(TEXT("ActionName")), TScriptInterface<ITIHHsBaseObject>(this));
	}
	
	UFUNCTION( Category ="TIHBase|BaseRoot")
	virtual void Attach(TScriptInterface<ITIHHsBaseObject>& parent)
	{
		// USceneComponent* rootScene;
		// if (AActor* parentActor = Cast<AActor>(parent.GetObject()))
		// {
		// 	rootScene->AttachToComponent(parentActor->GetRootComponent(),attachmentRules.AttachmentTransformRules);
		// 	mParent = parent; //	stuff
		//	parent->AttachNotify(this);
		// }
	}
	UFUNCTION( Category ="TIHBase|BaseRoot")
	virtual void SetTIHParent(TScriptInterface<ITIHHsBaseObject>& parent)
	{
		// //	todo
		// FTIHHsAttachmentRuleWrapper defaultRules = FTIHHsConfigures::GetDefaultAttachmentRules();
		// mParent = parent;
		// Attach(parent,defaultRules);
	}
	UFUNCTION( Category ="TIHBase|BaseRoot")
	virtual TScriptInterface<ITIHHsBaseObject> GetTIHParent() const
	{
		// //	todo
		return nullptr;
	}
	
	UFUNCTION( Category ="TIHBase|BaseRoot")
	virtual void AttachNotify(TScriptInterface<ITIHHsBaseObject>& child)
	{
		
	}
	
	UFUNCTION( Category ="TIHBase|BaseRoot")
	virtual void Detach()
	{
		//  TScriptInterface<ITIHHsBaseObject> parent = mParent->GetTIHParent();
		// if (AActor* parentActor = Cast<AActor>(parent.GetObject()))
		// {
		// 	FDetachmentTransformRules detachRule = FDetachmentTransformRules::KeepWorldTransform;
		// 	parentActor->DetachFromActor(detachRule);
		// 	parent->DetachNotify(this);
		// }
	} 
	UFUNCTION( Category ="TIHBase|BaseRoot")
	virtual void DetachNotify(TScriptInterface<ITIHHsBaseObject> child)
	{}
	UFUNCTION( Category ="TIHBase|BaseRoot")
	virtual FTIHHsAttachmentRuleWrapper GetAttachmentRuleWrapper() const
	{
		// //	todo
		return FTIHHsAttachmentRuleWrapper();
	}
	UFUNCTION( Category ="TIHBase|BaseRoot")
	virtual FTransform CalculateRelativeTransform(const FTransform& inRelativeTransform) const
	{
		return inRelativeTransform;
	}
	
	UFUNCTION( Category ="TIHBase|MetaData") //	variableFunction
	virtual FTIHHsMetaReport GetMetaReport()
	{
		return FTIHHsMetaReport();
	}
	
	UFUNCTION( Category ="TIHBase|MetaData") //	variableFunction
	virtual FTIHHsObjectIndividuality& GetIndividuality()
	{
		static FTIHHsObjectIndividuality emptyIndividuality;
		return emptyIndividuality;
	}
	UFUNCTION( Category = "TIHBase|MetaData")
	virtual bool ChangeIndividuality(const FTIHHsObjectIndividuality& individuality)
	{
		return false;
	}
	UFUNCTION( Category ="TIHBase|MetaData") //	variableFunction
	virtual void ChangeIndividualityNotify(const FTIHHsObjectIndividuality& prevState)
	{}
	
	UFUNCTION( Category ="TIHBase|Deck")
	virtual void QueryAttachmentSteps(FTIHHsAttachmentStepQueryList& steps) const
	{
		//steps.pushBack({.Query = TEXT("sanpping"), .Result = 0});
		//steps.Add({.Query = TEXT("check-collision"), .Result = 0});
		//steps.Add({.Query = TEXT("check-attach"), .Result = 0});
	}
	UFUNCTION( Category ="TIHBase|Deck")
	virtual const FTIHHsDeckSurfaceData& GetDeckSurfaceData() const
	{
		static FTIHHsDeckSurfaceData emptyDeckSurfaceData;
		return emptyDeckSurfaceData;	//	vs InstancedPropertyBag... 흠...
	}

	UFUNCTION( Category ="TIHBase|Placement")
	virtual bool DetectPlacementTarget(FHitResult& outHitResult)
	{
		/*
		 *	reValue = false;
		 *	GetWorld()->LineTraceSingleByChannel(
		 *		outHitResult,
		 *		placementPosition,
		 *		placementPosition + placementDirection * placeDetectf,
		 *	if (in linetrace) -> hit!:
		 *		TScriptInterface<ITIHHsBaseObj> baseObj(hit.getActor())
		 *		if(baseObj->GetInterface() != nullptr)
		 *		{
		 *			reValue = true;
		 *			outHitResult = hit;
		 *		}
		 *	
		 * 
		 */

		return false;
	}
	
	
	
	UFUNCTION( Category ="TIHBase|MetaData")
	virtual void VisitCollctingMetaData(class UTIHHsMetaDataComponent* metaDataComponent)
	{
		// //	메타데이터를 수집하는 함수, 기본적으로는 빈 함수
		// metaDataComponent->RegistMetaDataByTag()
	}

	UFUNCTION( Category ="TIHBase|MetaData")
	virtual void AnalizeMetaData()
	{
		// AActor* ownerActor = GetOwner();
		// ownerActor->GetComponents(components);
		// for(USceneComponent* comp : components)
		// {
		// 	if (compo->GetOwner() != ownerActor)
		// 	{
		// 		continue;
		// 	}
		// 	TScriptInterface<ITIHHsBaseObject> baseObj(comp);
		// 	baseObj->VisitCollctingMetaData(this);
		// }
	}
	
	UFUNCTION( Category ="TIHBase|StateTree")
	virtual void VisitCollectingIndividuality(class UTIHHsStateTreeComponent* stateTreeComponent)
	{
		// //	개체의 개별성을 수집하는 함수, 기본적으로는 빈 함수
		// stateTreeComponent->RegistIndividualityByTag()
	}


	
	UFUNCTION( Category ="TIHBase|Trigger")
	virtual void VisitCollectingTrigger(FTIHHsTriggerVisitor& triggerVisitor)	
	{
		// //	트리거를 수집하는 함수, 기본적으로는 빈 함수
		// triggerComponent->RegistTriggerByTag()
	}
};

UENUM()
enum class ETHsCommunicationType : uint8
{
	EUnknown UMETA(DisplayName = "Unknown"),
	ENotify UMETA(DisplayName = "Notify"),
	ERequest UMETA(DisplayName = "Request"),
	EReply UMETA(DisplayName = "Reply"),
	EInteraction UMETA(DisplayName = "Interaction"),
	
};

USTRUCT()
struct FTHsCommunicationMessage
{
	GENERATED_BODY()

	UPROPERTY()
	ETHsCommunicationType MessageType = ETHsCommunicationType::EUnknown;
	UPROPERTY()
	ETHsActionFunctionType FunctionType = ETHsActionFunctionType::EEvent_Click;
	UPROPERTY()
	bool bInteraction = false;
	UPROPERTY()
	bool bQuery = false;
	
	UPROPERTY()
	TScriptInterface<ITIHHsInteraction> ToTarget;
	UPROPERTY()
	TScriptInterface<ITIHHsInteraction> FromSource;
	UPROPERTY()
	TWeakObjectPtr<UObject> ContextObject = nullptr;
	FTHsCommunicationMessage () = default;
	FTHsCommunicationMessage(const FTHsCommunicationMessage& other) = default;
	FTHsCommunicationMessage(FTHsCommunicationMessage&& other) noexcept = default;
	FTHsCommunicationMessage& operator=(const FTHsCommunicationMessage& other) = default;
	FTHsCommunicationMessage& operator=(FTHsCommunicationMessage&& other) noexcept = default;
	FTHsCommunicationMessage (ETHsCommunicationType InMessageType,ETHsActionFunctionType InFunctionType,TScriptInterface<ITIHHsInteraction> InToTarget,TScriptInterface<ITIHHsInteraction> InFromSource,UObject* InContextObject)
		: MessageType(InMessageType)
		, FunctionType(InFunctionType)
		, ToTarget(InToTarget)
		, FromSource(InFromSource)
		, ContextObject(InContextObject)
	{}

	static FTHsCommunicationMessage MakeNotifyMessage(ETHsActionFunctionType functionType,TScriptInterface<ITIHHsInteraction> toTarget,TScriptInterface<ITIHHsInteraction> fromSource,UObject* contextObject)
	{
		return FTHsCommunicationMessage(ETHsCommunicationType::ENotify,functionType,toTarget,fromSource,contextObject);
	}
};

UINTERFACE()
class TIHHOUSINGCORE_API UTIHHsInteraction : public UInterface
{
	GENERATED_BODY()
};
//	626
class TIHHOUSINGCORE_API ITIHHsInteraction
{
	GENERATED_BODY()

public:
	UFUNCTION( Category ="Interaction")
	virtual void RegistInteraction(const FName& functionNameKey,TScriptInterface<ITIHHsInteraction> targetValue,ETIHHsFunctionRegistOvrrideOption overrideOption = ETIHHsFunctionRegistOvrrideOption::EAddOrOverride)
	{}
	
	UFUNCTION( Category = "Interaction")
	virtual const FTIHHsTag GetInteractionTag_InInteraction() const
	{
		return FTIHHsTag();
	}
	UFUNCTION( Category = "Interaction")
	virtual bool CheckInputLayer(int32 inputLayer) const
	{
		return false;
	}

	//	_Deprecated
	UFUNCTION(  Category = "Interaction|Event")
	virtual void ActionEventDown_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Event")
	virtual void ActionEventUp_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Event")
	virtual void ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Event")
	virtual void ActionEventDoubleTap_Deprecated(const FTIHHsActionParamIn& actionParamIn){}

	UFUNCTION(  Category = "Interaction|Process|Hold")
	virtual void ActionProcessHoldStart_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Process|Hold")
	virtual void ActionProcessHoldOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Process|Hold")
	virtual void ActionProcessHoldEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	
	UFUNCTION(  Category = "Interaction|Process|Move")
	virtual void ActionProcessMoveStart_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Process|Move")
	virtual void ActionProcessMoveOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Process|Move")
	virtual void ActionProcessMoveEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Process|Move")
	virtual void ActionProcessMouseMove_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Process|Move")
	virtual void ActionEventMoveAny_Deprecated(const FTIHHsActionParamIn& actionParamIn){}

	UFUNCTION(  Category = "Interaction|Process|Hover")
	virtual void ActionProcessHoverStart_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Process|Hover")
	virtual void ActionProcessHoverOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn){}
	UFUNCTION(  Category = "Interaction|Process|Hover")
	virtual void ActionProcessHoverEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn){}

	UFUNCTION(  Category = "Interaction|Etc")
	virtual void CommandAction_Deprecated(const FTIHHsActionParamIn& actionParamIn)
	{
		//	액션 커맨드, 기본적으로는 빈 함수
		//	이 함수는 액션 파라미터를 받아서 처리하는 함수로, 기본적으로는 아무 동작도 하지 않음
	}
	UFUNCTION(  Category = "Interaction|Etc")
	virtual void InteractionInit()
	{
		//	액션 커맨드, 기본적으로는 빈 함수
		//	이 함수는 액션 파라미터를 받아서 처리하는 함수로, 기본적으로는 아무 동작도 하지 않음
	}
	UFUNCTION(  Category = "Interaction|Etc")
	virtual void AttachGizmoTrigger(UObject* gizmoObject,const FTIHHsGizmoTriggerSpawnRule& gizmoTriggerSpawnRule,const FTIHHsGizmoTriggerSpawnBody& gizmoTriggerSpawnBody)
	{
		
	}
	UFUNCTION(  Category = "Interaction|Etc")
	virtual void BindKey(class UTHsActionPoint* action)
	{
		
	}
	UFUNCTION(  Category = "Interaction|Etc")
	virtual void UnBindKey(class UTHsActionPoint* action)
	{
		
	}

	
	UFUNCTION(  Category = "Interaction|Etc")
	virtual FGameplayTagContainer GetInteractionTags(){return FGameplayTagContainer();}

	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointDown(UTHsActionPoint* actionPoint){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPressing(UTHsActionPoint* actionPoint, float deltaTime){}
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointUp(UTHsActionPoint* actionPoint){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointClick(UTHsActionPoint* actionPoint){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointDoubleClick(UTHsActionPoint* actionPoint){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointHoldOngoingStart(UTHsActionPoint* actionPoint, float deltaTime){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointHoldOngoing(UTHsActionPoint* actionPoint, float deltaTime){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointHoldOngoingEnd(UTHsActionPoint* actionPoint, float deltaTime){};
	
	UFUNCTION()
	virtual void OverrideActionPointHoverStart(ATHsActionCursor* cursor, float deltaTime){};
	UFUNCTION()
	virtual void OverrideActionPointHoverOngoing(ATHsActionCursor* cursor, float deltaTime){};
	UFUNCTION()
	virtual void OverrideActionPointHoverEnd(ATHsActionCursor* cursor, float deltaTime){};

	UFUNCTION()
	virtual void OverrideActionPointClickDragStart(UTHsActionPoint* actionPoint, float deltaTime){};
	UFUNCTION()
	virtual void OverrideActionPointClickDragOngoing(UTHsActionPoint* actionPoint,float deltaTime){};
	UFUNCTION()
	virtual void OverrideActionPointClickDragEnd(UTHsActionPoint* actionPoint,float deltaTime){};
	//	ActionMouseMove
	UFUNCTION()
	virtual void OverrideActionPointMouseMove(ATHsActionCursor* cursor,float deltaTime){};
	UFUNCTION()
	virtual void OverrideActionPointSafeEnd(TScriptInterface<ITHsActionPointAPI> actionPoint){};
	UFUNCTION()
	virtual int32 OverrideCommunicateInteraction(const FTHsCommunicationMessage& message){return 1;}
	UFUNCTION()
	virtual int32 OverrideCommunicateInteractionString(const FString& stringMsgm, const FTHsCommunicationMessage& message){return 1;}
	UFUNCTION()
	virtual void OverrideConnectGizmo(TScriptInterface<class ITHsInteractiveGizmoAPI> gizmo){};
	UFUNCTION()
	virtual void OverrideDisconnectGizmo(TScriptInterface<class ITHsInteractiveGizmoAPI> gizmo){};
	UFUNCTION()
	virtual void OverrideSelection(bool bSelect){};
	UFUNCTION()
	virtual void OverrideSelectionNotify(bool bSelect, TScriptInterface<ITIHHsInteraction> source){};
	
	UFUNCTION()
	virtual ETHsInteractionAgentType GetInteractionAgentType() const
	{
		return ETHsInteractionAgentType::EActor;
	}
};

UCLASS()
class TIHHOUSINGCORE_API UTIHHsInteractionUtils : public UObject
{
	GENERATED_BODY()

public:
	static const FName Name_ActionEventDown ;
	static const FName Name_ActionEventUp ;
	static const FName Name_ActionEventTap ;
	static const FName Name_ActionEventDoubleTap ;
	static const FName Name_ActionProcessHoldStart ;
	static const FName Name_ActionProcessHoldOngoing ;
	static const FName Name_ActionProcessHoldEnd ;
	static const FName Name_ActionProcessMoveStart ;
	static const FName Name_ActionProcessMoveOngoing ;
	static const FName Name_ActionProcessMoveEnd ;
	static const FName Name_ActionProcessMouseMove ;
	static const FName Name_ActionEventMoveAny ;
	static const FName Name_ActionProcessHoverStart ;
	static const FName Name_ActionProcessHoverOngoing ;
	static const FName Name_ActionProcessHoverEnd ;

	static const FName Name_CommandAction;
	
	static const FName Name_InteractionInit;
	
};
UCLASS()
class TIHHOUSINGCORE_API UTIHHsBaseObjectUtils : public UObject
{
	GENERATED_BODY()
public:
/*
	Root: Transform 이나 Attach,Detach 같은 부분을 위주로 해야한다.
	Deck: 내부에 surface를 가지고 있을거임. 무슨말이냐면 여기에 Attach가 되는 거임
	placement: 자신이 놓아질 위치를 검사하는 바닥면임. 자신의 크기나 영역을 가지고 있음
	Trigger: 말그대로 트리거의 기즈모를 가져와서 생성해주는 놈임.
	


	 
 */
};

USTRUCT()
struct FTHsInteractiveGizmoHitResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHit = false;
	UPROPERTY()
	bool bOption0 = false;
	UPROPERTY()
	bool bOption1 = false;
	UPROPERTY()
	bool bOption2 = false;
	
	UPROPERTY()
	float RayT = TNumericLimits<float>::Max();
	UPROPERTY()
	FVector HitWorldLocation = FVector::ZeroVector;
	UPROPERTY()
	FVector HitWorldNormal = FVector::ZeroVector;
	UPROPERTY()
	FHitResult HitResult;
	UPROPERTY()
	TScriptInterface<ITIHHsInteraction> HitInteractionActor = nullptr;
	UPROPERTY()
	TScriptInterface<ITIHHsInteraction> HitInteractionComponent = nullptr;
	UPROPERTY()
	UObject* HitObject = nullptr;
	UPROPERTY()
	int32 HitSectionIndex = -1;
	UPROPERTY()
	int32 PartId = -1;

	FTHsInteractiveGizmoHitResult() = default;
	FTHsInteractiveGizmoHitResult(const FTHsInteractiveGizmoHitResult& other) = default;
	FTHsInteractiveGizmoHitResult(FTHsInteractiveGizmoHitResult&& other) noexcept = default;
	FTHsInteractiveGizmoHitResult& operator=(const FTHsInteractiveGizmoHitResult& other) = default;	
	FTHsInteractiveGizmoHitResult& operator=(FTHsInteractiveGizmoHitResult&& other) noexcept = default;
	
	void Clear()
	{
		bHit = false;
		bOption0 = false;
		bOption1 = false;
		bOption2 = false;
		RayT = TNumericLimits<float>::Max();
		HitWorldLocation = FVector::ZeroVector;
		HitWorldNormal = FVector::ZeroVector;
		HitInteractionActor = nullptr;
		HitInteractionComponent = nullptr;
		HitObject = nullptr;
		HitSectionIndex = -1;
		PartId = -1;
	}
	void Default()
	{
		Clear();
	}
	
	
};

UINTERFACE() class TIHHOUSINGCORE_API UTHsInteractiveGizmoAPI : public UInterface{GENERATED_BODY()};
class TIHHOUSINGCORE_API ITHsInteractiveGizmoAPI
{
	GENERATED_BODY()

public:
	virtual void ActiveGizmo(TScriptInterface<ITIHHsInteraction> target){}
	virtual void DeActiveGizmo(){}
	virtual bool IsActiveGizmo() const
	{
		return false;
	}
	virtual bool IsHitGizmo(){return false;}

	//virtual bool RaycastGizmo(const struct FTHsActionCursorData& cursor,FTHsInteractiveGizmoHitResult& outGizmoHitResult){return false;}
	virtual bool RaycastGizmoHitResult(const struct FTHsActionCursorData& cursor,FHitResult& outHitResult){return false;}

	
	virtual int32 GetGizmoPriority() const {return 0;}
	virtual void SetGizmoHover(bool bHover) {}
	virtual bool IsGizmoHover() const {return false;}

	virtual void Interaction(FTHsActionPointAnyParam& interactionAnyParam){}

	virtual void MoveGizmo(const FTransform& deltaTransform){}
	virtual void InitGizmo(class ATHsInteractiveGizmoManager* manager){}
	
	virtual ETHsInteractionAgentType GetInteractionAgentTypeGizmo() const
	{
		return ETHsInteractionAgentType::EActor;
	}
};

USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTHsActionCursorData
{
	GENERATED_BODY()

	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	FVector2D ScreenPosition = FVector2D::ZeroVector;
	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	FVector WorldLocation = FVector::ZeroVector;
	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	FVector WorldDirection = FVector::ZeroVector;
	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	FVector WorldRayEnd = FVector::ZeroVector;
	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	FHitResult Hit;
	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	int32 NextIndex = -1;
	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	int32 SelfIndex = -1;
	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	int32 PrevIndex = -1;
	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	bool bWritting = false;
	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	ETHsActionCursorMoveState MoveState = ETHsActionCursorMoveState::EMouseMovePaused;
	UPROPERTY(
	BlueprintReadWrite,EditAnywhere,
	meta=(Bitmask, BitmaskEnum="ETHsActionCursorState"))
	int32 ActionCursorState = static_cast<int32>(ETHsActionCursorState::ENone);

	UPROPERTY(
	BlueprintReadWrite,EditAnywhere,
	meta=(Bitmask, BitmaskEnum="ETHsActionCursorHitCategory"))	
	int32 ActionCursorHitCategory = static_cast<int32>(ETHsActionCursorHoverHitCategory::ENone);
	
	UPROPERTY(	BlueprintReadOnly)
	TScriptInterface<ITIHHsInteraction> CacheInteractionActor;
	UPROPERTY(	BlueprintReadOnly)
	TScriptInterface<ITIHHsInteraction> CacheInteractionComponent;
	
	UPROPERTY(	BlueprintReadWrite,EditAnywhere)
	ETHsActionCursorHoverType ActorHoverType = ETHsActionCursorHoverType::ENone;

	

	const FVector2D CalculatedDeltaScreenPosition(const FTHsActionCursorData& other) const
	{
		return ScreenPosition - other.ScreenPosition;
	}
	const FVector CalculatedDeltaWorldLocation(const FTHsActionCursorData& other) const
	{
		return WorldLocation - other.WorldLocation;
	}
	const FVector CalculatedDeltaWorldDirection(const FTHsActionCursorData& other) const
	{
		return (WorldDirection - other.WorldDirection).GetSafeNormal();
	}
	const FVector GetRayStart() const
	{
		return WorldLocation;
	}
	const FVector GetRayEnd() const
	{
		return WorldRayEnd;
	}
	void Clear()
	{
		ScreenPosition = FVector2D::ZeroVector;
		WorldLocation = FVector::ZeroVector;
		WorldDirection = FVector::ZeroVector;
		Hit = FHitResult();
		CacheInteractionActor = nullptr;
		CacheInteractionComponent = nullptr;
		ActionCursorState = static_cast<int32>(ETHsActionCursorState::ENone);
		ActionCursorHitCategory = static_cast<int32>(ETHsActionCursorHoverHitCategory::ENone);
		ActorHoverType = ETHsActionCursorHoverType::ENone;
		MoveState = ETHsActionCursorMoveState::EMouseMovePaused;
	}

	bool IsValid() const
	{
		return !ScreenPosition.IsZero() || !WorldLocation.IsZero() || !WorldDirection.IsZero() || Hit.IsValidBlockingHit();
	}

	void SetActionState(ETHsActionCursorState state)
	{
		ActionCursorState |= static_cast<int32>(state);
	}
	void SetActionStateNone()
	{
		ActionCursorState = static_cast<int32>(ETHsActionCursorState::ENone);
	}
	void ClearActionState(ETHsActionCursorState state)
	{
		ActionCursorState &= ~static_cast<int32>(state);
	}
	void ToggleActionState(ETHsActionCursorState state)
	{
		ActionCursorState ^= static_cast<int32>(state);
	}
	void OnActionDeprojectionValid()
	{
		SetActionState(ETHsActionCursorState::EValidDeprojectionValid);
	}
	void OnActionStateMove()
	{
		SetActionState(ETHsActionCursorState::EIsMove);
	}
	void OnActionStateHit()
	{
		SetActionState(ETHsActionCursorState::EIsHit);
	}
	void OnActionStateSameHitActor()
	{
		SetActionState(ETHsActionCursorState::EIsSameHitActor);
	}
	void OnActionStateSameComponent()
	{
		SetActionState(ETHsActionCursorState::EIsSameComponent);
	}
	void OnActionStateCastedTIHHsInteractionActor()
	{
		SetActionState(ETHsActionCursorState::EIsCastedTIHHsInteractionActor);
	}
	void OnActionStateCastedTIHHsInteractionComponent()
	{
		SetActionState(ETHsActionCursorState::EIsCastedTIHHsInteractionComponent);
	}
	void OffActionStateMove()
	{
		ClearActionState(ETHsActionCursorState::EIsMove);
	}
	void OffActionStateHit()
	{
		ClearActionState(ETHsActionCursorState::EIsHit);
	}
	void OffActionStateSameHitActor()
	{
		ClearActionState(ETHsActionCursorState::EIsSameHitActor);
	}
	void OffActionStateSameComponent()
	{
		ClearActionState(ETHsActionCursorState::EIsSameComponent);
	}
	void OffActionStateCastedTIHHsInteractionActor()
	{
		ClearActionState(ETHsActionCursorState::EIsCastedTIHHsInteractionActor);
	}
	void OffActionStateCastedTIHHsInteractionComponent()
	{
		ClearActionState(ETHsActionCursorState::EIsCastedTIHHsInteractionComponent);
	}
	
	
	bool IsActionStateNone() const
	{
		return ActionCursorState == static_cast<int32>(ETHsActionCursorState::ENone);
	}
	bool IsActionStateMove() const
	{
		return EnumHasAnyFlags(ToFlags<ETHsActionCursorState>(ActionCursorState),ETHsActionCursorState::EIsMove);
	}
	bool IsActionStateHit() const
	{
		return EnumHasAnyFlags(ToFlags<ETHsActionCursorState>(ActionCursorState),ETHsActionCursorState::EIsHit);
	}
	bool IsActionStateSameHitActor() const
	{
		return EnumHasAnyFlags(ToFlags<ETHsActionCursorState>(ActionCursorState),ETHsActionCursorState::EIsSameHitActor);
	}
	bool IsActionStateAnySame() const
	{
		return EnumHasAnyFlags(ToFlags<ETHsActionCursorState>(ActionCursorState),ETHsActionCursorState::EIsSameHitActor|ETHsActionCursorState::EIsSameComponent);
	}
	bool IsActionStateSameComponent() const
	{
		return EnumHasAnyFlags(ToFlags<ETHsActionCursorState>(ActionCursorState),ETHsActionCursorState::EIsSameComponent);
	}
	bool IsActionStateCastedTIHHsInteractionActor() const
	{
		return EnumHasAnyFlags(ToFlags<ETHsActionCursorState>(ActionCursorState),ETHsActionCursorState::EIsCastedTIHHsInteractionActor);
	}
	bool IsActionStateCastedTIHHsInteractionComponent() const
	{
		return EnumHasAnyFlags(ToFlags<ETHsActionCursorState>(ActionCursorState),ETHsActionCursorState::EIsCastedTIHHsInteractionComponent);
	}
	bool IsActionDeprojectionValid() const
	{
		return EnumHasAnyFlags(ToFlags<ETHsActionCursorState>(ActionCursorState),ETHsActionCursorState::EValidDeprojectionValid);
	}

	bool IsHoverActive() const
	{
		return ActorHoverType == ETHsActionCursorHoverType::EHoverStart || ActorHoverType == ETHsActionCursorHoverType::EHoverOngoing;
	}
	bool CheckCursorHitCategory(ETHsActionCursorHoverHitCategory hitCategory) const
	{
		return EnumHasAnyFlags(ToFlags<ETHsActionCursorHoverHitCategory>(ActionCursorHitCategory),hitCategory);
	}

	bool IsHitActiveStuff() const
	{
		//	not nullptr 임을 보장.
		return IsActionStateHit() && CheckCursorHitCategory(ETHsActionCursorHoverHitCategory::EStuff);
	}
};

UINTERFACE()
class UTHsGlobalDependency : public UInterface
{
	GENERATED_BODY()
};

class ITHsGlobalDependency
{
	GENERATED_BODY()

public:
	virtual void AddState(const FName& stateName,bool bEnable) = 0;
	virtual void RemoveState(const FName& stateName) = 0;
	virtual void EnableState(const FName& stateName) = 0;
	virtual void DisableState(const FName& stateName) = 0;
	virtual bool IsStateEnabled(const FName& stateName) const = 0;
	virtual bool HasState(const FName& stateName) const = 0;
	virtual void ClearStates() = 0;

	virtual void AddUObject(const FName& key,UObject* object) = 0;
	virtual UObject* GetUObject(const FName& key) const = 0;
	virtual void RemoveUObject(const FName& key) = 0;
	virtual void ClearUObjects() = 0;
	
	virtual void AddInstance(const FName& key,void** instance) {};//	not yet
	
};

UCLASS()
class UTHsGlobalDependencyCaller : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
};
