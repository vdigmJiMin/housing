// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHBlackBoardSubsystem.h"
#include "TIHHsBlackboard.inl"

TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_APPLY_DEFAULT_TYPE_TRAIT_CLASS_EXTERNAL)

int32 UTIHHsBlackBoardSubsystem::gBlackBoardSessionID = 0;
UTIHHsBlackBoardSubsystem* UTIHHsBlackBoardSubsystem::gBlackBoardSubsystem = nullptr;

void UTIHHsBlackBoardSubsystem::InitializeDefaultSizes()
{


}

void UTIHHsBlackBoardSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeDefaultSizes();
	

	
}

void UTIHHsBlackBoardSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	gBlackBoardSubsystem = this;
	mWorld = &InWorld;

	UTIHHsGlobalConfigure::SetTickForceDisable(false);
	UTIHHsGlobalConfigure::SetTickPauseEnable(true);
	UTIHHsGlobalConfigure::SetTickRuntimeActiveEnable(true);

	//mWriteLock = false;
	mCurrWriteTargetBBTicket = nullptr;
	mCurrWriteTargetSessionID = -1;
	mCurrentWriteTargetTicketID = -1;

	const int32 bbSesionsCapacity = 32;
	const int32 bbTicketsCapacity = 128;

	
	
	mBlackBoardSessions.Empty();
	
	mBlackBoardSessionIDs.Empty();

	mCurrBBTicketWriter = nullptr;
	mCurrBBTicketReaders.Empty();

	mBlackBoardSessionCloseTimers.Empty();
	mBlackBoardSessionCloseTickCounts.Empty();
	mBlackBoardSessionCloseReferenceCounts.Empty();
	mBlackBoardSessionCloseAlwaysSet.Empty();
	
	mTickInEditorActive = false;

	mBBTicketDeleteCount = 64;
	mBBSessionDeleteCount = 16;

	mBBDeleteBatchCount = 64;
	mBBDeleteBatchTime = 0.06666f;

	mBBSessionDeleteQueue.Empty();

	mBBSessionDeleteQueue.Empty();

	mCurrBBTicketReadersTimerLoopTime = 1.f / 240.0f;
	mCurrBBTicketReadersBatchCount = 32;
	
	mWorld->GetTimerManager().SetTimer(mCurrBBTicketReadersTimerHandle,
		this,&UTIHHsBlackBoardSubsystem::OnEvaluateCurrBBTicketReaders,
		mCurrBBTicketReadersTimerLoopTime,true,-1);
}


UTIHHsBBSession* UTIHHsBlackBoardSubsystem::GetBBSessionByBBTicketHandle(const FTIHHsBBTicketHandle& BBTicketHandle)
{
	return GetBBSessionById(BBTicketHandle.GetSessionID());
}

void UTIHHsBlackBoardSubsystem::OptimizationBBTicketElements(FTIHHsBBTicket& inTicket)
{
	
}

TArray<FTIHHsBBTicketElementForm> UTIHHsBlackBoardSubsystem::ConvertToBlackBoardElementForm(
	const TArray<FTIHHsBBTicketElement>& inTicketElements)
{
	TArray<FTIHHsBBTicketElementForm> outElements;
	outElements.Reserve(inTicketElements.Num());
	
	for (const FTIHHsBBTicketElement& bbTicketElement : inTicketElements)
	{
		FTIHHsBBTicketElementForm elementForm;
		if (bbTicketElement.GetDataCount() <= 0)
		{
			continue;
		}
		elementForm.ElementName =  FString::Printf(TEXT("%s-%d-%d"),*FTIHHsDefaultTypeTraitHelper::GetTypeName(bbTicketElement.GetDataType()),bbTicketElement.GetDataOffset(),bbTicketElement.GetDataCount());
		elementForm.ElementType =  FTIHHsDefaultTypeTraitHelper::GetTypeFromInt16(bbTicketElement.GetDataType());
		elementForm.ElementCount = bbTicketElement.GetDataCount();
		outElements.Add(elementForm);
	}
	
	outElements.Sort([](const FTIHHsBBTicketElementForm& lhs,const FTIHHsBBTicketElementForm& rhs)
	{
		if(lhs.ElementType == rhs.ElementType)
		{
			return lhs.ElementCount > rhs.ElementCount;
		}
		else   
		{
			return lhs.ElementType < rhs.ElementType;
		}
	});
	
	return outElements;
}


void UTIHHsBlackBoardSubsystem::RegisterBBSessionNameTable(FString sessionName, UTIHHsBBSession* newSession)
{
	mBlackBoardSessions.Add(sessionName,newSession);
	mBlackBoardSessionIDs.Add(newSession->mSessionID,sessionName);
}

UTIHHsBBSession*  UTIHHsBlackBoardSubsystem::CreateBBSessionInternal(FString sessionName, ETIHHsBBSessionCloseType sessionType, int32 sessionTickCount, float sessionTime)
{
	if (mBlackBoardSessions.Contains(sessionName))
	{
		UE_LOG(LogTemp,Warning,TEXT("BlackBoardSession Name is already exist %s"),*sessionName);
		FString addAny = TEXT("");
		int32 addCount = -1;
		while (mBlackBoardSessions.Contains(sessionName + addAny))
		{
			++addCount;
			addAny = FString::Printf(TEXT("%d"),addCount);
		}
		sessionName += addAny;
	}
	
	UTIHHsBBSession* newSession = (NewObject<UTIHHsBBSession>(GetTransientPackage()));
	if (newSession == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UTIHHsBlackBoardSubsystem::CreateBBSessionInternal() newSession is nullptr"));
		return nullptr;
	}
	
	newSession->SetSessionState(ETIHHsBBSessionState::EInitialized);
	newSession->mSessionID = GetCurrentNewBBSessionID();
	AdvanceCurrentNewBBSessionID();
	newSession->mSessionState = 0;
		
	newSession->mSessionData.SessionType = sessionType;
	newSession->mSessionData.SessionName = sessionName;
	newSession->mSessionData.SessionTime = sessionTime;
	newSession->mSessionData.SessionTickCount = sessionTickCount;
		
	RegisterBBSessionNameTable(sessionName, newSession);
		
	switch (sessionType) {
	case ETIHHsBBSessionCloseType::ETickBase:
		RegisterBBTicketCloseTickBase(newSession->mSessionID,sessionTickCount);
		break;
	case ETIHHsBBSessionCloseType::ETimeBase:
		RegisterBBTicketCloseTimeBase(newSession->mSessionID,sessionTime);
		break;
	case ETIHHsBBSessionCloseType::EReferenceCountBase:
		RegisterBBTicketCloseReferenceCountBase(newSession->mSessionID);
		break;
	case ETIHHsBBSessionCloseType::EAlways:
		RegisterBBTicketCloseAlways(newSession->mSessionID);
		break;
	}
	
	newSession->OnPostCreateSessionDelegate();
	newSession->SetSessionState(ETIHHsBBSessionState::EIdle);
	return newSession;
}


void UTIHHsBlackBoardSubsystem::RegisterBBTicketHandleInBBSession(TObjectPtr<UTIHHsBBSession> curBBSession,
	FTIHHsBBTicketHandle newTicketHandle)
{
	if (curBBSession == nullptr )
	{
		UE_LOG(LogTemp, Error, TEXT("UTIHHsBlackBoardSubsystem::RegistBBTicketForSession() curBBSession is nullptr"));
		return;
	}
	if (not newTicketHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UTIHHsBlackBoardSubsystem::RegistBBTicketForSession() newTicketHandle is nullptr"));
		return;
	}
	curBBSession->RegisterBBTicketHandle(newTicketHandle);
}

TSharedPtr<FTIHHsBBTicket> UTIHHsBlackBoardSubsystem::CreateBBTicket(TObjectPtr<UTIHHsBBSession> curBBSession,const TArray<FTIHHsBBTicketElementForm>& inReserveBBData)
{
	static int32 gTicketId = 0;
	FTIHHsBlackBoard& targetBB = curBBSession->GetBlackBoard();

	//	reserve
	TMap<int16,int32> elementReserveMap;
	for (const FTIHHsBBTicketElementForm& elementForm : inReserveBBData)
	{
		if (elementForm.ElementType != (ETIHHsBlackBoardDataType::EInvalid))
		{
			elementReserveMap.FindOrAdd(static_cast<int16>(elementForm.ElementType)) += elementForm.ElementCount;
		}
	}
	for (const TPair<int16, int32>& reseve : elementReserveMap)
	{
		targetBB.Reserve(reseve.Key,reseve.Value);
	}
	
	int32 sessionId = curBBSession->GetSessionID();
	int32 ticketId = gTicketId++;
	
	TSharedPtr<FTIHHsBBTicket> newTicket =  MakeShareable<FTIHHsBBTicket>(new FTIHHsBBTicket(sessionId,ticketId));
	//mBlackBoardTicketsAllList_desparate.Add(newTicket);

	newTicket->TicketElementsView.Get().Reserve(inReserveBBData.Num());

	//	get element type
	const int16 invalid = static_cast<int16>(ETIHHsBlackBoardDataType::EInvalid);
	bool hasElement = false;
	for (const FTIHHsBBTicketElementForm& elementForm : inReserveBBData)
	{
		int16 elementType = static_cast<int16>(elementForm.ElementType);
		if (elementType != invalid)
		{
			TArray<FTIHHsBBTicketElement>& tickElementView = newTicket->TicketElementsView.Get();
			int32 elementIdx = tickElementView.AddDefaulted();
			//	여기서부터는 실제로 데이터를 넣는것
			int16 elementType16 = static_cast<int16>(elementForm.ElementType);
			int16 arrayOffset = targetBB.AddDefaulted(elementType,elementForm.ElementCount);
			UE_LOG(LogTemp,Warning,TEXT("AddDefaulted %d %d"),elementType,elementForm.ElementCount);
			tickElementView[elementIdx].SetDataType(elementType16);
			tickElementView[elementIdx].SetDataCount(elementForm.ElementCount);
			tickElementView[elementIdx].SetDataSize(FTIHHsDefaultTypeTraitHelper::GetTypeSize(elementType));
			tickElementView[elementIdx].SetDataOffset(arrayOffset);
			if (not elementForm.ElementName.IsEmpty())
			{
				newTicket->TicketElementsNameMap.Add(
					elementForm.ElementName,
				FTIHHsBBTicketElementNamedView(elementType16,elementIdx));
			}
			hasElement = true;
		}
	}
	
	if (hasElement || inReserveBBData.Num() > 0)
	{
		newTicket->IncrementReadCount();
	}
	
	return newTicket;
}

FTIHHsBBTicketHandle UTIHHsBlackBoardSubsystem::TryPublishBBTicket(const FTIHHsBBSessionData& inSessionData, const TArray<FTIHHsBBTicketElementForm>& inReserveBBData)
{
	TObjectPtr< UTIHHsBBSession> curBBSession;
	//	makeSeesion 
	FString sessionName = inSessionData.SessionName;
	ETIHHsBBSessionCloseType sessionType = inSessionData.SessionType;
	int32 sessionTickCount = inSessionData.SessionTickCount;
	float sessionTime = inSessionData.SessionTime;

	//	이름만 가지고 생성
	if (mBlackBoardSessions.Contains(sessionName))
	{
		UE_LOG(LogTemp, Log, TEXT("BlackBoardSession Name is already exist %s"), *sessionName);
		curBBSession = mBlackBoardSessions[sessionName];
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT(" BlackBoardSession Name is not exist %s"), *sessionName);
		curBBSession = CreateBBSessionInternal(sessionName, sessionType, sessionTickCount, sessionTime);
	}
	
	if (curBBSession == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UTIHHsBlackBoardSubsystem::TryPublishBBTicket() curBBSession is nullptr"));
		return {};
	}
	
	curBBSession->SetSessionState(ETIHHsBBSessionState::ERunning);
	
	TSharedPtr<FTIHHsBBTicket> newTicket = CreateBBTicket(curBBSession, inReserveBBData);
	FTIHHsBBTicketHandle newTicketHandle = FTIHHsBBTicketHandle(*newTicket);
	
	RegisterBBTicketHandleInBBSession(curBBSession.Get(), newTicketHandle);

	UE_LOG(LogTemp, Log, TEXT("UTIHHsBlackBoardSubsystem::TryPublishBBTicket() - %s"), *newTicket->ToString());
	
	if (curBBSession->mBlackBoardTicketHandles.IsEmpty())
	{
		curBBSession->SetSessionState(ETIHHsBBSessionState::EIdle);
	}
	
	mBBTicketHandleTable.Add(newTicketHandle,newTicket);
	
	return newTicketHandle;
}

FTIHHsBBTicketHandle UTIHHsBlackBoardSubsystem::TryCopyBBTicket(const FTIHHsBBTicketHandle& inTicket)
{
	static FTIHHsBBTicketHandle reValueBBTicketHandle;
	reValueBBTicketHandle.SetInvalidate();
	if (IsValidBBTicket(inTicket))
	{
		TSharedPtr<FTIHHsBBTicket> ticketValue = mBBTicketHandleTable[inTicket];
		ticketValue->IncrementReadCount();
		reValueBBTicketHandle = inTicket;
	}
	return reValueBBTicketHandle;
}

FTIHHsBBTicketHandle UTIHHsBlackBoardSubsystem::TryInheritBBTicket(const FTIHHsBBTicketHandle& inTicket,
	const TArray<FTIHHsBBTicketElementForm>& inReserveBBData)
{
	static FTIHHsBBTicketHandle reValueBBTicketHandle;
	reValueBBTicketHandle.SetInvalidate();
	
	if (IsValidBBTicket(inTicket))
	{
		UTIHHsBBSession* curBBSession = GetBBSessionById(inTicket.GetSessionID());
		TSharedPtr<FTIHHsBBTicket> curBBTicket = GetBBTicketByHandle(inTicket);
		
		TArray<FTIHHsBBTicketElementForm> ticketElements;
		curBBTicket->GetTicketElementsForm(ticketElements);
		ticketElements.Append(inReserveBBData);
		reValueBBTicketHandle = TryPublishBBTicket(curBBSession->GetSessionData(), ticketElements);
	}
	return reValueBBTicketHandle;
}

FTIHHsBBTicketHandle UTIHHsBlackBoardSubsystem::TryMigrateBBTicket(const FTIHHsBBTicketHandle& inTicket,
	const FTIHHsBBSessionData& inTargetSessionData)
{
	static FTIHHsBBTicketHandle reValueBBTicketHandle;
	reValueBBTicketHandle.SetInvalidate();
	
	if (IsValidBBTicket(inTicket))
	{
		TSharedPtr<FTIHHsBBTicket> curBBTicket = GetBBTicketByHandle(inTicket);
		
		TArray<FTIHHsBBTicketElementForm> ticketElements;
		curBBTicket->GetTicketElementsForm(ticketElements);
	
		reValueBBTicketHandle = TryPublishBBTicket(inTargetSessionData, ticketElements);
	}

	return reValueBBTicketHandle;
}


void UTIHHsBlackBoardSubsystem::TestBlackBoardSessionCloseTypes()
{
	// 테스트용 공통 데이터
	TArray<FTIHHsBBTicketElementForm> testElements;
	testElements.Add(FTIHHsBBTicketElementForm(TEXT("TestInt"), ETIHHsBlackBoardDataType::EInt32, 1));

	UE_LOG(LogTemp, Warning, TEXT("========== Begin Session CloseType Test =========="));

	// 1. TickBase 테스트 (2틱 후 종료)
	{
		const FString tickSessionName = TEXT("TickSession");
		FTIHHsBBSessionData sessionData;
		sessionData.SessionType = ETIHHsBBSessionCloseType::ETickBase;
		sessionData.SessionName = tickSessionName;
		sessionData.SessionTickCount = 180;

		//FTIHHsBBTicket& tickTicket = TryPublishBBTicket(sessionData, testElements);
		//UE_LOG(LogTemp, Warning, TEXT("[TickBase] TicketID=%d, SessionID=%d"), tickTicket.TicketID, tickTicket.SessionID);
	}
	
	// 2. TimeBase 테스트 (3초 후 종료)
	{
		const FString timeSessionName = TEXT("TimeSession");
		UTIHHsBBSession* timeSession = CreateBBSessionInternal(
			timeSessionName,
			ETIHHsBBSessionCloseType::ETimeBase,
			0, // TickCount 안씀
			15.0f // 3초 후 자동 종료
		);

		TArray<FTIHHsBBTicketElementForm> timeElements;
		timeElements.Add(FTIHHsBBTicketElementForm(TEXT("TimeValue"), ETIHHsBlackBoardDataType::EFloat, 1));

		FTIHHsBlackBoard& bb = timeSession->GetBlackBoard();
		bb.Reserve(static_cast<int16>(ETIHHsBlackBoardDataType::EFloat), 1);
		
		UE_LOG(LogTemp, Warning, TEXT("[TimeBase] Created SessionID=%d, Will expire in 3 seconds."), timeSession->mSessionID);
	}

	UE_LOG(LogTemp, Warning, TEXT("========== End Session CloseType Test =========="));
}

void UTIHHsBlackBoardSubsystem::UpdateSession(const FString& inBBSessionName)
{
	
	
}

UTIHHsBBSession* UTIHHsBlackBoardSubsystem::GetBBSessionByName(const FString& inBBSessionName)
{
	UTIHHsBBSession* reValue = nullptr;
	if (mBlackBoardSessions.Contains(inBBSessionName))
	{
		reValue = mBlackBoardSessions[inBBSessionName];
	}
	return reValue;
}

UTIHHsBBSession* UTIHHsBlackBoardSubsystem::GetBBSessionById(const int16& inBBSessionID)
{
	UTIHHsBBSession* reValue = nullptr;
	if (mBlackBoardSessionIDs.Contains(inBBSessionID))
	{
		FString sessionName = mBlackBoardSessionIDs[inBBSessionID];
		reValue = GetBBSessionByName(sessionName);
	}
	checkf(mBlackBoardSessionIDs.Contains(inBBSessionID),TEXT("UTIHHsBlackBoardSubsystem::GetBBSessionById() - SessionID %d not found"),inBBSessionID);
	
	return reValue;
}



FTIHHsBBTicketWriter* UTIHHsBlackBoardSubsystem::WriteBegin(const FTIHHsBBTicketHandle& bbTicketHandle)
{
	bool expected = false;
	if (mWriteLock.CompareExchange(expected,true))
	{
		mCurrWriteTargetBBTicketHandle = bbTicketHandle;
		if (IsValidBBTicket(bbTicketHandle))
		{
			UTIHHsBBSession* bbSession = GetBBSessionById(mCurrWriteTargetBBTicketHandle.GetSessionID());
			mCurrBBTicketWriter = new FTIHHsBBTicketWriter(bbTicketHandle);
			if (not mCurrBBTicketWriter->IsAvailable())
			{
				UE_LOG(LogTemp, Error,TEXT("UTIHHsBlackBoardSubsystem::WriteBegin() - mCurrBBTicketWriter is not valid"));
				delete mCurrBBTicketWriter;
				mCurrBBTicketWriter = nullptr;
				mWriteLock.Store(false);
				return nullptr;
			}
			return mCurrBBTicketWriter;
		}
		else
		{
			mWriteLock.Store(false);
			UE_LOG(LogTemp, Error,TEXT("UTIHHsBlackBoardSubsystem::WriteBegin() - bbSession is nullptr"));
			return nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error,TEXT("UTIHHsBlackBoardSubsystem::WriteBegin() - mWriteLock is true"));
		//	TODO: 여기는 새롭게 다시 할지 아니면 그냥 에러처리를 할지인데, 그냥 에러로 판단
		return nullptr;
	}
}

bool UTIHHsBlackBoardSubsystem::WriteEnd(const FTIHHsBBTicketHandle& bbTicketHandle)
{
	if (mCurrWriteTargetBBTicketHandle.IsValid() && mCurrWriteTargetBBTicketHandle == bbTicketHandle)
	{
		if (mCurrBBTicketWriter != nullptr)
		{
			delete mCurrBBTicketWriter;
			mCurrBBTicketWriter = nullptr;
		}
		mCurrWriteTargetBBTicketHandle.SetInvalidate();
		mWriteLock.Store(false);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UTIHHsBlackBoardSubsystem::WriteEnd() - Attempted to end write without holding the lock or with mismatching ticket (Current: %s, Provided: %s)"),
			 *mCurrWriteTargetBBTicketHandle.ToString(), *bbTicketHandle.ToString());
		return false;
	}
}


void UTIHHsBlackBoardSubsystem::DeleteBBTicketBatchFunction()
{
	if (mBBTicketDeleteCount < mBBTicketDeleteQueue.Num())
	//if (mBBTicketDeleteCount < mBBTicketDeleteQueue_desperate.Num())
	{
		mWorld->GetTimerManager().SetTimer(mDeleteTicketTimerHandle, [this]()
		{
			int32 batchCount = 0;
			while (not mBBTicketDeleteQueue.IsEmpty() &&batchCount < mBBDeleteBatchCount )
			{
				FTIHHsBBTicketHandle deleteTicketHandle = mBBTicketDeleteQueue.First();
				mBBTicketDeleteQueue.PopFirst();
				
				if (deleteTicketHandle.IsValid())
				{
					if (mBBTicketHandleTable.Contains(deleteTicketHandle))
					{
						//	티켓테이블에서 삭제
						mBBTicketHandleTable.Remove(deleteTicketHandle);
					}
				}
				++batchCount;
			}
			if (mBBTicketDeleteQueue.IsEmpty())
			{
				mWorld->GetTimerManager().ClearTimer(mDeleteTicketTimerHandle);
			}
		}, mBBDeleteBatchTime, true);
	}
}

void UTIHHsBlackBoardSubsystem::MarkBBTicketForDelete(const FTIHHsBBTicketHandle& bbTicketHandle)
{
	TSharedPtr<FTIHHsBBTicket> targetTicketValue = GetBBTicketByHandle(bbTicketHandle);
	if (targetTicketValue == nullptr)
	{
		return;
	}
	UTIHHsBBSession* bbSession = GetBBSessionByBBTicketHandle(bbTicketHandle);
	if (not targetTicketValue->IsTicketExpired())
	{
		//	삭제전에 티켓을 변화시켜야 하기때문에 해당작업을 해준다. 물론 미리 expired되어있다면 문제 없음
		
		targetTicketValue->MakeExpired(bbSession);
	}
	
	//mBBTicketDeleteQueue_desperate.PushLast(targetTicketValue);
	mBBTicketDeleteQueue.PushLast(bbTicketHandle);
	if (mBBTicketBatchDeleteEnable)
	{
		DeleteBBTicketBatchFunction();
	}
	else
	{
		while (not mBBTicketDeleteQueue.IsEmpty())
		{
			FTIHHsBBTicketHandle deleteTicketHandle = mBBTicketDeleteQueue.First();
			mBBTicketDeleteQueue.PopFirst();
			
			if (deleteTicketHandle.IsValid())
			{
				if (mBBTicketHandleTable.Contains(deleteTicketHandle))
				{
					mBBTicketHandleTable.Remove(deleteTicketHandle);
				}
			}
		}
		
		if (mBBTicketBatchDeleteEnableBackUp == true)	//	이게 이전에 true 였다면 mBatchDeleteEnable 자체가 true 연단 말이므로 되돌려준다
		{
			ReturnBBTicketDeleteDirectly();
		}
	}
}

void UTIHHsBlackBoardSubsystem::DeleteBBSessionBatchFunction()
{
	if (mBBSessionDeleteCount < mBBSessionDeleteQueue.Num())
	{
		mWorld->GetTimerManager().SetTimer(mDeleteSessionTimerHandle, [this]()
		{
			int32 batchCount = 0;
			while (not mBBSessionDeleteQueue.IsEmpty() &&batchCount < mBBDeleteBatchCount )
			{
				UTIHHsBBSession* deleteSession = mBBSessionDeleteQueue.First();
				mBBSessionDeleteQueue.PopFirst();
				MarkAsGarbageSession(deleteSession);
				++batchCount;
			}
			
			if (mBBSessionDeleteQueue.IsEmpty())
			{
				mWorld->GetTimerManager().ClearTimer(mDeleteSessionTimerHandle);
				
			}
		}, mBBDeleteBatchTime, true,-0.1f);
	}
}

void UTIHHsBlackBoardSubsystem::MarkBBSessionForDelete(UTIHHsBBSession* bbSession)
{
	if (bbSession == nullptr)
	{
		return;
	}
	
	int32 sessionId = bbSession->GetSessionID();
	switch (bbSession->GetSessionType()) {
	case ETIHHsBBSessionCloseType::ETickBase:
		if (mBlackBoardSessionCloseTickCounts.Contains(sessionId))
		{
			mBlackBoardSessionCloseTickCounts.Remove(sessionId);
		}
		break;
	case ETIHHsBBSessionCloseType::ETimeBase:
		if (mBlackBoardSessionCloseTimers.Contains(sessionId))
		{
			if (mWorld->GetTimerManager().IsTimerActive(mBlackBoardSessionCloseTimers[sessionId]))
			{
				mWorld->GetTimerManager().ClearTimer(mBlackBoardSessionCloseTimers[sessionId]);
				mBlackBoardSessionCloseTimers.Remove(sessionId);
			}
		}
		break;
	case ETIHHsBBSessionCloseType::EReferenceCountBase:
		if (mBlackBoardSessionCloseReferenceCounts.Contains(sessionId))
		{
			mBlackBoardSessionCloseReferenceCounts.Remove(sessionId);
		}
		break;
	case ETIHHsBBSessionCloseType::EAlways:
		if (mBlackBoardSessionCloseAlwaysSet.Contains(sessionId))
		{
			mBlackBoardSessionCloseAlwaysSet.Remove(sessionId);
		}
		break;
	}
	
	mBBSessionDeleteQueue.PushLast(bbSession);
	bbSession->SetSessionState(ETIHHsBBSessionState::EMarkedForDestroy);
	if (mBBSessionBatchDeleteEnable)
	{
		DeleteBBSessionBatchFunction();
	}
	else
	{
		while (not mBBSessionDeleteQueue.IsEmpty())
		{
			UTIHHsBBSession* deleteSession = mBBSessionDeleteQueue.First();
			mBBSessionDeleteQueue.PopFirst();
			MarkAsGarbageSession(deleteSession);
		}
		if (mBBSessionBatchDeleteEnableBackUp == true)
		{
			ReturnBBSessionDeleteDirectly();
		}
	}
	
}

const TSharedPtr<FTIHHsBBTicketReader> UTIHHsBlackBoardSubsystem::ReadBlackBoard(
	const FTIHHsBBTicketHandle& bbTicketHandle)
{
	int32 bbSessionId = bbTicketHandle.GetSessionID();
	if (not IsValidBBSession(bbTicketHandle))
	{
		MarkBBTicketForDelete(bbTicketHandle);
		return nullptr;
	}
	//FTIHHsBBRWBase 
	UTIHHsBBSession* bbSession = GetBBSessionByBBTicketHandle(bbTicketHandle);
	
	TSharedPtr<FTIHHsBBTicketReader> newReader = MakeShared<FTIHHsBBTicketReader>(bbTicketHandle);
	if (not newReader->IsAvailable())
	{
		newReader.Reset();
		return nullptr;
	}
	mCurrBBTicketReaders.Add(newReader);
	bbSession->UseBBTicket(bbTicketHandle);
	
	return newReader;
}

void UTIHHsBlackBoardSubsystem::ProcessTickBase()
{
	TArray<int32> removeSessionIDs;
	for (TPair<int32, uint64>& tickBase :mBlackBoardSessionCloseTickCounts)
	{
		if (tickBase.Value == 0)
		{
			removeSessionIDs.Add(tickBase.Key);
		}
		tickBase.Value--;
	}
	for (const int32& sessionID : removeSessionIDs)
	{
		RemoveSession(sessionID);
	}
}

void UTIHHsBlackBoardSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ProcessTickBase();

	
}

void UTIHHsBlackBoardSubsystem::RegisterBBTicketCloseTickBase(const int32& inSessionID, const int32& inSessionTickCount)
{
	mBlackBoardSessionCloseTickCounts.Add(inSessionID,inSessionTickCount);
}

void UTIHHsBlackBoardSubsystem::RegisterBBTicketCloseTimeBase(const int32& inSessionID, const float& inSessionTime)
{
	FTimerHandle& newTimerHandle = mBlackBoardSessionCloseTimers.Add(inSessionID);
	UTIHHsBBSession* target = GetBBSessionById(inSessionID);
	mWorld->GetTimerManager().SetTimer(newTimerHandle,target,&UTIHHsBBSession::OnTimerExpired,inSessionTime,false);
}

void UTIHHsBlackBoardSubsystem::RegisterBBTicketCloseReferenceCountBase(const int32& inSessionID)
{
	mBlackBoardSessionCloseReferenceCounts.FindOrAdd(inSessionID) += 1;
}

void UTIHHsBlackBoardSubsystem::RegisterBBTicketCloseAlways(const int32& inSessionID)
{
	mBlackBoardSessionCloseAlwaysSet.Add(inSessionID);
}

void UTIHHsBlackBoardSubsystem::RemoveSession(int32 inSessionID)
{
	if (mBlackBoardSessionIDs.Contains(inSessionID))
	{
		FString sessionName = mBlackBoardSessionIDs[inSessionID];
		
		UTIHHsBBSession* session = (mBlackBoardSessions.Contains(sessionName)) ? mBlackBoardSessions[sessionName] : nullptr;
		if (session != nullptr)
		{
			session->SetSessionState(ETIHHsBBSessionState::EExpired);
			session->MakeExpiredTickets();
			
			session->PreDestroySession();
			mBlackBoardSessionIDs.Remove(inSessionID);
			mBlackBoardSessions.Remove(sessionName);
			MarkBBSessionForDelete(session);
		}
	}
}

void UTIHHsBlackBoardSubsystem::OnEvaluateCurrBBTicketReaders()
{
	if ( mCurrBBTicketReaders.Num() < mCurrBBTicketReadersBatchCount )
	{
		return;
	}
	for (int32 i = mCurrBBTicketReaders.Num() - 1; -1 < i  ; --i)
	{
		if (mCurrBBTicketReaders[i].IsValid() &&
			mCurrBBTicketReaders[i].Pin()->IsAvailable())
		{
			UTIHHsBBSession* bbSession = mCurrBBTicketReaders[i].Pin()->GetSession();
			TSharedPtr<FTIHHsBBTicket> bbTicket = mCurrBBTicketReaders[i].Pin()->GetTicket();
			if (bbSession && bbTicket.IsValid())
			{
				continue;
			}
		}
		mCurrBBTicketReaders.RemoveAt(i);
	}
}

void UTIHHsBlackBoardSubsystem::BeginDestroy()
{
	Super::BeginDestroy();

	mWriteLock.Store(false);
	
	if (mWorld)
	{
		mWorld->GetTimerManager().ClearTimer(mDeleteTicketTimerHandle);
		mWorld->GetTimerManager().ClearTimer(mDeleteSessionTimerHandle);
		for (auto target : mBlackBoardSessionCloseTimers)
		{
			if (mWorld->GetTimerManager().IsTimerActive(target.Value))
			{
				mWorld->GetTimerManager().ClearTimer(target.Value);
			}
		}
	}
	mBlackBoardSessionCloseTimers.Empty(0);
	mBlackBoardSessionCloseTickCounts.Empty(0);
	mBlackBoardSessionCloseReferenceCounts.Empty(0);
	mBlackBoardSessionCloseAlwaysSet.Empty(0);
	
	while (not mBBSessionDeleteQueue.IsEmpty())
	{
		UTIHHsBBSession* targetSession = mBBSessionDeleteQueue.First();
		mBBSessionDeleteQueue.PopFirst();
		if (targetSession == nullptr)
		{
			continue;
		}
		if (mBlackBoardSessionIDs.Contains(targetSession->GetSessionID()))
		{
			mBlackBoardSessionIDs.Remove(targetSession->GetSessionID());
		}
		if (mBlackBoardSessions.Contains(targetSession->GetBlackBoardName()))
		{
			mBlackBoardSessions.Remove(targetSession->GetBlackBoardName());
		}
		targetSession->PreDestroySession();
		MarkAsGarbageSession(targetSession);
	}
	mBBSessionDeleteQueue.Empty();
	
	for (TPair<FString, TObjectPtr<UTIHHsBBSession>>& session : mBlackBoardSessions)
	{
		if (session.Value == nullptr)
		{
			continue;
		}
		session.Value->PreDestroySession();
		UTIHHsBBSession* deleteSession = session.Value;
		MarkAsGarbageSession(deleteSession);
		session.Value = nullptr;
	}
	
	mBlackBoardSessionIDs.Empty(0);
	mBlackBoardSessions.Empty(0);
	
	if (mCurrBBTicketWriter != nullptr)
	{
		delete mCurrBBTicketWriter;
		mCurrBBTicketWriter = nullptr;
	}
	for (TWeakPtr<FTIHHsBBTicketReader>& readers :mCurrBBTicketReaders)
	{
		if (readers.IsValid() && readers.Pin().IsValid())
		{
			readers.Pin().Reset();
		}
	}
	mCurrBBTicketReaders.Empty(0);

	for (auto& ticketData:mBBTicketHandleTable)
	{
		if (ticketData.Value != nullptr)
		{
			ticketData.Value.Reset();
			ticketData.Value = nullptr;
		}
	}
	mBBTicketHandleTable.Empty();
	
}

TSharedPtr<FTIHHsBBTicket> UTIHHsBlackBoardSubsystem::GetBBTicketByHandle(const FTIHHsBBTicketHandle& inHandle)
{
	TSharedPtr<FTIHHsBBTicket> reValue = nullptr;
	if (mBBTicketHandleTable.Contains(inHandle))
	{
		reValue = mBBTicketHandleTable[inHandle];
	}
	return reValue;
}

TSharedPtr<FTIHHsBBTicket> UTIHHsBlackBoardSubsystem::GetBlackBoardTicketById(const int32& inSessionID, const int32& inTicketID)
{
	return GetBBTicketByHandle(FTIHHsBBTicketHandle(inSessionID,inTicketID));
}

bool UTIHHsBlackBoardSubsystem::IsValidBBSession(const FTIHHsBBTicketHandle& inTicketHandle)
{
	UTIHHsBBSession* bbSession = GetBBSessionByBBTicketHandle(inTicketHandle);

	//	세션이 없음
	if (bbSession == nullptr)
	{
		return false;
	}
	if (bbSession->GetSessionState() == static_cast<int32>(ETIHHsBBSessionState::EExpired) ||
		bbSession->GetSessionState() == static_cast<int32>(ETIHHsBBSessionState::EMarkedForDestroy) ||
		bbSession->GetSessionState() == static_cast<int32>(ETIHHsBBSessionState::EDestroying))
	{
		return false;
	}
	
	return true;
	
}

bool UTIHHsBlackBoardSubsystem::IsValidBBTicket(const FTIHHsBBTicketHandle& inTicketHandle)
{
	if (mBBTicketHandleTable.Contains(inTicketHandle) && IsValidBBSession(inTicketHandle))
	{
		TSharedPtr<FTIHHsBBTicket> targetTicketValue = GetBBTicketByHandle(inTicketHandle);
		
		if (targetTicketValue == nullptr)
		{
			return false;
		}
		if (not targetTicketValue.IsValid())
		{
			return false;
		}
		if ( targetTicketValue->IsTicketExpired())
		{
			return false;
		}
		
		return true;
	}
	return false;
}

void UTIHHsBlackBoardSubsystem::MarkAsGarbageSession(UTIHHsBBSession* targetSession)
{
	if (UTIHHsBBSession* deleteSession = targetSession)
	{
		deleteSession->ClearFlags(RF_BeginDestroyed);
		deleteSession->MarkAsGarbage();
		deleteSession = nullptr;
	}
}

void UTIHHsBlackBoardSubsystem::ExampleBBSessionCreate()
{
	UTIHHsBlackBoardSubsystem* bbSubsystem = UTIHHsBlackBoardSubsystem::GetBBSystem();
	FTIHHsBBSessionData exampleSessionData;
	exampleSessionData.SessionName = TEXT("TestSession");
	exampleSessionData.SessionType = ETIHHsBBSessionCloseType::EAlways;
	exampleSessionData.SessionTickCount = 0;
	exampleSessionData.SessionTime = 0.f;

	//	세션 생성은 무조건 이 함수를 통해서만 생성
	UTIHHsBBSession* exmapleCreateBBSession = bbSubsystem->CreateNewBBSession(exampleSessionData);
	{
		//	세션을 찾는 방법
		UTIHHsBBSession* exampleGetBBSessionByName = bbSubsystem->GetBBSessionByName(exampleSessionData.SessionName);
		UTIHHsBBSession* exampleGetBBSessionById = bbSubsystem->GetBBSessionById(exmapleCreateBBSession->GetSessionID());
	}

	//	ticket 생성
	{
		TArray<FTIHHsBBTicketElementForm> exampleElements;
		{
			FTIHHsBBTicketElementForm exampleElementForm0;
			exampleElementForm0.ElementName = TEXT("TestInt");
			exampleElementForm0.ElementType = ETIHHsBlackBoardDataType::EInt32;
			exampleElementForm0.ElementCount = 1;
			exampleElements.Add(exampleElementForm0);
	
			FTIHHsBBTicketElementForm exampleElementForm1;
			exampleElementForm1.ElementName = TEXT("TestInt");
			exampleElementForm1.ElementType = ETIHHsBlackBoardDataType::EInt32;
			exampleElementForm1.ElementCount = 1;
			exampleElements.Add(exampleElementForm1);
		}
		FTIHHsBBTicketHandle exampleTicketHandle = bbSubsystem->TryPublishBBTicket(exampleSessionData, exampleElements);
		
		//	티켓을 검증한다. IsValidBBTicket는 세션도 체크하고 티켓이 실제로 존재하는지와 만료되었는지 확인한다.
		if (bbSubsystem->IsValidBBTicket(exampleTicketHandle))
		{
			//	티켓벨류(실제 인스턴스)를 찾는 방법
			TSharedPtr<FTIHHsBBTicket> exampleBBTicket = bbSubsystem->GetBBTicketByHandle(exampleTicketHandle);
				
			FTIHHsBBTicketWriter* exampleWriter = bbSubsystem->WriteBegin(exampleTicketHandle);
			TTIHHsBBTicketElementHandleView<int, true> exampleWriteElement0 = exampleWriter->Elements<int32>();
			TTIHHsBBTicketElementWriter<int> exampleWriteElementArray = exampleWriteElement0[0];
			int& exampleWriteElementArrayValue = exampleWriteElementArray[0];
			/*
				type : [ element0,element1 ... ]
				elements : [ range0,range1 ... ]
				range : [ value0,value1 ... ]
				
				[ element0,element1 ... ] [ range0,range1 ... ] [ value0,value1 ... ]
				[ element0,element1 ... ] [ range0,range1 ... ] [ value0,value1 ... ]

			*/
			auto exampleWriteElement1 = exampleWriter->Elements<float>();			//	type
			for (auto exampleElementView : exampleWriteElement1)					//	elements
			{
				for (auto& exampleElementViewValue : exampleElementView)		//	range
				{
					exampleElementViewValue = 0.f;									//	value
				}
			}

			
			for (TTIHHsBBTicketElementWriter<int32> i :exampleWriteElement0)
			{
				for (auto j : i)
				{
					
				}
			}
			bbSubsystem->WriteEnd(exampleTicketHandle);
		}
			
	}
	
	
	
	
}

void UTIHHsBlackBoardSubsystem::ExampleBBTicketUse()
{
	
}

void ATIHHsBBRWTestActor::BeginPlay()
{
	Super::BeginPlay();

	
}

void ATIHHsBBRWTestActor::TestBlackBoardReadWrite()
{

}

void ATIHHsBBRWTestActor::TestBlackboardLifecycle()
{

}

void ATIHHsBBRWTestActor::TestBlackboardReadWriteAndTicket()
{
	UE_LOG(LogTemp, Log, TEXT("========== ATIHHsBBRWTestActor::TestBlackboardReadWriteAndTicket 시작 =========="));

	UTIHHsBlackBoardSubsystem* BBSubsystem = UTIHHsBlackBoardSubsystem::GetBBSystem();
	if (!BBSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("블랙보드 서브시스템을 가져올 수 없습니다."));
		return;
	}
	// 1. 세션 데이터 및 블랙보드 요소 정의
	FTIHHsBBSessionData SessionData;
	SessionData.SessionName = TEXT("TestReadWriteSession_Actor");
	SessionData.SessionType = ETIHHsBBSessionCloseType::EAlways; // 테스트를 위해 항상 유지되는 세션
	SessionData.SessionTickCount = 0;
	SessionData.SessionTime = 0.0f;

	TArray<FTIHHsBBTicketElementForm> ElementForms;
	ElementForms.Add(FTIHHsBBTicketElementForm(TEXT("MyInteger"), ETIHHsBlackBoardDataType::EInt32, 1));
	ElementForms.Add(FTIHHsBBTicketElementForm(TEXT("MyVectorArray"), ETIHHsBlackBoardDataType::EVector, 2)); // 2개의 FVector를 가지는 배열

	// 2. 티켓 발행
	FTIHHsBBTicketHandle TestTicketHandle = BBSubsystem->TryPublishBBTicket(SessionData, ElementForms);

	if (!TestTicketHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("티켓 발행에 실패했습니다."));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("티켓 발행 성공: %s"), *TestTicketHandle.ToString());


	// 3. 데이터 쓰기
	UE_LOG(LogTemp, Log, TEXT("데이터 쓰기 시작..."));
	FTIHHsBBTicketWriter* Writer = BBSubsystem->WriteBegin(TestTicketHandle);
	if (Writer)
	{
		// 정수형 데이터 쓰기
		TTIHHsBBTicketElementHandleView<int32, true> IntElements = Writer->Elements<int32>();
		if (IntElements.Num() > 0)
		{
			TTIHHsBBTicketElementWriter<int32> IntElementWriter = IntElements[0]; // 첫 번째 int32 요소
			if (IntElementWriter.Num() > 0)
			{
				IntElementWriter[0] = 12345;
				UE_LOG(LogTemp, Log, TEXT("MyInteger에 12345를 썼습니다."));
			}
		}

		// 벡터 배열 데이터 쓰기
		TTIHHsBBTicketElementHandleView<FVector, true> VectorElements = Writer->Elements<FVector>();
		if (VectorElements.Num() > 0)
		{
			TTIHHsBBTicketElementWriter<FVector> VectorElementWriter = VectorElements[0]; // 첫 번째 FVector 요소 (배열을 나타냄)
			if (VectorElementWriter.Num() >= 2) // 요소 개수가 2개 이상인지 확인
			{
				VectorElementWriter[0] = FVector(1.0f, 2.0f, 3.0f);
				VectorElementWriter[1] = FVector(4.0f, 5.0f, 6.0f);
				UE_LOG(LogTemp, Log, TEXT("MyVectorArray[0]에 (1,2,3)을, MyVectorArray[1]에 (4,5,6)을 썼습니다."));
			}
		}
		if (!BBSubsystem->WriteEnd(TestTicketHandle))
		{
			UE_LOG(LogTemp, Error, TEXT("데이터 쓰기 종료(WriteEnd)에 실패했습니다."));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("데이터 쓰기 성공."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WriteBegin에 실패하여 데이터 쓰기를 진행할 수 없습니다."));
	}
	UE_LOG(LogTemp, Log, TEXT("데이터 읽기 시작..."));
	const TSharedPtr<FTIHHsBBTicketReader> Reader = BBSubsystem->ReadBlackBoard(TestTicketHandle);
	if (Reader.IsValid() && Reader->IsAvailable())
	{
		// 정수형 데이터 읽기
		TTIHHsBBTicketElementHandleView<int> IntElementsReader = Reader->Elements<int32>();
		if (IntElementsReader.Num() > 0)
		{
			TTIHHsBBTicketElementReader<int> IntElementReader = IntElementsReader[0];
			if (IntElementReader.Num() > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("MyInteger 값 읽기: %d"), IntElementReader[0]);
			}
		}

		// 벡터 배열 데이터 읽기
		auto VectorElementsReader = Reader->Elements<FVector>();
		if (VectorElementsReader.Num() > 0)
		{
			TTIHHsBBTicketElementReader<UE::Math::TVector<double>> VectorElementReader = VectorElementsReader[0];
			if (VectorElementReader.Num() >= 2)
			{
				
				UE_LOG(LogTemp, Log, TEXT("MyVectorArray[0] 값 읽기: %s"), *VectorElementReader[0].ToString());
				UE_LOG(LogTemp, Log, TEXT("MyVectorArray[1] 값 읽기: %s"), *VectorElementReader[1].ToString());
			}
		}
		UE_LOG(LogTemp, Log, TEXT("데이터 읽기 성공."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ReadBlackBoard에 실패하여 데이터 읽기를 진행할 수 없습니다. (Reader가 유효하지 않거나 사용 불가능)"));
	}
	// 5. 티켓 제거
	// 주의: 티켓을 제거하면 Reader/Writer가 더 이상 유효하지 않게 됩니다.
	// 실제 사용 시에는 티켓 사용이 완전히 끝난 후에 제거해야 합니다.
	UE_LOG(LogTemp, Log, TEXT("티켓 제거 시도: %s"), *TestTicketHandle.ToString());
	BBSubsystem->MarkBBTicketForDelete(TestTicketHandle);
	UE_LOG(LogTemp, Log, TEXT("티켓 제거 요청 완료. (세션 타입에 따라 즉시 또는 배치 처리)"));

	// 6. 티켓 유효성 재확인 (선택 사항)
	if (BBSubsystem->IsValidBBTicket(TestTicketHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("MarkBBTicketForDelete 호출 후에도 티켓이 여전히 유효합니다. (배치 삭제 또는 Persist 티켓일 수 있음)"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("MarkBBTicketForDelete 호출 후 티켓이 유효하지 않습니다."));
	}
	// 세션 제거는 필요에 따라 호출 (여기서는 티켓만 제거)
	// if (UTIHHsBBSession* SessionToClose = BBSubsystem->GetBBSessionByBBTicketHandle(TestTicketHandle))
	// {
	//     UE_LOG(LogTemp, Log, TEXT("세션 제거 시도: %s"), *SessionToClose->GetBlackBoardName());
	//     BBSubsystem->MarkBBSessionForDelete(SessionToClose);
	// }

	UE_LOG(LogTemp, Log, TEXT("========== ATIHHsBBRWTestActor::TestBlackboardReadWriteAndTicket 종료 =========="));
	
}

