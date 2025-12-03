// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Subsystems/WorldSubsystem.h"
#include "TIHHsCore.h"
#include "TIHHsActionSystem.generated.h"




UCLASS()
class UTIHHsInteractionObject : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void ActionBegin(const FTIHHsActionParamIn& param) {};
	
	
	
	UFUNCTION()
	virtual void ActionEnd(const FTIHHsActionParamIn& param) {};
	
	FTIHActionGestureDelegate OnActionBeginDelegate;
	FTIHActionGestureDelegate OnActionEndDelegate;
};

// UCLASS()
// class UTIHHsActionSystemParts : public UObject, public ITIHHsInteraction
// {
// 	GENERATED_BODY()
//
// public:
// 	UFUNCTION()
// 	virtual void ActionEventDown(const FTIHHsActionParamIn& actionParamIn) override;
// 	UFUNCTION()
// 	virtual void ActionEventUp(const FTIHHsActionParamIn& actionParamIn) override;
// };

UCLASS()
class UTIHHsActionLayer : public UObject //, public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	//	ETraceTypeQuery TraceChannel, bool bTraceComplex, FHitResult& HitResult
	//	struct FHitResult& OutHit,const FVector& Start,const FVector& End,ECollisionChannel TraceChannel,const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam
	UFUNCTION(BlueprintCallable)
	void Init(
		UWorld* world,
		UTIHHsActionSystem* defaultActionSystem,
		ECollisionChannel defaultTraceChannel,
		bool defaultIsTraceComplex,

		FVector defaultLastWorldLocation,
		FVector defaultmLastWorldDirection,
		float defaultmLastTraceDistance 
	);
	UFUNCTION(BlueprintCallable)
	void SetUpLineTrace(ECollisionChannel traceChannel,const FVector& start,const FVector& direction,float traceDistance);
	UFUNCTION(BlueprintCallable)
	void SetUpLinTraceUnderMouse();
	
	UFUNCTION(BlueprintCallable)
	bool PerformLineTrace(FHitResult& hitResult);
	
private:
	UWorld* mWorld;
	UTIHHsActionSystem* mActionSystem;
	ETraceTypeQuery mCurrentTraceTypeQuery = ETraceTypeQuery::TraceTypeQuery1;
	bool mIsTraceComplex = false;
	FHitResult mLastHitResult;
	FVector mLastWorldLocation = FVector::ZeroVector;
	FVector mLastWorldDirection = FVector::ZeroVector;
	float mLastTraceDistance = 1000.0f;
	ECollisionChannel mLastTraceChannel = ECollisionChannel::ECC_Visibility;
};

USTRUCT(Blueprintable)
struct TIHHOUSINGCORE_API FTIHHsDBRole : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "DBRole",meta=(DisplayName="role_name"))
	FString RoleName;	//	역할 이름
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "DBRole",meta=(DisplayName="role_path"))
	FString RolePath;	//	역할 경로
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "DBRole",meta=(DisplayName="role_processor"))
	FString RoleProcessor;	//	역할 처리자

};
//	추후에 이름을 변경할 예정
USTRUCT(Blueprintable)
struct TIHHOUSINGCORE_API FTIHHsActionProfile : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "ActionProfile",meta=(DisplayName="action_name"))
	FString ActionName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "ActionProfile",meta=(DisplayName="asset_path"))
	FString AssetPath;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "ActionProfile",meta=(DisplayName="is_enabled"))
	int32 IsEnabled;
};
USTRUCT(Blueprintable)
struct TIHHOUSINGCORE_API FTIHHsActionMappingContextPathDBRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "ActionMappingContextDBRow",meta=(DisplayName="context_name"))
	FString ContextName;	//	액션 매핑 컨텍스트 이름

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "ActionMappingContextDBRow",meta=(DisplayName="context_path"))
	FString ContextPath;	//	액션 매핑 컨텍스트 경로
};

USTRUCT(Blueprintable)
struct FTIHHsActionParamOut
{
	GENERATED_BODY()

	UPROPERTY()
	FString ActionName;	//	액션 이름
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTIHHsActionMappingChnageDelegate, const FName&, PrevMappingContextName, const FName&, NewMappingContextName);

DECLARE_MULTICAST_DELEGATE_TwoParams(FTIHHsActionMappingRegistryProcDelegate, UEnhancedInputComponent*, const FEnhancedActionKeyMapping&);
DECLARE_MULTICAST_DELEGATE_OneParam(FTIHHsActionMappingUnRegistryProcDelegate,UEnhancedInputComponent*);

USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsActionMappingProcessorWrapper
{
	GENERATED_BODY()

	TMap<ETriggerEvent,uint32> BindingHandles;	//	트리거 이벤트와 바인딩 핸들 맵
	
	FTIHHsActionMappingRegistryProcDelegate RegistryDelegate;
	FTIHHsActionMappingUnRegistryProcDelegate UnRegistryDelegate;
};
/*	
 *	IA_main-select
 *	IA_sub-select
 *	IA_cancel
 *	 *	IA_keyMouse_MouseMove
 *	 
 *
 * 
 */
USTRUCT()
struct FTIHHsActionMappingContextPair
{
	GENERATED_BODY()

	UPROPERTY()
	FName Name;	//	현재 액션 매핑 컨텍스트 이름
	UPROPERTY()
	UInputMappingContext* Object;	//	현재 액션 매핑 컨텍스트
};
USTRUCT()
struct FTIHHsActionDownUpRecordState
{
	GENERATED_BODY()

	UPROPERTY()
	bool IsHit = false;	//	히트 여부, 액션이 발생한 위치에 오브젝트가 있는지 여부
	UPROPERTY()
	bool IsSequence = false;	//	시퀀스 여부, 액션이 시퀀스에서 발생한 경우 true
	UPROPERTY()
	bool HasActor = false;	//	액션이 발생한 위치에 액터가 있는지 여부
	UPROPERTY()
	bool HasComponent = false;	//	액션이 발생한 위치에 컴포넌트가 있는지 여부

	int32 GetCompactValue() const
	{
		return (IsHit ? 1 : 0) | (IsSequence ? 2 : 0) | (HasActor ? 4 : 0) | (HasComponent ? 8 : 0);
	}
	void Clear()
	{
		IsHit = false;
		IsSequence = false;
		HasActor = false;
		HasComponent = false;
	}
};
USTRUCT()
struct FTIHHsActionDownUpRecord
{
	GENERATED_BODY()

	UPROPERTY()
	FTIHHsActionDownUpRecordState PreviousState;	//	제스처 이름, 액션 이름과 관련된 정보
	UPROPERTY()
	FTIHHsActionDownUpRecordState CurrentState;
	
	UPROPERTY()
	FTIHHsActionParamIn ParamIn;

	UPROPERTY()
	float ActionTime = 0.0f;	//	액션 시간, 액션이 시작된 시간

	UPROPERTY()
	int32 ActionType = 0;	//	액션 타입, 0:None, 1: down, 2: up,

	UPROPERTY()
	FHitResult HitResult;	//	히트 결과, 액션이 발생한 위치와 관련된 정보

	void RecordCurrentState()
	{
		//CurrentState.IsSequence = CurrentState.IsHit;
		CurrentState.IsHit = HitResult.IsValidBlockingHit();
		CurrentState.HasActor = HitResult.GetActor() != nullptr;
		CurrentState.HasComponent = HitResult.GetComponent() != nullptr;
	}
	
	void Clear()
	{
		PreviousState.Clear();
		CurrentState.Clear();
		ParamIn.Clear();
		ActionTime = 0.0f;
		ActionType = 0;
		HitResult = FHitResult();
	}
	void RecordPreviousState()
	{
		PreviousState = CurrentState;	//	현재 상태를 이전 상태로 저장
	}
};
class FTIHHsInterfaceInvoker
{
public:
	using MethodPtr = void (ITIHHsInteraction::*)(const FTIHHsActionParamIn&);

	FTIHHsInterfaceInvoker(
		const TSparseArray<TScriptInterface<ITIHHsInteraction>>& inArray,
		MethodPtr inMethod
	):
		mRefArray(inArray),
		mInterfaceMethod(inMethod)
	{}
	void operator()(const FTIHHsActionParamIn& paramIn) const
	{
		for (const auto& interfaceObj : mRefArray)
		{
			if (interfaceObj.GetInterface() != nullptr)
			{
				//	인터페이스가 유효한 경우에만 호출
				(interfaceObj.GetInterface()->*mInterfaceMethod)(paramIn);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("TTIHHsInterfaceInvoker: Interface is null for object: %s"), *interfaceObj.GetObject()->GetName());
			}
		}
	}
private:
	const TSparseArray<TScriptInterface<ITIHHsInteraction>>& mRefArray;
	MethodPtr mInterfaceMethod;
};

/*
 *	인터페이스가 들어오고 빠지고를 위해서 만든건데 지금 안쓰고 있긴해.
 *	
 * 
 */
UCLASS()
class UTIHHsInterfaceRegistry : public UObject
{
	GENERATED_BODY()

public:
	using TIHInterfaceMethodPtr = void (ITIHHsInteraction::*)(const FTIHHsActionParamIn&);
	using TIHSparseArray = TSparseArray<TScriptInterface<ITIHHsInteraction>>;	//	인터페이스 레지스트리 배열, 인터페이스를 등록하는 배열

	UTIHHsInterfaceRegistry():
	mDirtyFlag(false)
	{
		mRegistryArray.Reserve(64);
		mInterfaceTable.Reserve(64);
	}
	void SetMethodPtr(TIHInterfaceMethodPtr methodPtr)
	{
		mMethodPtr = methodPtr;
		if (mMethodPtr != nullptr)
		{
			mDirtyFlag = true;
		}
	}
	UFUNCTION()
	void BroadCastMethod(const FTIHHsActionParamIn& paramIn)
	{
		if (mDirtyFlag)
		{
			Normalize();
		}
		if (mMethodPtr != nullptr)
		{
			for (const auto& interfaceObj : mRegistryArray)
			{
				if (interfaceObj.GetInterface() != nullptr)
				{
					//	인터페이스가 유효한 경우에만 호출
					(interfaceObj.GetInterface()->*mMethodPtr)(paramIn);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("TTIHHsInterfaceInvoker: Interface is null for object: %s"), *interfaceObj.GetObject()->GetName());
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UTIHHsInterfaceRegistry: MethodPtr is null, cannot invoke method."));
		}
	}
	UFUNCTION()
	bool RegisterInterface(const TScriptInterface<ITIHHsInteraction>& interfaceObj)
	{
		bool result = false;
		if (interfaceObj.GetInterface() != nullptr)
		{
			FObjectKey objKey(interfaceObj.GetObject());
			if (not mInterfaceTable.Contains(objKey))
			{
				int32 idx = mRegistryArray.Add(interfaceObj);
				mInterfaceTable.Add(objKey, idx);
				result = true;
				mDirtyFlag = true;
			}
		}
		return result;
	}
	UFUNCTION()
	void UnRegisterInterafce(const TScriptInterface<ITIHHsInteraction>& interfaceObj)
	{
		if (interfaceObj.GetInterface() != nullptr)
		{
			FObjectKey objKey(interfaceObj.GetObject());
			if (mInterfaceTable.Contains(objKey))
			{
				mRegistryArray.RemoveAt(mInterfaceTable[objKey]);
				mInterfaceTable.Remove(objKey);
				mDirtyFlag = true;
			}
		}
	}
	UFUNCTION()
	void UnRegisterInterafceByObject(const UObject* interfaceObj)
	{
		if (interfaceObj != nullptr)
		{
			FObjectKey objKey(interfaceObj);
			if (mInterfaceTable.Contains(objKey))
			{
				mRegistryArray.RemoveAt(mInterfaceTable[objKey]);
				mInterfaceTable.Remove(objKey);
				mDirtyFlag = true;
			}
		}
	}

private:
	void Normalize()
	{
		if (mRegistryArray.IsCompact()) return;
		
		for (int32 i = mRegistryArray.GetMaxIndex(); -1 < i; --i)
		{
			if (mRegistryArray.IsValidIndex(i))
			{
				bool isRemove = true;
				isRemove &= mRegistryArray[i].GetObject() == nullptr;
				if (not isRemove)
				{
					FObjectKey objKey(mRegistryArray[i].GetObject());
					isRemove &= objKey.ResolveObjectPtr() == nullptr;
					isRemove &= not mInterfaceTable.Contains(objKey);
				}
				isRemove &= mRegistryArray[i].GetInterface() == nullptr;
				if (isRemove)
				{
					mRegistryArray.RemoveAt(i);
					mInterfaceTable.Remove(FObjectKey(mRegistryArray[i].GetObject()));
				}
			}
		}
		mRegistryArray.Compact();
		mInterfaceTable.Empty();
		mInterfaceTable.Reserve(mRegistryArray.GetMaxIndex());
		auto beginItor = mRegistryArray.begin();
		auto endItor = mRegistryArray.end();
		for (auto i = beginItor; i!= endItor;++i)
		{
			mInterfaceTable.Add(FObjectKey( i->GetObject()), i.GetIndex());
		}
		mDirtyFlag = false;
	}
	bool mDirtyFlag = false;	//	더티 플래그, 레지스트리 배열이 변경되었는지 여부
	
	TIHSparseArray mRegistryArray;	//	인터페이스 레지스트리 배열, 인터페이스를 등록하는 배열
	TMap<FObjectKey, uint32> mInterfaceTable;
	TIHInterfaceMethodPtr mMethodPtr;
};

USTRUCT()
struct FTIHHsActionHoldEndData
{
	GENERATED_BODY()

	UPROPERTY()
	FHitResult HitResult;
};

UCLASS()
class ATTTT : public AActor
{
	GENERATED_BODY()

public:
	
};


UCLASS()
class TIHHOUSINGCORE_API UTIHHsActionPressGesture : public UObject
{
	GENERATED_BODY()
	friend class UTIHHsActionSystem;
	//static FTimerManager* gTimerManager;
	//static UWorld* gWorld;
	FTimerManager* mTimerMgr;
	class UTIHHsActionLayer* mActionLayer;

public:
	UWorld* mWorldContext;
	TScriptInterface<ITIHHsInteraction> mHoldTarget;
	FTIHHsActionHoldEndData mHoldEndData;	//	홀드 종료 파라미터, 홀드가 종료될 때 호출되는 파라미터
	FTIHHsActionDownUpRecord mActionDownRecord;	//	액션 다운 레코드, 액션이 발생한 위치와 관련된 정보
	FTIHHsActionDownUpRecord mActionUpRecord;	//	액션 업 레코드, 액션이 발생한 위치와 관련된 정보
	virtual void BeginDestroy() override;

	
	void Initialize(const FString& gestureName,class UTIHHsActionLayer* actionLayer, float expiredTime = 0.2f);

	bool IsDownHasObject() const
	{
		return mActionDownRecord.HitResult.GetActor() != nullptr || mActionDownRecord.HitResult.GetComponent() != nullptr;
	}
	
	bool IsDownHit() const
	{
		return mActionDownRecord.HitResult.IsValidBlockingHit() && IsDownHasObject();
	}
	
	void ActionDown()
	{
		AActor* prevActor = mActionDownRecord.HitResult.GetActor();
		FTimerManager& TimerManager = *mTimerMgr;
		{
			UE_LOG(LogTemp, Warning, TEXT("ActionDown %s"), *mGestureName);
			
			ProcessPlayerActionUp(mActionDownRecord, 1);
			
			OnGestureDelegateDown.Broadcast(mActionDownRecord.ParamIn);
			for (auto consignObj :mConsignInteractObjSArr)
			{
				if (consignObj.GetInterface() !=nullptr)
				{
					consignObj->ActionEventDown_Deprecated(mActionDownRecord.ParamIn);
				}
			}
		}
		
		if (TimerManager.IsTimerActive( mTimerHandleDoubleTapEvaluation))
		{
			mIsDoubleTapping = true;
			TimerManager.ClearTimer(mTimerHandleDoubleTapEvaluation);
			UE_LOG(LogTemp, Warning, TEXT("DoubleTap %s"), *mGestureName);
			FTIHHsActionParamIn param(mGestureName,"doubleTap");
			OnGestureDelegateDoubleTap.Broadcast(FTIHHsActionParamIn(mGestureName,"doubleTap"));
			for (auto consignObj :mConsignInteractObjSArr)
			{
				if (consignObj.GetInterface() !=nullptr)
				{
					consignObj->ActionEventDoubleTap_Deprecated(param);
				}
			}
			/*
				d0 u0 d1
				d0 == d1 && d1 == u0 == double tap
				d1 == u0 : IsSameGestureActor
				d0 == d1 : IsSequence
				d0 == u0 == d1 : double tap
				d0 == u0 != d1 : tap
				d0 == d1 != u0 : 지랄 던지고 잡고 던지고 잡고임.
			*/

			if (mActionDownRecord.CurrentState.IsSequence && IsSameGestureActor())
			{
				TScriptInterface<ITIHHsInteraction> hitActor(mActionDownRecord.HitResult.GetActor());
				if (hitActor.GetInterface() != nullptr)
				{
					hitActor->ActionEventDoubleTap_Deprecated(param);
				}
			}
			else if (mActionUpRecord.CurrentState.HasActor && prevActor == mActionUpRecord.HitResult.GetActor())
			{
				TScriptInterface<ITIHHsInteraction> hitActor(mActionUpRecord.HitResult.GetActor());
				if (hitActor.GetInterface() != nullptr)
				{
					param.ActionData = TEXT("tap");
					hitActor->ActionEventTap_Deprecated(param);
				}
			}
			//ProcessCallHitActor(param);
		}
		else //	first tap
		{
			mIsPressing = true;
			mIsDoubleTapping = false;
			
			TimerManager.SetTimer(mTimerHandleDoubleTapEvaluation,this,&UTIHHsActionPressGesture::DoubleTapExpired,mExpiredTime,false);
		}
		
	}
	bool IsSameGestureActor() const
	{
		return mActionDownRecord.CurrentState.HasActor && mActionUpRecord.CurrentState.HasActor && mActionDownRecord.HitResult.GetActor() == mActionUpRecord.HitResult.GetActor();
	}

	//	TODO: TraceTypeQuery1 이거 변경해야함. 변수로. 글로벌 변수로.
	void ProcessPlayerActionUp(FTIHHsActionDownUpRecord& curActionDownUp, int32 upDownFlag)
	{
		APlayerController* playerController = mWorldContext->GetFirstPlayerController();
		AActor* prevActor = curActionDownUp.HitResult.GetActor();
		curActionDownUp.RecordPreviousState();
		
		if (playerController&&playerController->GetHitResultUnderCursorByChannel(TraceTypeQuery1,false,curActionDownUp.HitResult))
		{
			//	collision for null
		}
		curActionDownUp.RecordCurrentState();
		if (curActionDownUp.CurrentState.HasActor && curActionDownUp.HitResult.GetActor() == prevActor)
		{
			curActionDownUp.CurrentState.IsSequence = true;
		}
		curActionDownUp.ActionTime = mWorldContext->GetTimeSeconds();
		curActionDownUp.ActionType = upDownFlag;	//	1: down
	}
	FTIHHsActionParamIn mHoldEndParam;	//	홀드 종료 파라미터, 홀드가 종료될 때 사용됨
	void ActionUp()
	{
		FTimerManager& TimerManager = *mTimerMgr;
		{
			UE_LOG(LogTemp, Warning, TEXT("ActionUp %s"), *mGestureName);

			FTIHHsActionDownUpRecord& curActionDownUp = mActionUpRecord;
			int32 upDownFlag = 2;
			
			ProcessPlayerActionUp(curActionDownUp, upDownFlag);
			
			OnGestureDelegateUp.Broadcast(mActionUpRecord.ParamIn);
			for (auto consignObj :mConsignInteractObjSArr)
			{
				if (consignObj.GetInterface() !=nullptr)
				{
					consignObj->ActionEventUp_Deprecated(mActionUpRecord.ParamIn);
				}
			}
		}
		if (TimerManager.IsTimerActive(mTimerHandleHoldOngoingLoop))
		{
			UE_LOG(LogTemp, Warning, TEXT("HoldEnd %s"), *mGestureName);
			TimerManager.ClearTimer(mTimerHandleHoldOngoingLoop);
			
			OnGestureDelegateHoldEnd.Broadcast(mHoldEndParam);
			for (auto consignObj :mConsignInteractObjSArr)
			{
				if (consignObj.GetInterface() !=nullptr)
				{
					consignObj->ActionProcessHoldEnd_Deprecated(mHoldEndParam);
				}
			}
			if (mHoldTarget.GetInterface() !=nullptr)
			{
				if (mActionLayer)
				{
					mActionLayer->SetUpLinTraceUnderMouse();
					mActionLayer->PerformLineTrace(mHoldEndData.HitResult);
				}
				mHoldEndParam.PropertyBag.SetValueStruct(TEXT("hold-end-data"), mHoldEndData);
				mHoldTarget->ActionProcessHoldEnd_Deprecated(mHoldEndParam);
			}
			mHoldTarget.SetObject(nullptr);
			mHoldTarget.SetInterface(nullptr);
			
		}
		else if (TimerManager.IsTimerActive(mTimerHandleDoubleTapEvaluation))
		{
			mIsPressing = false;
		}
	}
	
	void StopHold()
	{
		FTimerManager& TimerManager = *mTimerMgr;
		if (TimerManager.IsTimerActive(mTimerHandleHoldOngoingLoop))
		{
			TimerManager.ClearTimer(mTimerHandleHoldOngoingLoop);
			OnGestureDelegateHoldEnd.Broadcast(FTIHHsActionParamIn(mGestureName,"holdEnd"));
		}
	}
	void StopTaps()
	{
		if (mTimerMgr == nullptr)
		{
			return;
		}
		FTimerManager& TimerManager = *mTimerMgr;
		
		bool isOnTimerClear = true;
		
		isOnTimerClear &= mTimerHandleDoubleTapEvaluation.IsValid();
		isOnTimerClear &= TimerManager.IsTimerPending(mTimerHandleDoubleTapEvaluation);
		//isOnTimerClear &= TimerManager.TimerExists(mTimerHandleDoubleTapEvaluation);	//	여기 계속 문제가 생김
		//isOnTimerClear &= TimerManager.IsTimerActive(mTimerHandleDoubleTapEvaluation);
		if (isOnTimerClear)
		{
			TimerManager.ClearTimer(mTimerHandleDoubleTapEvaluation);
			mTimerHandleDoubleTapEvaluation.Invalidate();
			mIsPressing = false;
			mIsDoubleTapping = false;
		}
	}
	
	void StopAction()
	{
		StopTaps();
		StopHold();
	}
	
	void Clear()
	{
		StopAction();
		OnGestureDelegateDown.Clear();
		OnGestureDelegateUp.Clear();
		OnGestureDelegateTap.Clear();
		OnGestureDelegateDoubleTap.Clear();
		OnGestureDelegateHoldStart.Clear();
		OnGestureDelegateHoldOngoing.Clear();
		OnGestureDelegateHoldEnd.Clear();
		mGestureName.Empty();
		mIsPressing = false;
		mIsDoubleTapping = false;

		for (auto consignObj :mConsignInteractObjSArr)
		{
			UObject* obj = consignObj.GetObject();
			if (obj && mConsignInteractObjMap.Contains(obj))
			{
				mConsignInteractObjSArr.RemoveAt(mConsignInteractObjMap[obj]);
				mConsignInteractObjMap.Remove(obj);
			}
		}
		mConsignInteractObjSArr.Empty();
		mConsignInteractObjMap.Empty();
	}
	bool IsPressing() const
	{
		return mIsPressing;
	}
	bool IsDoubleTapping() const
	{
		return mIsDoubleTapping;
	}
	void AddConsignInteractObj(const TScriptInterface< ITIHHsInteraction>& consignObj)
	{
		if (not mConsignInteractObjMap.Contains(consignObj.GetObject())
			&& consignObj.GetInterface() != nullptr)
		{
			mConsignInteractObjMap.Add(consignObj.GetObject(),mConsignInteractObjSArr.Add(consignObj));
		}
	}
	void RemoveConsignInteractObj(const TScriptInterface< ITIHHsInteraction>& consignObj)
	{
		UObject* obj = consignObj.GetObject();
		if (obj && mConsignInteractObjMap.Contains(obj))
		{
			mConsignInteractObjSArr.RemoveAt(mConsignInteractObjMap[obj]);
			mConsignInteractObjMap.Remove(obj);
			mConsignInteractObjSArr.Compact();
		}
	}
	
	FTIHActionGestureDelegate OnGestureDelegateDown;
	FTIHActionGestureDelegate OnGestureDelegateUp;
	FTIHActionGestureDelegate OnGestureDelegateTap;
	FTIHActionGestureDelegate OnGestureDelegateDoubleTap;
	FTIHActionGestureDelegate OnGestureDelegateHoldStart;
	FTIHActionGestureDelegate OnGestureDelegateHoldOngoing;
	FTIHActionGestureDelegate OnGestureDelegateHoldEnd;

	
private:
	void ProcessCallHitActor(FTIHHsActionParamIn param) const;

	void DoubleTapExpired()
	{
		if (not mIsPressing && not mIsDoubleTapping)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tap %s"), *mGestureName);
			FTIHHsActionParamIn param(mGestureName,"tap");
			OnGestureDelegateTap.Broadcast(param);
			for (auto consignObj :mConsignInteractObjSArr)
			{
				if (consignObj.GetInterface() !=nullptr)
				{
					consignObj->ActionEventTap_Deprecated(param);
				}
			}
			if (mActionDownRecord.CurrentState.HasActor)
			{
				TScriptInterface< ITIHHsInteraction > hitActor(mActionDownRecord.HitResult.GetActor());
				if (hitActor.GetInterface() != nullptr)
				{
					hitActor->ActionEventTap_Deprecated(param);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HoldStart %s"), *mGestureName);
			FTIHHsActionParamIn param(mGestureName,"holdStart");
			OnGestureDelegateHoldStart.Broadcast(param);
			for (auto consignObj :mConsignInteractObjSArr)
			{
				if (consignObj.GetInterface() !=nullptr)
				{
					consignObj->ActionProcessHoldStart_Deprecated(param);
				}
			}
			FTimerManager& TimerManager = *mTimerMgr;
			TimerManager.SetTimer(mTimerHandleHoldOngoingLoop, this, &UTIHHsActionPressGesture::HoldOngoingLoop, 0.01667f, true);
			//ProcessCallHitActor(param);

			APlayerController* playerController = mWorldContext->GetFirstPlayerController();
			if (playerController && playerController->GetHitResultUnderCursorByChannel(TraceTypeQuery1, false, mHoldResult))
			{
				//	collision for null
				TScriptInterface<ITIHHsInteraction> hitActor( mHoldResult.GetActor());
				if (hitActor.GetInterface() !=nullptr)
				{
					hitActor->ActionProcessHoldStart_Deprecated(param);
					mHoldTarget = hitActor;	//	현재 액션이 발생한 위치의 액터를 저장
				}
			}
		}
	}
	FHitResult mHoldResult;	//	현재 액션이 발생한 위치의 히트 결과, 액션이 발생한 위치와 관련된 정보
	void HoldOngoingLoop()
	{
		UE_LOG(LogTemp, Warning, TEXT("HoldOngoing %s"), *mGestureName);
		float deltaTime = mWorldContext->GetDeltaSeconds();
		FTIHHsActionParamIn param(mGestureName,"holding",deltaTime);
		OnGestureDelegateHoldOngoing.Broadcast(param);
		for (auto consignObj :mConsignInteractObjSArr)
		{
			if (consignObj.GetInterface() !=nullptr)
			{
				consignObj->ActionProcessHoldOngoing_Deprecated(param);
			}
		}
		
		if (mHoldTarget.GetInterface() != nullptr)
		{
			mHoldTarget->ActionProcessHoldOngoing_Deprecated(param);
		}
		
	}
	
	FTimerHandle mTimerHandleDoubleTapEvaluation;
	FTimerHandle mTimerHandleHoldOngoingLoop;
	
	TMap<UObject*,int32> mConsignInteractObjMap;
	TSparseArray<TScriptInterface< ITIHHsInteraction>> mConsignInteractObjSArr;
	
	FString mGestureName;
	bool mIsPressing;
	bool mIsDoubleTapping;
	/*
	 * 어떤 액션의 것인지 정보저장
	 * expired 시간
	 */
	float mExpiredTime = 0.25f;	//	더블탭 타이머 만료 시간
};
USTRUCT()
struct FTIHHsActionRecordIdentity
{
	GENERATED_BODY()

	static constexpr int32 InvalidTickID = 0;	//	유효하지 않은 TickID, 레코드가 유효하지 않음을 나타냄
	
	static int32 GlobalTickID;
	static void ProgressTickID()
	{
		++GlobalTickID;
	}
	static int32 GetGlobalTickID()
	{
		return GlobalTickID;
	}
	
	UPROPERTY()
	int32 TickID;

	void Clear()
	{
		TickID = 0;
	}
};


USTRUCT()
struct FTIHHsActionRecordMove
{
	GENERATED_BODY()

	UPROPERTY()
	FTIHHsActionRecordIdentity Identity;	//	레코드 아이디, 레코드가 저장될때마다 증가함
	
	UPROPERTY()
	FVector2D ScreenPosition;	//	기록 위치

	UPROPERTY()
	FVector WorldLocation;

	UPROPERTY()
	FVector WorldDirection;

	UPROPERTY()
	float Duration;	//	기록 시간

	
	UPROPERTY()
	ETIHHsActionMoveStateType MoveType;	//	Idle, OutCursor, MoveStart, MoveOngoing, MoveEnd
	UPROPERTY()
	bool IsCursorIn;	//	커서가 위치에 있는지 여부
	UPROPERTY()
	bool IsDeproject;	//	커서가 위치에 있는지 여부
	UPROPERTY()
	bool IsCursorMove;	//

	FTIHHsActionRecordMove(): Identity(),
		ScreenPosition(FVector2D::ZeroVector),
		WorldLocation(FVector::ZeroVector),
		WorldDirection(FVector::ZeroVector),
		Duration(0.0f),
		MoveType(ETIHHsActionMoveStateType::EIdle),
		IsCursorIn(false),
		IsDeproject(false),
		IsCursorMove(false)
	{}

	void Clear()
	{
		Identity.Clear();
		ScreenPosition = FVector2D::ZeroVector;
		Duration = 0.0f;
		
	}
};
/*
		 *		name		prev	curr	condition			result
		 *		not			empty	empty	none				none
		 *		new			empty	exist	none				curr.Enter
		 *		end			exist	empty	none				prev.Exit
		 *		ongoing		exist	exist	prev == curr		prev.Ongoing
		 *		swap		exist	exist	prev != curr		prev.Exit + curr.Enter
		 * 
		 */
UENUM(BlueprintType)
enum class ETIHHsActionRecordHoverType : uint8
{
	ETIHHsActionRecordHover_None = 0,
	ETIHHsActionRecordHover_New = 1,
	ETIHHsActionRecordHover_End = 2,
	ETIHHsActionRecordHover_Swap = 3,
	ETIHHsActionRecordHover_Ongoing = 4,
};

USTRUCT()
struct FTIHHsActionRecordHover
{
	GENERATED_BODY()

	UPROPERTY()
	FTIHHsActionRecordIdentity Identity;
	
	UPROPERTY()
	FHitResult HitResult;

	UPROPERTY()
	float HoveringTime;	//	기록 시간
	
	UPROPERTY()
	ETIHHsActionRecordHoverType HoverType;	//	Enter, Ongoing, Exit
	
	BYTE _padding[3];	
	
	void Clear()
	{
		Identity.Clear();
		HitResult.Reset();
		HoveringTime = 0.0f;
		HoverType = ETIHHsActionRecordHoverType::ETIHHsActionRecordHover_None;
	}
};
USTRUCT()
struct FTIHHsActionRecordTaps
{
	GENERATED_BODY()

	UPROPERTY()
	FTIHHsActionRecordIdentity Identity;

	
	
};


template<typename TIHTemplateType,int32 ReserveCount = 32>
class TIHHOUSINGCORE_API TTIHHsActionRecordContainer
{
private:
	template<typename T>
		struct has_clear_method
	{
	private:
		template<typename U>
		static auto test(int) -> decltype(std::declval<U>().Clear(), std::true_type{});
        
		template<typename>
		static std::false_type test(...);
        
	public:
		static constexpr bool value = decltype(test<T>(0))::value;
	};

public:
	TTIHHsActionRecordContainer ()
		: mRecordTarget(0),
		  mLastIndex(0),
		  mRecording(false),
		  mValidRecordCount(0)
	{
		mRecords.SetNum(ReserveCount);
	}
	TTIHHsActionRecordContainer(const TArray<TIHTemplateType>& MRecords, const int32 MRecordTarget,
		const int32 MLastIndex, const bool bMRecording, const int32 MValidRecordCount)
		: mRecords(MRecords),
		  mRecordTarget(MRecordTarget),
		  mLastIndex(MLastIndex),
		  mRecording(bMRecording),
		  mValidRecordCount(MValidRecordCount)
	{
		mRecords.SetNum(ReserveCount);
	}

private:
	template<typename T>
	typename std::enable_if<has_clear_method<T>::value>::type
	ClearRecord(T& Record)
	{
		Record.Clear();
	}

	template<typename T>
	typename std::enable_if<!has_clear_method<T>::value>::type
	ClearRecord(T& Record)
	{
		FMemory::Memset(&Record, 0, sizeof(T));
	}
	
public:
	
	/*		pre   recordTarget
	 *		| -------- |
	 *		begin 을 누가 시작하든 닫아주지 않으면 닫기지 않고 redordTarget만 계속 변화한다.
	 *
	 */
	TIHTemplateType& BeginRecord()
	{
		if (not mRecording)
		{
			mRecording = true;
		}
		return mRecords[mRecordTarget];
	}
	void EndRecord()
	{
		if (mRecording)
		{
			mRecording = false;
			mRecentRecordIndex = mRecordTarget;
			mLastIndex = mRecordTarget;	
			++mRecordTarget;
			if ( mRecords.Num()<=mRecordTarget)
			{
				mRecordTarget = 0;
			}
			ClearRecord(mRecords[mRecordTarget]);
			mValidRecordCount = FMath::Max(mRecords.Num(), mRecordTarget + 1);
		}
	}
	const TIHTemplateType& GetRecent() const
	{
		return mRecords[mLastIndex];
	}
	const TIHTemplateType& GetRecordTarget() const
	{
		return mRecords[mRecordTarget];
	}
	const TIHTemplateType& GetLastRecord() const
	{
		return mRecords[mLastIndex];
	}
	const TIHTemplateType& GetPreviousRecord(int32 offset = 1) const
	{
		if (mValidRecordCount == 0)
		{
			static TIHTemplateType emptyRecord;
			return emptyRecord;	
		}
		offset = offset % mValidRecordCount;
		int32 prevIdx = (mRecordTarget - offset + mValidRecordCount)%mValidRecordCount;
		return mRecords[prevIdx];
	}
	int32 GetRecordCount() const
	{
		return mRecords.Num();
	}
	
private:
	
	void Reserve(int32 reserveCount)
	{
		if (mRecords.Num() < reserveCount)
		{
			mRecords.Reserve(reserveCount);
			for (int32 i = mRecords.Num(); i < reserveCount; ++i)
			{
				TIHTemplateType newRecord;
				ClearRecord(newRecord);
				mRecords.Add(newRecord);
			}
		}
	}
	
	TArray<TIHTemplateType> mRecords;	//	레코드 배열
	int32 mRecordTarget = 0;	//	현재 인덱스, 레코드가 추가될때마다 증가함
	int32 mLastIndex = 0;	//	마지막 인덱스, 레코드가 추가될때마다 증가함
	int32 mRecentRecordIndex = 0;	//	최근 레코드 인덱스, 레코드가 추가될때마다 증가함
	bool mRecording = false;	//	레코딩 중인지 여부
	int32 mValidRecordCount ;
};

USTRUCT()
struct FTIHHsActionSystemLinetraceData
{
	GENERATED_BODY()

	UPROPERTY()
	FHitResult HitResult;	//	라인 트레이스 결과
	
	UPROPERTY()
	FVector WorldLocationStart = FVector::ZeroVector;	//	라인 트레이스 시작 위치

	FVector WorldDirction = FVector::ZeroVector;	//	라인 트레이스 방향, 월드 좌표계에서의 방향
	float TraceDistance = 1000.0f;	//	라인 트레이스 거리
	
	UPROPERTY()
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Visibility;	//	라인 트레이스 채널

	FVector GetWorldLocationEnd(FVector dirction,float traceDistance = 1000.0f) const
	{
		return WorldLocationStart + dirction * traceDistance;
	}
};

USTRUCT(BlueprintType)
struct FTIHHsActionMoveMouseData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector2D ScreenPosition;	//	마우스 위치, 스크린 좌표계에서의 위치
	UPROPERTY()
	FVector WorldLocation;	//	월드 좌표계에서의 위치
	UPROPERTY()
	FVector WorldDirection;	//	월드 좌표계에서의 방향, 마우스가 바라보는 방향
	UPROPERTY()
	float DeletaTime;	
	UPROPERTY()
	bool IsMouseInViewport = false;	//	마우스가 뷰포트 안에 있는지 여부
	UPROPERTY()
	bool IsDeproject = false;	//	마우스가 디프로젝션 되었는지 여부, 월드 좌표계로 변환되었는지 여부
	UPROPERTY()
	bool IsMove = false;	//	마우스가 움직였는지 여부, 마우스가 움직인 경우 true, 그렇지 않으면 false

	
	FTIHHsActionMoveMouseData()
		: ScreenPosition(FVector2D::ZeroVector),
		  WorldLocation(FVector::ZeroVector),
		  WorldDirection(FVector::ZeroVector),
		  DeletaTime(0.0f)
	{}
	FTIHHsActionMoveMouseData(const FVector2D& InScreenPosition, const FVector& InWorldLocation, const FVector& InWorldDirection, float InDeltaTime)
		: ScreenPosition(InScreenPosition),
		  WorldLocation(InWorldLocation),
		  WorldDirection(InWorldDirection),
		  DeletaTime(InDeltaTime)
	{}
	FTIHHsActionMoveMouseData(const FTIHHsActionMoveMouseData& Other)
		: ScreenPosition(Other.ScreenPosition),
		  WorldLocation(Other.WorldLocation),
		  WorldDirection(Other.WorldDirection),
		  DeletaTime(Other.DeletaTime)
	{}
	FTIHHsActionMoveMouseData(FTIHHsActionMoveMouseData&& Other) noexcept
		: ScreenPosition(MoveTemp(Other.ScreenPosition)),
		  WorldLocation(MoveTemp(Other.WorldLocation)),
		  WorldDirection(MoveTemp(Other.WorldDirection)),
		  DeletaTime(Other.DeletaTime)
	{}
	FTIHHsActionMoveMouseData& operator=(const FTIHHsActionMoveMouseData& Other)
	{
		if (this != &Other)
		{
			ScreenPosition = Other.ScreenPosition;
			WorldLocation = Other.WorldLocation;
			WorldDirection = Other.WorldDirection;
			DeletaTime = Other.DeletaTime;
		}
		return *this;
	}
	FTIHHsActionMoveMouseData& operator=(FTIHHsActionMoveMouseData&& Other) noexcept
	{
		if (this != &Other)
		{
			ScreenPosition = MoveTemp(Other.ScreenPosition);
			WorldLocation = MoveTemp(Other.WorldLocation);
			WorldDirection = MoveTemp(Other.WorldDirection);
			DeletaTime = Other.DeletaTime;
		}
		return *this;
	}

	FString ToString() const
	{
		FString result = FString::Printf(TEXT("{ \"ScreenPosition\": { \"X\": %f, \"Y\": %f }, \"WorldLocation\": { \"X\": %f, \"Y\": %f, \"Z\": %f }, \"WorldDirection\": { \"X\": %f, \"Y\": %f, \"Z\": %f }, \"DeletaTime\": %f, \"IsMouseInViewport\": %s, \"IsDeproject\": %s, \"IsMove\": %s }"),
				ScreenPosition.X,
				ScreenPosition.Y,
				WorldLocation.X,
				WorldLocation.Y,
				WorldLocation.Z,
				WorldDirection.X,
				WorldDirection.Y,
				WorldDirection.Z,
				DeletaTime,
				IsMouseInViewport ? TEXT("true") : TEXT("false"),
				IsDeproject ? TEXT("true") : TEXT("false"),
				IsMove ? TEXT("true") : TEXT("false"));
		return result;
	}
	
};

UCLASS()
class UTIHHsActionMappingAdditionalBase : public UObject
{
	GENERATED_BODY()
public:
	
	virtual TArray<FEnhancedActionKeyMapping> GetAdditionalActionKeyMappings() const
	{
		return TArray<FEnhancedActionKeyMapping>();
	}
	void SetActionSystem(UTIHHsActionSystem* actionSystem)
	{
		mActionSystem = actionSystem;
	}
	UTIHHsActionSystem* GetActionSystem() const
	{
		return mActionSystem;
	}
private:
	UTIHHsActionSystem* mActionSystem = nullptr;
};

/*
 *	인풋키의 시작점을 알고 싶다면 OnWorldBeginPlay 찾아가라.
 *
 * 
 */
UCLASS()
class TIHHOUSINGCORE_API UTIHHsActionSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	//static FTIHHsActionDescription gActoinDescription;	
	
	using TIHHandleActionFunctionType = void(float deltaTime,const FVector2D& currMousePos,const FVector& currWorldLocation, const FVector& currWorldDirection,bool isMouseInViewport, bool isDeproject,bool isMove);
	
	void InitHandleMoveActionNodes();
	//mWorldTraceDistance,mCurrentTag.StaticCollisionChannel
	UFUNCTION(BlueprintCallable)
	ECollisionChannel GetCurrentTraceChannel() const
	{
		return mCurrentTag.StaticCollisionChannel;
	}
	UFUNCTION(BlueprintCallable)
	float GetWorldTraceDistance() const
	{
		return mWorldTraceDistance;
	}
	//void HandleMouseMovementRecords(float deltaTime, FVector2D currMousePos, FVector currWorldLocation,FVector currWorldDirection, ETIHHsActionMoveEventType currMoveEventType,bool isMouseInViewport, bool isDeproject, const FTIHHsActionRecordMove& prevRecord, bool isMove);
	
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual void Deinitialize() override;
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UTIHHsActionSystem, STATGROUP_Tickables);
	}
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/*
	 * @brief: 액션 매핑 컨텍스트를 등록한다.
	 * @hideParam: 수동입력
	 * @hideResult: mActionInputProcessors
	 */
	void ManualRegisterActionMappingsList();

	/*
	 *	@brief: 이게 하는 역할은 현재 mCurrentActionMappingContextObj에서 GetMappings의 이름들을 가져와서 mActionKeyMappingArr에 넣어주고, mActionInputProcessors안에 있는 manual로 설정된 인풋 기능을 연결하는것이다.
	 *	그래서 매뉴얼이 없다면 기능의 연결자체가 안될거임.
	 *	@hideParam: mCurrentActionMappingContextObj,mPlayerController,mActionInputProcessors
	 *	@hideResult: mEnhancedInputComponent,mActionKeyMappingArr
	 */
	void SettingInputActionProcessors();

	TStrongObjectPtr<class UTIHHsActionLayer> mActionLayer;
	/*
	 *	@brief: 액션 매핑 컨텍스트를 변경한다.
	 *	@hideParam: mCurrActionKeyMappingArr,mActionInputProcessors,mEnhancedInputComponent,mActionMappingContextObjTable,mPlayerController,
	 *	@hideResult: mCurrentActionMappingContextName
	 * 
	 */
	void ChangeActionMappingContext(const FName& inContextName);

	
	/*
Action List
	Common:
		main-select-down,main-select-up, sub-select-down,sub-select-up, cancel-down, cancel-up
	Specific:
		keyMouse:
			keyboard, cursorXY
		joyStick:
			virtual keyboard, virtual joyStick left, virtual joyStick right
Default
	KeyMouse:
		leftMouseButton: main-select
		rightMouseButton: sub-select
		esc: cancel
	Touch:
	JoyStick:
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActionSystem")
	FTIHHsActionMappingChnageDelegate OnActionMappingChange;	//	액션 매핑 컨텍스트 변경시 호출되는 델리게이트
	
	//void RegisterActionMappingProcessor(const FName& actionName, FTIHHsActionMappingProcessorWrapper processor,bool bOverride = false);
	//FTIHHsActionMappingRegistryProcDelegate* GetMappingProcessor(const FName& actionName);

private:
	void InitiateEnhanceInputMapping();

	//	핵심은 이건 그냥 헬퍼함수라는거임. DRY에 맞춰서 만든것뿐임.
	template<typename UserClass, typename FuncType, typename... BoundArgs>
	void RegisterActionMapping(
		const FName& ActionKey,				//	mActionInputProcessors에 등록하기 위한것 <- 이 이름자체가 키의 이름
		ETriggerEvent TriggerEvent,			//	ETriggerEvent 여기에 알아서 들어가서 볼것
		UEnhancedInputComponent* InputComp,	//	이 컴포넌트에 등록할것임.
		const FEnhancedActionKeyMapping& Mapping,
		UserClass* UserObject,
		FuncType&& Func,
		BoundArgs&&... Args    // 바인딩 시 추가로 넘길 인자들
	);

	//	hideArgIn: mActionInputProcessors
	void UnregisterActionMapping(
		const FName& ActionKey,
		ETriggerEvent TriggerEvent,
		UEnhancedInputComponent* InputComp
	);

	void SetupKeyboardMapping()
	{
		UInputAction* movementAction = NewObject<UInputAction>();
		movementAction->ValueType = EInputActionValueType::Boolean;

		TArray keys = {
			EKeys::A,EKeys::B,EKeys::C,EKeys::D,EKeys::E,EKeys::F,EKeys::G,EKeys::H,EKeys::I,EKeys::J,EKeys::K,EKeys::L,EKeys::M,EKeys::N,EKeys::O,EKeys::P,EKeys::Q,EKeys::R,EKeys::S,EKeys::T,EKeys::U,EKeys::V,EKeys::W,EKeys::X,EKeys::Y,EKeys::Z,
			EKeys::Zero,EKeys::One,EKeys::Two,EKeys::Three,EKeys::Four,EKeys::Five,EKeys::Six,EKeys::Seven,EKeys::Eight,EKeys::Nine,
			EKeys::SpaceBar,EKeys::Enter,EKeys::BackSpace,EKeys::Tab,EKeys::LeftShift,EKeys::RightShift,EKeys::LeftControl,EKeys::RightControl,
		};
	}
	
	//	MappingContext
	UPROPERTY()
	FString mActionMappingContextDBPathsSoftPath;//	IM의 경로들이 저장된 위치. IM이름들과 그 IM의 위치
	UPROPERTY()
	TMap<FName,UInputMappingContext*> mActionMappingContextObjTable;	//	IM의 이름과 런타임 객체
	UPROPERTY()
	TArray<FEnhancedActionKeyMapping> mCurrActionKeyMappingArr;	//	현재 액션 매핑 컨텍스트에 등록된 액션 매핑들
	UPROPERTY()
	TMap<FName,FTIHHsActionMappingProcessorWrapper> mActionInputProcessors;	//	액션 매핑 이름과 처리 함수의 맵
	
	UPROPERTY()
	FTIHHsActionMappingContextPair mCurrActionMappingContextPair;	//	현재 액션 매핑 컨텍스트 이름과 런타임 객체
	
	UEnhancedInputComponent* mEnhancedInputComponent;	//	현재 플레이어 컨트롤러의 EnhancedInputComponent

	void ProcessActionMapping_MainSelect_Down();
	void ProcessActionMapping_MainSelect_Up();

	void ProcessActionMapping_SubSelect_Down();
	void ProcessActionMapping_SubSelect_Up();

	void ProcessActionMapping_Cancel_Down();
	void ProcessActionMapping_Cancel_Up();

	void ProcessActionMapping_keyMouse_MouseMove();

	//	deprecated
	FVector2D mLastMousePos;
	void ProcessActionRawInput_MouseMoveX(float value)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProcessActionRawInput_MouseMoveX %f"), value);
		mLastMousePos.X = value;
	}
	void ProcessActionRawInput_MouseMoveY(float value)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProcessActionRawInput_MouseMoveY %f"), value);
		mLastMousePos.Y = value;
	}
public:

	UFUNCTION(BlueprintCallable, Category = "ActionSystem|ActionGesture")
	UTIHHsActionPressGesture* CreateActionPressGesture(const FString& gestureName, float expiredTime = 0.2f)
	{
		UTIHHsActionPressGesture* newGesture = NewObject<UTIHHsActionPressGesture>();
		newGesture->Initialize(gestureName,mActionLayer.Get(), expiredTime);
		newGesture->mTimerMgr = mTimerManager;
		newGesture->mWorldContext = mWorld;
		FName gestureNameFName(*gestureName);
		
		mActionPressGestures.Add(gestureNameFName)
		.Reset(newGesture);
	
		return newGesture;
	}
	UFUNCTION(BlueprintCallable, Category = "ActionSystem|ActionGesture")
	UTIHHsActionPressGesture* GetActionPressGesture(const FString& gestureName)
	{
		FName gestureNameFName(*gestureName);
		if (mActionPressGestures.Contains(gestureNameFName))
		{
			return mActionPressGestures[gestureNameFName].Get();
		}
		return nullptr;
	}
	UFUNCTION(BlueprintCallable, Category = "ActionSystem|ActionGesture")
	void ActionDown(const FName& actionName)
	{
		if (mActionPressGestures.Contains(actionName))
		{
			mActionPressGestures[actionName]->ActionDown();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ActionDown: ActionPressGesture not found for action: %s"), *actionName.ToString());
		}
	}
	UFUNCTION(BlueprintCallable, Category = "ActionSystem|ActionGesture")
	void ActionUp(const FName& actionName)
	{
		if (mActionPressGestures.Contains(actionName))
		{
			mActionPressGestures[actionName]->ActionUp();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ActionUp: ActionPressGesture not found for action: %s"), *actionName.ToString());
		}
	}
	UFUNCTION(BlueprintCallable, Category = "ActionSystem|ActionGesture")
	void StopHold(const FName& actionName)
	{
		if (mActionPressGestures.Contains(actionName))
		{
			mActionPressGestures[actionName]->StopHold();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("StopHold: ActionPressGesture not found for action: %s"), *actionName.ToString());
		}
	}
	UFUNCTION(BlueprintCallable, Category = "ActionSystem|ActionGesture")
	void StopTaps(const FName& actionName)
	{
		if (mActionPressGestures.Contains(actionName))
		{
			mActionPressGestures[actionName]->StopTaps();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("StopTaps: ActionPressGesture not found for action: %s"), *actionName.ToString());
		}
	}
	UFUNCTION(BlueprintCallable, Category = "ActionSystem|ActionGesture")
	void StopAction(const FName& actionName)
	{
		if (mActionPressGestures.Contains(actionName))
		{
			mActionPressGestures[actionName]->StopAction();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("StopAction: ActionPressGesture not found for action: %s"), *actionName.ToString());
		}
	}
	//	
	UFUNCTION(BlueprintCallable, Category = "ActionSystem|ActionGesture")
	void AddConsignInteractObj(const TScriptInterface< ITIHHsInteraction>& consignObj)
	{
		for (auto& actionPressGesture : mActionPressGestures)
		{
			actionPressGesture.Value->AddConsignInteractObj(consignObj);
		}
	}
	UFUNCTION(BlueprintCallable, Category = "ActionSystem|ActionGesture")
	void RemoveConsignInteractObj(const TScriptInterface< ITIHHsInteraction>& consignObj)
	{
		for (auto& actionPressGesture : mActionPressGestures)
		{
			actionPressGesture.Value->RemoveConsignInteractObj(consignObj);
		}
	}
	
private:
	TMap<FName,FTimerHandle> mActionTimerHandles;//	액션 타이머 핸들 맵,tap,doubletap,hold 처리, 키는 액션 이름, 값은 타이머 핸들
	//	doubleTap ExpireCheck
	TMap<FName,TStrongObjectPtr<UTIHHsActionPressGesture>> mActionPressGestures;	//	액션 이름과 액션 프레스 제스처 맵, 액션 이름으로 액션 프레스 제스처를 찾을 수 있다.
	
public:
	

	TTIHHsActionRecordContainer<FTIHHsActionRecordHover,16> mActionRecordHoverContainer;
	float mWorldTraceDistance = 10000.0f;	//	월드 트레이스 거리, 월드에서 트레이스할 거리
	/*
	 * 
	 */
	bool mOnActionTick = true;

	float mActionMouseStopTime = 0.0f;	//	마우스가 움직이지 않은 시간, 이 시간이 지나면 마우스가 움직이지 않았다고 판단한다.
	
	float mActionTickTime = 0.0f;
	float mActonTickTimeMax = 0.03333f;
	
	float mHoverTickTime = 0.0f;
	float mHoverTickTimeMax = 0.03333f;	//	최대 Hover Tick 시간, 이 시간동안 Hover를 처리하지 않으면 Hover가 종료된다.
	bool mForceHoverTick = false;	//	강제 Hover Tick, 이 값이 true이면 Hover Tick이 무조건 실행된다.
	TMap<FName,TScriptInterface<ITIHHsBaseObject>> mSoftOverrideObjects;	//	소프트 오버라이드 오브젝트, 이름과 오브젝트의 맵, 소프트 오버라이드된 오브젝트를 저장한다.

	
	void ProcessHoverRecord(float deltaTime);

	FTIHHsTag mCurrentTag;
	bool CheckInteraction(const TScriptInterface<ITIHHsInteraction>& baseObject,bool gameplayTagAll = true,bool dynamicTagCheck = false) const
	{
		bool reValue = true;
		if (baseObject.GetInterface() == nullptr)
		{
			reValue = false;
		}
		else
		{
			const FTIHHsTag& interfaceTag = baseObject->GetInteractionTag_InInteraction();
			if (gameplayTagAll)
			{
				if (not mCurrentTag.StaticGamePlayTags.HasAll(interfaceTag.StaticGamePlayTags))
				{
					reValue = false;
				}
			}
			else
			{
				if (not mCurrentTag.StaticGamePlayTags.HasAny(interfaceTag.StaticGamePlayTags))
				{
					reValue = false;
				}
			}

			if (dynamicTagCheck)
			{
				if (mCurrentTag.DynamicStringTag != interfaceTag.DynamicStringTag)
				{
					reValue = false;
				}
			}

			
		}
		return reValue;
	}

	bool HandleHoverTraceSwitch(FTIHHsActionRecordHover& hoverTarget, float deltaTime, FString& actionName, AActor* prevActor, AActor* currActor, TScriptInterface<ITIHHsInteraction> prevInterface, TScriptInterface<ITIHHsInteraction> currInterface);

	bool HandleHoverTraceResult(FTIHHsActionRecordHover& hoverTarget,float deltaTime);

	FORCEINLINE void WriteMoveRecord(FTIHHsActionRecordMove& target,bool isDeproject,bool isCursorIn, bool isCursorMove,const FVector2D& screenPosition, const FVector& worldLocation, const FVector& worldDirection, ETIHHsActionMoveStateType moveType)
	{
		target.IsDeproject = isDeproject;
		target.IsCursorIn = isCursorIn;
		target.IsCursorMove = isCursorMove;
		target.ScreenPosition = screenPosition;
		target.WorldLocation = worldLocation;
		target.WorldDirection = worldDirection;
		target.MoveType = moveType;
	}
	
	//	move
	FTIHActionGestureDelegate OnActionEventMouseMoveDelegate;

	TStrongObjectPtr<UTIHHsInterfaceRegistry> OnActionEventMouseMoveRegistry;


	FTIHActionGestureDelegate OnActionProcessMouseMoveStart;
	FTIHActionGestureDelegate OnActionProcessMouseMoveOngoing;
	FTIHActionGestureDelegate OnActionProcessMouseMoveEnd;
	FTIHActionGestureDelegate OnActionProcessMouseMoveOutCursor;	//	마우스가 커서 밖으로 나갔을때 호출되는 델리게이트
	
	void ProcessTraceMouseRecord(float deltaTime);

	UFUNCTION()
	const FTIHHsActionRecordMove& GetMoveRecordTarget() const
	{
		return mActionRecordMoveContainer.GetRecordTarget();
	}
	UFUNCTION()
	FVector CalculateAcceleration() const
	{
		const FTIHHsActionRecordMove& prevRecord = mActionRecordMoveContainer.GetPreviousRecord(1);
		const FTIHHsActionRecordMove& currRecord = mActionRecordMoveContainer.GetRecordTarget();
		
		FVector acceleration = FVector::ZeroVector;
		if (prevRecord.Identity.TickID != 0 && currRecord.Identity.TickID != 0)
		{
			float deltaTime = currRecord.Duration - prevRecord.Duration;
			if (deltaTime > 0.0f)
			{
				acceleration = (currRecord.WorldLocation - prevRecord.WorldLocation) / deltaTime;
			}
		}
		return acceleration;
	}
	UFUNCTION()
	FRay GetCurrentMouseRay() const
	{
		const FTIHHsActionRecordMove& currRecord = mActionRecordMoveContainer.GetRecent();
		return FRay(currRecord.WorldLocation, currRecord.WorldDirection);
	}
	
private:
	FTIHHsActionParamIn mOnMouseMoveParamIn;
	
	TArray<FTIHHsFunctionNode<TIHHandleActionFunctionType>> mActionMoveRootNodes;	//	액션 루트 노드, 액션이 시작될때 호출되는 함수들
	//TMap<int32,FTIHHsActionHandleFunctionNode<TIHHandleActionFunctionType>> mActionStateHandleFuncs;	//	액션 핸들 노드, 액션이 진행될때 호출되는 함수들
	FTIHHsFunctionNode<TIHHandleActionFunctionType> mActionMoveStateHandleFuncsTable[(int32)ETIHHsActionMoveStateType::EMax];
	
	float mMoveSensitive = 0.0001f;	//	마우스 움직임 민감도, 이 값보다 작으면 움직임으로 간주하지 않음
	
	TTIHHsActionRecordContainer<FTIHHsActionRecordMove,120> mActionRecordMoveContainer;	//	액션 레코드 컨테이너, 액션 레코드를 저장하는 컨테이너
	UWorld* mWorld;
	FTimerManager* mTimerManager;	//	타이머 매니저, 액션 타이머를 관리하기 위해 필요
	APlayerController* mPlayerController;
	APlayerCameraManager* mPlayerCameraManager;	//	플레이어 카메라 매니저, 카메라 위치와 방향을 가져오기 위해 필요
	float mMoveTickTime = 0.0f;	//	마우스 움직임 Tick 시간, 이 시간이 지나면 마우스 움직임을 처리함
	
	float mTempMoveTime = 0.0f;
	float mTempMoveTimeMax = 0.1f;	//	마우스가 움직이지 않은 시간, 이 시간이 지나면 마우스가 움직이지 않았다고 판단한다.
	float mTempMoveIdleTime = 0.0f;	//	마우스가 움직이지 않은 시간, 이 시간이 지나면 마우스가 움직이지 않았다고 판단한다.
	float mTempMoveIdleTimeMax = 0.1f;	//	마우스가 움직이지 않은 시간, 이 시간이 지나면 마우스가 움직이지 않았다고 판단한다.
	int32 mTempMouseMoveState;
	
	
	
public:

	const FString ToString(ETIHHsActionMoveEventType moveType)
	{
		FString result;
		switch (moveType) {
		case ETIHHsActionMoveEventType::EStop:
			result = TEXT("Event:Stop");
			break;
		case ETIHHsActionMoveEventType::EMove:
			result = TEXT("Event:Move");
			break;
		case ETIHHsActionMoveEventType::EOutCursor:
			result = TEXT("Event:OutCursor"); 
			break;
		case ETIHHsActionMoveEventType::EMax:
			result = TEXT("Event:Max");
			break;
		}
		return result;
	}
	const FString ToString(ETIHHsActionMoveStateType moveState)
	{
		FString result;
		switch (moveState) {
		case ETIHHsActionMoveStateType::EIdle:
			result = TEXT("State:Idle");
			break;
		case ETIHHsActionMoveStateType::EOutCursor:
			result = TEXT("State:OutCursor");
			break;
		case ETIHHsActionMoveStateType::EMoveStart:
			result = TEXT("State:MoveStart");
			break;
		case ETIHHsActionMoveStateType::EMoveOngoing:
			result = TEXT("State:MoveOngoing");
			break;
		case ETIHHsActionMoveStateType::EMoveEnd:
			result = TEXT("State:MoveEnd");
			break;
		case ETIHHsActionMoveStateType::EMax:
			result = TEXT("State:Max");
			break;
		}
		return result;
	}

	UTIHHsActionLayer* GetActionLayer() const
	{
		return mActionLayer.Get();
	}
	
	FTIHHsActionRecordMove& RecordBeginMoveRecord()
	{
		return mActionRecordMoveContainer.BeginRecord();
	}
	FTIHHsActionRecordHover& RecordBeginHoverRecord()
	{
		return mActionRecordHoverContainer.BeginRecord();
	}
	// struct FHitResult& OutHit,const FVector& Start,const FVector& End,ECollisionChannel TraceChannel,const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam

	FTIHHsActionSystemLinetraceData mWorldTraceData;

	FTIHHsActionSystemLinetraceData& GetWorldTraceData()
	{
		return mWorldTraceData;
	}
	
	UFUNCTION(BlueprintCallable)
	FVector GetCameraLocation() const
	{
		if (mPlayerController)
		{
			return mPlayerController->PlayerCameraManager->GetCameraLocation();
		}
		return FVector::ZeroVector;
	};
	UFUNCTION(BlueprintCallable)
	FVector GetCameraForwardVector() const
	{
		if (mPlayerCameraManager)
		{
			return mPlayerCameraManager->GetCameraRotation().Vector();
		}
		return FVector::ZeroVector;
	};
	UFUNCTION(BlueprintCallable)
	FVector GetCameraRightVector() const
	{
		if (mPlayerCameraManager)
		{
			const FRotator CamRot = mPlayerCameraManager->GetCameraRotation();
			return FRotationMatrix(CamRot).GetUnitAxis(EAxis::Y); // Right 방향
		}
		return FVector::ZeroVector;
	};
	UFUNCTION(BlueprintCallable)
	FVector GetCameraUpVector() const
	{
		if (mPlayerCameraManager)
		{
			const FRotator CamRot = mPlayerCameraManager->GetCameraRotation();
			return FRotationMatrix(CamRot).GetUnitAxis(EAxis::Z); // Up 방향
		}
		return FVector::ZeroVector;
	}
	
	UFUNCTION(BlueprintCallable)
	void SetAdditionalActionMappingObj(UTIHHsActionMappingAdditionalBase* additionalObj)
	{
		if (mAdditionalActionMappingObj)
		{
			mAdditionalActionMappingObj->ConditionalBeginDestroy();
			mAdditionalActionMappingObj = nullptr;
		}
		mAdditionalActionMappingObj = additionalObj;
		if (mAdditionalActionMappingObj)
		{
			mAdditionalActionMappingObj->SetActionSystem(this);
		}
	}

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return false;
		//return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
	}

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		return false;
	}
	UTIHHsActionMappingAdditionalBase* mAdditionalActionMappingObj;
};

template <typename UserClass, typename FuncType, typename ... BoundArgs>
void UTIHHsActionSystem::RegisterActionMapping(const FName& ActionKey, ETriggerEvent TriggerEvent,
	UEnhancedInputComponent* InputComp, const FEnhancedActionKeyMapping& Mapping, UserClass* UserObject,
	FuncType&& Func, BoundArgs&&... Args)
{
	FEnhancedInputActionEventBinding& Binding =
		InputComp->BindAction(
			Mapping.Action,
			TriggerEvent,
			UserObject,
			Forward<FuncType>(Func),
			Forward<BoundArgs>(Args)...
		);
	
	mActionInputProcessors[ActionKey].BindingHandles.Add(TriggerEvent, Binding.GetHandle());
	UE_LOG(LogTemp, Warning, TEXT("UTIHHsActionSystem::RegisterActionMapping End ActionKey: %s, TriggerEvent: %d"), *ActionKey.ToString(), static_cast<int32>(TriggerEvent));
}
FORCEINLINE void UTIHHsActionSystem::UnregisterActionMapping(const FName& ActionKey, ETriggerEvent TriggerEvent,
	UEnhancedInputComponent* InputComp)
{
	// 프로세서가 있는지 확인
	if (auto* Proc = mActionInputProcessors.Find(ActionKey))
	{
		// 해당 이벤트의 핸들이 저장되어 있는지 확인
		if (uint32* HandlePtr = Proc->BindingHandles.Find(TriggerEvent))
		{
			// 바인딩 해제
			InputComp->RemoveBindingByHandle(*HandlePtr);
			// 맵에서 제거
			Proc->BindingHandles.Remove(TriggerEvent);
		}
	}
}



//	Dependency Injection : 지금당장은 필요없음
UCLASS()
class UTIHHsDependencyInjection : public UObject
{
	GENERATED_BODY()
public:
	UTIHHsActionSystem* GetActionSystem();
	
};

