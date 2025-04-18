// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHBlackBoardSubsystem.h"
#include "TIHHsBlackboard.inl"

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
	
}



void UTIHHsBlackBoardSubsystem::RegisterBBSession(FString sessionName, UTIHHsBlackBoardSession* newSession)
{
	mBlackBoardSessions.Add(sessionName,newSession);
	mBlackBoardSessionIDs.Add(newSession->mSessionID,sessionName);
}

UTIHHsBlackBoardSession*  UTIHHsBlackBoardSubsystem::CreateBBSession(FString sessionName, ETIHHsBBSessionCloseType sessionType, int32 sessionTickCount, float sessionTime)
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
	
	UTIHHsBlackBoardSession* newSession = (NewObject<UTIHHsBlackBoardSession>(GetTransientPackage()));
		
	newSession->mSessionID = UTIHHsBlackBoardSubsystem::GetBBSessionID();
	UTIHHsBlackBoardSubsystem::AdvanceBBSessionID();
	newSession->mSessionState = 0;
		
	newSession->mSessionData.SessionType = sessionType;
	newSession->mSessionData.SessionName = sessionName;
	newSession->mSessionData.SessionTime = sessionTime;
	newSession->mSessionData.SessionTickCount = sessionTickCount;
		
	RegisterBBSession(sessionName, newSession);
		
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
	newSession->PostCreateSession();
	
	return newSession;
}

FTIHHsBBTicket& UTIHHsBlackBoardSubsystem::TryPublishBBTicket(const FTIHHsBBSessionData& inSessionData,
                                                                               const TArray<FTIHHsBlackBoardElementForm>& inReserveBBData)
{
	UTIHHsBlackBoardSession* curBBSession;

	//	TODO: 꼭 분리
	//	makeSeesion 
	FString sessionName = inSessionData.SessionName;
	ETIHHsBBSessionCloseType sessionType = inSessionData.SessionType;
	int32 sessionTickCount = inSessionData.SessionTickCount;
	float sessionTime = inSessionData.SessionTime;

	if (mBlackBoardSessions.Contains(sessionName))
	{
		curBBSession = mBlackBoardSessions[sessionName];
	}
	else
	{
		curBBSession = CreateBBSession(sessionName, sessionType, sessionTickCount, sessionTime);
	}
	
	FTIHHsBlackBoard& targetBB = curBBSession->GetBlackBoard();

	//	for reserve
	TMap<int16,int32> elementCountMap;
	for (const FTIHHsBlackBoardElementForm& elementForm : inReserveBBData)
	{
		if (elementForm.ElementType != (ETIHHsBlackBoardDataType::EInvalid))
		{
			elementCountMap.FindOrAdd(static_cast<int16>(elementForm.ElementType)) += elementForm.ElementCount;
		}
	}
	for (const TPair<int16, int32>& reseve : elementCountMap)
	{
		targetBB.Reserve(reseve.Key,reseve.Value);
	}
	FTIHHsBBTicket* newTicket = new FTIHHsBBTicket();
	
	newTicket->TicketElements.Get().Reserve(inReserveBBData.Num());
	const int16 invalid = static_cast<int16>(ETIHHsBlackBoardDataType::EInvalid);
	for (const FTIHHsBlackBoardElementForm& elementForm : inReserveBBData)
	{
		int16 elementType = static_cast<int16>(elementForm.ElementType);
		if (elementType != invalid)
		{
			int32 elementIdx = newTicket->TicketElements.Get().AddDefaulted();
			//	여기서부터는 실제로 데이터를 넣는것
			int16 arrayOffset = targetBB.AddDefaulted(elementType,elementForm.ElementCount);
			newTicket->TicketElements.Get()[elementIdx].SetDataType(static_cast<uint16>(elementType));
			newTicket->TicketElements.Get()[elementIdx].SetDataCount(elementForm.ElementCount);
			newTicket->TicketElements.Get()[elementIdx].SetDataSize(FTIHHsDefaultTypeTraitHelper::GetTypeSize(elementType));
			newTicket->TicketElements.Get()[elementIdx].SetDataOffset(arrayOffset);
		}
	}
	curBBSession->RegisterBlackBoardTicket(newTicket);
	
	return *newTicket;
}

FTIHHsBBTicketTicketReader* UTIHHsBlackBoardSubsystem::UseBlackBoardTicket(FTIHHsBBTicket& bbTicket)
{
	//	여기서부터는 실제로 데이터를 넣는것
	FTIHHsBBTicketTicketReader* ticketReader = new FTIHHsBBTicketTicketReader(bbTicket);
	if (ticketReader->IsValid())
	{
		UTIHHsBlackBoardSession* bbSession = GetSessionByTicket(bbTicket);
		if (bbSession)
		{
			
		}
		mCurrBBTicketReaders.Add(ticketReader);
		return ticketReader;
	}
	else
	{
		delete ticketReader;
		return nullptr;
	}
}


void UTIHHsBlackBoardSubsystem::TestBlackBoardSessionCloseTypes()
{
	// 테스트용 공통 데이터
	TArray<FTIHHsBlackBoardElementForm> testElements;
	testElements.Add(FTIHHsBlackBoardElementForm(TEXT("TestInt"), ETIHHsBlackBoardDataType::EInt32, 1));

	UE_LOG(LogTemp, Warning, TEXT("========== Begin Session CloseType Test =========="));

	// 1. TickBase 테스트 (2틱 후 종료)
	{
		const FString tickSessionName = TEXT("TickSession");
		FTIHHsBBSessionData sessionData;
		sessionData.SessionType = ETIHHsBBSessionCloseType::ETickBase;
		sessionData.SessionName = tickSessionName;
		sessionData.SessionTickCount = 180;

		FTIHHsBBTicket& tickTicket = TryPublishBBTicket(sessionData, testElements);
		UE_LOG(LogTemp, Warning, TEXT("[TickBase] TicketID=%d, SessionID=%d"), tickTicket.TicketID, tickTicket.SessionID);
	}
	
	// 2. TimeBase 테스트 (3초 후 종료)
	{
		const FString timeSessionName = TEXT("TimeSession");
		UTIHHsBlackBoardSession* timeSession = CreateBBSession(
			timeSessionName,
			ETIHHsBBSessionCloseType::ETimeBase,
			0, // TickCount 안씀
			15.0f // 3초 후 자동 종료
		);

		TArray<FTIHHsBlackBoardElementForm> timeElements;
		timeElements.Add(FTIHHsBlackBoardElementForm(TEXT("TimeValue"), ETIHHsBlackBoardDataType::EFloat, 1));

		FTIHHsBlackBoard& bb = timeSession->GetBlackBoard();
		bb.Reserve(static_cast<int16>(ETIHHsBlackBoardDataType::EFloat), 1);
		
		UE_LOG(LogTemp, Warning, TEXT("[TimeBase] Created SessionID=%d, Will expire in 3 seconds."), timeSession->mSessionID);
	}

	UE_LOG(LogTemp, Warning, TEXT("========== End Session CloseType Test =========="));
}

void UTIHHsBlackBoardSubsystem::UpdateSession(const FString& inBBSessionName)
{
	
	
}

UTIHHsBlackBoardSession* UTIHHsBlackBoardSubsystem::GetBlackBoardSessionByName(const FString& inBBSessionName)
{
	UTIHHsBlackBoardSession* reValue = nullptr;
	if (mBlackBoardSessions.Contains(inBBSessionName))
	{
		reValue = mBlackBoardSessions[inBBSessionName];
	}
	return reValue;
}

UTIHHsBlackBoardSession* UTIHHsBlackBoardSubsystem::GetBlackBoardSessionById(const int16& inBBSessionID)
{
	UTIHHsBlackBoardSession* reValue = nullptr;

	if (mBlackBoardSessionIDs.Contains(inBBSessionID))
	{
		FString sessionName = mBlackBoardSessionIDs[inBBSessionID];
		reValue = GetBlackBoardSessionByName(sessionName);
	}

	return reValue;
}

FTIHHsBBTicketTicketWriter* UTIHHsBlackBoardSubsystem::WriteBegin(FTIHHsBBTicket& bbTicket)
{
	if (not mWriteLock)
	{
		mCurrBBTicket = &bbTicket;
		mCurrSessionID = mCurrBBTicket->SessionID;
		mCurrentTicketID = mCurrBBTicket->TicketID;

		UTIHHsBlackBoardSession* bbSession = GetBlackBoardSessionById(mCurrSessionID);
		if (bbSession)
		{
			mCurrBBTicketWriter = new FTIHHsBBTicketTicketWriter(bbTicket);
			if (not mCurrBBTicketWriter->IsValid())
			{
				delete mCurrBBTicketWriter;
				mCurrBBTicketWriter = nullptr;
				return nullptr;
			}
		}
		return mCurrBBTicketWriter;
	}
	else
	{
		//	TODO: 여기는 새롭게 다시 할지 아니면 그냥 에러처리를 할지인데, 그냥 에러로 판단
		return nullptr;
	}
}

bool UTIHHsBlackBoardSubsystem::WriteEnd(const FTIHHsBBTicket& bbTicket)
{
	if (mWriteLock)
	{
		mWriteLock = false;
		if (mCurrBBTicketWriter != nullptr)
		{
			delete mCurrBBTicketWriter;
			mCurrBBTicketWriter = nullptr;
		}
		return true;
	}
	else
	{
		return false;
	}
}

const FTIHHsBBTicketTicketReader* UTIHHsBlackBoardSubsystem::ReadBlackBoard(FTIHHsBBTicket& bbTicket) 
{
	int32 sessionId = bbTicket.SessionID;
	UTIHHsBlackBoardSession* bbSession = GetBlackBoardSessionById(sessionId);	//	세션아이디는 만들때 부여
	if (bbSession== nullptr)
	{
		//	볼것도 없음
		return nullptr;
	}
	/*
	 *	session이 끝났는지 확인해줘야함
	 * 
	 */
	FTIHHsBBTicketTicketReader* newReader = new FTIHHsBBTicketTicketReader(bbTicket);
	if (newReader->IsValid())
	{
		mCurrBBTicketReaders.Add(newReader);
		return newReader;
	}
	else
	{
		delete newReader;
		return nullptr;
	}
	
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
		mBlackBoardSessionCloseTickCounts.Remove(sessionID);
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
	UTIHHsBlackBoardSession* target = GetBlackBoardSessionById(inSessionID);
	mWorld->GetTimerManager().SetTimer(newTimerHandle,target,&UTIHHsBlackBoardSession::OnTimerExpired,inSessionTime,false);
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
		
		UTIHHsBlackBoardSession* session = (mBlackBoardSessions.Contains(sessionName)) ? mBlackBoardSessions[sessionName] : nullptr;
		if (session != nullptr)
		{
			session->PreDestroySession();
			mBlackBoardSessionIDs.Remove(inSessionID);
			mBlackBoardSessions.Remove(sessionName);
		}
	}
	
}

