// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/THsEnhancedActionSystem.h"

#include "CanvasItem.h"
#include "ContextObjectStore.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "BaseGizmos/CombinedTransformGizmo.h"
#include "BaseGizmos/GizmoViewContext.h"
#include "BaseGizmos/TransformGizmoUtil.h"
#include "BaseGizmos/TransformProxy.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Gizmo/THsInteractiveGizmoCore.h"

class IAssetRegistry;
class FAssetRegistryModule;

UE_DEFINE_GAMEPLAY_TAG(THS_Action_Trait, "Action.Trait");
UE_DEFINE_GAMEPLAY_TAG(THS_Action_Trait_Selectable, "Action.Trait.Selectable");
UE_DEFINE_GAMEPLAY_TAG(THS_Action_Trait_Hoverable	, "Action.Trait.Hoverable");
UE_DEFINE_GAMEPLAY_TAG(THS_Action_Trait_Attachable, "Action.Trait.Attachable");
UE_DEFINE_GAMEPLAY_TAG(THS_Action_Trait_Movable	, "Action.Trait.Movable");

bool FTHsActionKeyBehaviorNodeContainer::ExecuteNodes(UTHsEnhancedActionSystem* actionSystem)
{
	bool result = true;
	switch (ExecuteType) {
	case ETHsActionKeyBehaviorNodeExecuteType::EFirstOne:
		if (not Nodes.IsEmpty())
		{
			FTHsActionKeyBehaviorNode& last = Nodes[0];
			if (last.BehaviorQuery != nullptr)
			{
				result = last.BehaviorQuery->TryExecute(*this, actionSystem);
			}
		}
		break;
	case ETHsActionKeyBehaviorNodeExecuteType::ELastOne:
		if (not Nodes.IsEmpty())
		{
			FTHsActionKeyBehaviorNode& last = Nodes.Last();
			if (last.BehaviorQuery != nullptr)
			{
				result = last.BehaviorQuery->TryExecute(*this, actionSystem);
			}
		}
		break;
	case ETHsActionKeyBehaviorNodeExecuteType::EAll:
		for (FTHsActionKeyBehaviorNode& node :Nodes)
		{
			if (node.BehaviorQuery != nullptr)
			{
				result &= node.BehaviorQuery->TryExecute(*this, actionSystem);
			}
		}
		break;
	case ETHsActionKeyBehaviorNodeExecuteType::ESequence:
		for (FTHsActionKeyBehaviorNode& node :Nodes)
		{
			if (node.BehaviorQuery != nullptr)
			{
				if (node.BehaviorQuery->TryExecute(*this, actionSystem))
				{
					result = true;
				}
				else
				{
					result = false;
					break;
				}
			}
		}
		break;
	case ETHsActionKeyBehaviorNodeExecuteType::ESelection:
		for (FTHsActionKeyBehaviorNode& node :Nodes)
		{
			if (node.BehaviorQuery != nullptr)
			{
				if (node.BehaviorQuery->TryExecute(*this, actionSystem))
				{
					result = true;
					break;
				}
				else
				{
					result = false;
				}
			}
		}
		break;
	}

	return result;
}

void UTHsSettingSubsystem::AddCustomKey(const FKey& key, const FString& category)
{
}

void UTHsSettingSubsystem::RemoveCustomKey(const FKey& key)
{
}

void UTHsSettingSubsystem::InitializeDefaultKeys()
{
	KeyboardKeys = TIHUtils::GlobalConfigDefault::ALL_KEYBOARD_KEYS.Array();
	MouseKeys = TIHUtils::GlobalConfigDefault::ALL_MOUSE_KEYS.Array();
	TouchKeys = TIHUtils::GlobalConfigDefault::ALL_TOUCH_KEYS.Array();

	UE_LOG(LogTemp, Log, TEXT("UTHsSettingSubsystem::InitializeDefaultKeys: KeyboardKeys Count=%d"), KeyboardKeys.Num());
}

void UTHsActionPoint::InitActionPoint(UTHsEnhancedActionSystem* actionSystem, const UInputAction* inputAction, const FKey& key)
{
	mActionSystem = actionSystem;
	mWorldContext = actionSystem->GetWorldContext();
	mInputAction = inputAction;
	mKey = key;
	mIndexInSystem = mActionSystem->AddActivatedActionPoints(this);
	if (const UTHsEnhancedActionSystemDataSetting* dataSetting = mActionSystem->GetDataSetting())
	{
		mHoldingStartThreshold = dataSetting->GetDefaultHoldStartThreshold();
		mDoubleClickThreshold = dataSetting->GetDefaultDoubleClickThreshold();
	}
	PostInitActionPoint(mActionSystem,mInputAction,mKey,mIndexInSystem);
	ActivateActionPointEnd();
}
void UTHsActionPoint::ActivateActionPointBegin()
{
	ClearTimeData();
	mActionSystem->ActivateActionPoint(this);
}

void UTHsActionPoint::ActivateActionPointEnd()
{
	mActionSystem->DeActivateActionPoint(this);
}

void UTHsActionPoint::ClearTimeData()
{
	mCurrentDeltaTime = 0.0f;
	mActivationTime = 0.0f;
	mOngoingElapseTime = 0.0f;
	
	mRecordedFirstDownTime = 0.0f;
	mRecordedEnterOngoingTime = 0.0f;
	mRecordedDoubleClickTime = 0.0f;
}

void UTHsActionPoint::HwInputKeyActionDown(const FInputActionValue& Value)
{
	mStartedValue = Value;
	//mStatePressing = true;
	mPressingStateType = ETHsActionPointPressingStateType::EPressStarted;
	mActionSystem->ActionDown(this);
	
	switch (mEvaluationState) {
	case ETHsActionPointEvaluationState::EWaitingDown:
		mRecordedFirstDownTime = FPlatformTime::Seconds();
		mEvaluationState = ETHsActionPointEvaluationState::EEvaluateOngoingStart;
		ActivateActionPointBegin();
		break;
	case ETHsActionPointEvaluationState::EEvaluateOngoingStart:
		
		UE_LOG(LogTemp, Warning, TEXT("OnInputActionStarted - Invalid State: ECheckingHoldStart"));
		break;
	case ETHsActionPointEvaluationState::EOngoing:
		UE_LOG(LogTemp, Warning, TEXT("OnInputActionStarted - Invalid State: EOngoing"));
		break;
	case ETHsActionPointEvaluationState::ECheckingDoubleClick:
		mEvaluationState = ETHsActionPointEvaluationState::EWaitingDown;
		mActionSystem->ActionDoubleClicked(this);
		break;
	}
}

auto UTHsActionPoint::HwInputKeyActionUp(const FInputActionValue& Value) -> void
{
	mCompletedValue = Value;
	//mStatePressing = false;
	mPressingStateType = ETHsActionPointPressingStateType::EPressEnded;
	mActionSystem->ActionUp(this);
	
	switch (mEvaluationState) {
	case ETHsActionPointEvaluationState::EWaitingDown:
		UE_LOG(LogTemp, Warning, TEXT("OnInputActionStarted - Invalid State: EWaitingDown"));
		break;
	case ETHsActionPointEvaluationState::EEvaluateOngoingStart:
		mEvaluationState = ETHsActionPointEvaluationState::ECheckingDoubleClick;
		mRecordedDoubleClickTime = FPlatformTime::Seconds();
		mCurrentDeltaTime =0.0f;
		mOngoingElapseTime = 0.0f;
		
		break;
	case ETHsActionPointEvaluationState::EOngoing:
		mStateOngoing = false;
		mEvaluationState = ETHsActionPointEvaluationState::EWaitingDown;
		mActionSystem->ActionOngoingEnd(this,mCurrentDeltaTime);
		ActivateActionPointEnd();
		break;
	case ETHsActionPointEvaluationState::ECheckingDoubleClick:
		UE_LOG(LogTemp, Warning, TEXT("OnInputActionStarted - Invalid State: ECheckingDoubleClick"));
		break;
	}
}

void UTHsActionPoint::EvaluateActionPoint(float CurrentTime, float frameDeltaTime)
{
	mActivationTime += frameDeltaTime;
	
	switch (mPressingStateType)
	{
	case ETHsActionPointPressingStateType::ENone:
		break;
	case ETHsActionPointPressingStateType::EPressStarted:
		mActionSystem->ActionPressing(this,frameDeltaTime);
		mPressingStateType = ETHsActionPointPressingStateType::EPressOngoing;
		break;
	case ETHsActionPointPressingStateType::EPressOngoing:
	mActionSystem->ActionPressing(this,frameDeltaTime);
		break;
	case ETHsActionPointPressingStateType::EPressEnded:
		mActionSystem->ActionPressing(this,frameDeltaTime);
		mPressingStateType = ETHsActionPointPressingStateType::ENone;
		break;
	}
	
	switch (mEvaluationState) {
	case ETHsActionPointEvaluationState::EWaitingDown:
			//	비활성화 된거 여기에서 제거하는 방법도 있음, 
			break;
	case ETHsActionPointEvaluationState::EEvaluateOngoingStart:
		if (mCurrentDeltaTime + frameDeltaTime < mHoldingStartThreshold)
		{
			mCurrentDeltaTime += frameDeltaTime;
		}
		else
		{
			mStateOngoing = true;
			
			mRecordedEnterOngoingTime = FPlatformTime::Seconds();
			mActionBindContext.DelegateOngoingExpiredCallback.Broadcast(this);
			
			mOngoingElapseTime = frameDeltaTime;
			mEvaluationState = ETHsActionPointEvaluationState::EOngoing;
		
			mActionSystem->ActionOngoingStart(this,frameDeltaTime);
		}
		break;
	case ETHsActionPointEvaluationState::EOngoing:
		mOngoingElapseTime+= frameDeltaTime;
		mActionSystem->ActionOngoing(this,frameDeltaTime);
		
		break;
	case ETHsActionPointEvaluationState::ECheckingDoubleClick:
		if (mCurrentDeltaTime + frameDeltaTime < mDoubleClickThreshold)
		{
			mCurrentDeltaTime += frameDeltaTime;
		}
		else
		{
			mActionSystem->ActionClicked(this);
			mEvaluationState = ETHsActionPointEvaluationState::EWaitingDown;
			ActivateActionPointEnd();
		}
		break;
	}
	PostEvaluateActionPoint(CurrentTime,frameDeltaTime);
}


void UTHsActionPoint::HwInputKeyActionTriggered(const FInputActionValue& Value)
{
	mTriggeredValue = Value;
}

void UTHsActionPoint::HwInputKeyActionCancel(const FInputActionValue& Value)
{
	switch (mEvaluationState) {
	case ETHsActionPointEvaluationState::EWaitingDown:
		break;
	case ETHsActionPointEvaluationState::EEvaluateOngoingStart:
		break;
	case ETHsActionPointEvaluationState::EOngoing:
		break;
	case ETHsActionPointEvaluationState::ECheckingDoubleClick:
		break;
	}
}

void UTHsActionPoint::HwInputKeyActionOngoing(const FInputActionValue& Value)
{
}

void UTHsActionPoint::SetActionSystem(UTHsEnhancedActionSystem* actionSystem)
{
	mActionSystem = actionSystem;
}




void ATHsActionCursor::InitActionCursor(UTHsEnhancedActionSystem* actionSystem, APlayerController* playerController,
	UTHsEnhancedActionSystemDataSetting* dataSetting)
{
	mActionSystem = actionSystem;
	mPlayerController = playerController;
	int32 dataSettingCount = 4;
	if (dataSetting)
	{
		if (1 < dataSetting->GetReserveRecordCount() )
		{
			dataSettingCount = dataSetting->GetReserveRecordCount();
		}
		mMoveSensitive = FMath::Max(0.00001f, dataSetting->GetMouseMoveSensitive());
		mMoveTimeThreshold = FMath::Max(1.0f / 360.f, dataSetting->GetMouseMoveTimeThreshold());
		mLineTraceDistance = FMath::Max(100.0f, dataSetting->GetLineTraceDistance());
		//mQueryParams.AddIgnoredActor(this);
	}
	
	mRecords.SetNum(dataSettingCount);
	
	for (int32 i = 0; i < dataSettingCount; ++i)
	{
		mRecords[i].PrevIndex = i - 1;
		mRecords[i].SelfIndex = i;
		mRecords[i].NextIndex = (i + 1) % dataSettingCount;
	}
	
	mCurrentIndex = 0;
	mRecords[0].PrevIndex = mRecords.Num() - 1;
	mActionParamIn.CallerObject = this;
	mRecords[mRecords[0].PrevIndex].NextIndex = 0;

	mHoverFilter.Reset(NewObject<UTHsFilter>(this));
	mHoverFilter->InitFilter({ THS_Action_Trait_Hoverable.GetTag() });

	mTraceChannel = ECC_Visibility;
}

void ATHsActionCursor::ExecuteCursorHoverActions(const FTHsActionCursorData& currRecordTarget,const FTHsActionCursorData& prevRecordTarget,float deltaTime)
{
	if (prevRecordTarget.ActorHoverType == ETHsActionCursorHoverType::EHoverEnd)
	{
		{
			FString debugString = FString::Printf(TEXT("%d PrevHoverType:"),prevRecordTarget.SelfIndex);
			
			switch (prevRecordTarget.ActorHoverType) {
			case ETHsActionCursorHoverType::ENone:debugString += TEXT("ENone "); break;
				break;
			case ETHsActionCursorHoverType::EHoverStart: debugString += TEXT("EHoverStart "); break;
				break;
			case ETHsActionCursorHoverType::EHoverOngoing: debugString += TEXT("EHoverOngoing ");
				break;
			case ETHsActionCursorHoverType::EHoverEnd: debugString += TEXT("EHoverEnd ");
				break;
			}
			debugString += FString::Printf(TEXT(" | %d CurrHoverType:"),currRecordTarget.SelfIndex);
			switch (currRecordTarget.ActorHoverType) {
			case ETHsActionCursorHoverType::ENone: debugString += TEXT("ENone ");
				break;
			case ETHsActionCursorHoverType::EHoverStart: debugString += TEXT("EHoverStart ");
				break;
			case ETHsActionCursorHoverType::EHoverOngoing: debugString += TEXT("EHoverOngoing ");
				break;
			case ETHsActionCursorHoverType::EHoverEnd: debugString += TEXT("EHoverEnd ");
				break;
			}
			UE_LOG( LogTemp, Log, TEXT("ATHsActionCursor::ExecuteCursorHoverActions: %s"), *debugString);
			if (prevRecordTarget.IsHoverActive() && prevRecordTarget.Hit.GetActor())
			{
				UE_LOG( LogTemp, Log, TEXT("    Prev Hover Actor: %s"), *prevRecordTarget.Hit.GetActor()->GetName());
			}
		}

		
		mActionSystem->ActionHoverEnd(this,deltaTime);
	}
	
	switch (currRecordTarget.ActorHoverType) {
	case ETHsActionCursorHoverType::ENone:
		break;
	case ETHsActionCursorHoverType::EHoverStart:
		mActionSystem->ActionHoverStart(this,deltaTime);
		break;
	case ETHsActionCursorHoverType::EHoverOngoing:
		mActionSystem->ActionHoverOngoing(this,deltaTime);
		break;
	case ETHsActionCursorHoverType::EHoverEnd:
		break;
	}
}


void ATHsActionCursor::UpdateCursorMoveForRecord(float deltaTime, FTHsActionCursorData& curRecordTarget,
	const FTHsActionCursorData& prevRecordTarget)
{
	curRecordTarget.Clear();
	FVector2D rawPixelPos;
	
	mPlayerController->GetMousePosition(rawPixelPos.X, rawPixelPos.Y);
		
	curRecordTarget.ScreenPosition = rawPixelPos;
		
	FVector2D deltaScreenPos = curRecordTarget.CalculatedDeltaScreenPosition(prevRecordTarget);
	if (deltaScreenPos.SizeSquared() > mMoveSensitive)
	{
		curRecordTarget.OnActionStateMove();
	}
		
	if (mPlayerController->DeprojectScreenPositionToWorld(curRecordTarget.ScreenPosition.X, curRecordTarget.ScreenPosition.Y, curRecordTarget.WorldLocation, curRecordTarget.WorldDirection))
	{
		curRecordTarget.OnActionDeprojectionValid();
		curRecordTarget.WorldRayEnd = curRecordTarget.WorldLocation + (curRecordTarget.WorldDirection * mLineTraceDistance);
	}
		
	if (curRecordTarget.IsActionStateMove())	//	이걸 위에 움직인것과 같이 두지 않은이유는 DeprojectScreenPositionToWorld 이거 때문임.
	{
		mDeltaScreenPosition = deltaScreenPos;
		mDeltaScreenDirection = deltaScreenPos.GetSafeNormal();
		mDeltaWorldLocation = curRecordTarget.CalculatedDeltaWorldLocation(prevRecordTarget);
		mDeltaWorldDirection = curRecordTarget.CalculatedDeltaWorldDirection(prevRecordTarget);
			
		mElapsedMoveTime += deltaTime;
	}
	else
	{
		mElapsedMoveTime = 0.0f;
		mDeltaScreenPosition = FVector2D::ZeroVector;
		mDeltaScreenDirection = FVector2D::ZeroVector;
		mDeltaWorldLocation = FVector::ZeroVector;
		mDeltaWorldDirection = FVector::ZeroVector;
		mPauseMoveTime += deltaTime;
	}
	
	UpdateCursorMovementState(curRecordTarget, deltaTime);
}

void ATHsActionCursor::UpdateCursorMovementState(FTHsActionCursorData& currRecordTarget, float deltaTime)
{
	if (currRecordTarget.IsActionStateMove())
	{
		//	For mouseMove
		switch (currRecordTarget.MoveState)
		{
		case ETHsActionCursorMoveState::EMouseMovePaused:
		case ETHsActionCursorMoveState::EMouseMoveEnded:
			currRecordTarget.MoveState = ETHsActionCursorMoveState::EMouseMoveStarted;
			break;
		case ETHsActionCursorMoveState::EMouseMoveStarted:
		case ETHsActionCursorMoveState::EMouseMoveOngoing:
			currRecordTarget.MoveState = ETHsActionCursorMoveState::EMouseMoveOngoing;
			break;
		}
		mActionSystem->ActionMouseMove(this,deltaTime);
	}
	else
	{
		switch (currRecordTarget.MoveState)
		{
		case ETHsActionCursorMoveState::EMouseMovePaused:
			break;
		case ETHsActionCursorMoveState::EMouseMoveEnded:
			currRecordTarget.MoveState = ETHsActionCursorMoveState::EMouseMovePaused;
			break;
		case ETHsActionCursorMoveState::EMouseMoveStarted:
		case ETHsActionCursorMoveState::EMouseMoveOngoing:
			currRecordTarget.MoveState = ETHsActionCursorMoveState::EMouseMoveEnded;
			break;
		}
	}
}

void ATHsActionCursor::VerifyHitForRecord()
{
	FTHsActionCursorData& curRecordTarget = mRecords[mCurrentIndex];
	if (curRecordTarget.IsActionDeprojectionValid())
	{
		FVector start = curRecordTarget.WorldLocation;
		FVector end = start + curRecordTarget.WorldDirection * 10000;
		
		if (GetWorld()->LineTraceSingleByChannel(
				curRecordTarget.Hit,
				start, end,
				mTraceChannel,
				mQueryParams)
		)
		{
			curRecordTarget.OnActionStateHit();
			
		}
		DrawDebugLine(GetWorld(), start, end, FColor::Yellow, false, 0.001f, 0, 0.5f);
		//	else { curRecordTarget.OffActionStateHit();}
	}
}

void ATHsActionCursor::UpdateHitActorForRecord(FTHsActionCursorData& curRecordTarget,const FTHsActionCursorData& prevRecordTarget) const
{
	if (AActor* hitActor = curRecordTarget.Hit.GetActor())
	{
		if (prevRecordTarget.Hit.GetActor() == hitActor)
		{
			curRecordTarget.OnActionStateHit();
			curRecordTarget.OnActionStateSameHitActor();
		}
		TScriptInterface<ITIHHsInteraction> castedInteractionActor(hitActor);
		if (castedInteractionActor.GetObject() != nullptr)
		{
			curRecordTarget.OnActionStateCastedTIHHsInteractionActor();
			curRecordTarget.CacheInteractionActor = castedInteractionActor;
		}
	}
	else
	{
		curRecordTarget.OffActionStateSameHitActor();
		curRecordTarget.OffActionStateCastedTIHHsInteractionActor();
	}
}

void ATHsActionCursor::UpdateHitComponentForRecord(FTHsActionCursorData& curRecordTarget,const FTHsActionCursorData& prevRecordTarget)
{
	if (UPrimitiveComponent* hitComponent =curRecordTarget.Hit.GetComponent())
	{
		if (prevRecordTarget.Hit.GetComponent() == hitComponent)
		{
			curRecordTarget.OnActionStateHit();
			curRecordTarget.OnActionStateSameComponent();
		}
		TScriptInterface<ITIHHsInteraction> castedInteractionComponent(hitComponent);
		if (castedInteractionComponent.GetObject() != nullptr)
		{
			curRecordTarget.OnActionStateCastedTIHHsInteractionComponent();
			curRecordTarget.CacheInteractionComponent = castedInteractionComponent;
		}
	}
	else
	{
		curRecordTarget.OffActionStateSameComponent();
		curRecordTarget.OffActionStateCastedTIHHsInteractionComponent();
	}
}

void ATHsActionCursor::UpdateCursorMovement(float deltaTime)
{
	// if (mPlayerController)
	// {
	// 	FTHsActionCursorData& curRecordTarget = mRecords[mCurrentIndex];
	// 	FTHsActionCursorData& prevRecordTarget = mRecords[curRecordTarget.PrevIndex];
	// 	/*
	// 	 *	hitWidget -> hitGizmo -> hitActor
	// 	 */
	// 	
	// 	curRecordTarget.bWritting = true;
	// 	UpdateCursorMoveForRecord(deltaTime, curRecordTarget,prevRecordTarget);
	// 	
	// 	VerifyHitForRecord();
	// 	
	// 	UpdateHoverStateForRecord(curRecordTarget,prevRecordTarget,deltaTime );
	// }
}

void ATHsActionCursor::UpdateCursorRecordBegin(FTHsActionCursorData& curRecordTarget, const FTHsActionCursorData& prevRecordTarget, float deltaTime)
{
	if (mPlayerController)
	{
		curRecordTarget.bWritting = true;
		UpdateCursorMoveForRecord(deltaTime, curRecordTarget,prevRecordTarget);
	}
}

void ATHsActionCursor::UpdateCursorRecordEnd(FTHsActionCursorData& currRecordTarget, FTHsActionCursorData& prevRecordTarget, float deltaTime)
{
	if (mPlayerController)
	{
		UpdateHitActorForRecord(currRecordTarget, prevRecordTarget);
		UpdateHitComponentForRecord(currRecordTarget, prevRecordTarget);

		// mHoverProcessed = currRecordTarget.IsActionStateHit() && 
		// mActionSystem->IsHoverHitPossible() &&
		// mActionSystem->GetCurrentHitType() == ETHsActionCursorHitCategory::EStuff;
		
		UpdateHoverStateForRecord(currRecordTarget,prevRecordTarget,deltaTime );
		
		ExecuteCursorHoverActions(currRecordTarget,prevRecordTarget,deltaTime);
		
		currRecordTarget.bWritting = false;
	}
}

void ATHsActionCursor::DebugLogRecord()
{
#if !(UE_BUILD_SHIPPING)
	// 1) 문자열 구성(너 코드 기반, 조건 보정)
	FString DebugString = FString::Printf(TEXT("ScreenPosition: %s "), *GetCurrentRecord().ScreenPosition.ToString());
	DebugString += FString::Printf(TEXT("| WorldLocation: %s "), *GetCurrentRecord().WorldLocation.ToString());
	DebugString += FString::Printf(TEXT("| WorldDirection: %s "), *GetCurrentRecord().WorldDirection.ToString());
	DebugString += FString::Printf(TEXT("| DeltaScreenPosition: %s "), *mDeltaScreenPosition.ToString());

	if (!mDeltaWorldLocation.IsNearlyZero())
	{
		DebugString += FString::Printf(TEXT("| DeltaWorldLocation: %s "), *mDeltaWorldLocation.ToString());
		// 월드 방향 화살표(네가 하던 디버그)
		//DrawDebugDirectionalArrow(
		//	GetWorld(),
		//	GetCurrentRecord().WorldLocation,
		//	GetCurrentRecord().WorldLocation + mDeltaWorldLocation * 100.0f,
		//	10.0f,
		//	FColor::Green,
		//	false,
		//	0.0f,   // 한 프레임
		//	0,
		//	2.0f
		//);
	}
	if (!mDeltaWorldDirection.IsNearlyZero())
	{
		DebugString += FString::Printf(TEXT("| DeltaWorldDirection: %s "), *mDeltaWorldDirection.ToString());
	}
	if (!mDeltaScreenDirection.IsNearlyZero())
	{
		DebugString += FString::Printf(TEXT("| DeltaScreenDirection: %s "), *mDeltaScreenDirection.ToString());
	}

	if (GetCurrentRecord().Hit.IsValidBlockingHit())
	{
		switch (GetCurrentRecord().ActorHoverType)
		{
		case ETHsActionCursorHoverType::ENone:
			DebugString += FString::Printf(TEXT("| Hit: %s "), *GetCurrentRecord().Hit.ToString()); break;
		case ETHsActionCursorHoverType::EHoverStart:
			DebugString += FString::Printf(TEXT("| Hit: %s (HoverStart) "), *GetCurrentRecord().Hit.ToString()); break;
		case ETHsActionCursorHoverType::EHoverOngoing:
			DebugString += FString::Printf(TEXT("| Hit: %s (HoverOngoing) "), *GetCurrentRecord().Hit.ToString()); break;
		case ETHsActionCursorHoverType::EHoverEnd:
			DebugString += FString::Printf(TEXT("| Hit: %s (HoverEnd) "), *GetCurrentRecord().Hit.ToString()); break;
		}
	}
	if (GetCurrentRecord().IsActionStateMove())                           DebugString += TEXT("| ActionState: Move ");
	if (GetCurrentRecord().IsActionStateHit())                            DebugString += TEXT("| ActionState: Hit ");
	if (GetCurrentRecord().IsActionStateSameHitActor())                   DebugString += TEXT("| ActionState: SameHitActor ");
	if (GetCurrentRecord().IsActionStateSameComponent())                  DebugString += TEXT("| ActionState: SameComponent ");
	if (GetCurrentRecord().IsActionStateCastedTIHHsInteractionActor())    DebugString += TEXT("| ActionState: CastedTIHHsInteractionActor ");
	if (GetCurrentRecord().IsActionStateCastedTIHHsInteractionComponent())DebugString += TEXT("| ActionState: CastedTIHHsInteractionComponent ");

	UE_LOG(LogTemp, Log, TEXT("%s"), *DebugString);

	// 2) 화면 우상단 표시(방향 화살표 + 텍스트) — 한 함수 안에서 One-shot 등록/해제
	//    - ΔScreenDirection 기준으로 화살표 방향, 없으면 오른쪽(1,0)
	const FVector2D Dir2D = mDeltaScreenDirection.IsNearlyZero()
		                        ? FVector2D(1.f, 0.f)
		                        : mDeltaScreenDirection.GetSafeNormal();

	// 지역 static으로 델리게이트 핸들 유지(다음 호출 전에 기존 등록 해제)
	static FDelegateHandle S_Handle;
	if (S_Handle.IsValid())
	{
		UDebugDrawService::Unregister(S_Handle);
		S_Handle.Reset();
	}

	// 그릴 때 쓸 파라미터를 값 캡처
	const float Margin   = 18.f;
	const float ArrowLen = 60.f;
	const float HeadLen  = 14.f;
	const float HeadW    = 7.f;
	const FString TextToShow = DebugString;
	
	FDebugDrawDelegate Draw = FDebugDrawDelegate::CreateLambda(
		[Dir2D, Margin, ArrowLen, HeadLen, HeadW, TextToShow](UCanvas* Canvas, APlayerController* PC)
		{
			if (!Canvas) return;

			const float W = Canvas->SizeX;
			const float H = Canvas->SizeY;

			// 화살표 앵커(우상단 살짝 안쪽)
			const FVector2D Anchor(W - Margin - ArrowLen, Margin + ArrowLen);
			const FVector2D A = Anchor;
			const FVector2D B = Anchor + Dir2D * ArrowLen;

			// 본체
			FCanvasLineItem L(A, B);
			L.SetColor(FLinearColor::Yellow);
			L.LineThickness = 2.f;
			Canvas->DrawItem(L);

			// 화살촉
			const FVector2D Perp(-Dir2D.Y, Dir2D.X);
			const FVector2D H1 = B - Dir2D * HeadLen + Perp * HeadW;
			const FVector2D H2 = B - Dir2D * HeadLen - Perp * HeadW;

			FCanvasLineItem L1(B, H1); L1.SetColor(FLinearColor::Yellow); L1.LineThickness = 2.f; Canvas->DrawItem(L1);
			FCanvasLineItem L2(B, H2); L2.SetColor(FLinearColor::Yellow); L2.LineThickness = 2.f; Canvas->DrawItem(L2);

			// 텍스트(우상단 화살표 밑)
			UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
			const float TextX = W - 520.f;          // 우측 정렬 흉내
			const float TextY = Margin + ArrowLen + 20.f;
			FCanvasTextItem T(FVector2D(TextX, TextY), FText::FromString(TextToShow),
			                  Font ? Font : GEngine->GetMediumFont(), FLinearColor::White);
			T.EnableShadow(FLinearColor::Black);
			Canvas->DrawItem(T);
		});

	// 등록하고
	S_Handle = UDebugDrawService::Register(TEXT("Game"), Draw);

	// 다음 틱에 자동 해제(한 프레임만 그리도록)
	if (UWorld* W = GetWorld())
	{
		FTimerHandle Tmp;
		W->GetTimerManager().SetTimerForNextTick([]
		{
			if (S_Handle.IsValid())
			{
				UDebugDrawService::Unregister(S_Handle);
				S_Handle.Reset();
			}
		});
	}
#endif // !(UE_BUILD_SHIPPING)
}


void ATHsActionCursor::UpdateHoverStateForRecord(FTHsActionCursorData& currRecordTarget,
	FTHsActionCursorData& prevRecordTarget, float deltaTime) const
{
	bool currentActiveEnable = currRecordTarget.IsHitActiveStuff();
	bool prevAcivatied = prevRecordTarget.IsHitActiveStuff();

	if (mActionSystem->IsHoverHitPossible() && mActionSystem->GetCurrentHitType() == ETHsActionCursorHoverHitCategory::EStuff)
	{
		if (currentActiveEnable)
		{
			if(prevAcivatied)
			{
				if (currRecordTarget.IsActionStateAnySame())
				{
					currRecordTarget.ActorHoverType = ETHsActionCursorHoverType::EHoverOngoing;
				}
				else
				{
					currRecordTarget.ActorHoverType = ETHsActionCursorHoverType::EHoverStart;
					prevRecordTarget.ActorHoverType = ETHsActionCursorHoverType::EHoverEnd;
				}
			}
			else
			{
				currRecordTarget.ActorHoverType = ETHsActionCursorHoverType::EHoverStart;
			}
		}
		else
		{
			currRecordTarget.ActorHoverType = ETHsActionCursorHoverType::ENone;
			if (prevAcivatied)
			{
				prevRecordTarget.ActorHoverType = ETHsActionCursorHoverType::EHoverEnd;
			}
		}
	}//	if (mActionSystem->IsHoverHitPossible() && mActionSystem->GetCurrentHitType() == ETHsActionCursorHitCategory::EStuff)
	else
	{
		if (prevAcivatied && prevRecordTarget.CheckCursorHitCategory(ETHsActionCursorHoverHitCategory::EStuff))
		{
			prevRecordTarget.ActorHoverType = ETHsActionCursorHoverType::EHoverEnd;
		}
		currRecordTarget.ActorHoverType = ETHsActionCursorHoverType::ENone;
	}
}

void ATHsActionCursor::OverrideActionPointHoverStart(ATHsActionCursor* cursor)
{
	const FTHsActionCursorData& curRecord = GetCurrentRecord();
	if (curRecord.IsActionStateCastedTIHHsInteractionActor())
	{
		if (mHoverFilter->IsMatch(curRecord.CacheInteractionActor))
		{
			curRecord.CacheInteractionActor->ActionProcessHoverStart_Deprecated(mActionParamIn);
		}
		
	}
}

void ATHsActionCursor::OverrideActionPointHoverOngoing(ATHsActionCursor* cursor)
{
	const FTHsActionCursorData& curRecord = GetCurrentRecord();
	if (curRecord.IsActionStateCastedTIHHsInteractionActor())
	{
		if (mHoverFilter->IsMatch(curRecord.CacheInteractionActor))
		{
			curRecord.CacheInteractionActor->ActionProcessHoverOngoing_Deprecated(mActionParamIn);
		}
	}
}

void ATHsActionCursor::OverrideActionPointHoverEnd(ATHsActionCursor* cursor)
{
	const FTHsActionCursorData& curRecord = GetCurrentRecord();
	if (curRecord.IsActionStateCastedTIHHsInteractionActor())
	{
		if (mHoverFilter->IsMatch(curRecord.CacheInteractionActor))
		{
			curRecord.CacheInteractionActor->ActionProcessHoverEnd_Deprecated(mActionParamIn);
		}
	}
}



/*
================================================================================================================
									Gizmo 
 */

FTHsToolsContextBase::FTHsToolsContextBase(UTHsEnhancedActionSystem* actionSystem, UTHsGizmoManagerWrapper* owner): mActionSubsystem(actionSystem)
	, mOwner(owner)
{}

FTHsToolsContextQueriesAPI::FTHsToolsContextQueriesAPI(UTHsEnhancedActionSystem* actionSystem,
	UTHsGizmoManagerWrapper* owner): FTHsToolsContextBase(actionSystem, owner), mSelectedActorsArrPtr(nullptr), mSelectedComponentsArrPtr(nullptr)
{
}

void FTHsToolsContextQueriesAPI::GetCurrentSelectionState(FToolBuilderState& StateOut) const
{
	StateOut.World = GetWorldContext();
	if (const UTHsGizmoManagerWrapper* Owner = GetOwner())
	{
		if (const UInteractiveToolsContext* toolContext = Owner->GetToolsContext())
		{
			StateOut.ToolManager = toolContext->ToolManager;
			StateOut.GizmoManager = toolContext->GizmoManager;
			StateOut.TargetManager = toolContext->TargetManager;
		}
	}
	if (mSelectedActorsArrPtr&&not mSelectedActorsArrPtr->IsEmpty())
	{
		StateOut.SelectedActors = *mSelectedActorsArrPtr;
	}
	if (mSelectedComponentsArrPtr && not mSelectedComponentsArrPtr->IsEmpty())
	{
		StateOut.SelectedComponents = *mSelectedComponentsArrPtr;
	}
}

void FTHsToolsContextQueriesAPI::GetCurrentViewState(FViewCameraState& StateOut) const
{
	GetActionSystem()->GetCurrentViewState(StateOut);
}

UMaterialInterface* FTHsToolsContextQueriesAPI::GetStandardMaterial(EStandardToolContextMaterials MaterialType) const
{
	UMaterialInterface* result = nullptr;
	result = mActionSubsystem->GetCurrentGizmoDefaultMaterial();
	 return result;
}

FViewport* FTHsToolsContextQueriesAPI::GetHoveredViewport() const
{
	FViewport* result = nullptr;
	if (GEngine && GEngine->GameViewport)
	{
		result = GEngine->GameViewport->Viewport;
	}
	return result;
}

void FTHsToolsContextQueriesAPI::PostInitToolsContext(const UTHsEnhancedActionSystemDataSetting* dataSetting)
{
	FTHsToolsContextBase::PostInitToolsContext(dataSetting);

	UTHsGizmoManagerWrapper* gizmoMgrWrapper = GetGizmoManagerWrapper();
	mSelectedActorsArrPtr = &(gizmoMgrWrapper->GetSelectedActors());
	mSelectedComponentsArrPtr = &(gizmoMgrWrapper->GetSelectedComponents());
	
	
}

FTHsToolsContextTransactionsAPI::FTHsToolsContextTransactionsAPI(UTHsEnhancedActionSystem* const actionSystem,
                                                                 UTHsGizmoManagerWrapper* const owner): FTHsToolsContextBase(actionSystem, owner)
{}

void FTHsToolsContextTransactionsAPI::DisplayMessage(const FText& Message, EToolMessageLevel Level)
{
}

void FTHsToolsContextTransactionsAPI::PostInvalidation()
{
	UE_LOG(LogTemp, Warning, TEXT("askl;djflkajsdklfjklasjdf"));
}

void FTHsToolsContextTransactionsAPI::BeginUndoTransaction(const FText& Description)
{
}

void FTHsToolsContextTransactionsAPI::EndUndoTransaction()
{
}

void FTHsToolsContextTransactionsAPI::AppendChange(UObject* TargetObject, TUniquePtr<FToolCommandChange> Change,
	const FText& Description)
{
}

bool FTHsToolsContextTransactionsAPI::RequestSelectionChange(const FSelectedObjectsChangeList& SelectionChange)
{
	return false;
}

void FTHsToolsContextTransactionsAPI::PostInitToolsContext(const UTHsEnhancedActionSystemDataSetting* dataSetting)
{
	FTHsToolsContextBase::PostInitToolsContext(dataSetting);
}

void ATHsTestCube::BeginPlay()
{
	Super::BeginPlay();

	mStaticMeshComponent = NewObject<UStaticMeshComponent>(this);
	mStaticMeshComponent->RegisterComponent();
	mStaticMeshComponent->SetMobility(EComponentMobility::Movable);
	mStaticMeshComponent->AddWorldTransform(FTransform(FVector(200,0,100)));
	//	/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'
	UStaticMesh* cubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	mStaticMeshComponent->SetStaticMesh(cubeMesh);
	RootComponent = mStaticMeshComponent;
}

void UTHsGizmoManagerWrapper::DebugTestCube()
{
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = mActionSystem->GetActionCursor();
	spawnParams.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
	mTestCube = mWorldContext->SpawnActor<ATHsTestCube>(ATHsTestCube::StaticClass(),spawnParams);
	
	mTransformProxy = NewObject<UTransformProxy>(mTestCube);
	mTransformProxy->OnBeginTransformEdit.AddLambda(
		[
		weakThis = TWeakObjectPtr(this),
		weakTarget = TWeakObjectPtr<ATHsTestCube>(mTestCube)
		](UTransformProxy* inProxy)
		{
			UE_LOG(LogTemp, Warning, TEXT("Transform Begin Edit Lambda"));
			if (weakThis.IsValid() && inProxy && weakTarget.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Transform Begin Edit"));
			}
		});

		
	// =========================[ 테스트 ]=========================
		
	UStaticMeshComponent* StaticMeshComp =  Cast<UStaticMeshComponent>(mTestCube->GetRootComponent());
	mTransformProxy->AddComponent(StaticMeshComp, true);
	mCombinedTransformGizmo = UE::TransformGizmoUtil::Create3AxisTransformGizmo(mToolManger,mTestCube,TEXT("testGizmo"));

	mCombinedTransformGizmo->SetWorld(mWorldContext);
	mCombinedTransformGizmo->SetVisibility(true);
	mCombinedTransformGizmo->SetActiveTarget(mTransformProxy);
	const FTransform TargetInitialTransform = mTransformProxy->GetTransform(); 
	mCombinedTransformGizmo->ReinitializeGizmoTransform(TargetInitialTransform);
		
	mCombinedTransformGizmo->SetUpdateHoverFunction(
		[this](UPrimitiveComponent* HoveredComponent, bool bHovering)
		{
			if (bHovering)
			{
				FString sival = HoveredComponent->GetRelativeTransform().ToString();
				UE_LOG(LogTemp, Warning, TEXT("what         %s            ?"),*sival);
			}
		});
}

void UTHsGizmoManagerWrapper::InitGizmoManager(UTHsEnhancedActionSystem* actionSystem, UWorld* worldContext)
{
	//mInteractiveGizmoManager = NewObject<UInteractiveGizmoManager>(this);
	mActionSystem = actionSystem;
	mWorldContext = worldContext;

	mToolsContextQueriesAPI = MakeUnique<FTHsToolsContextQueriesAPI>(mActionSystem,this);
	mToolsContextTransactionsAPI = MakeUnique<FTHsToolsContextTransactionsAPI>(mActionSystem,this);

	const UTHsEnhancedActionSystemDataSetting* dataSetting = mActionSystem->GetDataSetting();
	mToolsContextQueriesAPI->InitToolsContext(mWorldContext,dataSetting);
	mToolsContextTransactionsAPI->InitToolsContext(mWorldContext,dataSetting);

	//	중요!!!
	mToolsContext  = NewObject<UInteractiveToolsContext>(this);
	mToolsContext->Initialize(mToolsContextQueriesAPI.Get(), mToolsContextTransactionsAPI.Get());

	if (TObjectPtr<UInputRouter> inputRouter = mToolsContext->InputRouter)
	{
		inputRouter->bAutoInvalidateOnCapture = true;
		inputRouter->bAutoInvalidateOnHover = true;
	}
	
	mGizmoManager = mToolsContext->GizmoManager;
	mInputRouter = mToolsContext->InputRouter;
	mTargetManager = mToolsContext->TargetManager;
	mToolManger = mToolsContext->ToolManager;

	if (UE::TransformGizmoUtil::RegisterTransformGizmoContextObject(mToolsContext))
	{
		//DebugTestCube();
	}
	//	register view extension
	if (UContextObjectStore* Store = mToolsContext->ContextObjectStore)
	{
		if (UGizmoViewContext* ViewCtx = Store->FindContext<UGizmoViewContext>())
		{
			mGizmoViewExtension = FSceneViewExtensions::NewExtension< FTHsGizmoViewExtension >(ViewCtx,this);
		}
	}
}

void UTHsGizmoManagerWrapper::BeginDestroy()
{
	Super::BeginDestroy();
	if (mToolsContext)
	{
		//mToolsContext->Shutdown();
		mToolsContext = nullptr;
	}
	mGizmoManager = nullptr;
	
	mToolsContextQueriesAPI.Reset();
	mToolsContextTransactionsAPI.Reset();

	mActionSystem = nullptr;
	mWorldContext = nullptr;
}

void UTHsGizmoManagerWrapper::UpdateAutoValidation(float deltaTime)
{
	if (mAutoValidationTime + deltaTime <mAutoValidationMax)
	{
		mAutoValidationTime += deltaTime;
	}
	else
	{
		mAutoValidationTime = 0.0f;
		mValidationUpdate = ValidationToolsContext();
	}
}

void UTHsGizmoManagerWrapper::UpdateMouseInputStateForGizmo(FInputDeviceState& deviceState)
{
	deviceState.InputDevice = EInputDevices::Mouse;
	deviceState.Mouse.Position2D = mActionSystem->GetPosition2D();
	deviceState.Mouse.Delta2D = mActionSystem->GetDeltaScreenPosition();
	deviceState.Mouse.WorldRay = mActionSystem->GetWorldRay();
	mActionSystem->CaptureModifyKeyForGizmo(deviceState);
}

void UTHsGizmoManagerWrapper::UpdateGizmo(float deltaTime)
{
	if (mValidationUpdate)
	{
		const FKey leftMouseKey = EKeys::LeftMouseButton;
		FInputDeviceState deviceState = {};
		deviceState.InputDevice = EInputDevices::Mouse;
		deviceState.Mouse.Left.Button = leftMouseKey;
		UpdateMouseInputStateForGizmo(deviceState);
		
		const bool bNow = mActionSystem->IsActiveActionPointByKey(EKeys::LeftMouseButton);
		static bool bPrev = false;
		deviceState.Mouse.Left.Button    = EKeys::LeftMouseButton;
		deviceState.Mouse.Left.bPressed  = ( bNow && !bPrev );
		deviceState.Mouse.Left.bReleased = (!bNow &&  bPrev );
		deviceState.Mouse.Left.bDown     = bNow;
		
		const bool bPending = deviceState.Mouse.Left.bPressed || deviceState.Mouse.Left.bReleased;
		const bool bCapturing = mInputRouter->HasActiveMouseCapture();
		
		if (bPending || bCapturing)
		{
			//	여기에서 캡처가 걸렸는지 여부를 판단해야함. 그래야 뒤에 나중에 물체 클릭쪽으로 안넘김.
			mInputRouter->PostInputEvent(deviceState);

			// 4) 다운 루프는 '실제 캡처가 잡혔을 때만' 유지
			if (deviceState.Mouse.Left.bPressed && !mInputRouter->HasActiveMouseCapture())
			{
				// 캡처 실패 → 즉시 idle
				bPrev = false;
			}
			else
			{
				bPrev = bNow;
			}
		}
		else
		{
			mInputRouter->PostHoverInputEvent(deviceState);
			bPrev = false;
		}
		
		//UE_LOG(LogTemp, Warning, TEXT("leftState : %s"),*ToStringInputDeviceState());

		mToolManger->Tick(deltaTime);
		mGizmoManager->Tick(deltaTime);
	}
	UpdateAutoValidation(deltaTime);
}

bool UTHsGizmoManagerWrapper::ValidationToolsContext()
{
	bool result = true;
	result = mToolsContext != nullptr;
	if (result)
	{
		result &= mToolsContext->GizmoManager != nullptr;
		result &= mToolsContext->InputRouter != nullptr;
		result &= mToolsContext->ToolManager != nullptr;
		result &= mToolsContext->TargetManager != nullptr;
	}
	return result;
}


void ATHsEnhancedActionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

bool UTHsEnhancedActionSystem::LineTraceHitGizmo(const FTHsActionCursorData& cursorData) const
{
	return mInteractiveGizmoManager->LineTraceHitGizmo(cursorData);
}

void UTHsEnhancedActionSystem::UpdateInteractiveGizmo(const FTHsActionCursorData& cursorData)
{
	mInteractiveGizmoManager->UpdateInteractiveGizmo(cursorData);
}

TScriptInterface<ITIHHsInteraction> UTHsEnhancedActionSystem::LineTraceSelectableInteraction(FHitResult& outHitResult) const
{
	TScriptInterface<ITIHHsInteraction>  result = nullptr;
	
	if (mIsHitFlag && mIsActorHitFlag && mActionCursor != nullptr)
	{
		const FVector start = mActionCursor->GetCurrentStartWorldLocation();
		const FVector end = mActionCursor->GetCurrentEndWorldLocation();
		const ECollisionChannel traceChannel = mActionCursor->GetTraceChannel();
		
		if (mWorldContext->LineTraceSingleByChannel(
			outHitResult,
			start,end,traceChannel))
		{
			ITIHHsInteraction* castedActor = Cast<ITIHHsInteraction>(outHitResult.GetActor());
			ITIHHsInteraction* castedComponent = Cast<ITIHHsInteraction>(outHitResult.GetComponent());
			
			if (castedActor != nullptr && castedComponent != nullptr)
			{
				int32 actorAgent = ToBits(castedActor->GetInteractionAgentType());
				int32 componentAgent = ToBits(castedComponent->GetInteractionAgentType());
				if ((actorAgent | componentAgent) & ToBits(ETHsInteractionAgentType::ECondition))
				{
					if (actorAgent & ToBits(ETHsInteractionAgentType::EActor))
					{
						result.SetObject(outHitResult.GetActor());
						result.SetInterface(castedActor);
					}
					else if (componentAgent & ToBits(ETHsInteractionAgentType::EComponent))
					{
						result.SetObject(outHitResult.GetComponent());
						result.SetInterface(castedComponent);
					}
				}
			}
			else if (castedActor != nullptr)
			{
				int32 actorAgent = ToBits(castedActor->GetInteractionAgentType());
				if (actorAgent & ToBits(ETHsInteractionAgentType::EActor))
				{
					result.SetObject(outHitResult.GetActor());
					result.SetInterface(castedActor);
				}
			}
			else if (castedComponent != nullptr)
			{
				int32 componentAgent = ToBits(castedComponent->GetInteractionAgentType());
				if (componentAgent & ToBits(ETHsInteractionAgentType::EComponent))
				{
					result.SetObject(outHitResult.GetComponent());
					result.SetInterface(castedComponent);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("LineTraceSelectableInteraction: No ITIHHsInteraction found on hit actor or component."));
			}//	interaction condition check
		}//LineTraceSingleByChannel
	}
	
	return result;
}

TScriptInterface<ITIHHsInteraction> UTHsEnhancedActionSystem::LineTraceSelectableGizmo(FHitResult& outHitResult) const
{
	TScriptInterface<ITIHHsInteraction>  result = nullptr;
	if (mIsHitFlag && mIsGizmoHitFlag )
	{
		const FTHsActionCursorData& capture = GetCursorCapture();
		mInteractiveGizmoManager->UpdateInteractiveGizmo(capture);
		if (mInteractiveGizmoManager->IsAnyHitGizmo())
		{
			//	'기즈모 api 를 가진 액터'를 가져온다. 이건 인터페이스임.	액터로 처리하느냐 컴포넌트로 처리하느냐를 선택하게 해야함. 컨디션은 나중에.
			TScriptInterface<ITHsInteractiveGizmoAPI> bestGizmo = mInteractiveGizmoManager->GetBestGizmoBridge()->GetGizmoObject();

			if (bestGizmo != nullptr)
			{
				int32 agentType = ToBits(bestGizmo->GetInteractionAgentTypeGizmo());
				UObject* hitObject = bestGizmo.GetObject();
				
				if (agentType & ToBits(ETHsInteractionAgentType::ECondition))
				{
					outHitResult = mInteractiveGizmoManager->GetBestHitResult();
					ITIHHsInteraction* castedActor = Cast<ITIHHsInteraction>(outHitResult.GetActor());
					ITIHHsInteraction* castedComponent = Cast<ITIHHsInteraction>(outHitResult.GetComponent());
					
					if (castedActor && castedComponent)
					{
						if (agentType & ToBits(ETHsInteractionAgentType::EComponent))
						{
							hitObject = outHitResult.GetComponent();
						}
						result.SetObject(hitObject);
						result.SetInterface(Cast<ITIHHsInteraction>(hitObject));
					}
					else
					{
						outHitResult = {};
						UE_LOG(LogTemp, Warning, TEXT("LineTraceSelectableGizmo: No ITIHHsInteraction no interaction found on hit actor or component."));
					}
				}//if (agentType & ToBits(ETHsInteractionAgentType::ECondition))
				else if (agentType & ToBits(ETHsInteractionAgentType::EActor))
				{
					outHitResult = mInteractiveGizmoManager->GetBestHitResult();
					result.SetObject(hitObject);
					result.SetInterface(Cast<ITIHHsInteraction>(hitObject));
				}
				else if (agentType & ToBits(ETHsInteractionAgentType::EComponent))
				{
					outHitResult = mInteractiveGizmoManager->GetBestHitResult();
					result.SetObject(outHitResult.GetComponent());
					result.SetInterface(Cast<ITIHHsInteraction>(result.GetObject()));
				}
			}//if (bestGizmo != nullptr)
		}//mInteractiveGizmoManager->IsAnyHitGizmo()
	}
	return result;
}

TScriptInterface<ITIHHsInteraction> UTHsEnhancedActionSystem::LineTraceSelectableAny(FHitResult& outHitResult) const
{
	TScriptInterface<ITIHHsInteraction> result = nullptr;
	result = LineTraceSelectableGizmo(outHitResult);
	if (result == nullptr)
	{
		result = LineTraceSelectableInteraction(outHitResult);
	}
	
	if (result)
	{
		if (result->GetInteractionTags().HasTag(THS_Action_Trait_Selectable))
		{
			return result;
		}
	}
	return nullptr;
}

void ATHsEnhancedActionActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

bool UTHsEnhancedActionSystem::IsTickableInEditor() const
{
	UTHsSettingSubsystem* configSubsystem = TIHUtils::GetSettingSubsystem(GetWorld());
	return (configSubsystem != nullptr) ? configSubsystem->bIsEnhancedActionTickableInEditor : false;
}

bool UTHsEnhancedActionSystem::IsTickableWhenPaused() const
{
	UTHsSettingSubsystem* configSubsystem = TIHUtils::GetSettingSubsystem(GetWorld());
	return (configSubsystem != nullptr) ? configSubsystem->bIsTickableWhenPaused :false;
}

void UTHsEnhancedActionSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	//mConsignmentUObjectClasses.Add(UTHsActionSystemSelector::StaticClass());
	
	mWorldContext = &InWorld;
	if (UTHsSettingSubsystem* configSystem = TIHUtils::GetSettingSubsystem(mWorldContext))
	{
		configSystem->RegisterCallerOrBoolean(TEXT("FirstCompleteActionSystemInit"));
		configSystem->RegisterCallerOrBoolean(TEXT("InputKeySetupComplete"));
		configSystem->RegisterCallerOrBoolean(TEXT("CompleteCursorInit"));
		configSystem->RegisterCallerOrBoolean(TEXT("CompleteGizmoInit"));
	}
	mActionCursor = InWorld.SpawnActor<ATHsActionCursor>(ATHsActionCursor::StaticClass());
	
	const TArray<FEnhancedActionKeyMapping>& existingMappings = CrawlInputMappingContext();

	mCurrentActionFuncFilterFlags = ToBits(ETHsEnhancedActionFuncFilterFlag::EHover | ETHsEnhancedActionFuncFilterFlag::ECursorMove | ETHsEnhancedActionFuncFilterFlag::EClick);
	
	if (mDataSetting)
	{
		mGizmoDefaultMaterials = mDataSetting->Data.DefaultMaterialMap;
	}

	//	#### Gizmo Manager
	mInteractiveGizmoManager = InWorld.SpawnActor<ATHsInteractiveGizmoManager>(ATHsInteractiveGizmoManager::StaticClass());
	
	
	if (ProcessAllInputKeys(existingMappings))
	{
		mWorldContext->GetTimerManager().SetTimerForNextTick([this]()
		{
			BindAllActionsToPlayerController();
			bInitEnhancedActionSystem = true;

			if (mActionCursor)
			{
				mActionCursor->InitActionCursor(this, mWorldContext->GetFirstPlayerController(), mDataSetting);
				bInitCursor = true;
			}
			
			//if (mGizmoManagerWrapper == nullptr)
			//{
			//	mGizmoManagerWrapper = NewObject<UTHsGizmoManagerWrapper>(this);
			//	mGizmoManagerWrapper->InitGizmoManager(this, mWorldContext);
			//	bInitGizmoManager = true;
			//}
			
			if (UTHsSettingSubsystem* configSystem = TIHUtils::GetSettingSubsystem())
			{
				configSystem->OnSettingBooleanTrue(TEXT("InputKeySetupComplete"));
				configSystem->OnSettingBooleanTrue(TEXT("CompleteCursorInit"));
				configSystem->OnSettingBooleanTrue(TEXT("CompleteGizmoInit"));
				//	이거 지금 작동 안함.
			}

			mActionSystemSelector = NewObject<UTHsActionSystemSelector>(this);
			mActionSystemSelector->InitSelector(this);
			
			for (UClass* cls :mConsignmentUObjectClasses)
			{
				UObject* obj = NewObject<UObject>(this,cls);
				mConsignmentUObjects.Add(obj);
			}
			
			OnPostBindAllAction.Broadcast(this);
		});
	}
	
}

TArray<FEnhancedActionKeyMapping> UTHsEnhancedActionSystem::CrawlInputMappingContext()
{
	TArray<FEnhancedActionKeyMapping> allExistingMappings;
	//UObject* tryLoadDataSetting = StaticLoadObject(UTHsConfigSubsystem::StaticClass(), nullptr, TEXT("/TIHHousingSystem/Config/TIHHsConfigSubsystem.TIHHsConfigSubsystem"));
	UObject* tryLoadDataSetting = mDataSettingPath.IsValid() ? mDataSettingPath.TryLoad() : nullptr;
	if (tryLoadDataSetting)
	{
		if ((mDataSetting = Cast<UTHsEnhancedActionSystemDataSetting>(tryLoadDataSetting)))
		{
			if (mDataSetting->IsExploreInputMappingContext())
			{
				TSet<FString> excludeMappingPaths;
				TArray<FAssetData> allMappingContextAssets;
				//	excludeMappingPaths
				for (const FSoftObjectPath& path : mDataSetting->GetInputMappingContextPaths())
				{
					if (path.IsValid())
					{
						excludeMappingPaths.Add(path.ToString());
					}
				}

				//	explore all InputMappingContext assets
				FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				
				IAssetRegistry& assetRegistry = assetRegistryModule.Get();
				assetRegistry.GetAssetsByClass(UInputMappingContext::StaticClass()->GetClassPathName(), allMappingContextAssets, true);

				for (const FAssetData& assetData : allMappingContextAssets)
				{
					if (excludeMappingPaths.Contains(assetData.GetObjectPathString()))
					{
						continue;
					}
					if (UInputMappingContext* castedMappingContext = Cast<UInputMappingContext>(assetData.GetAsset()))
					{
						const TArray<FEnhancedActionKeyMapping>& mappings = castedMappingContext->GetMappings();
						for (const FEnhancedActionKeyMapping& mapping : mappings)
						{
							allExistingMappings.Add(mapping);
							UE_LOG(LogTemp, Warning, TEXT("Found existing mapping: %s -> %s"), 
								*mapping.Key.ToString(),
								mapping.Action ? *mapping.Action->GetName() : TEXT("NULL"));
						}
					}
				}
			}
			else
			{
				for (FSoftObjectPath imcPath :mDataSetting->GetInputMappingContextPaths())
				{
					if (UObject* imc = imcPath.TryLoad())
					{
						if (UInputMappingContext* castedMappingContext = Cast<UInputMappingContext>(imc))
						{
							const TArray<FEnhancedActionKeyMapping>& mappings = castedMappingContext->GetMappings();
							for (const FEnhancedActionKeyMapping& mapping : mappings)
							{
								allExistingMappings.Add(mapping);
								UE_LOG(LogTemp, Warning, TEXT("Found existing mapping: %s -> %s"), 
									*mapping.Key.ToString(),
									mapping.Action ? *mapping.Action->GetName() : TEXT("NULL"));
							}
						}
					}
				}
			}
		}
	}
	return allExistingMappings;
}

bool UTHsEnhancedActionSystem::ProcessAllInputKeys(const TArray<FEnhancedActionKeyMapping>& existingMappings)
{
	UTHsSettingSubsystem* configSubsystem = TIHUtils::GetSettingSubsystem(mWorldContext);
	
	if (configSubsystem == nullptr
		|| mDataSetting == nullptr)
	{
		if (configSubsystem == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Universal Input System: Failed to get TIHSettingSubsystem"));
		}
		if (mDataSetting == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Universal Input System: Failed to get DataSetting"));
		}
		return false;
	}
	
	mUniversalInputMappingContext = NewObject<UInputMappingContext>(this);

	const bool useAllInputKeys = mDataSetting->IsUseAllInputKeys();
	const TArray<FKey> allKeys = (useAllInputKeys) ? configSubsystem->GetAllInputKeys() : mDataSetting->GetKeyKeys();
	const TSet<FKey> excludeKeys = (useAllInputKeys) ? TSet<FKey>(mDataSetting->GetKeyKeys()) : TSet<FKey>();

	if (not existingMappings.IsEmpty())
	{
		for (const FEnhancedActionKeyMapping& eakm :existingMappings)
		{
			if (excludeKeys.Contains(eakm.Key) || mAllKeyActions.Contains(eakm.Key))
			{
				continue;
			}
			mAllKeyActions.Add(eakm.Key, eakm.Action);
			mUniversalInputMappingContext->MapKey(eakm.Action, eakm.Key);
		}
	}

	for (const FKey& key :allKeys)
	{
		if (excludeKeys.Contains(key) || mAllKeyActions.Contains(key))
		{
			continue;
		}
		UInputAction* newAction = NewObject<UInputAction>(this,*FString::Printf(TEXT("IA_%s"), *key.ToString()));
		newAction->ValueType = EInputActionValueType::Axis1D;
		mAllKeyActions.Add(key, newAction);
		mUniversalInputMappingContext->MapKey(newAction, key);
	}

	
	UE_LOG(LogTemp, Warning, TEXT("Universal Input System: Processed %d keys"), mAllKeyActions.Num());
	
	return not mAllKeyActions.IsEmpty();
}

void UTHsEnhancedActionSystem::BindAllActionsToPlayerController()
{
	if (mWorldContext)
	{
		if (APlayerController* pc = mWorldContext->GetFirstPlayerController())
		{
			mPlayerController = pc;
			
			 mPlayerController->bShowMouseCursor = mDataSetting->Data.bShowCursor;
			// Enhanced Input Subsystem에 Context 추가
			if (UEnhancedInputLocalPlayerSubsystem* eiLocalSubsystem = 
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
			{
				eiLocalSubsystem->AddMappingContext(mUniversalInputMappingContext, 0);
			}
			
			// 모든 Action을 통합 함수에 바인딩
			if (UEnhancedInputComponent* eic = Cast<UEnhancedInputComponent>(pc->InputComponent))
			{
				
				mAllActionPoints.Reserve(mAllKeyActions.Num());
				mEnableKeys.Reserve(mAllKeyActions.Num());
				
				mActivatedActionPoints.Reserve(mAllKeyActions.Num());
				mAllKeyNames.Reserve(mAllKeyActions.Num());
				for (auto& KeyActionPair : mAllKeyActions)
				{
					mAllKeyNames.Add(KeyActionPair.Key.GetFName(), KeyActionPair.Key);
					UE_LOG(LogTemp, Log, TEXT("%s" ), *KeyActionPair.Key.ToString());
					//	UTHsActionPoint 생성
					UTHsActionPoint* actionPoint = nullptr;
					
					if (TSubclassOf<UTHsActionPoint>* clsPath = mDataSetting->GetOverrideActionPointPath(
						KeyActionPair.Key))
					{
						if (UClass* cls = clsPath->Get())
						{
							if (cls->IsChildOf(UTHsActionPoint::StaticClass()))
							{
								actionPoint = NewObject<UTHsActionPoint>(this,cls);
								if (cls == UTHsActionMouseClick::StaticClass() || cls->IsChildOf(UTHsActionMouseClick::StaticClass()))
								{
									mAllMouseClickActions.Add(Cast<UTHsActionMouseClick>( actionPoint));
									bInitClickActions = true;
								}
							}
						}
					}
					if (actionPoint == nullptr)
					{
						actionPoint = NewObject<UTHsActionPoint>(this);
					}

					const UInputAction* action = KeyActionPair.Value;
					
					actionPoint->InitActionPoint(this,action,KeyActionPair.Key);

					mAllActionPoints.Add(KeyActionPair.Key, actionPoint);
					mEnableKeys.Add(KeyActionPair.Key);
					if (actionPoint->IsBindKeyActionStarted())
					{
						eic->BindAction(action,ETriggerEvent::Started,actionPoint,&UTHsActionPoint::HwInputKeyActionDown);
					}
					if (actionPoint->IsBindKeyActionCompleted())
					{
						eic->BindAction(action,ETriggerEvent::Completed,actionPoint,&UTHsActionPoint::HwInputKeyActionUp);
					}
					if (actionPoint-> IsBindKeyActionTriggered() )
					{
						eic->BindAction(action,ETriggerEvent::Triggered,actionPoint,&UTHsActionPoint::HwInputKeyActionTriggered);
					}
					if (actionPoint->IsBindKeyActionCancelled() )
					{
						eic->BindAction(action,ETriggerEvent::Canceled,actionPoint,&UTHsActionPoint::HwInputKeyActionCancel);
					}
					if (actionPoint->IsBindKeyActionOngoing())
					{
						eic->BindAction(action,ETriggerEvent::Ongoing,actionPoint,&UTHsActionPoint::HwInputKeyActionOngoing);
					}
					
				}
				
				UE_LOG(LogTemp, Warning, TEXT("Universal Input System: Bound %d actions"), mAllKeyActions.Num());
				
			}
		}
	}
}

void UTHsEnhancedActionSystem::ActivateActionPoint(UTHsActionPoint* actionPoint)
{
	if (actionPoint)
	{
		int32 systemIdx = actionPoint->GetIndexInSystem();
		if (not IsActionPointActivatedByIndex(systemIdx))
		{
			mActivatedActionPoints[systemIdx] = actionPoint;
		}
	}
}
void UTHsEnhancedActionSystem::DeActivateActionPoint(UTHsActionPoint* actionPoint)
{
	if (actionPoint)
	{
		int32 systemIdx = actionPoint->GetIndexInSystem();
		if (IsActionPointActivatedByIndex(systemIdx))
		{
			mActivatedActionPoints[systemIdx] = nullptr;
		}
	}
}

void UTHsEnhancedActionSystem::ActionDoubleClicked(UTHsActionPoint* actionPoint)
{
	if (CheckEnableAction(actionPoint))
	{
		actionPoint->OverrideActionPointDoubleClick();
		actionPoint->ActivateActionPointEnd();
		//	여기에서 기즈모도 처리해줘야하는데, 어떻게 하지?
	}
}
 void UTHsEnhancedActionSystem::ActionClicked(UTHsActionPoint* actionPoint)
{
	if (CheckEnableAction(actionPoint))
	{
		actionPoint->OverrideActionPointClick();
		actionPoint->GetActionBindContext().DelegateEventClick.Broadcast(actionPoint);
	}
}

 void UTHsEnhancedActionSystem::ActionDown(UTHsActionPoint* actionPoint)
{
	//	마우스 클릭에 관해서만 이걸 호출해줘야함.
	//ChangeFilterFuncFlag(ETHsEnhancedActionFuncFilterFlag::EHover,false);
	if (CheckEnableAction(actionPoint))
	{
		mCurrentActionPoint.Add(actionPoint->GetActionPointKey(),actionPoint);
		actionPoint->OverrideActionPointDown();
		actionPoint->GetActionBindContext().DelegateEventDown.Broadcast(actionPoint);
		//OnProcessTickChanged(false);
		UE_LOG(LogTemp, Warning, TEXT(" Down!!!!! %s ActionDown"), *actionPoint->GetActionPointKeyString());
	}
}
void UTHsEnhancedActionSystem::ActionPressing(UTHsActionPoint* actionPoint, float deltaTime)
{
	if (CheckEnableAction(actionPoint))
	{
		actionPoint->OverrideActionPressing(actionPoint,deltaTime);
		actionPoint->GetActionBindContext().DelegateProcessPressing.Broadcast(actionPoint,deltaTime);
	}
}
 void UTHsEnhancedActionSystem::ActionUp(UTHsActionPoint* actionPoint)
{
	//	마우스 클릭에 관해서만 이걸 호출해줘야함.
	//ChangeFilterFuncFlag(ETHsEnhancedActionFuncFilterFlag::EHover,true);
	if (CheckEnableAction(actionPoint))
	{
		if (mCurrentActionPoint.Contains(actionPoint->GetActionPointKey()))
		{
			mCurrentActionPoint.Remove(actionPoint->GetActionPointKey());
		}
		actionPoint->OverrideActionPointUp();
		actionPoint->GetActionBindContext().DelegateEventUp.Broadcast(actionPoint);
		//OnProcessTickChanged(true);
	}
}

 void UTHsEnhancedActionSystem::ActionOngoingStart(UTHsActionPoint* actionPoint,float deltaTime)
{
	if (CheckEnableAction(actionPoint))
	{
		actionPoint->OverrideActionPointOngoingStart(deltaTime);
		actionPoint->GetActionBindContext().DelegateProcessHoldStart.Broadcast(actionPoint,deltaTime);
		ActionOngoing(actionPoint,deltaTime);
	}
}

 void UTHsEnhancedActionSystem::ActionOngoing(UTHsActionPoint* actionPoint,float deltaTime)
{
	if (CheckEnableAction(actionPoint))
	{
		actionPoint->OverrideActionPointOngoing(deltaTime);
		actionPoint->GetActionBindContext().DelegateProcessHoldOngoing.Broadcast(actionPoint,deltaTime);
	}
}

 void UTHsEnhancedActionSystem::ActionOngoingEnd(UTHsActionPoint* actionPoint,float deltaTime)
{
	if (CheckEnableAction(actionPoint))
	{
		ActionOngoing(actionPoint,deltaTime);	//	나중에 이거 포함할지 안할지 정하든가.
		actionPoint->OverrideActionPointOngoingEnd(deltaTime);
		actionPoint->GetActionBindContext().DelegateProcessHoldEnd.Broadcast(actionPoint,deltaTime);
	}
}

void UTHsEnhancedActionSystem::ActionDragingStart(UTHsActionPoint* actionPoint,float deltaTime)
{
	if (CheckEnableAction(actionPoint))
	{
		actionPoint->OverrideActionPointClickDragStart(deltaTime);
		actionPoint->GetActionBindContext().DelegateProcessDragStart.Broadcast(actionPoint,deltaTime);
	}
}

void UTHsEnhancedActionSystem::ActionDraging(UTHsActionPoint* actionPoint, float deltaTime)
{
	if (CheckEnableAction(actionPoint))
	{
		actionPoint->OverrideActionPointClickDragOngoing(deltaTime);
		actionPoint->GetActionBindContext().DelegateProcessDragOngoing.Broadcast(actionPoint,deltaTime);
	}
	
}

void UTHsEnhancedActionSystem::ActionDragingEnd(UTHsActionPoint* actionPoint,float deltaTime)
{
	if (CheckEnableAction(actionPoint))
	{
		actionPoint->OverrideActionPointClickDragEnd(deltaTime);
		actionPoint->GetActionBindContext().DelegateProcessDragEnd.Broadcast(actionPoint,deltaTime);
	}
}

void UTHsEnhancedActionSystem::ActionMouseMove(ATHsActionCursor* cursor, float deltaTime)
{
	if (mIsCursorMoveFlag)
	{
		DelegateActionCursorMove.Broadcast(cursor,deltaTime);
	}
	//	TODO: 일단은 시스템에서 DelegateActionCursorMove 를 호출해줄 거임.
	
}

void UTHsEnhancedActionSystem::ActionHoverStart(ATHsActionCursor* cursor, float deltaTime)
{
	if (AActor* hitActor= cursor->GetCurrentHitResult().GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("-0--------------ActionHoverStart deltaTime: %f {{ %s }}"), deltaTime, *hitActor->GetName());
	}
}

void UTHsEnhancedActionSystem::ActionHoverOngoing(ATHsActionCursor* cursor, float deltaTime)
{
	if (UPrimitiveComponent* primitive = cursor->GetCurrentHitResult().GetComponent())
	{
		//UE_LOG(LogTemp, Log, TEXT("ActionHoverOngoing deltaTime: %f {{ %s }}"), deltaTime, *primitive->GetName());
	}
}

void UTHsEnhancedActionSystem::ActionHoverEnd(ATHsActionCursor* cursor, float deltaTime)
{
	if (cursor)
	{
		UE_LOG(LogTemp, Warning, TEXT(" UTHsEnhancedActionSystem::ActionHoverEnd Hover End"));
		
	}
}

void ATHsActionCursor::OverrideActionPointMouseMove(ATHsActionCursor* cursor, float deltaTime)
{
	//UE_LOG(LogTemp, Log, TEXT("ScreenDelta: %s"), *mDeltaScreenPosition.ToString());
}


bool UTHsEnhancedActionSystem::GlobalHit(FHitResult& hitResult) const
{
	//mActionCursor->CursorLineTrace(mGlobalHitResult);
	//	이제 기즈모도 linetrace 가 되니깐 이건 어떻게 해야할까나...
	//	
	//mWorldContext->LineTraceSingleByChannel()

	/*
	//	지금 충돌 자체 기능이 열려있는가
	//	충돌 채널은?
	//	밑에 보면 글로벌 힛은 이게 전부고, 기즈모는
	//	충돌 자체 기능이 열려있는가
	//	기즈모 충돌이 열려있는가
	//	기즈모 호버가 열려있는가
	//	기즈모 충돌 채널은?
		액터 충돌이 열려있는가
		액터 호버가 열려있는가
		액터 충돌 채널은?
	 * 
	 */
	bool isNotHoverable = true;
	
	if (mWorldContext->LineTraceSingleByChannel(
			hitResult,
			mActionCursor->GetCurrentStartWorldLocation(),
			mActionCursor->GetCurrentEndWorldLocation(),
			mActionCursor->GetTraceChannel()
	))
	{
		//	hit
		//UE_LOG(LogTemp, Log, TEXT("Global Hit Actor: %s"), *mGlobalHitResult.GetActor()->GetName());
		//ActionCursorHitCategoryStuff();
		
		if (AActor* hitActor = hitResult.GetActor())
		{
			TScriptInterface<ITIHHsInteraction> interactionTarget = hitActor;
			if (interactionTarget != nullptr)
			{
				if (interactionTarget->GetInteractionTags().HasTag(THS_Action_Trait_Hoverable))
				{
					isNotHoverable = false;
				}
			}
		}
		if (UPrimitiveComponent* hitComponent = hitResult.GetComponent())
		{
			TScriptInterface<ITIHHsInteraction> interactionTarget = hitComponent;
			if (interactionTarget != nullptr)
			{
				if (interactionTarget->GetInteractionTags().HasTag(THS_Action_Trait_Hoverable))
				{
					isNotHoverable = false;
				}
			}
		}
		DrawDebugLine(
			mWorldContext,
			mActionCursor->GetCurrentStartWorldLocation(),
			mActionCursor->GetCurrentEndWorldLocation(),
			FColor::Green,
			false,
			1/60.0f,
			0,
			0.1f
		);
	}

	
	return isNotHoverable;
}

void UTHsEnhancedActionSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (mInputTickElapsedTime < /*mInputTickThreshold*/ 0.0016f)
	{
		mInputTickElapsedTime += DeltaTime;
		return;
	}

	if (OnTickEnable() && mIsHitFlag)
	{
		if (mIsActionKeyFlag)
		{
			UpdateActionPoints();
		}
		
		if (mIsHoverFlag)
		{
			ClearGlobalHit();
			FTHsActionCursorData& cursorRecord = mActionCursor->GetCurrentCursorData();
			FTHsActionCursorData& prevCursorRecord = mActionCursor->GetPreviousCursorData();
			
			mActionCursor->UpdateCursorRecordBegin(cursorRecord, prevCursorRecord, mInputTickElapsedTime);
			OnCurrentHitTypeChanged(ETHsActionCursorHoverHitCategory::ENone);
			//	클릭시에 이걸 꺼야함.
			if (IsProcessTick())
			{
				if (IsCursorOnWorld())
				{
					bool gizmoHit = false;
					FHitResult emptyResult = {};
					GlobalHit(emptyResult);
					/*
						1. 글로벌 히트를 한다. 왜냐하면 occlude 처리를 위해서.
						2. 먼저 관계없는것을 충돌 시키고나서 기즈모를 충돌시킨다.
						3. 기즈모가 히트되었으면 기즈모로 처리
						4. 기즈모가 히트되지 않았으면 액터로 처리
						5. 어차피 위젯은 최상단에서 처리한다. 하지만 이러면 사실 문제가 조금 발생하는데, 위젯으로 넘어갈때 end 가 제대로 호출 안될 가능성이 있음
						6. 그래서 새로운 방식을 사용할 예정임.

						문제점:
						- 클릭은 기즈모나 액터나 둘다 같이 처리하는데, 지금은 액터만 처리함.
						- 레코드는 무조건 이루어져야함. 현재 새롭게 레코드 하는거임.
						1. linetrace 를 먼저해서 actor 히트를 해본다.
					 	
					 	 
					 */
				
					if (mIsGizmoHitFlag && mInteractiveGizmoManager->LineTraceHitGizmo(GetCurrentCursorRecord()))
					{
						gizmoHit = true;
					}
					
					mActionCursor->ProcessHitActor(cursorRecord,emptyResult);
				
					if (gizmoHit)
					{
						cursorRecord.OffActionStateHit();
						OnCurrentHitTypeChanged(ETHsActionCursorHoverHitCategory::EGizmo);
						mActionCursor->ActionCursorHitCategoryGizmo();
					}
					else
					{
						OnCurrentHitTypeChanged(ETHsActionCursorHoverHitCategory::EStuff);
					}
				}
				else
				{
					OnCurrentHitTypeChanged(ETHsActionCursorHoverHitCategory::EWidget);
				}
			}

			mActionCursor->UpdateCursorRecordEnd(cursorRecord, prevCursorRecord, mInputTickElapsedTime);
		}//	hover end
		
		if (mIsActionKeyFlag && mIsSpecialActionFlag)
		{
			for (UTHsActionMouseClick* mouseClickObj :mAllMouseClickActions)
			{
				//	down 과 move만 있으면 된다.
				if (mEnableKeys.Contains(mouseClickObj->GetActionPointKey()))
				{
					mouseClickObj->EvaluateDragging(mActionCursor,mInputTickElapsedTime);
					DelegateCursorDragging.Broadcast(mActionCursor,mInputTickElapsedTime);
				}
			}
		}//	special action end
		mInputTickElapsedTime = 0.0f;
		//	여기에 있어야하는 이유는 틱이 이루어질때든 아니든 레코드는 진행되어야함.
		mActionCursor->AdvanceRecordIndex();
	}//	on tick enable
}//	tick end
void UTHsEnhancedActionSystem::Deinitialize()
{
	Super::Deinitialize();
}

void UTHsEnhancedActionSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}



void UTHsActionMouseClick::PostInitActionPoint(UTHsEnhancedActionSystem* actionSystem,const UInputAction* inputAction, const FKey& key, int32 index)
{
	Super::PostInitActionPoint(actionSystem, inputAction, key, index);
	
	mPlayerController = actionSystem->GetPlayerController();
	mActionBindContext.bBindKeyActionStarted = true;
	mActionBindContext.bBindKeyActionCompleted = true;
	
	mHoldingStartThreshold = 0.3f;
	mDoubleClickThreshold = 0.15f;
	
	mClickFilter.Reset(NewObject<UTHsFilter>(this));
	mClickFilter->InitFilter({ THS_Action_Trait_Selectable.GetTag() });

	mActionParamIn.CallerObject = this;

	mActionCursor = mActionSystem->GetActionCursor();
}

void UTHsActionMouseClick::OverrideActionPointClick()
{
	Super::OverrideActionPointClick();

	FInputDeviceState deviceState;
	deviceState.InputDevice = EInputDevices::Mouse;
	
}

void UTHsActionMouseClick::OverrideActionPointDoubleClick()
{
	Super::OverrideActionPointDoubleClick();
	UE_LOG(LogTemp, Warning, TEXT("11111 UTHsActionMouseClick: %s ActionDoubleClick"), *GetActionPointKeyString());
}

void UTHsActionMouseClick::OverrideActionPointOngoingStart(const float deltaTime)
{
	Super::OverrideActionPointOngoingStart(deltaTime);
	
	UE_LOG(LogTemp, Warning, TEXT("111111 UTHsActionMouseClick: %s ActionOngoingStart"), *GetActionPointKeyString());
	
}

void UTHsActionMouseClick::OverrideActionPointOngoing(const float deltaTime)
{
	Super::OverrideActionPointOngoing(deltaTime);
	UE_LOG(LogTemp, Warning, TEXT("1111 UTHsActionMouseClick: %s ActionOngoing"), *GetActionPointKeyString());
}

void UTHsActionMouseClick::OverrideActionPointOngoingEnd(const float deltaTime)
{
	Super::OverrideActionPointOngoingEnd(deltaTime);
	UE_LOG(LogTemp, Warning, TEXT("11111 UTHsActionMouseClick: %s ActionOngoingEnd"), *GetActionPointKeyString());
}

void UTHsActionMouseClick::EvaluateDragging(ATHsActionCursor* cursorObj, float deltaTime)
{
	const bool validDrag = IsOngoing() && cursorObj->IsMoving();
	
	switch (mDragState)
	{
	case ETHsActionMouseClickDragState::EDragNone:
		if (validDrag)
		{
			HandleDragStart(deltaTime);
		}
		break;
	case ETHsActionMouseClickDragState::EDragStarted:
		if (validDrag)
		{
			HandleDragOngoing(deltaTime);
		}
		else
		{
			HandleDragEnd(deltaTime);
		}
		break;
	case ETHsActionMouseClickDragState::EDragOngoing:
		if (validDrag)
		{
			HandleDragOngoing(deltaTime);
		}
		else
		{
			HandleDragEnd(deltaTime);
		}
		break;
	case ETHsActionMouseClickDragState::EDragEnded:
		if (validDrag)
		{
			HandleDragStart(deltaTime);
		}
		else
		{
			mDragState = ETHsActionMouseClickDragState::EDragNone;
		}
		break;
	}
}
void UTHsActionMouseClick::PostEvaluateActionPoint(float CurrentTime, float frameDeltaTime)
{
	Super::PostEvaluateActionPoint(CurrentTime, frameDeltaTime);
}

void UTHsActionMouseClick::OverrideActionPressing(UTHsActionPoint* actionPoint, float deltaTime)
{
	Super::OverrideActionPressing(actionPoint, deltaTime);
	/*
	 * 250919 되는데 문제가 이거 초기화를 안해서 다운시에 계속 누적이 됨.
	 * 그래서 up시에 초기화 해주는 코드를 넣어줘야함.
	 * 즉 down 시에는 pressed 로 해주고
	 * 그다음 down
	 * up 시에 release 후에 제거
	 * 그럼 이게 다행인게, 내꺼가 무조건 한번은 호출을 하도록 되어있음. 다음틱에 작동하니깐
	 * 그럼 click 을 변형해서 만들어야함. release는 up 시점에 놓고 그때 끝나고 초기화.
	 */
	// if (mActionSystem->OnEventMousePressingForGizmo(this))
	// {
	// 	
	// }
}

void UTHsActionMouseClick::OverrideActionPointDown()
{
	Super::OverrideActionPointDown();
	mActionSystem->OnEventMouseDownForGizmo(this);
}

void UTHsActionMouseClick::OverrideActionPointUp()
{
	Super::OverrideActionPointUp();
	mActionSystem->OnEventMouseUpForGizmo(this);
}

void UTHsActionModifierClick::OverrideActionPointDown()
{
	Super::OverrideActionPointDown();
	mActionSystem->SetModifierKey(mKey,true);
}

void UTHsActionModifierClick::OverrideActionPointUp()
{
	Super::OverrideActionPointUp();
	mActionSystem->SetModifierKey(mKey,false);
}

void FTHsGizmoViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	// [여기서 할 일]
	// - 일반적으로는 가족 단위 설정(ShowFlags 등)을 만지지만, 본 용도(UGizmoViewContext 갱신)에는 필수 작업 없음.
	// - 멀티 뷰포트 환경에서 특정 Viewport만 대상으로 삼고 싶다면 InViewFamily.Viewport 등을 확인해 플래그 세팅 가능.
	//   (예: 현재 호버 중인 뷰포트만 활성화하는 조건을 기록)
}

void FTHsGizmoViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
	// [역할/타이밍]
	// - 각 FSceneView가 셋업될 때 "뷰마다" 호출 (게임 스레드, 렌더 스레드 아님)
	// - 여기서 FSceneView가 완성된 직후이므로 UGizmoViewContext 갱신에 최적
	if (UGizmoViewContext* viewCtx = mViewContext.Get())
	{
		if (IsPrimaryView(InView))
		{
			viewCtx->ResetFromSceneView(InView);
		}
	}
}
void FTHsGizmoViewExtension::InitGizmoViewExtension(UTHsGizmoManagerWrapper* inGizmoWrapper)
{
	mGizmoWrapper = inGizmoWrapper;
	if (UContextObjectStore* store = mGizmoWrapper->GetToolsContext()->ContextObjectStore)
	{
		if (UGizmoViewContext* viewCtx = store->FindContext<UGizmoViewContext>())
		{
			mViewContext = viewCtx ;
		}
	}
}
void FTHsGizmoViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	if (UGizmoViewContext* viewCtx = mViewContext.Get())
	{
		if (InViewFamily.Views.Num() > 0 && InViewFamily.Views[0] != nullptr)
		{
			const FSceneView* firstView = InViewFamily.Views[0];
			viewCtx->ResetFromSceneView(*firstView);
		}
	}
}


void UTHsActionSystemSelector::InitSelector(UTHsEnhancedActionSystem* actionSystem)
{
	mSelectedInteraction = nullptr;
	mActionSystem = actionSystem;
	mWorldContext = actionSystem->GetWorldContext();
	mLeftMouseClickAction = Cast<UTHsActionMouseClick>(mActionSystem->GetActionPointByKey(EKeys::LeftMouseButton));

	mLeftMouseClickAction->GetActionBindContext().DelegateEventClick.AddUObject(this,&UTHsActionSystemSelector::OverrideActionPointClick);
	
	mLeftMouseClickAction->GetActionBindContext().DelegateEventDown.AddLambda(
	[weakThis = TWeakObjectPtr<UTHsActionSystemSelector>(this)](UTHsActionPoint* actionPoint)
	{
		if (weakThis.IsValid())
		{
			weakThis->GetActionSystem()->OnProcessTickChanged(false);
			if (TScriptInterface<ITIHHsInteraction> selectObj = weakThis->GetSelectedInteraction())
			{
				selectObj->OverrideActionPointDown(actionPoint);
			}
		}
	});

	mLeftMouseClickAction->GetActionBindContext().DelegateProcessDragStart.AddUObject(this, &UTHsActionSystemSelector::OverrideActionPointClickDragStart);
		
	mLeftMouseClickAction->GetActionBindContext().DelegateProcessDragOngoing.AddUObject(this, &UTHsActionSystemSelector::OverrideActionPointClickDragOngoing);
		
	mLeftMouseClickAction->GetActionBindContext().DelegateProcessDragEnd.AddUObject(this, &UTHsActionSystemSelector::OverrideActionPointClickDragEnd);

	mLeftMouseClickAction->GetActionBindContext().DelegateProcessPressing.AddUObject(this,&UTHsActionSystemSelector::OverrideActionPressing);
	
	mActionSystem->DelegateActionCursorMove.AddUObject(this,&UTHsActionSystemSelector::OverrideActionPointMouseMove);
}

void UTHsActionSystemSelector::OverrideActionPointClick(UTHsActionPoint* actionPoint)
{
	if (mWorldContext == nullptr || mActionSystem == nullptr)
	{
		return;
	}
	
	auto selectObj = mActionSystem->LineTraceSelectableAny(mLastHitResult);
	if (selectObj != nullptr)
	{
		if (mSelectedInteraction!=nullptr && selectObj.GetObject() == mSelectedInteraction.GetObject())
		{
			mSelectedInteraction->OverrideCommunicateInteraction(FTHsCommunicationMessage::MakeNotifyMessage(ETHsActionFunctionType::EEvent_Click,mSelectedInteraction,this,mLeftMouseClickAction.Get()
			));
		}
		
		UE_LOG(LogTemp, Warning, TEXT(" -------------------UTHsActionSystemSelector::OverrideActionPointClick Select Obj: %s"), *mLastHitResult.GetComponent()->GetName());
		selectObj->OverrideActionPointClick(actionPoint);
		OnSelectedInteraction(selectObj);
		//mSelectedInteraction = selectObj;
	}
	else
	{
		OffSelectedInteraction(mSelectedInteraction);
	}
	
}

void UTHsActionSystemSelector::OverrideActionPointDoubleClick(UTHsActionPoint* actionPoint)
{
	if (mWorldContext == nullptr || mActionSystem == nullptr)
	{
		return;
	}
	
	auto selectObj = mActionSystem->LineTraceSelectableAny(mLastHitResult);
	if (selectObj != nullptr)
	{
		if (mSelectedInteraction.GetObject() != nullptr)
		{
			mSelectedInteraction->OverrideCommunicateInteractionString(TEXT("gizmo"),FTHsCommunicationMessage::MakeNotifyMessage(ETHsActionFunctionType::EEvent_DoubleClick,mSelectedInteraction,this,mLeftMouseClickAction.Get()
			));
		}
		selectObj->OverrideActionPointDoubleClick(actionPoint);
		mSelectedInteraction = selectObj;
	}
}

void UTHsActionSystemSelector::OverrideActionPointHoldOngoingStart(UTHsActionPoint* actionPoint, float deltaTime)
{
	//ITIHHsInteraction::OverrideActionPointHoldOngoingStart(actionPoint, deltaTime);
	
}

void UTHsActionSystemSelector::OverrideActionPointHoldOngoing(UTHsActionPoint* actionPoint, float deltaTime)
{
	//ITIHHsInteraction::OverrideActionPointHoldOngoing(actionPoint, deltaTime);
}

void UTHsActionSystemSelector::OverrideActionPointHoldOngoingEnd(UTHsActionPoint* actionPoint, float deltaTime)
{
	//ITIHHsInteraction::OverrideActionPointHoldOngoingEnd(TODO, deltaTime);
}

void UTHsActionSystemSelector::OverrideActionPointClickDragStart(UTHsActionPoint* actionPoint, float deltaTime)
{
	if (mSelectedInteraction != nullptr)
	{
		mSelectedInteraction->OverrideActionPointClickDragStart(actionPoint,deltaTime);
	}
}

void UTHsActionSystemSelector::OverrideActionPointClickDragOngoing(UTHsActionPoint* actionPoint,float deltaTime)
{
	
	if (mSelectedInteraction != nullptr)
	{
		mSelectedInteraction->OverrideActionPointClickDragOngoing(actionPoint,deltaTime);
	}
}

void UTHsActionSystemSelector::OverrideActionPointClickDragEnd(UTHsActionPoint* actionPoint,float deltaTime)
{
	if (mSelectedInteraction != nullptr)
	{
		mSelectedInteraction->OverrideActionPointClickDragEnd(actionPoint,deltaTime);
	}
}

void UTHsActionSystemSelector::OverrideActionPressing(UTHsActionPoint* actionPoint, float deltaTime)
{
	if (mSelectedInteraction != nullptr)
	{
		mSelectedInteraction->OverrideActionPressing(actionPoint,deltaTime);
	}
}

void UTHsActionSystemSelector::OverrideActionPointDown(UTHsActionPoint* actionPoint)
{
	mActionSystem->OnProcessTickChanged(false);
}

void UTHsActionSystemSelector::OverrideActionPointUp(UTHsActionPoint* actionPoint)
{
	mActionSystem->OnProcessTickChanged(true);
}

void UTHsActionSystemSelector::OverrideActionPointHoverStart(ATHsActionCursor* cursor, float deltaTime)
{
	//ITIHHsInteraction::OverrideActionPointHoverStart(cursor, deltaTime);
}

void UTHsActionSystemSelector::OverrideActionPointHoverEnd(ATHsActionCursor* cursor, float deltaTime)
{
	//ITIHHsInteraction::OverrideActionPointHoverEnd(cursor, deltaTime);
}

void UTHsActionSystemSelector::OverrideActionPointMouseMove(ATHsActionCursor* cursor, float deltaTime)
{
	if (mSelectedInteraction != nullptr)
	{
		mSelectedInteraction->OverrideActionPointMouseMove(cursor,deltaTime);
	}
}

void UTHsActionSystemSelector::OverrideActionPointSafeEnd(TScriptInterface<ITHsActionPointAPI> actionPoint)
{
	//ITIHHsInteraction::OverrideActionPointSafeEnd(actionPoint);
}

int32 UTHsActionSystemSelector::OverrideCommunicateInteraction(const FTHsCommunicationMessage& message)
{
	if (mSelectedInteraction != nullptr)
	{
		return mSelectedInteraction->OverrideCommunicateInteraction(message);
	}
	return -1;
}

int32 UTHsActionSystemSelector::OverrideCommunicateInteractionString(const FString& stringMsgm,                                                                     const FTHsCommunicationMessage& message)
{
	if (mSelectedInteraction != nullptr)
	{
		return mSelectedInteraction->OverrideCommunicateInteractionString(stringMsgm,message);
	}
	return -1;
}

void UTHsActionSystemSelector::OverrideConnectGizmo(TScriptInterface<class ITHsInteractiveGizmoAPI> gizmo)
{
	//ITIHHsInteraction::OverrideConnetGizmo(gizmo);
}

void UTHsActionSystemSelector::OverrideDisconnectGizmo(TScriptInterface<class ITHsInteractiveGizmoAPI> gizmo)
{
	//ITIHHsInteraction::OverrideDisconnetGizmo(gizmo);
}

void ATHsActionCursor::DebugCursorInfo(float RayLength, float Duration, bool bDoTrace, TEnumAsByte<ECollisionChannel> TraceChannel, bool bOnScreenText)
{
	UWorld* worldContext = GetWorld();
	if (worldContext == nullptr)
	{
		return;
	}

	const FVector2D screenPos = GetCurrentRecord().ScreenPosition;
	const FVector2D screenDelta = mDeltaScreenPosition;
	FRay deviceRay = GetCurrentWorldRay();

	const FVector Origin = deviceRay.Origin;
	const FVector Dir    = deviceRay.Direction.GetSafeNormal();
	const FVector End    = Origin + (Dir.IsNearlyZero() ? FVector::ForwardVector : Dir) * FMath::Max(10.f, RayLength);

	DrawDebugSphere(worldContext, Origin, 5.f, 12, FColor::Red, false, Duration);
	DrawDebugDirectionalArrow(worldContext, Origin, Origin + (Dir.IsNearlyZero() ? FVector::ForwardVector : Dir)*120.f,25.f,FColor::Green,false,Duration,0,2.f);
	DrawDebugLine(worldContext, Origin, End, FColor::Blue, false, Duration, 0, 0.8f);
	FHitResult HR;
	bool bHit = false;
	if (bDoTrace)
	{
		FCollisionQueryParams QP(SCENE_QUERY_STAT(THsCursorTrace), /*bTraceComplex*/ true, this);
		QP.bReturnPhysicalMaterial = true;
		bHit = worldContext->LineTraceSingleByChannel(HR, Origin, End, TraceChannel, QP);

		if (bHit)
		{
			DrawDebugSphere(worldContext, HR.ImpactPoint, 10.f, 16, FColor::Yellow, false, Duration);
			DrawDebugLine(worldContext, HR.ImpactPoint, HR.ImpactPoint + HR.ImpactNormal * 60.f, FColor::Yellow, false, Duration, 0, 2.f);

			if (UPrimitiveComponent* Comp = HR.GetComponent())
			{
				const FBox B = Comp->Bounds.GetBox();
				DrawDebugBox(worldContext, B.GetCenter(), B.GetExtent(), FQuat::Identity, FColor::Yellow, false, Duration, 0, 0.5f);
			}
		}
	}
	const FString WorldText = FString::Printf(
	   TEXT("THs Cursor Debug\n")
	   TEXT("Screen : (%.1f, %.1f)\n")
	   TEXT("Delta  : (%.2f, %.2f)\n")
	   TEXT("Ray.O  : (%.1f, %.1f, %.1f)\n")
	   TEXT("Ray.D  : (%.3f, %.3f, %.3f)\n")
	   TEXT("Len    : %.0f  Trace=%s\n")
	   TEXT("Hit    : %s\n")
	   TEXT("H.Pos  : %s\n")
	   TEXT("H.Nor  : %s\n")
	   TEXT("Comp   : %s"),
	   screenPos.X, screenPos.Y,
	   screenDelta.X, screenDelta.Y,
	   Origin.X, Origin.Y, Origin.Z,
	   Dir.X, Dir.Y, Dir.Z,
	   RayLength, bDoTrace ? TEXT("On") : TEXT("Off"),
	   bHit ? TEXT("Y") : TEXT("N"),
	   bHit ? *HR.ImpactPoint.ToString() : TEXT("-"),
	   bHit ? *HR.ImpactNormal.ToString() : TEXT("-"),
	   (bHit && HR.GetComponent()) ? *HR.GetComponent()->GetName() : TEXT("-")
   );
	DrawDebugString(worldContext, Origin + (Dir.IsNearlyZero()?FVector::ForwardVector:Dir) * 180.f, WorldText, nullptr, FColor::Cyan, Duration, false);
	if (bOnScreenText && GEngine)
	{
		const FString HUD = FString::Printf(
			TEXT("[Cursor] Scr(%.0f,%.0f) Δ(%.1f,%.1f)  RayLen=%.0f  Hit=%s  Comp=%s"),
			screenPos.X, screenPos.Y, screenDelta.X, screenDelta.Y, RayLength,
			bHit ? TEXT("Y") : TEXT("N"),
			(bHit && HR.GetComponent()) ? *HR.GetComponent()->GetName() : TEXT("-")
		);
		// 같은 키로 갱신되도록 this 포인터를 메시지 키로 사용
		GEngine->AddOnScreenDebugMessage((uint64)((PTRINT)this), Duration, FColor::Cyan, HUD);
	}
}

void ATHsActionCursor::DebugCursorVisualization()
{
	// =========================[ 최종 디버깅 비주얼라이저 ]=========================
#if !(UE_BUILD_SHIPPING)
	if (GEngine && mPlayerController && GetWorld())
	{
		if (UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport())
		{
			FVector2D ViewportSize;
			ViewportClient->GetViewportSize(ViewportSize);
			
			const float AspectRatio = ViewportSize.X / ViewportSize.Y;
			const float FOV = mPlayerController->PlayerCameraManager ? mPlayerController->PlayerCameraManager->GetFOVAngle() : 0.f;

			FString DebugText = FString::Printf(TEXT("[Cursor's World] FOV: %.2f, AspectRatio: %.4f"), FOV, AspectRatio);
			GEngine->AddOnScreenDebugMessage(11, 0.f, FColor::Cyan, DebugText);
		}
	}
	
	if (GEngine && mPlayerController && GetWorld())
	{
		// --- 1. ActionCursor가 계산한 "문제의 레이" ---
		const FVector CursorRayStart = GetCurrentRecord().WorldLocation;
		const FVector CursorRayDir = GetCurrentRecord().WorldDirection;
		const FVector CursorRayEnd = CursorRayStart + CursorRayDir * 5000.0f;

		// 초록색으로 그립니다.
		DrawDebugLine(GetWorld(), CursorRayStart, CursorRayEnd, FColor::Green, false, -1.f, 0, 3.0f);

		// --- 2. PlayerController에서 직접 얻은 "정답 레이" ---
		FVector CamLocation;
		FRotator CamRotation;
		mPlayerController->GetPlayerViewPoint(CamLocation, CamRotation);
		const FVector CamRayStart = CamLocation;
		const FVector CamRayDir = CamRotation.Vector();
		const FVector CamRayEnd = CamRayStart + CamRayDir * 5000.0f;

		// 파란색으로 그립니다.
		DrawDebugLine(GetWorld(), CamRayStart, CamRayEnd, FColor::Blue, false, -1.f, 0, 3.0f);

		// --- 3. 화면에 수치 정보 출력 ---
		FVector2D ScreenPos = GetCurrentRecord().ScreenPosition;
		FString DebugText1 = FString::Printf(TEXT("Screen Pos: %s"), *ScreenPos.ToString());
		FString DebugText2 = FString::Printf(TEXT("GREEN Ray Dir (Cursor): %s"), *CursorRayDir.ToString());
		FString DebugText3 = FString::Printf(TEXT("BLUE  Ray Dir (Camera): %s"), *CamRayDir.ToString());

		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, DebugText1);
		GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Green, DebugText2);
		GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Blue, DebugText3);
	}
#endif
	// ===========================================================================
}