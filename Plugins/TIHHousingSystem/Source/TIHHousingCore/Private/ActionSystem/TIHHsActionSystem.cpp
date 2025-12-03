// #include "TIHHsSQLiteHelper.h"
// #include "BaseGizmos/AxisPositionGizmo.h"
// #include "BaseGizmos/GizmoActor.h"

#include "ActionSystem/TIHHsActionSystem.h"
#include "TIHHsCore.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

//FTimerManager* UTIHHsActionPressGesture::gTimerManager = nullptr;
//UWorld* UTIHHsActionPressGesture::gWorld = nullptr;

void UTIHHsActionPressGesture::BeginDestroy()
{
	UObject::BeginDestroy();
	Clear();
	
}

void UTIHHsActionPressGesture::Initialize(const FString& gestureName, class UTIHHsActionLayer* actionLayer,
	float expiredTime)
{
	mActionLayer = actionLayer;
	mGestureName = gestureName;
	mExpiredTime = expiredTime;
	mIsPressing = false;
	mIsDoubleTapping = false;
		
	mActionDownRecord.Clear();
	mActionDownRecord.ParamIn.ActionName = mGestureName;
	mActionDownRecord.ParamIn.ActionData = TEXT("down");

	mActionUpRecord.Clear();
	mActionUpRecord.ParamIn.ActionName = mGestureName;
	mActionUpRecord.ParamIn.ActionData = TEXT("up");

	mHoldEndParam.ActionName = mGestureName;
	mHoldEndParam.ActionData = TEXT("holdEnd");
		
	mHoldEndParam.PropertyBag.AddProperty(TEXT("hold-end-data"),EPropertyBagPropertyType::Struct,TBaseStructure<FTIHHsActionHoldEndData>::Get());
		
}

void UTIHHsActionPressGesture::ProcessCallHitActor(FTIHHsActionParamIn param) const
{
	APlayerController* playerController = mWorldContext->GetFirstPlayerController();
	FHitResult hitResult;
	if (mActionLayer == nullptr)
	{
		return;
	}
	
	mActionLayer->SetUpLinTraceUnderMouse();
	if (mActionLayer->PerformLineTrace(hitResult))
	{
		TScriptInterface<ITIHHsInteraction> curHitActor(hitResult.GetActor());
		if (curHitActor.GetInterface())
		{
			if (TEXT("tap")==param.ActionData)
			{
				curHitActor->ActionEventTap_Deprecated(param);
			}
			else if (TEXT("doubleTap")==param.ActionData)
			{
				curHitActor->ActionEventDoubleTap_Deprecated(param);
			}
			else if (TEXT("holdStart")==param.ActionData)
			{
				curHitActor->ActionProcessHoldStart_Deprecated(param);
			}
			else if (TEXT("holding")==param.ActionData)
			{
				curHitActor->ActionProcessHoldOngoing_Deprecated(param);
			}
			else if (TEXT("holdEnd")==param.ActionData)
			{
				UE_LOG(LogTemp, Warning, TEXT("UTIHHsActionPressGesture::ProcessCallHitActor holdEnd"));
				curHitActor->ActionProcessHoldEnd_Deprecated(param);
			}
				
		}
	}
}


ETickableTickType UTIHHsActionSystem::GetTickableTickType() const
{
	return ETickableTickType::Never;
	//return ETickableTickType::Conditional;
}

bool UTIHHsActionSystem::IsTickable() const
{
	return false;
	//return UTIHHsGlobalConfigure::IsTickEnable(mWorld);
}

bool UTIHHsActionSystem::IsTickableInEditor() const
{
	return false;
}

bool UTIHHsActionSystem::IsTickableWhenPaused() const
{
	return false;
}


void UTIHHsActionSystem::ChangeActionMappingContext(const FName& inContextName)
{
	for (const FEnhancedActionKeyMapping& actionKeyMapping :mCurrActionKeyMappingArr)
	{
		const FName& actionKeyName = actionKeyMapping.GetMappingName();
		if (mActionInputProcessors.Contains(actionKeyName))
		{
			mActionInputProcessors[actionKeyName].UnRegistryDelegate.Broadcast(mEnhancedInputComponent);
		}
	}
	
	if (mActionMappingContextObjTable.Contains(inContextName))
	{
		UInputMappingContext* contextObj = mActionMappingContextObjTable[inContextName];
		
		if (mPlayerController)
		{
			UEnhancedInputLocalPlayerSubsystem* eilSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(mPlayerController->GetLocalPlayer());
			if (eilSubsystem != nullptr)
			{
				eilSubsystem->ClearAllMappings();
				eilSubsystem->AddMappingContext(contextObj, 0);
				
				FName oldContextName = mCurrActionMappingContextPair.Name;
				mCurrActionMappingContextPair.Name = inContextName;
				
				mCurrActionMappingContextPair.Object = contextObj;
				OnActionMappingChange.Broadcast(oldContextName,inContextName);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to get EnhancedInputLocalPlayerSubsystem"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Action mapping context '%s' not found"), *inContextName.ToString());
	}
}


void UTIHHsActionSystem::ProcessActionMapping_MainSelect_Down()
{
	ActionDown(TEXT("IA_main-select"));
}

void UTIHHsActionSystem::ProcessActionMapping_MainSelect_Up()
{
	//LogTemp, Warning, TEXT("ProcessActionMapping_MainSelect_Up"));
	ActionUp(TEXT("IA_main-select"));
}

void UTIHHsActionSystem::ProcessActionMapping_SubSelect_Down()
{
	//UE_LOG(LogTemp, Warning, TEXT("ProcessActionMapping_SubSelect_Down"));
	ActionDown(TEXT("IA_sub-select"));
}

void UTIHHsActionSystem::ProcessActionMapping_SubSelect_Up()
{
	//UE_LOG(LogTemp, Warning, TEXT("ProcessActionMapping_SubSelect_Up"));
	ActionUp(TEXT("IA_sub-select"));
}

void UTIHHsActionSystem::ProcessActionMapping_Cancel_Down()
{
	//UE_LOG(LogTemp, Warning, TEXT("ProcessActionMapping_Cancel_Down"));
	ActionDown(TEXT("IA_cancel"));
}

void UTIHHsActionSystem::ProcessActionMapping_Cancel_Up()
{
	//UE_LOG(LogTemp, Warning, TEXT("ProcessActionMapping_Cancel_Up"));
	ActionUp(TEXT("IA_cancel"));
}

void UTIHHsActionSystem::ProcessActionMapping_keyMouse_MouseMove()
{
	//UE_LOG(LogTemp, Warning, TEXT("ProcessActionMapping_keyMouse_MouseMove"));
}



void UTIHHsActionSystem::InitiateEnhanceInputMapping()
{
	mActionMappingContextDBPathsSoftPath =  TEXT("DataTable'/TIHHousingSystem/resources/db/mappingPaths.mappingPaths'");
	mCurrActionMappingContextPair.Name = TEXT("key-mouse");

	FSoftObjectPath softObjPath(mActionMappingContextDBPathsSoftPath);
	
	UDataTable* actionMappingContextPathDT = Cast<UDataTable>(softObjPath.TryLoad());
	if (actionMappingContextPathDT != nullptr)
	{
		actionMappingContextPathDT->ForeachRow<FTIHHsActionMappingContextPathDBRow>(TEXT("FTIHHsActionMappingContextDBRow::Initialize"),
			[this](const FName& rowName,const FTIHHsActionMappingContextPathDBRow& row)-> void
			{
				UInputMappingContext* mappingContext = LoadObject<UInputMappingContext>(nullptr, *row.ContextPath);
				if (mappingContext)
				{
					mActionMappingContextObjTable.Add(FName(*row.ContextName),mappingContext );
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to load InputMappingContext from path: %s"), *row.ContextPath);
				}
			});
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UTIHHsActionSystem::Initialize Failed to load DataTable from path: %s"), *mActionMappingContextDBPathsSoftPath);
	}
}

void UTIHHsActionSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	InitiateEnhanceInputMapping();
	
}

void UTIHHsActionSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// mActionLayer.Reset(NewObject<UTIHHsActionLayer>());
	// if (mActionLayer)
	// {
	// 	mActionLayer->Init(&InWorld,this,mCurrentTag.StaticCollisionChannel,false,FVector::ZeroVector,FVector::ZeroVector,mWorldTraceDistance);
	// }
	//
	// UTIHHsInterfaceRegistry* raw= NewObject<UTIHHsInterfaceRegistry>();
	// OnActionEventMouseMoveRegistry.Reset(raw);
	// OnActionEventMouseMoveRegistry->SetMethodPtr(&ITIHHsInteraction::ActionProcessMouseMove);
	//
	// //UTIHHsActionPressGesture::gWorld = &InWorld;
	// //UTIHHsActionPressGesture::gTimerManager = &InWorld.GetTimerManager();
	//
	// /*
	//  *	액션 이름이 IA_main-select이 놈이 있다면
	//  *		이놈의 이벤트중 Start든 Complete든 어쨌든 동작은 같을거니 RegisterDelegate를 호출하여 등록해준다.
	//  *		해당 이벤트와 이름이 같다면 해당 이벤트에 연결하라는 말이다.
	//  *		이렇게 되면 내가 다른 키를 추가하고 싶다면 해야할일은 다음과 같다.
	//  *		IM의 에셋위치를 등록(key-mouse,joystick,gamepad 등등)
	//  *		그 IM에 액션들을 등록해준다.
	//  *		그 액션들의 목적과 기능은 사전에 만들어놔야하며, 추후에 다른곳에서 오버라이드시 추가는 가능하다.
	//  *			mActionInputProcessors사실 여기에 하나씩 추가하면 되긴함. 시스템의 Initialize보다 먼저 호출되는게 없어서 그렇지..
	//  *			그렇지만 중요한건 만약 key-mouse에 뭐 다른 프로그래머가 추가를 하면 추가는 가능하다는거다.
	// *		수동으로 매핑에 대한 등록액션들을 등록했다면 이제 인풋매니저에서 등록된 IA들을 찾아준다.
	// *		찾은 IA를 보면 mActionInputProcessors 에 등록된 액션 등록프로세스를 실행해준다. 즉 사전에 이런것에 바인딩해라 같은거임
	// *		등록이 완료된후 매핑된 이름에 따라 제스쳐라는걸 만들어준다. 제스쳐란 그 액션의 타이머를 담당할 놈이라는것이다. 그럼 이벤트는?
	//  *		
	//  */
	// //	hideParam: manual, hideResult: mActionInputProcessors
	// ManualRegisterActionMappingsList();	
	//
	// if (mActionInputProcessors.IsEmpty())
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Failed to register any action mappings. Please ensure that you have registered at least one action mapping processor."));
	// }
	//
	// mWorld = &InWorld;
	// mPlayerController = mWorld->GetFirstPlayerController();
	// {
	// 	mPlayerController->bShowMouseCursor = true;
	// 	mPlayerController->bEnableClickEvents = true;
	// 	mPlayerController->bEnableMouseOverEvents = true;
	// 	mPlayerCameraManager = mPlayerController->PlayerCameraManager;
	// }
	// mEnhancedInputComponent = mPlayerController->FindComponentByClass<UEnhancedInputComponent>();
	// {
	// 	FInputModeGameAndUI inputMode;
	// 	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// 	inputMode.SetHideCursorDuringCapture(false);
	// 	inputMode.SetWidgetToFocus(nullptr); // No specific widget to focus
	// 	mPlayerController->SetInputMode(inputMode);
	// }
	//
	// mTimerManager = &mWorld->GetTimerManager();
	// mCurrActionMappingContextPair.Object = mActionMappingContextObjTable.Contains(mCurrActionMappingContextPair.Name) ? mActionMappingContextObjTable[mCurrActionMappingContextPair.Name] : nullptr;
	//
	//
	// TArray<FName> actionNames;
	// mActionInputProcessors.GetKeys(actionNames);
	// mActionTimerHandles.Reserve(mCurrActionKeyMappingArr.Num());
	//
	// for (const FEnhancedActionKeyMapping& keyMappingContext : mCurrActionKeyMappingArr)
	// {
	// 	FName actionName = keyMappingContext.GetMappingName();
	// 	mActionTimerHandles.Add(actionName);
	// }
	//
	// if (mActionMappingContextObjTable.Contains(TEXT("key-mouse")))
	// {
	// 	ChangeActionMappingContext(TEXT("key-mouse"));
	// 	SettingInputActionProcessors();
	// }
	// else
	// {
	// 	mCurrActionMappingContextPair.Object = nullptr;
	// 	UE_LOG(LogTemp, Error, TEXT("Failed to find ActionMappingContext: key-mouse"));
	// }
	//
	// if (mEnhancedInputComponent != nullptr)
	// {
	// 	if (mEnhancedInputComponent->HasBindings())
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("EnhancedInputComponent has bindings."));
	// 		
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("EnhancedInputComponent has no bindings."));
	// 	}
	// }
	//
	// InitHandleMoveActionNodes();
	// mMoveSensitive = 0.00000001f;
	//
	// mOnMouseMoveParamIn.ActionName = TEXT("move");
	// mOnMouseMoveParamIn.ActionData = TEXT("delegate");
	// mOnMouseMoveParamIn.PropertyBag.AddProperty(TEXT("mouse-data"),EPropertyBagPropertyType::Struct,
	// 	TBaseStructure<FTIHHsActionMoveMouseData>::Get());
	//
	//
}


void UTIHHsActionSystem::ProcessTraceMouseRecord(float deltaTime)
{
	static FTIHHsActionMoveMouseData mouseData;
	FVector2D currMousePos;
	FVector currWorldLocation;
	FVector currWorldDirection;
	ETIHHsActionMoveEventType currMoveEventType = ETIHHsActionMoveEventType::EStop;
	//UE_LOG(LogTemp, Warning, TEXT("MySubsystem::Tick Called - Frame: %d, Time: %f"), GFrameCounter, GetWorld()->GetTimeSeconds());
	bool isMouseInViewport = true;
	bool isDeproject = false;
	isMouseInViewport &= mPlayerController->GetMousePosition(currMousePos.X,currMousePos.Y);
	isDeproject = mPlayerController->DeprojectScreenPositionToWorld(currMousePos.X, currMousePos.Y, currWorldLocation, currWorldDirection);
	isMouseInViewport &= isDeproject;
	
	const FTIHHsActionRecordMove& prevRecord = mActionRecordMoveContainer.GetLastRecord();
	
	bool isMove = false;
	if (isMouseInViewport&&isDeproject)
	{
		if (not FMath::IsNearlyEqual(prevRecord.ScreenPosition.X, currMousePos.X, mMoveSensitive) || not FMath::IsNearlyEqual(prevRecord.ScreenPosition.Y, currMousePos.Y, mMoveSensitive))
		{
			isMove = true;
			mouseData.ScreenPosition = currMousePos;
			mouseData.WorldLocation = currWorldLocation;
			mouseData.WorldDirection = currWorldDirection;
			mouseData.DeletaTime = deltaTime;
			mouseData.IsMouseInViewport = isMouseInViewport;
			mouseData.IsDeproject = isDeproject;
			mouseData.IsMove = isMove;
			
			mOnMouseMoveParamIn.PropertyBag.SetValueStruct(TEXT("mouse-data"),mouseData);
			
			mActionMouseStopTime = 0.0f;
			currMoveEventType = ETIHHsActionMoveEventType::EMove;
		}
		else // 커서와 변환은 가능하지만 움직임이 멈췄다.
		{
			currMoveEventType = ETIHHsActionMoveEventType::EStop;
			mTempMoveTime= 0.0f;
		}
	}
	else
	{
		currMoveEventType = ETIHHsActionMoveEventType::EOutCursor;
		mTempMoveTime= 0.0f;
	}
	const int32 preveMoveState = static_cast<int32>(prevRecord.MoveType);
	const int32 currMoveEventTypeType = static_cast<int32>(currMoveEventType);

	if (isMove)
	{
		OnActionEventMouseMoveRegistry->BroadCastMethod(mOnMouseMoveParamIn);
		OnActionEventMouseMoveDelegate.Broadcast(mOnMouseMoveParamIn);
	}
	
	mActionMoveRootNodes[preveMoveState][currMoveEventTypeType].Function(deltaTime, currMousePos, currWorldLocation, currWorldDirection, isMouseInViewport, isDeproject, isMove);
	
}

void UTIHHsActionLayer::Init(UWorld* world,UTIHHsActionSystem* defaultActionSystem, ECollisionChannel defaultTraceChannel, bool defaultIsTraceComplex,
	 FVector defaultLastWorldLocation, FVector defaultmLastWorldDirection,
	float defaultmLastTraceDistance)
{
	if (mWorld == nullptr)
	{
		if (GEngine)
		{
			mWorld = GEngine->GetWorldFromContextObject(this,EGetWorldErrorMode::LogAndReturnNull);
		}
		if (GEngine && mWorld == nullptr)
		{
			mWorld = GEngine->GetWorldContexts()[0].World();
		}
		if (mWorld == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("UTIHHsActionLayer::Init: World is null."));
			if (IsRooted())
			{
				RemoveFromRoot();
			}
			ClearFlags(RF_Standalone | RF_Public);
			MarkAsGarbage();
			return;
		}
	}
	mActionSystem = defaultActionSystem ;
	if (mActionSystem  == nullptr)
	{
		mActionSystem = mWorld->GetSubsystem<UTIHHsActionSystem>();
	}
	
	mWorld = world;
	mLastTraceChannel = defaultTraceChannel;
	mIsTraceComplex = defaultIsTraceComplex;
	mLastWorldLocation = defaultLastWorldLocation;
	mLastWorldDirection = defaultmLastWorldDirection;
	mLastTraceDistance = defaultmLastTraceDistance;
	
}

void UTIHHsActionLayer::SetUpLineTrace(ECollisionChannel traceChannel, const FVector& start, const FVector& direction, float traceDistance)
{
	if (mWorld == nullptr)
	{
		return;
	}
	mLastTraceChannel = traceChannel;
	mLastWorldLocation = start;
	mLastWorldDirection = direction;
	mLastTraceDistance = traceDistance;
}

void UTIHHsActionLayer::SetUpLinTraceUnderMouse()
{
	if (mWorld == nullptr || mActionSystem == nullptr)
	{
		return;
	}
	const FTIHHsActionRecordMove& record = mActionSystem->GetMoveRecordTarget();
	mLastWorldLocation = record.WorldLocation;
	mLastWorldDirection = record.WorldDirection;
	mLastTraceChannel = mActionSystem->GetCurrentTraceChannel();
	mLastTraceDistance = mActionSystem->GetWorldTraceDistance();
}

bool UTIHHsActionLayer::PerformLineTrace(FHitResult& hitResult)
{
	if (mWorld == nullptr)
	{
		return false;
	}
	bool result = mWorld->LineTraceSingleByChannel(
		hitResult,
		mLastWorldLocation,
		mLastWorldLocation + mLastWorldDirection * mLastTraceDistance,
		mLastTraceChannel,
		FCollisionQueryParams(NAME_None, mIsTraceComplex)
	);
	return result;
}


void UTIHHsActionSystem::InitHandleMoveActionNodes()
{
	constexpr int32 RecordMoveType_Idle = static_cast<int32>(ETIHHsActionMoveStateType::EIdle);
	constexpr int32 RecordMoveType_OutCursor = static_cast<int32>(ETIHHsActionMoveStateType::EOutCursor);
	constexpr int32 RecordMoveType_MoveStart = static_cast<int32>(ETIHHsActionMoveStateType::EMoveStart);
	constexpr int32 RecordMoveType_MoveOngoing = static_cast<int32>(ETIHHsActionMoveStateType::EMoveOngoing);
	constexpr int32 RecordMoveType_MoveEnd = static_cast<int32>(ETIHHsActionMoveStateType::EMoveEnd);
	
	constexpr int32 EvaluateType_Stop = static_cast<int32>(ETIHHsActionMoveEventType::EStop);
	constexpr int32 EvaluateType_Move = static_cast<int32>(ETIHHsActionMoveEventType::EMove);
	constexpr int32 EvaluateType_OutCursor = static_cast<int32>(ETIHHsActionMoveEventType::EOutCursor);
	
	//	Idle공통
	mActionMoveStateHandleFuncsTable[RecordMoveType_Idle].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		FTIHHsActionRecordMove& currRecord = mActionRecordMoveContainer.BeginRecord();
		WriteMoveRecord(
			currRecord,
			isDeproject,isMouseInViewport,isMove,
			currMousePos,currWorldLocation,currWorldDirection,
			ETIHHsActionMoveStateType::EIdle
		);
		mActionRecordMoveContainer.EndRecord();
	};
	//	OutCursor공통
	mActionMoveStateHandleFuncsTable[RecordMoveType_OutCursor].Function = 
	[this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		FTIHHsActionRecordMove& currRecord = mActionRecordMoveContainer.BeginRecord();
		WriteMoveRecord(
			currRecord,
			isDeproject,isMouseInViewport,isMove,
			currMousePos,currWorldLocation,currWorldDirection,ETIHHsActionMoveStateType::EOutCursor
		);
		OnActionProcessMouseMoveOutCursor .Broadcast(FTIHHsActionParamIn(TEXT("move"),TEXT("out"),deltaTime));
		mActionRecordMoveContainer.EndRecord();
	};
	
	mActionMoveStateHandleFuncsTable[RecordMoveType_MoveStart].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		FTIHHsActionRecordMove& currRecord = mActionRecordMoveContainer.BeginRecord();
		WriteMoveRecord(
			currRecord,
			isDeproject,isMouseInViewport,isMove,
			currMousePos,currWorldLocation,currWorldDirection,
			ETIHHsActionMoveStateType::EMoveStart
		);
		OnActionProcessMouseMoveStart.Broadcast(FTIHHsActionParamIn(TEXT("move"),TEXT("start"),deltaTime));
		mActionRecordMoveContainer.EndRecord();
	};
	
	mActionMoveStateHandleFuncsTable[RecordMoveType_MoveOngoing].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		FTIHHsActionRecordMove& currRecord = mActionRecordMoveContainer.BeginRecord();
		WriteMoveRecord(
			currRecord,
			isDeproject,isMouseInViewport,isMove,
			currMousePos,currWorldLocation,currWorldDirection,
			ETIHHsActionMoveStateType::EMoveOngoing
		);
		OnActionProcessMouseMoveOngoing.Broadcast(FTIHHsActionParamIn(TEXT("move"),TEXT("ongoing"),deltaTime));
		mActionRecordMoveContainer.EndRecord();
	};
	
	mActionMoveStateHandleFuncsTable[RecordMoveType_MoveEnd].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		FTIHHsActionRecordMove& currRecord = mActionRecordMoveContainer.BeginRecord();
		WriteMoveRecord(
			currRecord,
			isDeproject,isMouseInViewport,isMove,
			currMousePos,currWorldLocation,currWorldDirection,
			ETIHHsActionMoveStateType::EMoveEnd
		);
		OnActionProcessMouseMoveEnd.Broadcast(FTIHHsActionParamIn(TEXT("move"),TEXT("end"),deltaTime));
		mActionRecordMoveContainer.EndRecord();
	};
	
	
	
	mActionMoveRootNodes.SetNum(static_cast<int32>(ETIHHsActionMoveStateType::EMax));
	for (auto& rootNode :mActionMoveRootNodes)
	{
		rootNode.NextNodes.SetNum(static_cast<int32>(ETIHHsActionMoveEventType::EMax));
	}
	
	//	----------------------------------------------------------------
	//	record Idle
	mActionMoveRootNodes[RecordMoveType_Idle][EvaluateType_Stop].Function =
		[this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
			//UE_LOG(LogTemp, Warning, TEXT("멈춰있다가 멈춤"));
			mActionMoveStateHandleFuncsTable[RecordMoveType_Idle].Function(deltaTime, currMousePos, currWorldLocation, currWorldDirection, isMouseInViewport, isDeproject, isMove);
		mTempMoveIdleTime += deltaTime;
			mTempMoveIdleTime = 0.0f;
		if (mTempMoveIdleTimeMax <= mTempMoveIdleTime)
		{
			mActionMoveStateHandleFuncsTable[RecordMoveType_Idle].Function(mTempMoveIdleTime + deltaTime, currMousePos, currWorldLocation, currWorldDirection, isMouseInViewport, isDeproject, isMove);
		}
	};
	mActionMoveRootNodes[RecordMoveType_Idle][EvaluateType_Move].Function =
		[this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		mActionMoveStateHandleFuncsTable[RecordMoveType_MoveStart].Function(deltaTime, currMousePos, currWorldLocation, currWorldDirection, isMouseInViewport, isDeproject, isMove);
	};
	
	mActionMoveRootNodes[RecordMoveType_Idle][EvaluateType_OutCursor].Function =
		[this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		const FTIHHsActionRecordMove& prevRecord = mActionRecordMoveContainer.GetLastRecord();
		mActionMoveStateHandleFuncsTable[RecordMoveType_OutCursor].Function(
			deltaTime, prevRecord.ScreenPosition, prevRecord.WorldLocation, prevRecord.WorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};
	
	//	----------------------------------------------------------------
	//	OutCursor
	mActionMoveRootNodes[RecordMoveType_OutCursor][EvaluateType_Stop].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("커서가 밖에 나갔는데 안으로 들어와 멈춤"));

		const FTIHHsActionRecordMove& prevRecord = mActionRecordMoveContainer.GetLastRecord();
		mActionMoveStateHandleFuncsTable[RecordMoveType_Idle].Function(
			deltaTime, prevRecord.ScreenPosition, prevRecord.WorldLocation, prevRecord.WorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};
	mActionMoveRootNodes[RecordMoveType_OutCursor][EvaluateType_Move].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("커서가 밖에 나갔는데 안으로 들어와 움직임"));

		const FTIHHsActionRecordMove& prevRecord = mActionRecordMoveContainer.GetLastRecord();
		mActionMoveStateHandleFuncsTable[RecordMoveType_MoveStart].Function(
			deltaTime, prevRecord.ScreenPosition, prevRecord.WorldLocation, prevRecord.WorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};
	mActionMoveRootNodes[RecordMoveType_OutCursor][EvaluateType_OutCursor].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("커서가 밖에 나갔는데 계속 나가있음"));
		const FTIHHsActionRecordMove& prevRecord = mActionRecordMoveContainer.GetLastRecord();
		mActionMoveStateHandleFuncsTable[RecordMoveType_OutCursor].Function(
			deltaTime, prevRecord.ScreenPosition, prevRecord.WorldLocation, prevRecord.WorldDirection, isMouseInViewport, isDeproject, isMove
		);
		//OnActionProcessMouseMoveOutCursor.Broadcast(FTIHHsActionParamIn(TEXT("move"),TEXT("out"),deltaTime));
	};
	//	----------------------------------------------------------------
	//	MoveStart
	mActionMoveRootNodes[RecordMoveType_MoveStart][EvaluateType_Stop].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("움직임을 시작했다가 멈춤"));

		mActionMoveStateHandleFuncsTable[RecordMoveType_MoveEnd].Function(
			deltaTime, currMousePos, currWorldLocation, currWorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};
	mActionMoveRootNodes[RecordMoveType_MoveStart][EvaluateType_Move].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("움직임을 시작했다가 계속 움직임"));
		mActionMoveStateHandleFuncsTable[RecordMoveType_MoveOngoing].Function(
			deltaTime, currMousePos, currWorldLocation, currWorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};
	mActionMoveRootNodes[RecordMoveType_MoveStart][EvaluateType_OutCursor].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("움직임을 시작했다가 커서가 밖으로 나갔다."));
		const FTIHHsActionRecordMove& prevRecord = mActionRecordMoveContainer.GetLastRecord();
		mActionMoveStateHandleFuncsTable[RecordMoveType_MoveEnd].Function(
			deltaTime, prevRecord.ScreenPosition, prevRecord.WorldLocation, prevRecord.WorldDirection, isMouseInViewport, isDeproject, isMove
		);
		mActionMoveStateHandleFuncsTable[RecordMoveType_OutCursor].Function(
			deltaTime, prevRecord.ScreenPosition, prevRecord.WorldLocation, prevRecord.WorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};
	//	----------------------------------------------------------------
	//	MoveOngoing
	mActionMoveRootNodes[RecordMoveType_MoveOngoing][EvaluateType_Stop].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("움직이다가 멈춤"));
		mActionMoveStateHandleFuncsTable[RecordMoveType_MoveEnd].Function(
			deltaTime, currMousePos, currWorldLocation, currWorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};
	mActionMoveRootNodes[RecordMoveType_MoveOngoing][EvaluateType_Move].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("움직이고 있고 계속 움직임"));
		OnActionProcessMouseMoveOngoing.Broadcast(FTIHHsActionParamIn(TEXT("move"),TEXT("ongoing"),deltaTime));
		FTIHHsActionRecordMove& beginRecord = mActionRecordMoveContainer.BeginRecord();
		WriteMoveRecord(beginRecord,
			isDeproject,isMouseInViewport,isMove,
			currMousePos,currWorldLocation,currWorldDirection,
			ETIHHsActionMoveStateType::EMoveOngoing
		);
		mActionRecordMoveContainer.EndRecord();
		mMoveTickTime += deltaTime;
		// if (mTempMoveTimeMax<=mMoveTickTime )
		// {
		// 	mMoveTickTime = 0.0f;
		// 	FTIHHsActionRecordMove& beginRecord = mActionRecordMoveContainer.BeginRecord();
		// 	WriteMoveRecord(beginRecord,
		// 		isDeproject,isMouseInViewport,isMove,
		// 		currMousePos,currWorldLocation,currWorldDirection,
		// 		ETIHHsActionMoveStateType::EMoveOngoing
		// 	);
		// 	mActionRecordMoveContainer.EndRecord();
		// }
	};
	mActionMoveRootNodes[RecordMoveType_MoveOngoing][EvaluateType_OutCursor].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("움직이다가 커서 밖으로 나감"));
		const FTIHHsActionRecordMove& prevRecord = mActionRecordMoveContainer.GetLastRecord();
		mActionMoveStateHandleFuncsTable[RecordMoveType_MoveEnd].Function(
			deltaTime, prevRecord.ScreenPosition, prevRecord.WorldLocation, prevRecord.WorldDirection, isMouseInViewport, isDeproject, isMove
		);
		mActionMoveStateHandleFuncsTable[RecordMoveType_OutCursor].Function(
			deltaTime, prevRecord.ScreenPosition, prevRecord.WorldLocation, prevRecord.WorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};
	//	----------------------------------------------------------------
	//	MoveEnd
	mActionMoveRootNodes[RecordMoveType_MoveEnd][EvaluateType_Stop].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		mActionMoveStateHandleFuncsTable[RecordMoveType_OutCursor].Function(
			deltaTime, currMousePos, currWorldLocation, currWorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};
	mActionMoveRootNodes[RecordMoveType_MoveEnd][EvaluateType_Move].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		mActionMoveStateHandleFuncsTable[RecordMoveType_MoveStart].Function(
			deltaTime, currMousePos, currWorldLocation, currWorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};
	mActionMoveRootNodes[RecordMoveType_MoveEnd][EvaluateType_OutCursor].Function = [this](float deltaTime, const FVector2D& currMousePos, const FVector& currWorldLocation, const FVector& currWorldDirection, bool isMouseInViewport, bool isDeproject, bool isMove)
	{
		const FTIHHsActionRecordMove& prevRecord = mActionRecordMoveContainer.GetLastRecord();
		mActionMoveStateHandleFuncsTable[RecordMoveType_OutCursor].Function(
			deltaTime, prevRecord.ScreenPosition, prevRecord.WorldLocation, prevRecord.WorldDirection, isMouseInViewport, isDeproject, isMove
		);
	};

	//	최초의 record
	FTIHHsActionRecordMove& beginRecord = mActionRecordMoveContainer.BeginRecord();
	WriteMoveRecord(beginRecord,
		false,false,false,
		FVector2D::ZeroVector, FVector::ZeroVector, FVector::ZeroVector,
		ETIHHsActionMoveStateType::EIdle
	);
	mActionRecordMoveContainer.EndRecord();
	OnActionProcessMouseMoveStart.AddLambda([this](const FTIHHsActionParamIn& param)
	{
		//UE_LOG(LogTemp, Warning, TEXT("[[Mouse Move Start: %s]]"), *param.ActionName);
		mTempMoveIdleTime = 0.0f;
		mTempMoveTime = 0.0f;
		mMoveTickTime = 0.0f;
	});
	OnActionProcessMouseMoveOngoing.AddLambda([this](const FTIHHsActionParamIn& param)
	{
		//UE_LOG(LogTemp, Warning, TEXT("[[Mouse Move Ongoing: %s]]"), *param.ActionName);
		mTempMoveIdleTime = 0.0f;
		mTempMoveTime = 0.0f;
		mMoveTickTime = 0.0f;
	});
	OnActionProcessMouseMoveEnd.AddLambda([this](const FTIHHsActionParamIn& param)
	{
		//UE_LOG(LogTemp, Warning, TEXT("[[Mouse Move End: %s]]"), *param.ActionName);
		mTempMoveIdleTime = 0.0f;
		mTempMoveTime = 0.0f;
		mMoveTickTime = 0.0f;
	});
	OnActionProcessMouseMoveOutCursor.AddLambda([this](const FTIHHsActionParamIn& param)
	{
		//UE_LOG(LogTemp, Warning, TEXT("[[Mouse Move Out Cursor: %s]]"), *param.ActionName);
		mTempMoveIdleTime = 0.0f;
		mTempMoveTime = 0.0f;
		mMoveTickTime = 0.0f;
	});
}

void UTIHHsActionSystem::ProcessHoverRecord(float deltaTime)
{
	const FTIHHsActionRecordMove& recentTarget = mActionRecordMoveContainer.GetRecent();
	FTIHHsActionRecordHover& hoverTarget = mActionRecordHoverContainer.BeginRecord();
	bool lineTraceEnable = mForceHoverTick;
		
	/*
		 *	강제로 호버틱을 키거나, 마우스가 움직였거나, 맥스 틱타임이 되었거나, 지금 ongoing이거나
		 */
	if (recentTarget.IsCursorMove
		|| hoverTarget.HoverType == ETIHHsActionRecordHoverType::ETIHHsActionRecordHover_Ongoing)
	{
		lineTraceEnable = true;
		mHoverTickTime = 0.0f;
	}
	else 
	{
		if (mHoverTickTime <= mHoverTickTimeMax )
		{
			mHoverTickTime += deltaTime;
		}
		else
		{
			mHoverTickTime = 0.0f;
			lineTraceEnable = true;
		}
	}
	
	if (lineTraceEnable)
	{
		mActionLayer->SetUpLinTraceUnderMouse();
		mActionLayer->PerformLineTrace(hoverTarget.HitResult);
		
		//mPlayerController->GetHitResultUnderCursorByChannel()
		HandleHoverTraceResult(hoverTarget,deltaTime);
	}
		
}

bool UTIHHsActionSystem::HandleHoverTraceResult(FTIHHsActionRecordHover& hoverTarget, float deltaTime)
{
	bool reValue = false;
	static FString actionName = TEXT("hover");
	/*
		 *		name		prev	curr	condition			result
		 *		not			empty	empty	none				none
		 *		new			empty	exist	none				curr.Enter
		 *		end			exist	empty	none				prev.Exit
		 *		ongoing		exist	exist	prev == curr		prev.Ongoing
		 *		swap		exist	exist	prev != curr		prev.Exit + curr.Enter
		 *		
		 */
	static FInstancedPropertyBag HoverOngoingPropertyBag;
	static FInstancedPropertyBag HoverSwapPropertyBag;
	
	const FTIHHsActionRecordHover& prevHover = mActionRecordHoverContainer.GetRecordTarget();

	AActor* prevActor = prevHover.HitResult.GetActor();
	AActor* currActor = hoverTarget.HitResult.GetActor();
		
	TScriptInterface<ITIHHsInteraction> prevInterface( prevActor);
	TScriptInterface<ITIHHsInteraction> currInterface(currActor);

	//	onging이 이전에도 있었다면 그냥 여기에서 바로 ongoing을 처리해준다.
	if (hoverTarget.HoverType == ETIHHsActionRecordHoverType::ETIHHsActionRecordHover_Ongoing)
	{
		if (currActor != nullptr && prevActor == currActor)	//	다만 조건을 검사안하지는 않는데, 지금 객체가 있고, 같을때 검사해준다.
		{
			hoverTarget.HoveringTime += deltaTime;	//	마찬가지로 계속 델타타임을 더해주고 실행한다. 왜냐하면 이건 계속 이전의 타겟일거니깐
			//UE_LOG(LogTemp, Warning, TEXT("-0-0-0-0-0-0-0- superOngoing"));
			currInterface->ActionProcessHoverOngoing_Deprecated(FTIHHsActionParamIn(actionName,TEXT(""),deltaTime));
			//ITIHHsInteraction::Execute_ActionProcessHoverOngoing(	currActor,	FTIHHsActionParamIn(actionName,TEXT(""),deltaTime)	);
		}
		else // 그런데 저기중 하나의 조건이 맞지가 않았다면 ongoing을 남겨줘야 하기에 end를 해주고, 다시 레코드를 시작하고 그 레코드를 검증한다. 아마도 이때 OnGoing은 뜰 수 없을거다. 하지만 OnGoing과 관련된게 HandleHoverTraceSwitch에 있는 이유는 최초의 첫번째는 존재해야하기 때문이다.
		{
			reValue = true;
			mActionRecordHoverContainer.EndRecord();
			auto& newHoverTarget = mActionRecordHoverContainer.BeginRecord();
			HandleHoverTraceSwitch(newHoverTarget, deltaTime, actionName, prevActor, currActor, prevInterface, currInterface);
		}
	}
	else
	{
		reValue = HandleHoverTraceSwitch(hoverTarget, deltaTime, actionName, prevActor, currActor, prevInterface, currInterface);
	}
	return reValue;
}



bool UTIHHsActionSystem::HandleHoverTraceSwitch(FTIHHsActionRecordHover& hoverTarget, float deltaTime,FString& actionName, AActor* prevActor, AActor* currActor, TScriptInterface<ITIHHsInteraction> prevInterface,TScriptInterface<ITIHHsInteraction> currInterface)
{
	bool reValue = false;
	if (not CheckInteraction(currActor))
	{
		currInterface.SetInterface(nullptr);
		currInterface.SetObject(nullptr);
	}
	FTIHHsIndirectObject a;
	if (prevInterface.GetInterface() != nullptr)
	{
		if (currInterface.GetInterface() != nullptr)
		{
			if (prevInterface.GetInterface() == currInterface.GetInterface())
			{
				//	ongoing
				hoverTarget.HoverType = ETIHHsActionRecordHoverType::ETIHHsActionRecordHover_Ongoing;
				hoverTarget.HoveringTime = 0.0f;
				currInterface->ActionProcessHoverOngoing_Deprecated(FTIHHsActionParamIn(actionName,TEXT(""),deltaTime));
				if (a.IsValid() && a.Individuality->CanBeGPHovered())
				{
					a.TIHInteraction->ActionProcessHoverOngoing_Deprecated(FTIHHsActionParamIn(actionName,TEXT(""),deltaTime));
				}
				//ITIHHsInteraction::Execute_ActionProcessHoverOngoing(currActor,FTIHHsActionParamIn(actionName,TEXT(""),deltaTime));
			}
			else
			{
				//	swap
				hoverTarget.HoverType = ETIHHsActionRecordHoverType::ETIHHsActionRecordHover_Swap;
				hoverTarget.HoveringTime = 0.0f;
				prevInterface->ActionProcessHoverEnd_Deprecated(FTIHHsActionParamIn(actionName,TEXT("")));
				currInterface->ActionProcessHoverStart_Deprecated(FTIHHsActionParamIn(actionName,TEXT("")));
				reValue = true;
				mActionRecordHoverContainer.EndRecord();
			}
		}
		else
		{
			hoverTarget.HoverType = ETIHHsActionRecordHoverType::ETIHHsActionRecordHover_End;
			hoverTarget.HoveringTime = 0.0f;
			prevInterface->ActionProcessHoverEnd_Deprecated(FTIHHsActionParamIn(actionName,TEXT("")));
			//ITIHHsInteraction::Execute_ActionProcessHoverEnd(prevActor,FTIHHsActionParamIn(actionName,TEXT("")));
			reValue = true;
			mActionRecordHoverContainer.EndRecord();
		}
	}
	else
	{
		if (currInterface.GetInterface() != nullptr)
		{
			hoverTarget.HoverType = ETIHHsActionRecordHoverType::ETIHHsActionRecordHover_New;
			hoverTarget.HoveringTime = 0.0f;
			currInterface->ActionProcessHoverStart_Deprecated(FTIHHsActionParamIn(actionName,TEXT("")));
			//ITIHHsInteraction::Execute_ActionProcessHoverStart(	currActor,	FTIHHsActionParamIn(actionName,TEXT(""))	);
			reValue = true;
			mActionRecordHoverContainer.EndRecord();
		}
		//	else none
	}
	return reValue;
}

void UTIHHsActionSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// if (mOnActionTick)
	// {
	// 	if (mActionTickTime < mActonTickTimeMax)
	// 	{
	// 		mActionTickTime += DeltaTime;
	// 	}
	// 	else
	// 	{
	// 		ProcessTraceMouseRecord(mActionTickTime);
	// 		ProcessHoverRecord(mActionTickTime);
	// 		mActionTickTime = 0.0f;
	// 	}
	// }
	// else
	// {
	// 	mActionTickTime = 0.0f;
	// }
	
}

void UTIHHsActionSystem::Deinitialize()
{
	Super::Deinitialize();
}


void UTIHHsActionSystem::ManualRegisterActionMappingsList()
{
	TArray<FName> actionList = {
		TEXT("IA_main-select"),
		TEXT("IA_sub-select"),
		TEXT("IA_cancel"),
		TEXT("IA_cursor-move")
	};

	/*
	 *	이거 뭐하는거냐면 해당 액션이 있을때 이것 이것에 반응하고
	 *  	없어질때 이것 이것에 반응하는 그런거임
	 
template <typename UserClass, typename FuncType, typename ... BoundArgs>
	void UTIHHsActionSystem::RegisterActionMapping(
		const FName& ActionKey,			//	mActionInputProcessors
		ETriggerEvent TriggerEvent,
		UEnhancedInputComponent* InputComp,
		const FEnhancedActionKeyMapping& Mapping,
		UserClass* UserObject,
		FuncType&& Func,
		BoundArgs&&... Args)
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
	 */
	{
		FName actionName = TEXT("IA_main-select");
		mActionInputProcessors.Add(actionName);
		mActionInputProcessors[actionName].RegistryDelegate.AddLambda(
			[actionName,this](UEnhancedInputComponent* eic, const FEnhancedActionKeyMapping& mapping)->void
			{
				RegisterActionMapping(actionName,ETriggerEvent::Started,eic,mapping,this,&UTIHHsActionSystem::ProcessActionMapping_MainSelect_Down);
				RegisterActionMapping(actionName,ETriggerEvent::Completed,eic,mapping,this,&UTIHHsActionSystem::ProcessActionMapping_MainSelect_Up);
			}
		);
		mActionInputProcessors[actionName].UnRegistryDelegate.AddLambda(
			[actionName,this](UEnhancedInputComponent* eic)->void
			{
				UnregisterActionMapping(actionName,ETriggerEvent::Started,eic);
				UnregisterActionMapping(actionName,ETriggerEvent::Completed,eic);
			}
		);
	}
	
	{
		FName actionName = TEXT("IA_sub-select");
		mActionInputProcessors.Add(actionName);
		mActionInputProcessors[actionName].RegistryDelegate.AddLambda(
			[actionName,this](UEnhancedInputComponent* eic, const FEnhancedActionKeyMapping& mapping)->void
			{
				RegisterActionMapping(actionName,ETriggerEvent::Started,eic,mapping,this,&UTIHHsActionSystem::ProcessActionMapping_SubSelect_Down);
				RegisterActionMapping(actionName,ETriggerEvent::Completed,eic,mapping,this,&UTIHHsActionSystem::ProcessActionMapping_SubSelect_Up);
			}
		);
		mActionInputProcessors[actionName].UnRegistryDelegate.AddLambda(
			[actionName,this](UEnhancedInputComponent* eic)->void
			{
				UnregisterActionMapping(actionName,ETriggerEvent::Started,eic);
				UnregisterActionMapping(actionName,ETriggerEvent::Completed,eic);
			}
		);
	}
	
	{
		FName actionName = TEXT("IA_cancel");
		mActionInputProcessors.Add(actionName);
		mActionInputProcessors[actionName].RegistryDelegate.AddLambda(
			[actionName,this](UEnhancedInputComponent* eic, const FEnhancedActionKeyMapping& mapping)->void
			{
				RegisterActionMapping(actionName,ETriggerEvent::Started,eic,mapping,this,&UTIHHsActionSystem::ProcessActionMapping_Cancel_Down);
				RegisterActionMapping(actionName,ETriggerEvent::Completed,eic,mapping,this,&UTIHHsActionSystem::ProcessActionMapping_Cancel_Up);
			}
		);
		mActionInputProcessors[actionName].UnRegistryDelegate.AddLambda(
			[actionName,this](UEnhancedInputComponent* eic)->void
			{
				UnregisterActionMapping(actionName,ETriggerEvent::Started,eic);
				UnregisterActionMapping(actionName,ETriggerEvent::Completed,eic);
			}
		);
	}

}

void UTIHHsActionSystem::SettingInputActionProcessors()
{
	if (mCurrActionMappingContextPair.Object != nullptr
		&& mPlayerController != nullptr
		&& mEnhancedInputComponent != nullptr)
	{
		mCurrActionKeyMappingArr.Empty();
		mCurrActionKeyMappingArr = mCurrActionMappingContextPair.Object->GetMappings();

		if (mAdditionalActionMappingObj)
		{
			mCurrActionKeyMappingArr.Append(mAdditionalActionMappingObj->GetAdditionalActionKeyMappings());
		}
		UInputAction* newInputAction = NewObject<UInputAction>(this);
		newInputAction->ValueType = EInputActionValueType::Boolean;
		
		
		FEnhancedActionKeyMapping newMapping;
		newMapping.Action = newInputAction;
		
		
		
	//	mPlayerController->GetHitResultUnderCursorByChannel()
		
		for (const FEnhancedActionKeyMapping& keyMappingContext :mCurrActionKeyMappingArr)
		{
			const FName& mappingName = keyMappingContext.Action.GetFName();
			if (mActionInputProcessors.Contains(mappingName))
			{
				mActionInputProcessors[mappingName].RegistryDelegate.Broadcast(mEnhancedInputComponent,keyMappingContext);
				
				CreateActionPressGesture(mappingName.ToString(),0.25f);
				UE_LOG(LogTemp, Warning, TEXT("[[ Action mapping registered: %s ]]"), *mappingName.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No processor registered for action mapping: %s"), *mappingName.ToString());
			}
		}
	}
	else
	{
		if (mCurrActionMappingContextPair.Object == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Current ActionMappingContext is null. Please ensure it is set before calling SettingInputActionProcessors."));
		}
		if (mPlayerController == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController is null. Please ensure it is set before calling SettingInputActionProcessors."));
		}
		if (mEnhancedInputComponent == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("EnhancedInputComponent is null. Please ensure it is set before calling SettingInputActionProcessors."));
		}
	}
}
