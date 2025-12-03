// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHsStuffBase.h"


#include "Containers/Deque.h"
#include "Prefabs/TIHHsPrefabCore.h"
#include "Surface/TIHHsSurfaceCore.h"
#include "Test/TIHHsTestAllComponent.h"
#include "TIHComponents/TIHHsStateTreeComponent.h"
#include "TIHHousingCore/Public/TIHComponents/TIHHsBaseRootComponent.h"
#include "TIHHousingCore/Public/TIHComponents/TIHHsDeckComponent.h"
#include "TIHHousingCore/Public/TIHComponents/TIHHsMetaDataComponent.h"
#include "TIHHousingCore/Public/TIHComponents/TIHHsPlacementComponent.h"
#include "TIHHousingCore/Public/TIHComponents/TIHHsTriggerComponent.h"


DECLARE_STATS_GROUP(TEXT("TIHHsStuffBase"), STATGROUP_TIHHsStuffBase, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("TIHHsStuffBase Tick"), STAT_TIHHsStuffBase_Tick, STATGROUP_TIHHsStuffBase);


/*
*  = TEXT("trigger")
 = TEXT("placement")
 = TEXT("metaObject")
 = TEXT("defaultMode")
 = TEXT("transformChecker")
 = TEXT("transformModifier")
 = TEXT("attachChecker")
 */


void ATIHHsStuffTrigger::ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	UE_LOG(LogTemp, Log, TEXT("ATIHHsStuffTrigger"));
}

void ATIHHsStuffTrigger::ActionProcessHoldOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	UE_LOG(LogTemp, Log, TEXT("ATIHHsStuffTrigger ActionProcessHoldOngoing"));
}

// Sets default values
ATIHHsStuffBase::ATIHHsStuffBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//RootComponent = CreateDefaultSubobject<UTIHHsBaseRootComponent>(TEXT("RootComponent"));
//
	//mMetaDataComponent = CreateDefaultSubobject<UTIHHsMetaDataComponent>(TEXT("MetaDataComponent"));
	//mPlacementComponent = CreateDefaultSubobject<UTIHHsPlacementComponent>(TEXT("PlacementComponent"));
	//mTriggerComponent = CreateDefaultSubobject<UTIHHsTriggerComponent>(TEXT("TriggerComponent"));
	//mDeckComponent = CreateDefaultSubobject<UTIHHsDeckComponent>(TEXT("DeckComponent"));
	//mStateTreeComponent = CreateDefaultSubobject<UTIHHsStateTreeComponent>(TEXT("StateTreeComponent"));
	//
	//mPlacementComponent->SetupAttachment(RootComponent);
	//mTriggerComponent->SetupAttachment(RootComponent);
	//mMetaDataComponent->SetupAttachment(RootComponent);
	//mDeckComponent->SetupAttachment(RootComponent);
	//
	//mTestAllComponent = CreateDefaultSubobject<UTIHHsTestAllComponent>(TEXT("TestAllComponent"));
	//mTestAllComponent->SetupAttachment(RootComponent);
	//AddOwnedComponent(mStateTreeComponent);
	
}

// Called when the game starts or when spawned
void ATIHHsStuffBase::BeginPlay()
{
	Super::BeginPlay();
	TArray<UActorComponent*> empty;
	Execute_ProcessGeneration(this,empty);
	
	UE_LOG(LogTemp, Warning, TEXT("------ATIHHsStuffBase BeginPlay"));
	
}

// Called every frame
void ATIHHsStuffBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATIHHsStuffBase::BeginDestroy()
{
	Super::BeginDestroy();
	TArray<UActorComponent*> empty;
	Execute_ProcessGeneration(
		this,
		empty
	);
}

void ATIHHsStuffBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(LogTemp, Warning, TEXT("ATIHHsStuffBase PostInitializeComponents"));
}

void ATIHHsStuffBase::InternalOrder(FTIHHsStuffOrderData orderData)
{
	if (not orderData.IsValid() )
	{
		return;
	}
	
	USceneComponent* rootComp = GetRootComponent();
	
	if (rootComp != nullptr)
	{
		if (orderData.OrderType == ETIHHsStuffOrder::LevelOrder)	//	queue based order
		{
			TDeque<USceneComponent*> traversalQue;
			traversalQue.PushLast(rootComp);

			while (not traversalQue.IsEmpty())
			{
				USceneComponent* currComp = nullptr;
				traversalQue.TryPopFirst(currComp);
				if (currComp == nullptr)
				{
					continue;
				}
				orderData.Execute(currComp);

				const TArray<USceneComponent*>& childComps = currComp->GetAttachChildren();
				for (USceneComponent* child :childComps)
				{
					traversalQue.PushLast(child);
				}
			}
		}
		else // stack based order
		{
			struct FTIHTraversalFrame
			{
				USceneComponent* SComponent;
				int32 ChildIndex;
				bool IsOnChildrenProcess;

				FTIHTraversalFrame(USceneComponent* InComponent)
					: SComponent(InComponent)
					, ChildIndex(0)
					, IsOnChildrenProcess(false)
				{}
			};

			TArray<FTIHTraversalFrame> traversalStack;
		
			//	For SceneComponent
			traversalStack.Push(FTIHTraversalFrame(rootComp));
			while (traversalStack.Num() > 0)
			{
				FTIHTraversalFrame& currFrame = traversalStack.Last();
				USceneComponent* currComp = currFrame.SComponent;
			
				if (orderData.OrderType == ETIHHsStuffOrder::EPreOrder)
				{
					if (not currFrame.IsOnChildrenProcess)
					{
						orderData.Execute(currComp);
						currFrame.IsOnChildrenProcess = true;
					}

					const TArray<USceneComponent*>& childComps = currComp->GetAttachChildren();
					if (currFrame.ChildIndex < childComps.Num())
					{
						traversalStack.Push(FTIHTraversalFrame(childComps[currFrame.ChildIndex]));
						++currFrame.ChildIndex;
					}
					else
					{
						traversalStack.Pop();
					}
				}
				else if (orderData.OrderType == ETIHHsStuffOrder::EPostOrder)
				{
					const TArray<USceneComponent*>& childComps = currComp->GetAttachChildren();
					if (currFrame.ChildIndex < childComps.Num())
					{
						traversalStack.Push(FTIHTraversalFrame(childComps[currFrame.ChildIndex]));
						++currFrame.ChildIndex;
					}
					else
					{
						orderData.Execute(currComp);
						traversalStack.Pop();
					}
				}
			}//	stack based while
		}//	stack based order
	}//	if rootComp is not null
	
	//	For ActorComponent
	TInlineComponentArray<UActorComponent*> allComps(this);
	for (UActorComponent* actComp:allComps)
	{
		if (not actComp->IsA<USceneComponent>())
		{
			orderData.Execute(actComp);
		}
	}
	
}

void ATIHHsStuffBase::LoadTemplateData(const FString& templatePath)
{
	/*
	 *	내부에 셋팅을 위한 데이터들을 포함해야한다.
	 *	InstancedStruct를 사용해도 될거같긴함.
	 *	baseComponent
	 *		필요한거 없음 그냥위치?
	 *	meta
	 *		아직은 없음
	 *	placement
	 *	trigger
	 *		이걸 위해서 한거라 트리거의 클래스의 위치를 해놓으면 될거임.
	 *	deck
	 *	stateTree
	 *  그리고 뭐를 사용해야할까? JSON? XNL?
	 *  일단 JSON으로 해보자. 우선 각 컴포넌트를 위한 전용 컨텍스트가 있던가 그 객체자체를 변환하자.
	 *  
	 */
	
	
}

int32 ATIHHsStuffBase::GetProcessGenPriority() const
{
	return 0;
}

int32 ATIHHsStuffBase::ProcessWorldInteraction(TArray<TScriptInterface<ITIHHsBaseObject>>& others)
{



	
	return ITIHHsBaseObject::ProcessWorldInteraction(others);
}

int32 ATIHHsStuffBase::GetObjectValidation()
{
	return ITIHHsBaseObject::GetObjectValidation();
}

bool ATIHHsStuffBase::CheckImplementFunction(FName functionName) const
{
	return ITIHHsBaseObject::CheckImplementFunction(functionName);
}

int32 ATIHHsStuffBase::GetInitPriority(FTIHHsPriorityQueryList& list) const
{
	return ITIHHsBaseObject::GetInitPriority(list);
}

void ATIHHsStuffBase::Attach(TScriptInterface<ITIHHsBaseObject>& parent)
{
	ITIHHsBaseObject::Attach(parent);
}

void ATIHHsStuffBase::SetTIHParent(TScriptInterface<ITIHHsBaseObject>& parent)
{
	ITIHHsBaseObject::SetTIHParent(parent);
}

TScriptInterface<ITIHHsBaseObject> ATIHHsStuffBase::GetTIHParent() const
{
	return ITIHHsBaseObject::GetTIHParent();
}

void ATIHHsStuffBase::AttachNotify(TScriptInterface<ITIHHsBaseObject>& child)
{
	ITIHHsBaseObject::AttachNotify(child);
}

void ATIHHsStuffBase::Detach()
{
	ITIHHsBaseObject::Detach();
}

void ATIHHsStuffBase::DetachNotify(TScriptInterface<ITIHHsBaseObject> child)
{
	ITIHHsBaseObject::DetachNotify(child);
}

FTIHHsObjectIndividuality& ATIHHsStuffBase::GetIndividuality()
{
	return ITIHHsBaseObject::GetIndividuality();
}

bool ATIHHsStuffBase::ChangeIndividuality(const FTIHHsObjectIndividuality& individuality)
{
	return ITIHHsBaseObject::ChangeIndividuality(individuality);
}

void ATIHHsStuffBase::ChangeIndividualityNotify(const FTIHHsObjectIndividuality& prevState)
{
	ITIHHsBaseObject::ChangeIndividualityNotify(prevState);
}

void ATIHHsStuffBase::QueryAttachmentSteps(FTIHHsAttachmentStepQueryList& steps) const
{
	ITIHHsBaseObject::QueryAttachmentSteps(steps);
}

const FTIHHsDeckSurfaceData& ATIHHsStuffBase::GetDeckSurfaceData() const
{
	return ITIHHsBaseObject::GetDeckSurfaceData();
}

bool ATIHHsStuffBase::DetectPlacementTarget(FHitResult& outHitResult)
{
	return ITIHHsBaseObject::DetectPlacementTarget(outHitResult);
}

void ATIHHsStuffBase::VisitCollctingMetaData(UTIHHsMetaDataComponent* metaDataComponent)
{
	ITIHHsBaseObject::VisitCollctingMetaData(metaDataComponent);
}

void ATIHHsStuffBase::AnalizeMetaData()
{
	ITIHHsBaseObject::AnalizeMetaData();
}

void ATIHHsStuffBase::VisitCollectingIndividuality(UTIHHsStateTreeComponent* stateTreeComponent)
{
	ITIHHsBaseObject::VisitCollectingIndividuality(stateTreeComponent);
}

void ATIHHsStuffBase::VisitCollectingTrigger(FTIHHsTriggerVisitor& triggerVisitor)
{
	//	triggerVisitor 여기 안에 instance정보가 들어가있을거임.
	//	그럼 이게 spawn정보지. 뭐야. 즉 FTIHHsTriggerVisitorNode == FTIHHsGizmoTriggerSpawnData 임. 내가 나눌필요가 없던거임. 이미 유저는 저기에 모든 정보를 넣어야되는거임. 즉 component에서 VisitCollectingTrigger 를 호출할때 등록하고, 처리하는 방법까지 지시해서 넣을 수 있음. 넘어오는 정보는 이미 new된 정보고, 거기에 그냥 콜백만 호출해서 처리하게 해주면 되는거임.
	if (triggerVisitor.VisitorNodes.Num() == 0)
	{
		return;
	}
	if (UTIHHsGizmoTriggerPrefabSubsystem* triggerMgr = GetWorld()->GetSubsystem<UTIHHsGizmoTriggerPrefabSubsystem>())
	{
		const TArray<FTIHHsGizmoTriggerSpawnData>& nodes = triggerVisitor.VisitorNodes;
		triggerMgr->ProcessInstanceGizmoTriggerByVisitor(this,triggerVisitor);
	}
	
	
}

int32 ATIHHsStuffBase::ProcessGeneration_Implementation(TArray<UActorComponent*>& others)
{
	others.Empty();
	//	자기 자신을 포함하고 자신의 계층을 포함한다가 아니라 그냥 다 포함시켜.
	
	if (mRootComponent == nullptr)
	{
		mRootComponent = NewObject<UTIHHsBaseRootComponent>(this, TEXT("RootComponent"));
		mRootComponent->RegisterComponent();
		SetRootComponent(mRootComponent);
		mRootComponent->SetIsReplicated(true);
	}
	
	(TryCreateBagicComponent_Internal(mMetaDataComponent, TEXT("MetaDataComponent"), mRootComponent));
	(TryCreateBagicComponent_Internal(mPlacementComponent, TEXT("PlacementComponent"), mRootComponent));
	(TryCreateBagicComponent_Internal(mTriggerComponent, TEXT("TriggerComponent"), mRootComponent));
	(TryCreateBagicComponent_Internal(mDeckComponent, TEXT("DeckComponent"), mRootComponent));
	//TryCreateBagicComponent_Internal(mStateTreeComponent, TEXT("StateTreeComponent"), mRootComponent);
	(TryCreateBagicComponent_Internal(mTestAllComponent, TEXT("TestAllComponent"), mRootComponent));
	
	GetComponents(others,true);
	Execute_ProcessGeneration(mRootComponent.Get(), others);
	FTIHHsInputActionVisitor inputVisitor;
	FTIHHsTriggerVisitor triggerVisitor;
	InternalOrder(FTIHHsStuffOrderData{ETIHHsStuffOrder::EPreOrder,
		[&](UActorComponent* actorComponent)->void
		{
			if (actorComponent)
			{
				TScriptInterface<ITIHHsBaseObject> baseObject(actorComponent);
				if (baseObject.GetInterface() != nullptr)
				{
					baseObject->VisitCollctingInputAction(inputVisitor);
					baseObject->VisitCollectingTrigger(triggerVisitor);
				}
			}
		}
	});
	mInteractionTable.Append(inputVisitor.FunctionMap);
	VisitCollectingTrigger(triggerVisitor);
	return others.Num();
}

void ATIHHsStuffBase::RegistInteraction(const FName& functionNameKey, TScriptInterface<ITIHHsInteraction> targetValue,
	ETIHHsFunctionRegistOvrrideOption overrideOption)
{
	//ITIHHsInteraction::RegistInteraction(functionNameKey, targetValue, overrideOption);
}

const FTIHHsTag ATIHHsStuffBase::GetInteractionTag_InInteraction() const
{

	return ITIHHsInteraction::GetInteractionTag_InInteraction();
}

void ATIHHsStuffBase::ActionEventDown_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//TIHDRY_ACTION(ActionEventDown_Deprecated);
}

void ATIHHsStuffBase::ActionEventUp_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//TIHDRY_ACTION(ActionEventUp_Deprecated);
}

void ATIHHsStuffBase::ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	static int32 a = 0;
		UE_LOG(LogTemp,Warning,TEXT("[[[[ATIHHsStuffBase::ActionEventTap]]]]"));
	//TIHDRY_ACTION(ActionEventTap_Deprecated);
}

void ATIHHsStuffBase::ActionEventDoubleTap_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//TIHDRY_ACTION(ActionEventDoubleTap_Deprecated);
	UE_LOG(LogTemp,Log, TEXT("----ATIHHsStuffBase::ActionEventDoubleTap called"));
}

void ATIHHsStuffBase::ActionProcessHoldStart_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//TIHDRY_ACTION(ActionProcessHoldStart_Deprecated);
	UE_LOG(LogTemp,Log, TEXT("ATIHHsStuffBase::ActionProcessHoldStart called"));
}

void ATIHHsStuffBase::ActionProcessHoldOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//TIHDRY_ACTION(ActionProcessHoldOngoing_Deprecated);
	//ITIHHsInteraction::ActionProcessHoldOngoing(actionParamIn);
}

void ATIHHsStuffBase::ActionProcessHoldEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//TIHDRY_ACTION(ActionProcessHoldEnd_Deprecated);
	UE_LOG(LogTemp,Log, TEXT("ATIHHsStuffBase::ActionProcessHoldEnd called"));
}

void ATIHHsStuffBase::ActionProcessMoveStart_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//TIHDRY_ACTION(ActionProcessMoveStart_Deprecated);
	//ITIHHsInteraction::ActionProcessMoveStart(actionParamIn);
}

void ATIHHsStuffBase::ActionProcessMoveOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//TIHDRY_ACTION(ActionProcessMoveOngoing_Deprecated);
}

void ATIHHsStuffBase::ActionProcessMoveEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//TIHDRY_ACTION(ActionProcessMoveEnd_Deprecated);

}

void ATIHHsStuffBase::ActionProcessHoverStart_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//TIHDRY_ACTION(ActionProcessHoverStart_Deprecated);
	UE_LOG(LogTemp, Warning, TEXT("ATIHHsStuffBase::ActionProcessHoverStart called"));
}

void ATIHHsStuffBase::ActionProcessHoverOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	//UE_LOG(LogTemp, Warning, TEXT("ATIHHsStuffBase::ActionProcessHoverOngoing called"));
	//TIHDRY_ACTION(ActionProcessHoverOngoing_Deprecated);
}

void ATIHHsStuffBase::ActionProcessHoverEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	UE_LOG(LogTemp, Warning, TEXT("ATIHHsStuffBase::ActionProcessHoverEnd called"));
	//TIHDRY_ACTION(ActionProcessHoverEnd_Deprecated);
}


void UTIHHsFrameWorkSupportSystem::PostWorldInitialization(UWorld* inWorld, const UWorld::InitializationValues IVS)
{
	mWorld = inWorld;
	if (mWorld)
	{
		UE_LOG(LogTemp, Warning, TEXT("----TIHHsFrameWorkSupportSystem::PostWorldInitialization inWorld valid----"));
		UTIHHsSurfaceProcRegistrySubSystem* registry = mWorld->GetSubsystem<UTIHHsSurfaceProcRegistrySubSystem>();
		registry->RegisterProcessor(TEXT("grid-check"), NewObject<UTIHHsSurfaceGridSnapProc>(registry));
		
		//mTestStuffRuntimeActor = mWorld->SpawnActor<ATIHHsStuffBase>(ATIHHsStuffBase::StaticClass(),FTransform::Identity);
	}
	
}


void UTIHHsFrameWorkSupportSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this,&UTIHHsFrameWorkSupportSystem::PostWorldInitialization);
}

//
// int32 ATIHHsStuffBase::ProcessWorldInteraction(TArray<TScriptInterface<ITIHHsBaseObject>>& others)
// {
// 	int32 result = 0;
// 	TArray<USceneComponent*> components;
// 	mRootComponent->GetChildrenComponents(false,components);
// 	TArray<TScriptInterface<ITIHHsBaseObject>> errorList;
// 	for (USceneComponent* comp : components)
// 	{
// 		TScriptInterface<ITIHHsBaseObject> obj(comp);
// 		if (obj.GetInterface() !=nullptr)
// 		{
// 			int32 tempResult = obj->ProcessWorldInteraction(others);
// 			if ( tempResult < 0)
// 			{
// 				result = -1;
// 				break;
// 			}
// 			else if (tempResult > 0)
// 			{
// 				obj->ChangeTIHState(ETIHHsStateDetail::ETIHHsStates_Initiate_CompleteFinalizing);
// 				continue;
// 			}
// 			else
// 			{
// 				errorList.Add(obj);
// 			}
// 		}
// 	}
// 	return result;
// }

// int32 ATIHHsStuffBase::ProcessGeneration_Implementation(TArray<USceneComponent*>& others)
// {
// 	int32 result = 0;
// 	others.Empty();
// 	TArray<USceneComponent*> components;
// 	mRootComponent->GetChildrenComponents(false,components);
// 	TMap<int32,TArray<TScriptInterface<ITIHHsBaseObject>> > groupedComponents;
// 	others.Reserve(components.Num());
// 	for (USceneComponent* comp : components)
// 	{
// 		TScriptInterface<ITIHHsBaseObject> obj(comp);
// 		if (obj.GetInterface() !=nullptr)
// 		{
// 			int32 priority = obj->GetProcessGenPriority();
// 			groupedComponents.FindOrAdd(priority).Add(obj);
// 			others.Add(comp);
// 			obj->ChangeTIHState(ETIHHsStateDetail::ETIHHsStates_Initiate_Starting);
// 		}
// 	}
// 	
// 	TArray<int32> keys;
// 	groupedComponents.GetKeys(keys);
// 	keys.Sort([](const int32& a, const int32& b)
// 	{
// 		return a < b;
// 	});
//
// 	TArray<TScriptInterface<ITIHHsBaseObject>> errorList;
// 	
// 	for (const int32& key:keys)
// 	{
// 		for (TScriptInterface<ITIHHsBaseObject>& baseObj : groupedComponents[key])
// 		{
// 			int32 tempResult = Execute_ProcessGeneration(baseObj.GetObject(), others);
// 			if ( tempResult < 0)
// 			{
// 				result = -1;
// 				break;
// 			}
// 			else if (tempResult > 0)
// 			{
// 				continue;
// 			}
// 			else
// 			{
// 				errorList.Add(baseObj);
// 			}
// 		}
// 		
// 		if (result < 0)
// 		{
// 			break;
// 		}
// 	}
//
// 	if (not errorList.IsEmpty())
// 	{
// 		for (const TScriptInterface<ITIHHsBaseObject>& error :errorList)
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("ProcessGeneration failed for %s"), *error.GetObject()->GetName());
// 			//error->ChangeTIHState(ETIHHsStateDetail::ETIHHsStates_Initiate_Error);
// 		}
// 		result = 0;
// 	}
// 	ChangeTIHState(ETIHHsStateDetail::ETIHHsStates_Initiate_CompleteFinalizing);
// 	return result;
// }
//
//
//

