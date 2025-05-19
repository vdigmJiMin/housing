// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TIHHsBlackboard.h"
#include "TIHHsCore.h"
#include "Containers/Deque.h"
#include "TIHBlackBoardSubsystem.generated.h"


class FTIHHsBBReader_desperate;
class FTIHHsBBWriter_desperate;
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
	//	Create Ticket
	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|PublishTicket")
	FTIHHsBBTicketHandle TryPublishBBTicket(const FTIHHsBBSessionData& inSessionData,  const TArray<FTIHHsBBTicketElementForm>& inReserveBBData);
	
	
	//	Copy Ticket: other ticket address, same id
	FTIHHsBBTicketHandle TryCopyBBTicket(const FTIHHsBBTicketHandle& inTicket);
	//	Inherit Ticket: other ticket address, other id, and inherit data
	FTIHHsBBTicketHandle TryInheritBBTicket(const FTIHHsBBTicketHandle& inTicket, const TArray<FTIHHsBBTicketElementForm>& inReserveBBData);
	//	Migrate Ticket: other ticket address, other id, and migrate data, other session
	FTIHHsBBTicketHandle TryMigrateBBTicket(const FTIHHsBBTicketHandle& inTicket,const FTIHHsBBSessionData& inTargetSessionData); 
	
	//	read and use Ticket
	const TSharedPtr<FTIHHsBBTicketReader> ReadBlackBoard(const FTIHHsBBTicketHandle& bbTicketHandle);
	
	//	update Ticket
	FTIHHsBBTicketWriter* WriteBegin(const FTIHHsBBTicketHandle& bbTicketHandle);
	bool WriteEnd(const FTIHHsBBTicketHandle& bbTicketHandle);

	//	삭제 처리에 대한 설정함수
	UFUNCTION()
	void SetBBTicketDeleteDirectly(){mBBTicketBatchDeleteEnableBackUp = mBBTicketBatchDeleteEnable;mBBTicketBatchDeleteEnable = false;}
	UFUNCTION()
	void SetBBSessionDeleteDirectly(){	mBBSessionBatchDeleteEnableBackUp = mBBSessionBatchDeleteEnable;mBBSessionBatchDeleteEnable = false;}
	
	void ReturnBBTicketDeleteDirectly()	{mBBTicketBatchDeleteEnable = mBBTicketBatchDeleteEnableBackUp;	mBBTicketBatchDeleteEnableBackUp = false;	}
	void ReturnBBSessionDeleteDirectly(){mBBSessionBatchDeleteEnable = mBBSessionBatchDeleteEnableBackUp;mBBSessionBatchDeleteEnableBackUp = false;	}
	
	//	무조건 지우는 함수
	void MarkBBTicketForDelete(const FTIHHsBBTicketHandle& bbTicketHandle);
	void MarkBBSessionForDelete(UTIHHsBBSession* bbSession);

	//	배치로 지우는 함수
	void DeleteBBTicketBatchFunction();
	void DeleteBBSessionBatchFunction();

	//	기본 함수들
	void InitializeDefaultSizes();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	//	세션들고오기
	UTIHHsBBSession* GetBBSessionByName(const FString& inBBSessionName);
	UTIHHsBBSession* GetBBSessionById(const int16& inBBSessionID);
	UTIHHsBBSession* GetBBSessionByBBTicketHandle(const FTIHHsBBTicketHandle& BBTicketHandle);

	//	도움함수
	TArray<FTIHHsBBTicketElementForm> ConvertToBlackBoardElementForm(const TArray<FTIHHsBBTicketElement>& inTicketElements);

	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Test")
	void TestBlackBoardSessionCloseTypes();

	void UpdateSession(const FString& inBBSessionName);

	static int32 GetCurrentNewBBSessionID(){if ( gBlackBoardSessionID ==INT32_MAX )	{gBlackBoardSessionID = 0;	}	return gBlackBoardSessionID;}
	static void AdvanceCurrentNewBBSessionID()
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
	void SessionTimerExpired(UTIHHsBBSession* targetSession);

	UTIHHsBBSession* CreateNewBBSession(const FTIHHsBBSessionData& inSessionData)
	{
		return CreateBBSessionInternal(inSessionData.SessionName,inSessionData.SessionType,inSessionData.SessionTickCount,inSessionData.SessionTime);
	}

	static void RegisterBBTicketHandleInBBSession( TObjectPtr<UTIHHsBBSession> curBBSession, FTIHHsBBTicketHandle newTicketHandle);

private:
	UTIHHsBBSession* CreateBBSessionInternal(FString sessionName, ETIHHsBBSessionCloseType sessionType, int32 sessionTickCount, float sessionTime);
	
	void RegisterBBSessionNameTable(FString sessionName, UTIHHsBBSession* newSession);
	
	void RegisterBBTicketCloseTickBase(const int32& inSessionID,const int32& inSessionTickCount);
	void RegisterBBTicketCloseTimeBase(const int32& inSessionID,const float& inSessionTime);
	void RegisterBBTicketCloseReferenceCountBase(const int32& inSessionID);
	void RegisterBBTicketCloseAlways(const int32& inSessionID);

	void RemoveSession(int32 inSessionID);

	void OptimizationBBTicketElements(FTIHHsBBTicket& inTicket);

	static TSharedPtr<FTIHHsBBTicket> CreateBBTicket(TObjectPtr< UTIHHsBBSession> inSession,const TArray<FTIHHsBBTicketElementForm>& inReserveBBData);
	

	
	//bool mWriteLock;
	TAtomic<bool> mWriteLock{false};
	
	static int32 gBlackBoardSessionID;
	FTIHHsBBTicket* mCurrWriteTargetBBTicket;
	FTIHHsBBTicketHandle mCurrWriteTargetBBTicketHandle;
	int32 mCurrWriteTargetSessionID;
	int32 mCurrentWriteTargetTicketID;
	
	UPROPERTY()
	TMap<FString, TObjectPtr<UTIHHsBBSession>> mBlackBoardSessions;	//
	
	TMap<int32,FString> mBlackBoardSessionIDs;

	FTIHHsBBTicketWriter* mCurrBBTicketWriter;
	TArray<TWeakPtr<FTIHHsBBTicketReader>> mCurrBBTicketReaders;
	
	FTimerHandle mCurrBBTicketReadersTimerHandle;
	float mCurrBBTicketReadersTimerLoopTime;
	int32 mCurrBBTicketReadersBatchCount;
	
	void OnEvaluateCurrBBTicketReaders();

public:
	virtual void BeginDestroy() override;


	//	0524
	TSharedPtr<FTIHHsBBTicket> GetBBTicketByHandle(const FTIHHsBBTicketHandle& inHandle);
	//	//	0524
	TSharedPtr<FTIHHsBBTicket> GetBlackBoardTicketById(const int32& inSessionID, const int32& inTicketID);
	

	//	단순하게 세션만 확인	0524
	bool IsValidBBSession(const FTIHHsBBTicketHandle& inTicketHandle);
	//	당연하지만 세션도 티켓을 모두 확인 0524
	bool IsValidBBTicket(const FTIHHsBBTicketHandle& inTicketHandle);
	
	
private:
	//	reference by mBlackBoardSessions
	TMap<int32,FTimerHandle> mBlackBoardSessionCloseTimers;
	TMap<int32,uint64> mBlackBoardSessionCloseTickCounts;	//	0을 먼저 제거하고 카운트 감소
	TMap<int32,int32> mBlackBoardSessionCloseReferenceCounts;	//	0을 먼저 제거하고 카운트 감소
	TSet<int32> mBlackBoardSessionCloseAlwaysSet;	//	

	UPROPERTY()
	UWorld* mWorld;

	bool mTickInEditorActive;

	bool mBBTicketBatchDeleteEnableBackUp;	//	todo: config
	bool mBBTicketBatchDeleteEnable;		//	todo: config	//	처음부터 false 로 해두면 항상 false임

	bool mBBSessionBatchDeleteEnableBackUp;	//	todo: config
	bool mBBSessionBatchDeleteEnable;		//	todo: config
	
	int32 mBBTicketDeleteCount;				//	todo: config
	int32 mBBSessionDeleteCount;			//	todo: config
	
	int32 mBBDeleteBatchCount;	//	64		//	todo: config
	float mBBDeleteBatchTime;	//	0.06666f//	todo: config
	
	FTimerHandle mDeleteTicketTimerHandle;
	FTimerHandle mDeleteSessionTimerHandle;
	

	TDeque<FTIHHsBBTicketHandle> mBBTicketDeleteQueue;
	
	TDeque<UTIHHsBBSession*> mBBSessionDeleteQueue;

	static void MarkAsGarbageSession(UTIHHsBBSession* targetSession);

	//	caution : 오로지 서브시스템에서 뎅글링을 없애기위한것이다. 물론 서브시스템에서
	//TSet<FTIHHsBBTicket*> mBlackBoardTicketsAllList_desparate;
	
	TMap<FTIHHsBBTicketHandle,TSharedPtr<FTIHHsBBTicket>> mBBTicketHandleTable;

	void ExampleBBSessionCreate();
	void ExampleBBTicketUse();

	//	세션검증,티켓검증 후 공통으로 해줘야하는 애들의 경우에 이곳에 넣고 공통으로 검증해주는게 좋을거같은데 어디서는 제거해주고 어디서는 상태만 변경하고 그러는걸 일관성을 맞춰야하는데 쉽지 않네.
	//	근데 세션검증에 실패했으면 그냥 일단 넣고 생각하는것도 나쁘지 않을듯?
	TSet<FTIHHsBBTicketHandle> mVertiiedCandidateTicketHandles;
	//	expired 인지 expired라 제거해야한다면 자동제거
	//void VertifyTicketHandle(FTIHHsBBTicketHandle inTicketHandle);
	
};



inline void UTIHHsBlackBoardSubsystem::SessionTimerExpired(UTIHHsBBSession* targetSession)
{
	//	여기에서 연결을 끊어줘야한다.
	int32 sessionId = targetSession->GetSessionID();
	mBlackBoardSessionCloseTimers.Remove(sessionId);
	RemoveSession(sessionId);
}

UCLASS()
class ATIHHsBBRWTestActor : public AActor
{
	GENERATED_BODY()

public:
	
virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable,CallInEditor)
	void TestBlackBoardReadWrite();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void TestBlackboardLifecycle();

	// 블랙보드 읽기, 쓰기, 티켓 제거 테스트 함수 Blackboard Read, Write, Ticket Removal Test Function
	UFUNCTION(BlueprintCallable, CallInEditor)
	void TestBlackboardReadWriteAndTicket();
};

