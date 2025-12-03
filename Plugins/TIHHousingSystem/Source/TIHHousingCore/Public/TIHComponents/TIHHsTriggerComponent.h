// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TIHHsCore.h"
#include "BaseGizmos/GizmoActor.h"

#include "Components/SceneComponent.h"
#include "Cores/TIHHsCoreClasses.h"
#include "TIHHsTriggerComponent.generated.h"


class UTIHHsActionSystem;

UENUM(BlueprintType)
enum ETIHHsTriggerShape : uint8
{
	ETriggerShape_Arrow,
	ETriggerShape_Box,
	ETriggerShape_Sphere,
	ETriggerShape_Capsule,
	ETriggerShape_Plane,
	ETriggerShape_Custom
};

UENUM(BlueprintType)
enum ETIHHsTriggerPosMethodTypes : uint8
{
	ETriggerPosMethod_Absolute,
	ETriggerPosMethod_Relative,
	ETriggerPosMethod_Custom
};

USTRUCT()
struct TIHHOUSINGCORE_API FTIHHsTriggerPosMethodParams
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform Transform;

	TFunction<bool(USceneComponent*)> PosMethodFunction;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTIHHsTriggerActionCallBackDelegate);

struct FTIHHsTriggerRegister;
struct FTIHHsTriggerRegisterChildren;

USTRUCT(BlueprintType)
struct FTIHHsTriggerRegister
{
	GENERATED_BODY()

	UPROPERTY()
	UClass* UEClass;

	UPROPERTY()
	FName TriggerFunctionName;
	
	UPROPERTY()
	FTransform Offset;

	UPROPERTY()
	FInstancedPropertyBag AddtionalProperties;

	FTIHHsTriggerRegister()
		: UEClass(nullptr), TriggerFunctionName(NAME_None), Offset(FTransform::Identity) {}
	FTIHHsTriggerRegister(UClass* InUEClass, FName InTriggerFunctionName, const FTransform& InOffset)
		: UEClass(InUEClass), TriggerFunctionName(InTriggerFunctionName), Offset(InOffset) {}
	FTIHHsTriggerRegister(const FTIHHsTriggerRegister& Other)
		: UEClass(Other.UEClass), TriggerFunctionName(Other.TriggerFunctionName), Offset(Other.Offset), AddtionalProperties(Other.AddtionalProperties) {}
	FTIHHsTriggerRegister(FTIHHsTriggerRegister&& Other) noexcept
		: UEClass(MoveTemp(Other.UEClass)), TriggerFunctionName(MoveTemp(Other.TriggerFunctionName)), Offset(MoveTemp(Other.Offset)), AddtionalProperties(MoveTemp(Other.AddtionalProperties)) {}
	FTIHHsTriggerRegister& operator=(const FTIHHsTriggerRegister& Other)
	{
		if (this != &Other)
		{
			UEClass = Other.UEClass;
			TriggerFunctionName = Other.TriggerFunctionName;
			Offset = Other.Offset;
			AddtionalProperties = Other.AddtionalProperties;
		}
		return *this;
	}
	FTIHHsTriggerRegister& operator=(FTIHHsTriggerRegister&& Other) noexcept
	{
		if (this != &Other)
		{
			UEClass = MoveTemp(Other.UEClass);
			TriggerFunctionName = MoveTemp(Other.TriggerFunctionName);
			Offset = MoveTemp(Other.Offset);
			AddtionalProperties = MoveTemp(Other.AddtionalProperties);
		}
		return *this;
	}
};



UCLASS()
class UTIHHsConnectionNode : public UObject, public ITIHHsBaseObject
{
	GENERATED_BODY()
public:
	virtual int32 ProcessGeneration_Implementation(TArray<UActorComponent*>& others) override;

	UPROPERTY()
	UTIHHsConnectionNode* ParentNode = nullptr;	
	/*
	 *	tag 와 함수
	 *	UObject로 만들거임.
	 *	만약 
	 * 
	 */
	TSet<TObjectPtr<UTIHHsConnectionNode>> Children;
	
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FTIHHsActionableDelegate, const FTIHHsActionParamIn&, ActionParamIn);

USTRUCT()
struct FTIHHsTriggerRegistryCallBackDetail
{
	GENERATED_BODY()
	//	트리거 콜백 함수, 액션 파라미터를 받는 함수
	TFunction<void(const FTIHHsActionParamIn&)> CallBackFunction;
	//	트리거 콜백 인터페이스, 인터페이스 형태로 저장됨
	TScriptInterface<ITIHHsInteraction> CallBackInterface;
	/*
	 *
	 *	if CallBackFunctionType == ETIHHsTriggerFunctionType::ECallInterface:
	 *	if CallBackFunctionName == "ActionEventTap": 
	 *		
	 */
	
	//	액션 가능한 델리게이트, 액션 파라미터를 받는 델리게이트
	UPROPERTY()
	FTIHHsActionableDelegate ActionableDelegate;
	//	트리거 콜백 함수 이름, 일반 함수 형태로 저장됨
	FName CallBackFunctionName;
	//	트리거 콜백 함수 타입, 람다 함수, 인터페이스 호출, 일반 함수 등등
	ETIHHsTriggerFunctionType CallBackFunctionType = ETIHHsTriggerFunctionType::ELambda;	
};

USTRUCT()
struct FTIHHsTriggerRegistryNode
{
	GENERATED_BODY()

	UPROPERTY()
	FString MeshPath;
	
	UPROPERTY()
	FTransform Offset;

	UPROPERTY()
	TMap<FName, FTIHHsTriggerRegistryCallBackDetail> CallBacks;	//	트리거 콜백 함수들, 이름과 콜백 함수의 맵

	/*
	 *	필요없잖아.
	 *	TODO: 나중에 앞뒤 관계에 관한거 추가,해당 노드를 어떻게 취급할지도 추가
	 *	
	 * 
	 */
	
};

USTRUCT()
struct FTIHHsTriggerRegistryContainer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FTIHHsTriggerRegistryNode> Nodes;	//	트리거 레지스트리 노드들, 노드의 배열
};

UCLASS()
class UTIHHsTriggerContext : public UObject
{
	GENERATED_BODY()

public:
	float TriggerUpdateTime = 0.1f;	//	트리거 업데이트 시간, 트리거가 업데이트되는 시간 간격
};

UCLASS()
class UTIHHsProbeTraceMouse : public UObject,public ITIHHsInteraction
{
	GENERATED_BODY()
	friend class ATIHHsTriggerInteractableActor;
	static FTimerManager* gTimerManager;
public:
	UWorld* mWorldContext;
	UFUNCTION(BlueprintCallable)
	void SetWorldContext(UWorld* worldContext)
	{
		mWorldContext = worldContext;
	}
	UFUNCTION(BlueprintCallable)
	void TraceBegin(TScriptInterface<ITIHHsInteraction> target);
	UFUNCTION()
	void TraceUpdate(const FTIHHsActionParamIn& paramIn);
	UFUNCTION(BlueprintCallable)
	void TraceEnd();

private:
	//	마우스의 상태를 가져오는게 필요함.
	TScriptInterface<ITIHHsInteraction> mTarget;	//	트레이스 대상 인터페이스, 트레이스가 시작될 때 대상 인터페이스를 저장함
	FPlane mMovementPlane;	//	마우스 트레이스 평면, 마우스가 움직이는 평면
	bool mIsTracing = false;	//	트레이스 중인지 여부
	FDelegateHandle mOnMouseMoveHandle;	//	마우스 이동 핸들, 마우스가 움직일 때 호출되는 델리게이트 핸들
	UTIHHsActionSystem* mActionSystem;

	TObjectPtr<class UTIHHsPlaneSurface> mMovementSurface = nullptr;	//	마우스 트레이스 평면 서피스, 마우스가 움직이는 평면 서피스
	FHitResult mHitResult;
};

UCLASS()
class UTIHHsProbeTraceTarget : public UObject
{
	GENERATED_BODY()
	//static FTimerManager* gTimerManager;
public:
	UFUNCTION(BlueprintCallable)
	void TraceBegin(USceneComponent* subject,USceneComponent* objective,bool onLoop = true,UCurveFloat* curveFloat = nullptr)
	{
		if (not mIsTracing)
		{
			if (subject ==nullptr|| objective==nullptr)
			{
				mIsTracing = false;
				return;
			}
			mIsTracing = true;
			mSubjectComponent = subject;
			mObjectiveComponent = objective;
			mLastElapsedTime = 0.0f;
			mCurveFloat = curveFloat;
		
			if (mTimerMgr == nullptr)
			{
				mTimerMgr = &mSubjectComponent->GetWorld()->GetTimerManager();
			}
			const FTransform objW = mObjectiveComponent->GetComponentTransform();
			const FTransform subW = mSubjectComponent->GetComponentTransform();
			mRelativeTransform = subW.GetRelativeTransform(objW);	//	상대 트랜스폼 계산, 목표 컴포넌트와 대상 컴포넌트의 상대 트랜스폼을 계산함
			//mTimerMgr->SetTimer(mLoopTimerHandle, this, &UTIHHsProbeTraceTarget::TraceUpdate, 0.1f, onLoop);
		}
	}
	UFUNCTION(BlueprintCallable)
	void TraceEnd();
	
	TFunction<bool(USceneComponent*,USceneComponent*)> EndConditionFunction = nullptr;	//	트레이스 종료 조건 함수, 트랜스폼 대상 컴포넌트를 인자로 받는 함수
	void TraceUpdate();
private:
	
	bool mIsTracing = false;	//	트레이스 중인지 여부
	float mLastElapsedTime = 0.f;
	FTimerManager* mTimerMgr = nullptr;	//	타이머 매니저, 트레이스 중일 때 타이머를 사용하여 트랜스폼을 업데이트함
	UCurveFloat* mCurveFloat = nullptr;	//	트레이스 커브, 트랜스폼 대상 컴포넌트의 트랜스폼을 업데이트할 때 사용됨
	USceneComponent* mSubjectComponent = nullptr;	//	트랜스폼 대상 컴포넌트
	USceneComponent* mObjectiveComponent = nullptr;	//	트랜스폼 목표 컴포넌트
	FTimerHandle mLoopTimerHandle;	//	트레이스 타이머 핸들, 트레이스 중일 때 타이머를 사용하여 트랜스폼을 업데이트함
	FTransform mRelativeTransform;	//	나중에 offset으로 대체
};


UCLASS()
class ATIHHsTriggerInteractableActor : public AActor, public ITIHHsInteraction
{
	GENERATED_BODY()
	friend class UTIHHsTriggerComponent;
public:
	virtual void ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;

	FTransform mOffset;
	ATIHHsTriggerInteractableActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void ActionProcessHoldStart_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoldOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void ActionProcessHoldEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	
	TObjectPtr<UTIHHsProbeTraceTarget> mTraceTarget;
	TObjectPtr<UTIHHsProbeTraceMouse> mTraceMouse;
	TObjectPtr<class UTIHHsProbeTraceMouse2> mTraceMouseProbe;
protected:
	UStaticMeshComponent* mStaticMeshComponent = nullptr;
	UTIHHsTriggerComponent* mLinkedTriggerComponent = nullptr;	//	트리거 컴포넌트
};

USTRUCT()
struct FTIHHsTriggerLink
{
	GENERATED_BODY()

	UPROPERTY()
	UTIHHsTriggerComponent* TriggerComponent = nullptr;

	// UPROPERTY()
	// ATIHHsTriggerGizmo* TriggerGizmo = nullptr;

	UPROPERTY()
	FTIHHsObjectIndividuality Individuality;	//	개체의 개별성
	
	UPROPERTY()
	FTransform Transform;
	UPROPERTY()
	FString MeshPath;

	/*
	 *	
	 * 
	 */
};



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TIHHOUSINGCORE_API UTIHHsTriggerComponent : public USceneComponent, public ITIHHsBaseObject, public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTIHHsTriggerComponent();

	void OffVisibleTriggerGizmo()
	{
		if (mInteractableActor)
		{
			mInteractableActor->Destroy();
			mInteractableActor = nullptr;
		}
		mVisibleTriggerGizmo = false;
	}
	void OnVisibleTriggerGizmo()
	{
		if (mInteractableActor == nullptr)
		{
			FTransform transform = GetComponentTransform();
			transform.SetLocation(transform.GetLocation() + FVector(0, 0, 100));
			mInteractableActor = GetWorld()->SpawnActor<ATIHHsTriggerInteractableActor>(ATIHHsTriggerInteractableActor::StaticClass(), transform);
			
			mInteractableActor->mLinkedTriggerComponent = this;
			mInteractableActor->InteractionInit();
		}
		mVisibleTriggerGizmo = true;
	}
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool mVisibleTriggerGizmo = false;	//	트리거 기즈모를 보일지 여부
	TObjectPtr<ATIHHsTriggerInteractableActor> mInteractableActor = nullptr;	//	트리거 인터랙션 액터
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;
	virtual void VisitCollectingTrigger(FTIHHsTriggerVisitor& triggerVisitor) override;

	UFUNCTION(BlueprintCallable)
	void RegisterTrigger(const FTIHHsTriggerRegister& triggerRegister);
	virtual int32 ProcessGeneration_Implementation(TArray<UActorComponent*>& others) override;
	virtual void VisitCollctingInputAction(FTIHHsInputActionVisitor& actionVisitor) override;

	TArray<FTIHHsTriggerRegister> mRegisterData;
};

UCLASS()
class UTIHHsProbeAttachTest : public UTIHHsProbeBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void AttachTest(TScriptInterface<ITIHHsBaseObject> parent,TScriptInterface<ITIHHsBaseObject> child);
	virtual void Init(FTIHHsProbeInitData initData) override;
	virtual void StartProbe() override;
	virtual bool IsProbeRunning() override;
	virtual void UpdateProbe(float deltaTime) override;
	virtual void StopProbe() override;

	
};
