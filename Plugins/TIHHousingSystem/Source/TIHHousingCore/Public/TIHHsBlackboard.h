// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Templates/TypeHash.h"
#include "TIHHsBlackboardDefines.h"
#include "TIHHsBlackboard.generated.h"

struct FTIHHsBBTicketElementForm;
struct FTIHHsBBTicketHandle;
struct FTIHHsBBTicketElement;
struct FTIHHsBBTicket;

/*
	 *	세션종료에 대한 타입들
	 *	잠깐 있는것 : temp 틱카운트에 달린것
	 *	EReserve 이거 어떻게 하지? 이거 지우든가
	 *	EReference 이거는 레퍼런스 카운트가 0 이되면 사라짐
	 *	EAlways 이거는 항상 존재하는것
	 *	TODO
	 *	추가해야할것, 특정 오브젝트의 라이프 사이클에 의존한다.
	 *	특정 이벤트에 의존한다
	*/
UENUM()
enum class ETIHHsBBSessionCloseType : uint8
{
	
	ETickBase,
	ETimeBase,
	EReferenceCountBase,
	EAlways
};

UENUM()
enum class ETIHHsBlackBoardProcessorType : uint8
{
	EKnown,
	ETicket,
	ETicketElement,
};

USTRUCT(BlueprintType)
struct FTIHHsBBTicketElement
{
	GENERATED_BODY()

	static constexpr int64 DataTypeMask = 0xFFFF000000000000;
	static constexpr int64 DataOffsetMask = 0x0000FFFF00000000;
	static constexpr int64 DataCountMask = 0x00000000FFFF0000;
	static constexpr int64 DataSizeMask = 0x000000000000FFFF;

	static constexpr int64 DataTypeShift = 48;
	static constexpr int64 DataOffsetShift = 32;
	static constexpr int64 DataCountShift = 16;
	static constexpr int64 DataSizeShift = 0;

	static FTIHHsBBTicketElement& GetDefault()
	{
		static FTIHHsBBTicketElement Default;
		return Default;
	}
	
	UPROPERTY()
	int64 PackagedData;

	FTIHHsBBTicketElement() = default;

	FTIHHsBBTicketElement(int64 InPackagedData)
		: PackagedData(InPackagedData)
	{
	}

	FTIHHsBBTicketElement(uint16 InDataType, uint16 InDataOffset, uint16 InDataCount, uint16 InDataSize)
		: PackagedData(0)
	{
		PackagedData = (int64(InDataType) << DataTypeShift) | (int64(InDataOffset) << DataOffsetShift) | (
			int64(InDataCount) << DataCountShift) | int64(InDataSize);
	}

	uint16 GetDataType() const
	{
		return (PackagedData & DataTypeMask) >> DataTypeShift;
	}

	uint16 GetDataOffset() const
	{
		return (PackagedData & DataOffsetMask) >> DataOffsetShift;
	}
	uint16 GetDataBeginOffset() const
	{
		return (PackagedData & DataOffsetMask) >> DataOffsetShift;
	}
	//	not index, but offset
	uint16 GetDataEndOffset() const
	{
		return ((PackagedData & DataOffsetMask) >> DataOffsetShift) + GetDataSize();
	}

	uint16 GetDataCount() const
	{
		return (PackagedData & DataCountMask) >> DataCountShift;
	}

	uint16 GetDataSize() const
	{
		return (PackagedData & DataSizeMask) >> DataSizeShift;
	}

	void SetDataType(uint16 InDataType)
	{
		PackagedData = (PackagedData & ~DataTypeMask) | (int64(InDataType) << DataTypeShift);
	}

	void SetDataOffset(uint16 InDataOffset)
	{
		PackagedData = (PackagedData & ~DataOffsetMask) | (int64(InDataOffset) << DataOffsetShift);
	}

	void SetDataCount(uint16 InDataCount)
	{
		PackagedData = (PackagedData & ~DataCountMask) | (int64(InDataCount) << DataCountShift);
	}

	void SetDataSize(uint16 InDataSize)
	{
		PackagedData = (PackagedData & ~DataSizeMask) | (int64(InDataSize) << DataSizeShift);
	}
};
USTRUCT(BlueprintType)
struct FTIHHsBBTicketState
{
	GENERATED_BODY()
};
USTRUCT(BlueprintType)
struct FTIHHsBBTicketElementNamedView
{
	using BBTicketElementNamedViewType = int32;
	static constexpr BBTicketElementNamedViewType ElementTypeMask = 0xFFFF0000;
	static constexpr BBTicketElementNamedViewType ElementIndexMask = 0xFFFF;


	static constexpr BBTicketElementNamedViewType ElementTypeShift = 16;
	static constexpr BBTicketElementNamedViewType ElementIndexShift = 0;

	
	GENERATED_BODY()
	FTIHHsBBTicketElementNamedView():
		PackagedData(-1)
	{
	};

	BBTicketElementNamedViewType PackagedData;
	
	FTIHHsBBTicketElementNamedView(int16 InElementType, int16 ElementsIndex)
		: PackagedData(0)
	{
		PackagedData = (int64(InElementType) << ElementTypeShift) | (int64(ElementsIndex) << ElementIndexShift);
	}
	int16 GetElementType() const
	{
		return (PackagedData & ElementTypeMask) >> ElementTypeShift;
	}
	int16 GetElementsIndex() const
	{
		return (PackagedData & ElementIndexMask) >> ElementIndexShift;
	}
	
	void SetElementType(int16 InElementType)
	{
		PackagedData = (PackagedData & ~ElementTypeMask) | (int64(InElementType) << ElementTypeShift);
	}
	void SetElementsIndex(int16 InElementsIndex)
	{
		PackagedData = (PackagedData & ~ElementIndexMask) | (int64(InElementsIndex) << ElementIndexShift);
	}
	
	bool IsValid() const
	{
		return PackagedData != -1;
	}
	/*
	int16 TypeId;					//	type
	int16 ElementsIndex;			//	TicketElementsView Index
	int16 ElementPerCount;			//	how many elements
	int16 ElementNameInstanceId;	//	name instance id
	*/
};

USTRUCT(BlueprintType)
struct FTIHHsBBTicket
{
	GENERATED_BODY()
	friend class UTIHHsBlackBoardSubsystem;
	friend class UTIHHsBBSession;

	friend class FTIHHsBBTicketHandlerBase;
	friend class FTIHHsBBTicketReader;
	friend class FTIHHsBBTicketWriter;

	FTIHHsBBTicket(): SessionID(-1), TicketID(-1), TicketFlag(0), ReadCount(0),
	                  TicketElementsView(MakeShared<TArray<FTIHHsBBTicketElement>>())
	{
	};
	FTIHHsBBTicket(int32 InSessionID, int32 InTicketID)
		: SessionID(InSessionID),
		  TicketID(InTicketID), TicketFlag(0), ReadCount(0),
		  TicketElementsView(MakeShared<TArray<FTIHHsBBTicketElement>>())
	{
	}

	FTIHHsBBTicket(int32 InSessionID, int32 InTicketID,TSharedRef<TArray<FTIHHsBBTicketElement>> other)
		: SessionID(InSessionID),
		  TicketID(InTicketID), TicketFlag(0), ReadCount(0), TicketElementsView(other)
	{
	}

	~FTIHHsBBTicket()
	{
		SessionID = -1;
		TicketID = -1;
		ReadCount = 0;
		ClearReadCount();
		// if ( not TicketElementsView->IsEmpty() && TicketElementsView.GetSharedReferenceCount() < 2)
		// {
		// 	
		// 	TicketElementsView->Empty(0);
		// }
		
		
	}
	int32 GetSessionID() const
	{
		return SessionID;
	}
	int32 GetTicketID() const
	{
		return TicketID;
	}
	int32 GetReadCount() const
	{
		return ReadCount;
	}
	void IncrementReadCount() const
	{
		ReadCount++;
	}
	void DecrementReadCount() const
	{
		ReadCount--;
		if (ReadCount < 0)
		{
			ReadCount = 0;
		}
	}
	void ClearReadCount() const
	{
		ReadCount = 0;
	}
	//	호출하는 곳: 아예 삭제전에 그냥 호출하는곳인데 expired가 안되었을때만 한번 더 호출해준다., 세션이 종료될때, 티켓이 사용되고 readCount가끝났을때.
	void MakeExpired(UTIHHsBBSession* inSession);
	//	수동적이며 세션이 종료될때만 확인할 수 있다. 그것외에는 직접 session을 찾아서 확인해야함
	bool IsTicketExpired() const
	{
		//	이걸 만들어주는건 세션임
		return SessionID == -1 || (not IsPersist() && GetReadCount() <= 0);
	}
	

	TSharedRef<TArray<FTIHHsBBTicketElement>> GetTicketElements() const
	{
		return TicketElementsView;
	}
	TSharedRef<TArray<FTIHHsBBTicketElement>> ChangeTicketElements(const TSharedRef<TArray<FTIHHsBBTicketElement>>& inTicketElements)
	{
		TicketElementsView = inTicketElements;
		return TicketElementsView;
	}
	FString ToString() const
	{
		FString result = TEXT("\"TicketID\": ") + FString::FromInt(TicketID) + TEXT("\"SessionID\": ") +
			FString::FromInt(SessionID) + TEXT("\"ReadCount\": ") + FString::FromInt(ReadCount) + TEXT("\"TicketElementsView\":[\n");
		for (const auto& element : *TicketElementsView)
		{
			result += TEXT("{\"ElementType\": ") + FString::FromInt(element.GetDataType()) +
				TEXT("\"ElementOffset\": ") + FString::FromInt(element.GetDataOffset()) +
				TEXT("\"ElementCount\": ") + FString::FromInt(element.GetDataCount()) +
				TEXT("\"ElementSize\": ") + FString::FromInt(element.GetDataSize()) + TEXT("},");
		}
		if (result.EndsWith(TEXT(",")))
		{
			result.RemoveAt(result.Len() - 1, 1);
		}
		result += TEXT("\n ]\n");
		return result;
	}
	void SetTicketPersist()
	{
		TicketFlag |= 0x1;
	}
	void SetTicketNotPersist()
	{
		TicketFlag &= ~0x1;
	}
	bool IsPersist() const
	{
		return  TicketFlag & 0x1;
	}
	bool GetTicketElementsForm(TArray<FTIHHsBBTicketElementForm>& outElements) const;

	
private:
	TSharedRef<TArray<FTIHHsBBTicketElement>> GetTicketElements()
	{
		return TicketElementsView;
	}
	UPROPERTY()
	int32 SessionID;
	UPROPERTY()
	int32 TicketID;
	
	UPROPERTY()
	int32 TicketFlag;
	
	mutable int32 ReadCount;

	//	TODO: 추후에 writer와 reader에서 가져오는 코드를 만들어야함.
	TMap<FString,FTIHHsBBTicketElementNamedView> TicketElementsNameMap;
	
	TSharedRef<TArray<FTIHHsBBTicketElement>> TicketElementsView;
};

USTRUCT(Blueprintable,BlueprintType)
struct FTIHHsBBTicketHandle
{
	GENERATED_BODY()
public:
	static const int64 INVALID_PACKED_ID = -1LL; // int64 리터럴 사용	
	FTIHHsBBTicketHandle():TicketPackedID(INVALID_PACKED_ID)
	{
	}
	using PackedIDType = int64;
private:
	UPROPERTY()
	int64 TicketPackedID;
	
public:
	FTIHHsBBTicketHandle(FTIHHsBBTicket& InTicket)
	{
		if (InTicket.GetSessionID() == INDEX_NONE || InTicket.GetTicketID() == INDEX_NONE)
		{
			TicketPackedID = INVALID_PACKED_ID;
		}
		
		TicketPackedID = (static_cast<int64>(InTicket.GetSessionID()) << 32) | (static_cast<int64>(InTicket.GetTicketID()) & 0xFFFFFFFFLL);
	}
	
	FTIHHsBBTicketHandle(int32 InSessionID, int32 InTicketID)
	{
		if (InSessionID == INDEX_NONE || InTicketID == INDEX_NONE)
		{
			TicketPackedID = INVALID_PACKED_ID;
		}
		
		TicketPackedID = (static_cast<int64>(InSessionID) << 32) | (static_cast<int64>(InTicketID) & 0xFFFFFFFFLL);
	}
	explicit FTIHHsBBTicketHandle(int64 InPackedID):
		TicketPackedID(InPackedID)
	{
	}
	FTIHHsBBTicketHandle(const FTIHHsBBTicketHandle& Other):
		TicketPackedID(Other.TicketPackedID)
	{
	}
	FTIHHsBBTicketHandle(FTIHHsBBTicketHandle&& Other):
		TicketPackedID(MoveTemp(Other.TicketPackedID))
	{
		Other.TicketPackedID = INVALID_PACKED_ID;
	}
	FTIHHsBBTicketHandle& operator=(const FTIHHsBBTicketHandle& Other)
	{
		if (this != &Other)
		{
			TicketPackedID = Other.TicketPackedID;
		}
		return *this;
	}
	FTIHHsBBTicketHandle& operator=(FTIHHsBBTicketHandle&& Other)
	{
		if (this != &Other)
		{
			TicketPackedID = MoveTemp(Other.TicketPackedID);
			Other.TicketPackedID = INVALID_PACKED_ID;
		}
		return *this;
	}
	
	FORCEINLINE bool IsValid() const
	{
		return (-1 < GetSessionID() && -1 < GetTicketID() && TicketPackedID != INVALID_PACKED_ID);	
	}
	void SetInvalidate()
	{
		TicketPackedID = INVALID_PACKED_ID;
	}
	FORCEINLINE int32 GetSessionID() const{return static_cast<int32>(TicketPackedID >> 32);}
	FORCEINLINE int32 GetTicketID() const{ return static_cast<int32>(TicketPackedID & 0xFFFFFFFFLL);}
	FORCEINLINE int64 GetPackedID() const{return TicketPackedID;}
	
	FORCEINLINE bool operator==(const FTIHHsBBTicketHandle& Other) 	const	{	return TicketPackedID == Other.TicketPackedID;	}
	FORCEINLINE bool operator!=(const FTIHHsBBTicketHandle& Other) 	const	{	return TicketPackedID != Other.TicketPackedID;	}
	FORCEINLINE bool operator<(const FTIHHsBBTicketHandle&	Other) 	const	{	return TicketPackedID < Other.TicketPackedID;	}
	FORCEINLINE bool operator>(const FTIHHsBBTicketHandle&	Other) 	const	{	return TicketPackedID > Other.TicketPackedID;	}
	FORCEINLINE bool operator<=(const FTIHHsBBTicketHandle& Other) 	const	{	return TicketPackedID <= Other.TicketPackedID;	}
	FORCEINLINE bool operator>=(const FTIHHsBBTicketHandle& Other) 	const	{	return TicketPackedID >= Other.TicketPackedID;	}

	explicit  operator int64() const {return TicketPackedID;}
	
	friend uint32 GetTypeHash(const FTIHHsBBTicketHandle& Handle)
	{
		return ::GetTypeHash(Handle.GetPackedID());
	}
	FString ToString()const
	{
		return FString::Printf(TEXT("\"ticketHandle\":{\"SessionID\":%d,\"TicketID\":%d}"), GetSessionID(), GetTicketID());
	}
	static const FTIHHsBBTicketHandle Invalid;
};


USTRUCT(BlueprintType)
struct FTIHHsBlackBoard
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<bool> BBValue_Bool;
	UPROPERTY()
	TArray<int8> BBValue_Int8;
	UPROPERTY()
	TArray<int16> BBValue_Int16;
	UPROPERTY()
	TArray<int32> BBValue_Int32;
	UPROPERTY()
	TArray<int64> BBValue_Int64;
	UPROPERTY()
	TArray<uint8> BBValue_Uint8;
	UPROPERTY()
	TArray<uint16> BBValue_Uint16;
	UPROPERTY()
	TArray<uint32> BBValue_Uint32;
	UPROPERTY()
	TArray<uint64> BBValue_Uint64;
	UPROPERTY()
	TArray<float> BBValue_Float;
	UPROPERTY()
	TArray<double> BBValue_Double;
	UPROPERTY()
	TArray<FString> BBValue_String;
	UPROPERTY()
	TArray<FVector> BBValue_Vector;
	UPROPERTY()
	TArray<FVector2D> BBValue_Vector2D;
	UPROPERTY()
	TArray<FVector4> BBValue_Vector4;
	UPROPERTY()
	TArray<FQuat> BBValue_Quat;
	UPROPERTY()
	TArray<FTransform> BBValue_Transform;
	UPROPERTY()
	TArray<FLinearColor> BBValue_LinearColor;
	UPROPERTY()
	TArray<FColor> BBValue_Color;
	UPROPERTY()
	TArray<FName> BBValue_Name;
	UPROPERTY()
	TArray<UObject*> BBValue_UObject;
	UPROPERTY()
	TArray<UClass*> BBValue_UClass;
	UPROPERTY()
	TArray<UActorComponent*> BBValue_UActorComponent;

	TArray<FVariant> BBValue_FVariant;

	void Reserve(int16 type, int32 count)
	{
		switch (type)
		{
		case static_cast<int16>(ETIHHsBlackBoardDataType::EVoidPtr): break;
			TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_RESERVE_CASE)
		default: break;
		}
	}
	void Empty(int16 type)
	{
		switch (type)
		{
		case static_cast<int16>(ETIHHsBlackBoardDataType::EVoidPtr): break;
			TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_EMPTY_CASE)
		default: break;
		}
	}

	int32 AddDefaulted(int16 type, int32 count)
	{
		int32 reValue = 0;
		switch (type)
		{
			TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_ADD_DEFAULTED_CASE)
			default: break;
		}
		return reValue;
	}
};



USTRUCT(BlueprintType)
struct FTIHHsBBSessionData
{
	GENERATED_BODY()

	UPROPERTY()
	ETIHHsBBSessionCloseType SessionType;

	UPROPERTY()
	FString SessionName;

	UPROPERTY()
	float SessionTime;

	UPROPERTY()
	int32 SessionTickCount;
};


/*
 *	outer: GetTransientPackage
 *	owner: UTIHHsBlackBoardSubsystem 
 *	destory: ETIHHsBBSessionCloseType 에 따라 다름.
 */
UENUM()
enum class ETIHHsBBSessionState
{
	EUnknown = 0,		//	초기상태
	EInitialized,		//	처음 CreateSession 호출시 처음 상태 지나가는 상태임
	EIdle,				//	처음 CreateSession 호출한 즉 초기상태와 같을때 
	ERunning,				//	제대로 티켓도 발부했을때
	EPending,			//	이건 모르겠음 사용중일때 쓰려고 한건데..
	EExpired,			//	만료가 되었을때 상태	지나가는 상태
	EMarkedForDestroy,	//	MarkBBSessionForDelete 호출했을때 
	EDestroying,		//	DestroySession 호출했을때 <- CanModifyDataForTicket을 우회하기위해서 그리고 죽음을 기다리는거임
};

UCLASS()
class UTIHHsBBSession : public UObject
{
public:
	//	대용량으로 모든 티켓들을 expired시키는거
	void MakeExpiredTickets();
	virtual void BeginDestroy() override;

private:
	GENERATED_BODY()
	friend class UTIHHsBlackBoardSubsystem;

	friend class FTIHHsBBTicketReader;
public:
	UTIHHsBBSession(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer), mSessionID(-1),
		  mSessionState(0), mSessionData(FTIHHsBBSessionData())
	{
		mSessionID = -1;
		mSessionState = 0;
	}

	// UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Session")
	// void InitializeBlackBoardSession();

	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Session")
	ETIHHsBBSessionCloseType GetSessionType() const
	{
		return mSessionData.SessionType;
	}

	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Session")
	FString GetBlackBoardName() const
	{
		return mSessionData.SessionName;
	}

	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Session")
	int32 GetSessionID() const
	{
		return mSessionID;
	}

	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Session")
	int32 GetSessionState() const
	{
		return mSessionState;
	}
	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Session")
	float GetSessionTime() const
	{
		return mSessionData.SessionTime;
	}

	virtual void OnPostCreateSessionDelegate();
	virtual void PreDestroySession();

	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Session")
	FTIHHsBlackBoard& GetBlackBoard()
	{
		return mBlackBoardData;
	}

	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Session")
	void OnTimerExpired();

	//	이걸 변화시켜야함.
	void RegisterBBTicketHandle(const FTIHHsBBTicketHandle& inTicketHandle);
	void UnRegisterBBTicketHandle(const FTIHHsBBTicketHandle& inTicketHandle)
	{
		if (mBlackBoardTicketHandles.Contains(inTicketHandle))
		{
			mBlackBoardTicketHandles.Remove(inTicketHandle);
		}
	}

	//	TODO: valid 가 아니라 존재하는지 확인하는 걸로 이름을 변경할것
	bool IsRegistedTicketHandle(const FTIHHsBBTicketHandle& inTicketHandle)const;
	
	bool IsBBSessionRunning() const
	{
		return mSessionState == static_cast<int32>(ETIHHsBBSessionState::ERunning);
	}

	void SetSessionState(ETIHHsBBSessionState inState)
	{
		mSessionState = static_cast<int32>(inState);
	}

	FTIHHsBBSessionData GetSessionData() const
	{
		return mSessionData;
	}
	
private:
	//	티켓 핸들로 사용
	void UseBBTicket(const FTIHHsBBTicketHandle& inTicketHandle);
	
	TSet<FTIHHsBBTicketHandle> mBlackBoardTicketHandles;
	
	UPROPERTY()
	FTIHHsBlackBoard mBlackBoardData;
	
	UPROPERTY()
	int32 mSessionID;

	UPROPERTY()
	int32 mSessionState;

	UPROPERTY()
	FTIHHsBBSessionData mSessionData;
};


//	type erasure
class FTIHHsBBRWBase
{
public:
	virtual ~FTIHHsBBRWBase() = default;

	FTIHHsBBRWBase(const FTIHHsBBTicketHandle& inTicketHandle);
	FTIHHsBBRWBase(const FTIHHsBBTicketHandle& inTicketHandle,FTIHHsBBTicketElement& target);

	bool IsAvailable() const
	{
		return mValid;
	}

	const FTIHHsBBTicketElement& GetTargetElement() const
	{
		return *mBBTicketElement;
	}

	const UTIHHsBBSession* GetTargetSessionConst() const
	{
		return mTargetBBSession;
	}
	UTIHHsBBSession* GetSession() const
	{
		return mTargetBBSession;
	}
	TSharedPtr<FTIHHsBBTicket> GetTicket()
	{
		return mBBTicket;
	}

	TSharedPtr<FTIHHsBBTicket> GetTicketConst() const
	{
		return mBBTicket;
	}
	FTIHHsBBTicketElement* GetTicketElement()
	{
		return mBBTicketElement;
	}
	const FTIHHsBBTicketElement* GetTicketElementConst() const
	{
		return mBBTicketElement;
	}
	const UTIHHsBBSession* GetSessionConst() const
	{
		return mTargetBBSession;
	}

	const FTIHHsBlackBoard& GetBlackBoard() const
	{
		return mTargetBBSession->GetBlackBoard();
	}

	// 맴버변수 접근, 해당 타입확인
	virtual bool IsWriteable() const
	{
		return false;
	}

	virtual bool IsReadable() const
	{
		return false;
	}

	virtual ETIHHsBlackBoardProcessorType GetProcessorType() const
	{
		return ETIHHsBlackBoardProcessorType::EKnown;
	}
	bool GetValid() const
	{
		return mValid;
	}
protected:
	void SetValid(bool inValid)
	{
		mValid = inValid;
	}
	
	bool mValid;
	TSharedPtr<FTIHHsBBTicket> mBBTicket;
	UTIHHsBBSession* mTargetBBSession;
	FTIHHsBBTicketElement* mBBTicketElement;
	
};



/*
 *	@brief BlackBoard Element Form
 *	@details 티켓의 엘리먼트를 만들때 사용하는 구조체
 */
USTRUCT(BlueprintType)
struct FTIHHsBBTicketElementForm
{
	GENERATED_BODY()

	UPROPERTY()
	FString ElementName;
	UPROPERTY()
	ETIHHsBlackBoardDataType ElementType;
	UPROPERTY()
	int32 ElementCount;

	FTIHHsBBTicketElementForm()
		: ElementName(""),
		  ElementType(ETIHHsBlackBoardDataType::EInvalid),
		  ElementCount(0)
	{
	}

	FTIHHsBBTicketElementForm(const FString& InElementName, const ETIHHsBlackBoardDataType& InElementType,
	                            const int32& InElementCount)
		: ElementName(InElementName),
		  ElementType(InElementType),
		  ElementCount(InElementCount)
	{
	}

	FTIHHsBBTicketElementForm(const FTIHHsBBTicketElementForm& InElement)
		: ElementName(InElement.ElementName),
		  ElementType(InElement.ElementType),
		  ElementCount(InElement.ElementCount)
	{
	}

	FTIHHsBBTicketElementForm& operator=(const FTIHHsBBTicketElementForm& InElement)
	{
		ElementName = InElement.ElementName;
		ElementType = InElement.ElementType;
		ElementCount = InElement.ElementCount;
		return *this;
	}

	FTIHHsBBTicketElementForm& operator=(FTIHHsBBTicketElementForm&& InElement)
	{
		ElementName = MoveTemp(InElement.ElementName);
		ElementType = MoveTemp(InElement.ElementType);
		ElementCount = MoveTemp(InElement.ElementCount);
		return *this;
	}
};
