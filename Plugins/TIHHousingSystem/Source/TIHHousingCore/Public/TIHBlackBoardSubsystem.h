// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TIHHsBlackboard.h"
#include "TIHHsCore.h"
#include "TIHBlackBoardSubsystem.generated.h"


class FTIHHsBBTicketTicketReader;
class FTIHHsBBTicketTicketWriter;
/**
 * 
 */
UCLASS()
class UTIHHsBlackBoardSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	static UTIHHsBlackBoardSubsystem* gBlackBoardSubsystem;
public:
	static UTIHHsBlackBoardSubsystem* GetBBSystem()
	{
		return gBlackBoardSubsystem;
	}
	
	void InitializeDefaultSizes();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	

	UTIHHsBlackBoardSession* GetSessionByTicket(FTIHHsBBTicket& bbTicket);
	
	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|PublishTicket")
	FTIHHsBBTicket& TryPublishBBTicket(const FTIHHsBBSessionData& inSessionData,  const TArray<FTIHHsBlackBoardElementForm>& inReserveBBData);

	FTIHHsBBTicketTicketReader* UseBlackBoardTicket(FTIHHsBBTicket& bbTicket);	
	
	
	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Test")
	void TestBlackBoardSessionCloseTypes();

	

	void UpdateSession(const FString& inBBSessionName);
	UTIHHsBlackBoardSession* GetBlackBoardSessionByName(const FString& inBBSessionName);
	UTIHHsBlackBoardSession* GetBlackBoardSessionById(const int16& inBBSessionID);

	static int32 GetBBSessionID()
	{
		if ( gBlackBoardSessionID ==INT32_MAX )
		{
			gBlackBoardSessionID = 0;
		}
		return gBlackBoardSessionID;
	}
	static void AdvanceBBSessionID()
	{
		if ( gBlackBoardSessionID ==INT32_MAX )
		{
			gBlackBoardSessionID = 0;
		}
		else
		{
			gBlackBoardSessionID++;
		}
	}
	
	FTIHHsBBTicketTicketWriter* WriteBegin(FTIHHsBBTicket& bbTicket);
	bool WriteEnd(const FTIHHsBBTicket& bbTicket);

	const FTIHHsBBTicketTicketReader * ReadBlackBoard(FTIHHsBBTicket& bbTicket);
	void ProcessTickBase();

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UTIHHsBlackBoardSubsystem, STATGROUP_Tickables);
	}
	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Conditional;
	}

	virtual bool IsTickable() const override
	{
		return UTIHHsGlobalConfigure::IsTickEnable(mWorld);
	}

	virtual bool IsTickableInEditor() const override
	{
#ifdef WITH_EDITOR
		return mTickInEditorActive;
#else
		return UTIHHsGlobalConfigure::IsTickEnable(mWorld);
#endif
	
	}

	virtual bool IsTickableWhenPaused() const override
	{
		return UTIHHsGlobalConfigure::IsTickEnable(mWorld);
	}

	virtual void Tick(float DeltaTime) override;
	void SessionTimerExpired(UTIHHsBlackBoardSession* targetSession);

private:
	//	이거는 에러상황이 나오면 안된다. 즉 무조건 제작은 해주지만 겹치는 이름에 관해서는 제거해줘야한다.
	//	사실 지금 id 로 작동시키고 있기에 이게 맞나 싶긴한데 일단은 무조건 성공을 목표로해준다.
	//	실패도 많이 만들면 안된다.
	UTIHHsBlackBoardSession* CreateBBSession(FString sessionName, ETIHHsBBSessionCloseType sessionType, int32 sessionTickCount, float sessionTime);
	void RegisterBBSession(FString sessionName, UTIHHsBlackBoardSession* newSession);
	
	void RegisterBBTicketCloseTickBase(const int32& inSessionID,const int32& inSessionTickCount);
	void RegisterBBTicketCloseTimeBase(const int32& inSessionID,const float& inSessionTime);
	void RegisterBBTicketCloseReferenceCountBase(const int32& inSessionID);
	void RegisterBBTicketCloseAlways(const int32& inSessionID);

	void RemoveSession(int32 inSessionID);

	
	bool mWriteLock;
	static int32 gBlackBoardSessionID;
	FTIHHsBBTicket* mCurrBBTicket;
	int32 mCurrSessionID;
	int32 mCurrentTicketID;
	
	TMap<FString, UTIHHsBlackBoardSession*> mBlackBoardSessions;	//	
	TMap<int32,FString> mBlackBoardSessionIDs;

	FTIHHsBBTicketTicketWriter* mCurrBBTicketWriter;
	TArray<FTIHHsBBTicketTicketReader*> mCurrBBTicketReaders;

	TMap<int32,FTimerHandle> mBlackBoardSessionCloseTimers;
	TMap<int32,uint64> mBlackBoardSessionCloseTickCounts;	//	0을 먼저 제거하고 카운트 감소
	TMap<int32,int32> mBlackBoardSessionCloseReferenceCounts;	//	0을 먼저 제거하고 카운트 감소
	TSet<int32> mBlackBoardSessionCloseAlwaysSet;	//	

	
	uint64 mBlackBoardSessionCloseTickCount;
	
	UWorld* mWorld;

	bool mTickInEditorActive;
	
};

inline void UTIHHsBlackBoardSubsystem::SessionTimerExpired(UTIHHsBlackBoardSession* targetSession)
{
	//	여기에서 연결을 끊어줘야한다.
	int32 sessionId = targetSession->GetSessionID();
	mBlackBoardSessionCloseTimers.Remove(sessionId);
	RemoveSession(sessionId);
}

