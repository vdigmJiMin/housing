// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHTapActionSubSystem.h"
#include "TIHHsCore.h"
#include "GameFramework/InputDeviceSubsystem.h"

FTimerManager* UTIHHsTapActionPoint::gTimerManager = nullptr;
UTIHHsTapActionSubSystem* UTIHHsTapActionPoint::gMouseSubSystem = nullptr;

int64 UTIHHsTapActionSubSystem::gMouseSubSystemTickCount = 0;

void UTIHHsTapActionPoint::OnDownAction()
{
	if (gTimerManager->IsTimerActive(mMouseDoubleTapTimerHandle))
	{
		mDoubleTapState  = true;	//	ClearTimer를 호출하면 expired가 호출되어서 해줘야함.
		gTimerManager->ClearTimer(mMouseDoubleTapTimerHandle);
		OnBroadcastDoubleTap();
	}
	else
	{
		mHoldState = true;
		mDoubleTapState = false;
		gTimerManager->SetTimer(mMouseDoubleTapTimerHandle,this,&UTIHHsTapActionPoint::OnDoubleTapTimerExpired,mDoubleTapThresholdTime,false);
	}
}

void UTIHHsTapActionPoint::OnUpAction()
{
	//	holding 중
	if (gTimerManager->IsTimerActive(mMouseHoldingTimerHandle))
	{
		gTimerManager->ClearTimer(mMouseHoldingTimerHandle);
		OnBroadcastHoldEnd();
	}
	else if (gTimerManager->IsTimerActive(mMouseDoubleTapTimerHandle))//	아직 더블 평가중
	{
		mHoldState = false;
	}
}

void UTIHHsTapActionPoint::OnBroadcastTap()
{
	UE_LOG(LogTemp, Warning, TEXT("%s OnBroadcastTap"),*mControlPointKey);
}

void UTIHHsTapActionPoint::OnDoubleTapTimerExpired()
{
	if (not mHoldState && not mDoubleTapState)
	{
		OnBroadcastTap();
	}
	else
	{
		OnBroadcastHoldStart();
	}
}

void UTIHHsTapActionPoint::OnBroadcastHoldStart()
{
	gTimerManager->SetTimer(mMouseHoldingTimerHandle,this,&UTIHHsTapActionPoint::OnBroadcastHolding,0.1f,true);
	UE_LOG(LogTemp, Warning, TEXT("%s OnBroadcastHoldStart"),*mControlPointKey);
}

void UTIHHsTapActionPoint::OnBroadcastHolding()
{
	UE_LOG(LogTemp, Warning, TEXT("%s OnBroadcastHolding"),*mControlPointKey);
}

void UTIHHsTapActionPoint::OnBroadcastHoldEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("%s OnBroadcastHoldEnd"),*mControlPointKey);
}

void UTIHHsTapActionPoint::OnBroadcastDoubleTap()
{
	UE_LOG(LogTemp, Warning, TEXT("%s OnBroadcastDoubleTap"),*mControlPointKey);
}

void UTIHHsTapActionPoint::ClearTimerHandles()
{
	if (gTimerManager->IsTimerActive(mMouseDoubleTapTimerHandle))
	{
		gTimerManager->ClearTimer(mMouseDoubleTapTimerHandle);
	}
	if (gTimerManager->IsTimerActive(mMouseHoldStartTimerHandle))
	{
		gTimerManager->ClearTimer(mMouseHoldStartTimerHandle);
	}
	if (gTimerManager->IsTimerActive(mMouseHoldingTimerHandle))
	{
		gTimerManager->ClearTimer(mMouseHoldingTimerHandle);
	}
}

void UTIHHsTapActionPoint::BeginDestroy()
{
	UObject::BeginDestroy();
	UE_LOG(LogTemp, Warning, TEXT("%s BeginDestroy"),*mControlPointKey);
	ClearTimerHandles(); 
}

ATIHHsTapActionSubsystemActor::ATIHHsTapActionSubsystemActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void UTIHHsTapActionSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	mTickForceDisable = true;
	mTickPauseEnable = false;
	mTickRuntimeActive = true;
	mTickInEditorActive = false;

	mRayDistance = 1000.0f;
	mMouseCollisionChannel = ECollisionChannel::ECC_Visibility;

	mFramePerSecond = 30;
	mCurrNodeIndex = 0;
	mPrevNodeIndex = 0;
	mMouseActionNodeList.SetNum(mFramePerSecond * 3);
	

	const float interval = 1.0f / mFramePerSecond;
	mMouseTraceEnable = false;
	mCurrMouseHoverInterval = interval;
	mPrevMouseHoverInterval = interval;
	

	mPlayerController = nullptr;
	mWorld = nullptr;

	mDoubleTapThresholdTime = 0.1f;
	mHoldThresholdTime = mDoubleTapThresholdTime + 0.1f;

	mHoverNodeCount = mFramePerSecond;
	mCurrHoverNodeIndex = 0;
	mCurrentHoverActionNodes.SetNum(mHoverNodeCount);

	
}


void UTIHHsTapActionSubSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	mWorld = &InWorld;
	mPlayerController = ( mWorld->GetFirstPlayerController());
	
	mMouseSubsystemActor = mWorld->SpawnActor<ATIHHsTapActionSubsystemActor>();
	
	UTIHHsTapActionPoint::gTimerManager = &mWorld->GetTimerManager();
	UTIHHsTapActionPoint::gMouseSubSystem = this;
	
	UE_LOG(LogTemp, Warning, TEXT("nigga"));
	mDoubleTapThresholdTime = 0.2f;
	mHoldThresholdTime = mDoubleTapThresholdTime + 0.05f;
	
	if (mPlayerController == nullptr || mWorld == nullptr)
	{
		mTickForceDisable = true;
	}
	else
	{
		mTickForceDisable = false;
	}
}

bool UTIHHsTapActionSubSystem::IsTickEnable() const
{
	bool reValue = false;
	if (not UTIHHsGlobalConfigure::IsTickForceDisable())
	{
		if (mWorld == nullptr)
		{
			reValue = false;
		}
		else
		{
			if (mWorld->IsPaused())
			{
				reValue = UTIHHsGlobalConfigure::IsTickPauseEnable();
			}
			else
			{
				reValue = UTIHHsGlobalConfigure::IsTickRuntimeActive();
			}
		}
	}
	return reValue;
}
 void UTIHHsTapActionSubSystem::OnDoubleTapTimerExpired()
{
	if (not mWorld->GetTimerManager().IsTimerActive(mMouseHoldStartTimerHandle))
	{
		OnBroadcastTap();
	}
}



 void UTIHHsTapActionSubSystem::OnBroadcastDoubleTap()
{
	UE_LOG(LogTemp, Warning, TEXT("OnBroadcastDoubleTap"));
}

 void UTIHHsTapActionSubSystem::OnBroadcastTap()
{
	UE_LOG(LogTemp, Warning, TEXT("OnBroadcastTap"));
}

void UTIHHsTapActionSubSystem::OnBroadcastHoldStart()
{
	UE_LOG(LogTemp, Warning, TEXT("OnBroadcastHoldStart"));
	mWorld->GetTimerManager().SetTimer(mMouseHoldingTimerHandle,this,&UTIHHsTapActionSubSystem::OnBroadcastHolding,0.1f,true);
}

void UTIHHsTapActionSubSystem::OnBroadcastHolding()
{
	UE_LOG(LogTemp, Warning, TEXT("onBroadcastHolding"));
}

void UTIHHsTapActionSubSystem::OnBroadcastHoldEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("OnBroadcastHoldEnd"));
}

void UTIHHsTapActionSubSystem::OnDownAction()
{
	FTIHHsMouseActionNode& curHoverNode = GetCurrHoverNode();
	FTIHHsMouseActionNode temp;
	temp.ActionType = 1;
	temp.MouseScreenPosition = curHoverNode.MouseScreenPosition;
	temp.MouseWorldPosition = curHoverNode.MouseWorldPosition;
	RegisterMouseAction(temp);

	FTimerManager& timerMgr = mWorld->GetTimerManager();
	
	//	dud || dudu
	if (timerMgr.IsTimerActive(mMouseDoubleTapTimerHandle))
	{
		timerMgr.ClearTimer(mMouseDoubleTapTimerHandle);
		OnBroadcastDoubleTap();
	}
	else
	{
		timerMgr.SetTimer(mMouseDoubleTapTimerHandle,this,&UTIHHsTapActionSubSystem::OnDoubleTapTimerExpired,mDoubleTapThresholdTime,false);
		timerMgr.SetTimer(mMouseHoldStartTimerHandle,this,&UTIHHsTapActionSubSystem::OnBroadcastHoldStart,mHoldThresholdTime,false);
	}

}

void UTIHHsTapActionSubSystem::OnUpAction()
{
	FTIHHsMouseActionNode& curHoverNode = GetCurrHoverNode();
	FTIHHsMouseActionNode temp;
	temp.ActionType = 2;
	temp.MouseScreenPosition = curHoverNode.MouseScreenPosition;
	temp.MouseWorldPosition = curHoverNode.MouseWorldPosition;
	RegisterMouseAction(temp);
	
	FTimerManager& timerManager = mWorld->GetTimerManager();
	
	if (timerManager.IsTimerActive(mMouseHoldStartTimerHandle))
	{
		mWorld->GetTimerManager().ClearTimer(mMouseHoldStartTimerHandle);
	}
	else if (timerManager.IsTimerActive(mMouseHoldingTimerHandle))
	{
		mWorld->GetTimerManager().ClearTimer(mMouseHoldingTimerHandle);
		OnBroadcastHoldEnd();
	}
}

void UTIHHsTapActionSubSystem::SettingControlPoint(FString inControlPointName,float doubleTime,float holdTime)
{
	if (not mMouseSubsystemActor->mControlPointMap.Contains(inControlPointName))
	{
		UTIHHsTapActionPoint* newControlPoint = NewObject<UTIHHsTapActionPoint>(this);
		
		newControlPoint->OnSettingMouseSystem(inControlPointName,doubleTime,holdTime);
		mMouseSubsystemActor->mControlPointMap.Add(inControlPointName,newControlPoint);
	}
	else
	{
		mMouseSubsystemActor->mControlPointMap[inControlPointName]->OnSettingMouseSystem(inControlPointName,doubleTime,holdTime);
	}
}


void UTIHHsTapActionSubSystem::OnDonwActionByControlPoint(FString inControlPointName)
{
	if (mMouseSubsystemActor->mControlPointMap.Contains(inControlPointName))
	{
		mMouseSubsystemActor->mControlPointMap[inControlPointName]->OnDownAction();
	}
	
}

void UTIHHsTapActionSubSystem::OnUpActionByControlPoint(FString inControlPointName)
{
	if (mMouseSubsystemActor->mControlPointMap.Contains(inControlPointName))
	{
		mMouseSubsystemActor->mControlPointMap[inControlPointName]->OnUpAction();		
	}
}

void UTIHHsTapActionSubSystem::OnMouseTraceAuto(float inInterval)
{
	OnMoveTrack();
	mWorld->GetTimerManager().SetTimer(mMouseTraceTimerHandle,this,&UTIHHsTapActionSubSystem::OffMoveTrack,inInterval);
	
}

void UTIHHsTapActionSubSystem::OnMoveTrack()
{
	mPrevMouseHoverInterval = mCurrMouseHoverInterval;
	mMouseTraceEnable = true;
}

void UTIHHsTapActionSubSystem::OffMoveTrack()
{
	mMouseTraceEnable = false;
	mCurrMouseHoverInterval = mPrevMouseHoverInterval;
}

void UTIHHsTapActionSubSystem::Tick(float DeltaTime)
{
	static float gMouseTrackTickTime=  0.0f;
	Super::Tick(DeltaTime);
	if (not IsTickEnable())
	{
		UE_LOG(LogTemp, Error, TEXT("IsTickEnable: %d"), IsTickEnable());
		return;
	}
	++gMouseSubSystemTickCount;
	
	if (not mMouseTraceEnable && gMouseTrackTickTime < mCurrMouseHoverInterval )
	{
		gMouseTrackTickTime += DeltaTime;
		return;
	}
	gMouseTrackTickTime = 0.0f;
	
	FTIHHsMouseActionNode& prevHoverNode = GetPrevHoverNode();
	FTIHHsMouseActionNode& targetHoverNode = GetCurrHoverNode();
	
	bool mouseInGame = mPlayerController->GetMousePosition(
		targetHoverNode.MouseScreenPosition.X,targetHoverNode.MouseScreenPosition.Y);
	
	FVector worldLocation;
	FVector worldDirection;
	
	if (mPlayerController->DeprojectScreenPositionToWorld(targetHoverNode.MouseScreenPosition.X,targetHoverNode.MouseScreenPosition.Y,worldLocation,worldDirection))
	{
		if (not FMath::IsNearlyEqual(targetHoverNode.MouseScreenPosition.X,prevHoverNode.MouseScreenPosition.X,mMouseMoveSensitive) ||
			not FMath::IsNearlyEqual(targetHoverNode.MouseScreenPosition.Y,prevHoverNode.MouseScreenPosition.Y,mMouseMoveSensitive))
		{
			targetHoverNode.ActionType = 3;
			targetHoverNode.NodeTick = gMouseSubSystemTickCount;
			targetHoverNode.NodeTime = mWorld->GetTimeSeconds();
			
			ProgressHoverNode();
			
			AActor* prevHitActor = mHitResult.GetActor();
			AActor* currHitActor = nullptr;
			
			if (mWorld->LineTraceSingleByChannel(
				mHitResult,
				worldLocation,
				worldLocation + worldDirection * mRayDistance,
				mMouseCollisionChannel
			))
			{
				currHitActor = mHitResult.GetActor();
				targetHoverNode.MouseWorldPosition = mHitResult.Location;
			}
			
			
			// conditions
			if (prevHitActor == nullptr)
			{
				if (currHitActor != nullptr)
				{
					Cast<ITIHHsCommonObject>(currHitActor)->OnMouseActionHoverEnter();
				}
			}
			else    //	이전에 무언가가 있었음.
			{
				if (currHitActor == nullptr)	//	현재는 없음
				{
					Cast<ITIHHsCommonObject>(prevHitActor)->OnMouseActionHoverExit();
				}
				else   //	현재도 있음
				{
					if (prevHitActor != currHitActor) //	이전과 현재가 다름
					{
						Cast<ITIHHsCommonObject>(prevHitActor)->OnMouseActionHoverExit();
						Cast<ITIHHsCommonObject>(currHitActor)->OnMouseActionHoverEnter();
					}
					else //	이전과 현재가 같음
					{
						Cast<ITIHHsCommonObject>(currHitActor)->OnMouseActionHovering();
					}
				}
			}

			
		}
	}
	
	
}

void UTIHHsTapActionSubSystem::OnSettingMouseSystem(APlayerController* PlayerController)
{
	mPlayerController = PlayerController;
	mWorld = PlayerController->GetWorld();
	if (mPlayerController == nullptr || mWorld == nullptr)
	{
		mTickForceDisable = true;
	}
	else
	{
		mTickForceDisable = false;
	}
}

void UTIHHsTapActionSubSystem::RegisterMouseAction(const FTIHHsMouseActionNode& inNode)
{
	static int32 gNodeID = 0;
	if (not IsTickEnable())
	{
		UE_LOG(LogTemp, Error, TEXT("TickCheck"));
		return;
	}
	if (gNodeID == INT32_MAX)
	{
		gNodeID = 0;
	}
	mPrevNodeIndex = mCurrNodeIndex++;
	if (mCurrNodeIndex >= mMouseActionNodeList.Num())
	{
		mCurrNodeIndex = 0;
	}
	FTIHHsMouseActionNode& curNode = mMouseActionNodeList[mCurrNodeIndex].Clear();
	curNode.NodeID = gNodeID++;
	curNode.NodeTick = gMouseSubSystemTickCount;
	curNode.NodeTime = GetWorld()->GetRealTimeSeconds();
	curNode.ActionType = inNode.ActionType;
	curNode.MouseScreenPosition = inNode.MouseScreenPosition;
	curNode.MouseWorldPosition = inNode.MouseWorldPosition;
}

FTIHHsMouseActionNode& UTIHHsTapActionSubSystem::GetPrevHoverNode()
{
	if (mCurrHoverNodeIndex - 1 < 0)
	{
		return mCurrentHoverActionNodes.Last();
	}
	return mCurrentHoverActionNodes[mCurrHoverNodeIndex - 1];
}

FTIHHsMouseActionNode& UTIHHsTapActionSubSystem::GetCurrHoverNode()
{
	static FTIHHsMouseActionNode error;
	if (mCurrentHoverActionNodes.IsValidIndex(mCurrHoverNodeIndex))
	{
		return mCurrentHoverActionNodes[mCurrHoverNodeIndex];
	}
	error.Clear();
	error.ActionType = INDEX_NONE;
	return error;
}

FTIHHsMouseActionNode& UTIHHsTapActionSubSystem::ProgressHoverNode()
{
	if (mCurrHoverNodeIndex + 1 < mCurrentHoverActionNodes.Num())
    {
		++mCurrHoverNodeIndex;
    }
	else
	{
        mCurrHoverNodeIndex = 0;
	}
	FTIHHsMouseActionNode& curHoverNode = GetCurrHoverNode();
	curHoverNode.Clear();
	return curHoverNode;
}
