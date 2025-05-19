// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TIHHsCore.h"
#include "Subsystems/WorldSubsystem.h"
#include "TIHTapActionSubSystem.generated.h"

/**
 * 
 */
UENUM()
enum class ETIHHsMouseActionType : uint8
{
	ETIHHsMouseAction_None = 0,
	ETIHHsMouseAction_Down = 1,
	ETIHHsMouseAction_Up = 2,
	ETIHHsMouseAction_Move = 3,
};

USTRUCT()
struct FTIHHsMouseActionNode
{
	GENERATED_BODY()

	UPROPERTY()
	int32 NodeID;

	UPROPERTY()
	int64 NodeTick;

	UPROPERTY()
	double NodeTime;
	
	UPROPERTY()
	int32 ActionType;	

	UPROPERTY()
	FVector2D MouseScreenPosition;

	UPROPERTY()
	FVector MouseWorldPosition;

	FTIHHsMouseActionNode() :
		NodeID(INDEX_NONE),
		NodeTick(INDEX_NONE),
		ActionType(INDEX_NONE),
		MouseScreenPosition(FVector2D::ZeroVector),
		MouseWorldPosition(FVector::ZeroVector)
	{
	};

	FTIHHsMouseActionNode& Clear()
	{
		NodeID = 0;
		NodeTick = 0;
		ActionType = 0;
		MouseScreenPosition = FVector2D::ZeroVector;
		MouseWorldPosition = FVector::ZeroVector;
		return *this;
	}
};
/*
 *	move
 *		키보드
 *			w,a,s,d,q,e
 *		마우스
 *			휠 업,다운,셀렉
 *	기타기능
 *		키보드
 *	tap: 물체 상호작용
 *	doubleTap:
 *		select:
 *		menu:
 *	hover
 *	hold
 *		holdStart
 *		holding
 *		holdEnd
 * 인테리어모드
 *	
 * 빌드모드
*	wasdqe: 카메라 이동
 *	tap: 물체 상호작용
 *	doubleTap:
 *	hover
 *	hold
 *		holdStart
 *		holding
 *		holdEnd
 * 뷰모드
 *	wasdqe: 카메라 이동
 *	tap: 물체 상호작용
 *	doubleTap:
 *		select:
 *		menu:
 *	hover
 *	hold
 *		holdStart
 *		holding
 *		holdEnd
 *
 * 
 * 
 */
UCLASS()
class ATIHHsTapFocusTarget : public AActor
{
	GENERATED_BODY()
	friend class UTIHHsTapActionSubSystem;
public:
	virtual void GrapObject() {};

	//virtual void FocusOn();
private:
	UPROPERTY()
	FHitResult mHitResult;
};


using TIHHsControlPointKeyType = FString;
UCLASS(Blueprintable, BlueprintType)
class UTIHHsTapActionPoint : public UObject
{
	GENERATED_BODY()
	friend class UTIHHsTapActionSubSystem;
	static FTimerManager* gTimerManager;
	static UTIHHsTapActionSubSystem* gMouseSubSystem;
public:
	void OnSettingMouseSystem(TIHHsControlPointKeyType controlPointkey,float doubleTime,float holdTime)
	{
		ClearTimerHandles();
		mControlPointKey = controlPointkey;
		mDoubleTapThresholdTime = doubleTime;
		if (FMath::IsNearlyEqual(mDoubleTapThresholdTime,holdTime))
		{
			holdTime = 0.1f;
		}
		mHoldThresholdTime = mDoubleTapThresholdTime + holdTime;
	}
	
	void OnDownAction();
	void OnUpAction();

	void OnBroadcastTap();
	void OnDoubleTapTimerExpired();
	void OnBroadcastHoldStart();
	void OnBroadcastHolding();
	void OnBroadcastHoldEnd();
	void OnBroadcastDoubleTap();
	void ClearTimerHandles();

	bool GetHoldState() const
	{
		return mHoldState;
	}
	void SetHoldState(bool inHoldState)
	{
		mHoldState = inHoldState;
	}
	
	virtual void BeginDestroy() override;

private:
	TIHHsControlPointKeyType mControlPointKey;
	
	float mDoubleTapThresholdTime;
	float mHoldThresholdTime;
	bool mHoldState;
	bool mDoubleTapState;
	
	FTimerHandle mMouseDoubleTapTimerHandle;
	FTimerHandle mMouseHoldStartTimerHandle;
	FTimerHandle mMouseHoldingTimerHandle;
};

UCLASS()
class ATIHHsTapActionSubsystemActor : public AActor
{
	GENERATED_BODY()
	friend class UTIHHsTapActionSubSystem;
public:
	ATIHHsTapActionSubsystemActor();
private:
	UPROPERTY()
	TMap<FString,TObjectPtr<UTIHHsTapActionPoint>> mControlPointMap; 
};


UCLASS()
class UTIHHsTapActionSubSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	static int64 gMouseSubSystemTickCount;
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;	//	UWorld::BeginPlay()에서 호출된다.
	
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UTIHHsMouseSubSystem, STATGROUP_Tickables);
	}
private:
	bool IsTickEnable() const;

public:
	//	원래는 EnhancedInput 에서 호출해줘야함. 즉 이건 호출함수임.
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void OnDownAction();
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void OnUpAction();

	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void SettingControlPoint(FString inControlPointName,float doubleTime = 0.2,float holdTime = 0.3);
	
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action2")
	void OnDonwActionByControlPoint(FString inControlPointName);
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action2")
	void OnUpActionByControlPoint(FString inControlPointName);
	
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void OnMouseTraceAuto(float inInterval);
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void OnMoveTrack();
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void OffMoveTrack();
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem2|Action")
	void OnSettingMouseSystem(APlayerController* PlayerController);
	UPROPERTY()
	FTimerHandle mMouseDoubleTapTimerHandle;

	UPROPERTY()
	FTimerHandle mMouseHoldStartTimerHandle;
	UPROPERTY()
	FTimerHandle mMouseHoldingTimerHandle;
	
	void OnDoubleTapTimerExpired();

	
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void OnBroadcastDoubleTap();
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void OnBroadcastTap();
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void OnBroadcastHoldStart();
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void OnBroadcastHolding();
	UFUNCTION(BlueprintCallable, Category = "TIHHsMouseSubSystem|Action")
	void OnBroadcastHoldEnd();

private:
	UPROPERTY()
	TObjectPtr<ATIHHsTapActionSubsystemActor> mMouseSubsystemActor;
	
	void RegisterMouseAction(const FTIHHsMouseActionNode& inNode);
	//UPROPERTY()
	//TMap<FString,TObjectPtr<UTIHHsTapActionPoint>> mControlPointMap; 
	
	bool mTickForceDisable;
	bool mTickPauseEnable;
	bool mTickRuntimeActive;
	bool mTickInEditorActive;
	
	//	mouse info
	UPROPERTY()
	float mRayDistance;

	UPROPERTY()
	TEnumAsByte<ECollisionChannel> mMouseCollisionChannel;
	
	//	nodeList
	UPROPERTY()
	int32 mCurrNodeIndex;
	UPROPERTY()
	int32 mPrevNodeIndex;
	
	UPROPERTY()
	TArray<FTIHHsMouseActionNode> mMouseActionNodeList;

	//	EvalueateMouseAction
	UPROPERTY()
	bool mMouseActionEvaluating;
	int32 mMouseActionEvaluatingCount;
	
	//	mouse trace
	UPROPERTY()
	bool mMouseTraceEnable;	
	UPROPERTY()
	float mCurrMouseHoverInterval;
	UPROPERTY()
	float mPrevMouseHoverInterval;
	UPROPERTY()
	FTimerHandle mMouseTraceTimerHandle;
	UPROPERTY()
	bool mMouseTraceDelegateActive;
	

	// 	mouse thresholdTime
	int32 mFramePerSecond; 
	UPROPERTY()
	double mDoubleTapThresholdTime;	// 중요한건 이전 이벤트에서 해당 이벤트로의 시간이 경과가 되어야한다는것이다.
	UPROPERTY()
	double mHoldThresholdTime;
	
	UPROPERTY()
	double mSingleTapThresholdTime;
	
	// 	mouse hover
	//UPROPERTY()
	//FTIHHsMouseActionNode mCurrentHoverActionNode;

	UPROPERTY()
	int32 mCurrHoverNodeIndex;
	int32 mHoverNodeCount;
	UPROPERTY()
	TArray<FTIHHsMouseActionNode> mCurrentHoverActionNodes;
	float mMouseMoveSensitive;

	FTIHHsMouseActionNode& GetPrevHoverNode();
	FTIHHsMouseActionNode& GetCurrHoverNode();

	FTIHHsMouseActionNode& ProgressHoverNode();

	int32 mMouseState;
	
	UPROPERTY()
	APlayerController* mPlayerController;
	UWorld* mWorld;
	FHitResult mHitResult;

	UPROPERTY()
	TObjectPtr<ATIHHsTapFocusTarget> mFocusActor;
};




inline bool UTIHHsTapActionSubSystem::IsTickable() const
{
	return UTIHHsGlobalConfigure::IsTickEnable(mWorld);
}

inline bool UTIHHsTapActionSubSystem::IsTickableInEditor() const
{
#ifdef WITH_EDITOR
	return mTickInEditorActive;
#else
	return UTIHHsGlobalConfigure::IsTickEnable(mWorld);
#endif
	
}

inline bool UTIHHsTapActionSubSystem::IsTickableWhenPaused() const
{
	return UTIHHsGlobalConfigure::IsTickEnable(mWorld);
}
