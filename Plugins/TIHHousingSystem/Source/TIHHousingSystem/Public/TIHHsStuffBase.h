// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStructContainer.h"
#include "PropertyBag.h"
#include "TIHHsCore.h"
#include "GameFramework/Actor.h"
#include "TIHHsStuffBase.generated.h"

class UTIHHsDeckComponent;
class UTIHHsPlacementComponent;

USTRUCT()
struct FInstancedTestStruct
{
	GENERATED_BODY()

	UPROPERTY()
	int32 test;
};

#define TIHDRY_ACTION(ActionFuncName) \
if (mInteractionTable.Contains(UTIHHsInteractionUtils::Name_##ActionFuncName))\
{\
	mInteractionTable[UTIHHsInteractionUtils::Name_##ActionFuncName]->ActionFuncName(actionParamIn);\
}
//else \
//{\
//	UE_LOG(LogTemp, Warning, TEXT("TIHHsStuffBase - %s not found in interaction table!"), *Name_##ActionName.ToString());\
//}

UCLASS()
class TIHHOUSINGSYSTEM_API ATIHHsStuffTrigger : public AActor, public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	virtual void ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoldOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
};

UCLASS()
class TIHHOUSINGSYSTEM_API UTIHHsTestBox : public UStaticMeshComponent, public ITIHHsInteraction
{
	GENERATED_BODY()
public:
	
};

DECLARE_DELEGATE_OneParam(FTIHHsStuffOrderDelegate,UActorComponent*);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTIHHsStuffOrderDynamicDelegate, UActorComponent*, inParam);
UENUM()
enum class ETIHHsStuffOrder
{
	EPreOrder,
	EPostOrder,
	LevelOrder,
};




USTRUCT()
struct FTIHHsStuffOrderData
{
	GENERATED_BODY()

	UPROPERTY()
	ETIHHsStuffOrder OrderType = ETIHHsStuffOrder::EPreOrder;

	UPROPERTY()
	ETIHHsFunctionType FunctionType;

	//	Callbacks
	//	Reflects the type of function to call when executing the order.
	UPROPERTY()
	FTIHHsStuffOrderDynamicDelegate DynamicDelegateFunc;// ETIHHsStuffOrderFunctionType::EDynamicDelegate;
	UPROPERTY()
	FName UFunctionTypeName;	//	ETIHHsStuffOrderFunctionType::EUFunction;
	UPROPERTY()
	UObject* UFunctionTypeObject;	//	ETIHHsStuffOrderFunctionType::EUFunction;

	//	Raw function pointers only for C++ code.
	FTIHHsStuffOrderDelegate DelegateTypeFunc;	//	ETIHHsStuffOrderFunctionType::EDelegate;
	
	TFunction<void(UActorComponent*)> TFunctionTypeFunc;	//	ETIHHsStuffOrderFunctionType::ETFunction;
	
	void(*StaticFunctionTypeFunc)(UActorComponent*);	//	ETIHHsStuffOrderFunctionType::EStaticFunction;

	typedef void (UObject::*TIHOrderMemberFunctionPtr)(UActorComponent*);
	TIHOrderMemberFunctionPtr MemberFunctionPtrFunc = nullptr;	//	ETIHHsStuffOrderFunctionType::EMemberFunction;

	FTIHHsStuffOrderData() = default;

	explicit FTIHHsStuffOrderData(ETIHHsStuffOrder inType): FunctionType(), UFunctionTypeObject(nullptr),StaticFunctionTypeFunc(nullptr)
	{
		FunctionType = ETIHHsFunctionType::EJustOrder;
	}
	explicit FTIHHsStuffOrderData(ETIHHsStuffOrder inType,int32): FunctionType(), UFunctionTypeObject(nullptr),StaticFunctionTypeFunc(nullptr)
	{
		FunctionType = ETIHHsFunctionType::EJustOrderDebugLog;
	}

	explicit FTIHHsStuffOrderData(ETIHHsStuffOrder inType, FTIHHsStuffOrderDelegate inDelegate)
		: OrderType(inType), UFunctionTypeObject(nullptr), DelegateTypeFunc(inDelegate), StaticFunctionTypeFunc(nullptr)
	{
		FunctionType = ETIHHsFunctionType::EDelegate;
	}

	explicit FTIHHsStuffOrderData(ETIHHsStuffOrder inType, FTIHHsStuffOrderDynamicDelegate inDelegate)
		: OrderType(inType), DynamicDelegateFunc(inDelegate), UFunctionTypeObject(nullptr),
		  StaticFunctionTypeFunc(nullptr)
	{
		FunctionType = ETIHHsFunctionType::EDynamicDelegate;
	}

	explicit FTIHHsStuffOrderData(ETIHHsStuffOrder inType, UObject* inObject, FName inFunctionName)
		: OrderType(inType), UFunctionTypeName(inFunctionName), UFunctionTypeObject(inObject),
		  StaticFunctionTypeFunc(nullptr)
	{
		FunctionType = ETIHHsFunctionType::EUObject;
	}

	explicit FTIHHsStuffOrderData(ETIHHsStuffOrder inType, TFunction<void(UActorComponent*)> inFunction)
		: OrderType(inType), UFunctionTypeObject(nullptr), TFunctionTypeFunc(inFunction),
		  StaticFunctionTypeFunc(nullptr)
	{
		FunctionType = ETIHHsFunctionType::ETFunction;
	}

	explicit FTIHHsStuffOrderData(ETIHHsStuffOrder inType, void(*inFunction)(UActorComponent*))
		: OrderType(inType), UFunctionTypeObject(nullptr), StaticFunctionTypeFunc(inFunction)
	{
		FunctionType = ETIHHsFunctionType::EStaticFunction;
	}

	explicit FTIHHsStuffOrderData(ETIHHsStuffOrder inType, UObject* inObject, FName inFunctionName, TIHOrderMemberFunctionPtr inMemberFunction)
		: OrderType(inType), UFunctionTypeName(inFunctionName), UFunctionTypeObject(inObject),
		  StaticFunctionTypeFunc(nullptr), MemberFunctionPtrFunc(inMemberFunction)
	{
		FunctionType = ETIHHsFunctionType::EMemberFunction;
	}

	explicit FTIHHsStuffOrderData(ETIHHsStuffOrder inType, TIHOrderMemberFunctionPtr inMemberFunction)
		: OrderType(inType), UFunctionTypeObject(nullptr), StaticFunctionTypeFunc(nullptr),
		  MemberFunctionPtrFunc(inMemberFunction)
	{
		FunctionType = ETIHHsFunctionType::EMemberFunction;
	}


	bool IsValid() const
	{
		bool result = false;
		switch (FunctionType) {
		case ETIHHsFunctionType::EDelegate:
			result = DelegateTypeFunc.IsBound();
			break;
		case ETIHHsFunctionType::ETFunction:
			TFunctionTypeFunc.CheckCallable();
			result = TFunctionTypeFunc != nullptr;
			break;
		case ETIHHsFunctionType::EDynamicDelegate:
			result = DynamicDelegateFunc.IsBound();
			break;
		case ETIHHsFunctionType::EUObject:
			result = UFunctionTypeObject != nullptr && !UFunctionTypeName.IsNone();
			if (result)
			{
				if (UFunction* func = UFunctionTypeObject->FindFunction(UFunctionTypeName))
				{
					result &= func->NumParms == 1;
					result &= func->ParmsSize == sizeof(UActorComponent*);
				}
				else
				{
					result = false;
				}
			}
			
			break;
		case ETIHHsFunctionType::EStaticFunction:
			result = StaticFunctionTypeFunc != nullptr;
		
			break;
		case ETIHHsFunctionType::EMemberFunction:
			if (UFunctionTypeObject && MemberFunctionPtrFunc)
			{
				result = true;
			}
			break;
		case ETIHHsFunctionType::EInvalid:
			result = false;
			break;
		case ETIHHsFunctionType::EJustOrder:
		case ETIHHsFunctionType::EJustOrderDebugLog:
			result = true;	//	Just order, no function to call.
			break;
		}
		return result;
	}

	//	check Validation and execute the function.
	void Execute(UActorComponent* inParam) const
	{
		if (not IsValid())
		{
			return;
		}
		operator()(inParam);
	}

	void operator()(UActorComponent* inParam) const
	{
		switch (FunctionType)
		{
		case ETIHHsFunctionType::EDynamicDelegate:
			DynamicDelegateFunc.Execute(inParam);
			break;
		case ETIHHsFunctionType::EUObject:
			if (UFunction* func = UFunctionTypeObject->FindFunction(UFunctionTypeName))
			{
				struct FTIHTempParams
				{
					UActorComponent* Component;
				} objParam;
				objParam.Component = inParam;
				func->ProcessEvent(func,&objParam);
			}
			break;
		case ETIHHsFunctionType::ETFunction:
			TFunctionTypeFunc(inParam);
			break;
		case ETIHHsFunctionType::EDelegate:
			DelegateTypeFunc.Execute(inParam);
			break;
		case ETIHHsFunctionType::EStaticFunction:
			StaticFunctionTypeFunc(inParam);
			break;
		case ETIHHsFunctionType::EMemberFunction:
			(UFunctionTypeObject->*MemberFunctionPtrFunc)(inParam);
			break;
		case ETIHHsFunctionType::EInvalid:
		case ETIHHsFunctionType::EJustOrder:
			break;
		case ETIHHsFunctionType::EJustOrderDebugLog:
			if (inParam)
			{
				UE_LOG(LogTemp, Warning, TEXT("TIHHsStuffBase - Order executed: %s"), *inParam->GetName());
			}
			break;
		}
	}
};


UCLASS()
class TIHHOUSINGSYSTEM_API ATIHHsStuffBase : public AActor, public ITIHHsBaseObject,public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATIHHsStuffBase();

	/*
	 *	checkState(int32 filterBool,const FTIHHsState& state)
	 * 
	 */
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;
	virtual void PostInitializeComponents() override;
	
	virtual void InternalOrder(FTIHHsStuffOrderData orderData);

	UFUNCTION()
	void LoadTemplateData(const FString& templatePath);
	
	TScriptInterface<ITIHHsBaseObject>& GetStuffFunctionInterface(const FName& funcName)
	{
		static TScriptInterface<ITIHHsBaseObject> emptyInterface;
		if (mFunctionTable.Contains(funcName))
		{
			return mFunctionTable[funcName];
		}
		return emptyInterface;
	}
	TScriptInterface<ITIHHsInteraction>& GetStuffInteractionInterface(const FName& interactionName)
	{
		static TScriptInterface<ITIHHsInteraction> emptyInterface;
		if (mInteractionTable.Contains(interactionName))
		{
			return mInteractionTable[interactionName];
		}
		return emptyInterface;
	}

	

	virtual void RegistFunction(const FName& functionNameKey, TScriptInterface<ITIHHsBaseObject> targetValue,
		ETIHHsFunctionRegistOvrrideOption overrideOption = ETIHHsFunctionRegistOvrrideOption::
		EAddOrOverride) override
	{
		switch (overrideOption)
		{
		case ETIHHsFunctionRegistOvrrideOption::ENone:
			break;
		case ETIHHsFunctionRegistOvrrideOption::EOverride:
			{
				if (mFunctionTable.Contains(functionNameKey))
				{
					mFunctionTable[functionNameKey] = targetValue;
				}
			}break;
		case ETIHHsFunctionRegistOvrrideOption::EAddOrOverride:
			{
				mFunctionTable.FindOrAdd(functionNameKey) = targetValue;
			}break;
		case ETIHHsFunctionRegistOvrrideOption::EFindOrNot:
			{
				if (not mFunctionTable.Contains(functionNameKey))
				{
					mFunctionTable.Add(functionNameKey,targetValue);
				}
			}break;
		}
	}

	virtual int32 GetProcessGenPriority() const override;
	virtual int32 ProcessWorldInteraction(TArray<TScriptInterface<ITIHHsBaseObject>>& others) override;
	virtual int32 GetObjectValidation() override;
	virtual bool CheckImplementFunction(FName functionName) const override;
	virtual int32 GetInitPriority(FTIHHsPriorityQueryList& list) const override;
	virtual void Attach(TScriptInterface<ITIHHsBaseObject>& parent) override;	
	virtual void SetTIHParent(TScriptInterface<ITIHHsBaseObject>& parent) override;
	virtual TScriptInterface<ITIHHsBaseObject> GetTIHParent() const override;
	virtual void AttachNotify(TScriptInterface<ITIHHsBaseObject>& child) override;
	virtual void Detach() override;
	virtual void DetachNotify(TScriptInterface<ITIHHsBaseObject> child) override;
	virtual FTIHHsObjectIndividuality& GetIndividuality() override;
	virtual bool ChangeIndividuality(const FTIHHsObjectIndividuality& individuality) override;
	virtual void ChangeIndividualityNotify(const FTIHHsObjectIndividuality& prevState) override;
	virtual void QueryAttachmentSteps(FTIHHsAttachmentStepQueryList& steps) const override;
	virtual const FTIHHsDeckSurfaceData& GetDeckSurfaceData() const override;
	virtual bool DetectPlacementTarget(FHitResult& outHitResult) override;
	virtual void VisitCollctingMetaData(UTIHHsMetaDataComponent* metaDataComponent) override;
	virtual void AnalizeMetaData() override;
	virtual void VisitCollectingIndividuality(UTIHHsStateTreeComponent* stateTreeComponent) override;
	virtual void VisitCollectingTrigger(FTIHHsTriggerVisitor& triggerVisitor) override;
	virtual int32 ProcessGeneration_Implementation(TArray<UActorComponent*>& others) override;
	virtual void RegistInteraction(const FName& functionNameKey, TScriptInterface<ITIHHsInteraction> targetValue,
		ETIHHsFunctionRegistOvrrideOption overrideOption = ETIHHsFunctionRegistOvrrideOption::EAddOrOverride) override;
	virtual const FTIHHsTag GetInteractionTag_InInteraction() const override;
	virtual void ActionEventDown_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionEventUp_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionEventDoubleTap_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoldStart_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoldOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoldEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessMoveStart_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessMoveOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessMoveEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoverStart_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoverOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoverEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;


	
protected:
	FTIHHsTag mTIHHsTag;
	FTIHHsState mTIHHsState; 

	TMap<FName,TScriptInterface<ITIHHsBaseObject>> mFunctionTable;
	TMap<FName,TScriptInterface<ITIHHsInteraction>> mInteractionTable;
	//TMap<FName,TFunction<void(const FTIHHsActionParamIn&)>> mActionEventTable;	//	액션 이벤트 테이블, 액션 이름과 액션 이벤트 함수의 맵
	
	TObjectPtr<class UTIHHsBaseRootComponent> mRootComponent;
	TObjectPtr<class UTIHHsMetaDataComponent> mMetaDataComponent;
	TObjectPtr<class UTIHHsPlacementComponent> mPlacementComponent;
	TObjectPtr<class UTIHHsTriggerComponent> mTriggerComponent;
	TObjectPtr<class UTIHHsDeckComponent> mDeckComponent;
	TObjectPtr<class UTIHHsStateTreeComponent> mStateTreeComponent;
	TObjectPtr<class UTIHHsTestAllComponent> mTestAllComponent;

	//	DRY
	template<typename T = UActorComponent>
	T* TryCreateBagicComponent_Internal(TObjectPtr<T>& outComponent, FName componentName,USceneComponent* rootComponent)
	{
		if (outComponent == nullptr)
		{
			outComponent = NewObject<T>(this, componentName);
		}
		if (outComponent)
		{
			outComponent->RegisterComponent();
			if (USceneComponent* sceneComponent = Cast<USceneComponent>(outComponent))
			{
				if (sceneComponent != GetRootComponent())
				{
					sceneComponent->AttachToComponent(rootComponent,FAttachmentTransformRules::KeepRelativeTransform);
				}
			}
			//outComponent->AttachObject(mRootComponent,FAttachmentTransformRules::KeepRelativeTransform);
			outComponent->SetIsReplicated(true);
		}
		return outComponent.Get();
	}

public:
	virtual void VisitCollctingInputAction(FTIHHsInputActionVisitor& actionVisitor) override;
	virtual FTIHHsAttachmentRuleWrapper GetAttachmentRuleWrapper() const override;
	virtual void ActionEventMoveAny_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;

protected:
	FInstancedPropertyBag mSharingPropertyBag;

	TMap<FName,FInstancedStruct> mContextStructArray;	//	컨텍스트 구조체 컨테이너, 액션 컨텍스트 구조체를 저장하는 컨테이너
};

inline void ATIHHsStuffBase::ActionEventMoveAny_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	ITIHHsInteraction::ActionEventMoveAny_Deprecated(actionParamIn);
}

inline FTIHHsAttachmentRuleWrapper ATIHHsStuffBase::GetAttachmentRuleWrapper() const
{
	return ITIHHsBaseObject::GetAttachmentRuleWrapper();
}

inline void ATIHHsStuffBase::VisitCollctingInputAction(FTIHHsInputActionVisitor& actionVisitor)
{
	
}
//FWorldInitializationValues
USTRUCT()
struct FTIHHsInitializationValues
{
	GENERATED_BODY()

	UPROPERTY()
	bool IsCreated_SurfaceProcRegistrySubSystem = false;
	UPROPERTY()
	bool IsCreated_TapActionSubSystem = false;
	
	FWorldInitializationValues UeWorld_InitializationValues;
};

UCLASS()
class TIHHOUSINGSYSTEM_API UTIHHsFrameWorkSupportSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	

public://UWorld* /*World*/, const UWorld::InitializationValues /*IVS*/
	void PostWorldInitialization(UWorld* inWorld, const UWorld::InitializationValues IVS);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		return true;
	}
private:
	UWorld* mWorld;
	TObjectPtr<ATIHHsStuffBase> mTestStuffRuntimeActor = nullptr;
};

