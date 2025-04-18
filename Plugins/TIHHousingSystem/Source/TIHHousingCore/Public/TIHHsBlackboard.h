// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TIHHsBlackboard.generated.h"

/**
 * 
 */
UENUM()
enum class ETIHHsBlackBoardDataType : uint8
{
	EVoidPtr = 0,
	EBool,
	EInt8,
	EInt16,
	EInt32,
	EInt64,
	EUint8,
	EUint16,
	EUint32,
	EUint64,
	EFloat,
	EDouble,
	EString,
	EVector,
	EVector2D,
	EVector4,
	EQuat,
	ETransform,
	ELinearColor,
	EColor,
	EName,
	EUObject,
	EUClass,
	EUActorComponent,
	EInvalid,
	EDefaultMax
};

UENUM()
enum class ETIHHsBBSessionCloseType : uint8
{
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

// TYPE_NAME_BOOL Bool
// TYPE_NAME_INT8 Int8
// TYPE_NAME_INT16 Int16
// TYPE_NAME_INT32 Int32
// TYPE_NAME_INT64 Int64
// TYPE_NAME_UINT8 Uint8
// TYPE_NAME_UINT16 Uint16
// TYPE_NAME_UINT32 Uint32
// TYPE_NAME_UINT64 Uint64
// TYPE_NAME_FLOAT Float
// TYPE_NAME_DOUBLE Double
// TYPE_NAME_STRING FString
// TYPE_NAME_VECTOR FVector
// TYPE_NAME_VECTOR2D FVector2D
// TYPE_NAME_VECTOR4 FVector4
// TYPE_NAME_QUAT FQuat
// TYPE_NAME_TRANSFORM FTransform
// TYPE_NAME_LINEAR_COLOR FLinearColor
// TYPE_NAME_COLOR FColor
// TYPE_NAME_NAME FName
// TYPE_NAME_OBJECT UObject
// TYPE_NAME_CLASS UClass
// TYPE_NAME_ACTOR_COMPONENT UActorComponent

#define TIHMACRO_CASE_BEGIN(type) case static_cast<int16>(ETIHHsBlackBoardDataType::E##type): 


#define TIHMACRO_CASE_INNER_BEGIN(type) \
case static_cast<int16>(ETIHHsBlackBoardDataType::E##type):{ auto& array = BBValue_##type;
#define TIHMACRO_CASE_INNER_END } break

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

	void Reserve(int16 type, int32 count)
	{
		switch (type)
		{
		case static_cast<int16>(ETIHHsBlackBoardDataType::EVoidPtr): break;
			TIHMACRO_CASE_INNER_BEGIN(Bool)				array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int8)				array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int16)				array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int32)				array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int64)				array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint8)				array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint16)        		array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint32)        		array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint64)        		array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Float)        		array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Double)       		array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(String)       		array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Vector)        		array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Vector2D)      		array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Vector4)        	array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Quat)				array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Transform)      	array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(LinearColor)    	array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Color)				array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Name)				array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(UObject)        	array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(UClass)				array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(UActorComponent)	array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
		default: break;
		}
	}
	void Empty(int16 type)
	{
		switch (type)
		{
		case static_cast<int16>(ETIHHsBlackBoardDataType::EVoidPtr): break;
			TIHMACRO_CASE_INNER_BEGIN(Bool)				array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int8)				array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int16)				array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int32)				array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int64)				array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint8)				array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint16)        		array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint32)        		array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint64)        		array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Float)        		array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Double)       		array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(String)       		array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Vector)        		array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Vector2D)      		array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Vector4)        	array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Quat)				array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Transform)      	array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(LinearColor)    	array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Color)				array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Name)				array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(UObject)        	array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(UClass)				array.Empty(); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(UActorComponent)	array.Empty(); TIHMACRO_CASE_INNER_END;
		default: break;
		}
	}

	int32 AddDefaulted(int16 type, int32 count)
	{
		int32 reValue = 0;
		switch (type)
		{
			TIHMACRO_CASE_INNER_BEGIN(Bool) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int8) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int16) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int32) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Int64) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint8) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint16) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint32) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Uint64) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Float) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Double) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(String) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Vector) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Vector2D) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Vector4) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Quat) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Transform) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(LinearColor) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Color) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(Name) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(UObject) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(UClass) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			TIHMACRO_CASE_INNER_BEGIN(UActorComponent) reValue = array.AddDefaulted(count); TIHMACRO_CASE_INNER_END;
			default: break;
		}
		return reValue;
	}
};

struct FTIHHsBBTicketElement;
struct FTIHHsBBTicket;


#define GET_BBDATA(type,typeName) \
	bool GetBBData_##typeName(FTIHHsBBTicketElement& bbTicketElement,TArray<type>& outData) \
	{ \
		if (bbTicketElement.GetDataType() == (uint16)ETIHHsBlackBoardDataType::E##typeName) \
		{ \
			int32 dataCount = bbTicketElement.GetDataCount(); \
			int32 dataSize = bbTicketElement.GetDataSize(); \
			int32 dataOffset = bbTicketElement.GetDataOffset(); \
			outData.SetNumUninitialized(dataCount); \
			mBlackBoardData. \
			FMemory::Memcpy(outData.GetData(),  + dataOffset, dataCount * dataSize); \
			return true; \
		} \
		return false; \
	}

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
UCLASS()
class UTIHHsBlackBoardSession : public UObject
{
	GENERATED_BODY()
	friend class UTIHHsBlackBoardSubsystem;

public:
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

	virtual void PostCreateSession();
	virtual void PreDestroySession();

	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Session")
	FTIHHsBlackBoard& GetBlackBoard()
	{
		return mBlackBoardData;
	}

	UFUNCTION(BlueprintCallable, Category = "TIHHsBlackBoard|Session")
	void OnTimerExpired();

	void RegisterBlackBoardTicket(FTIHHsBBTicket* inTicket)
	{
		mBlackBoardTickets.FindOrAdd(inTicket)+=1;
	}
private:
	//	키,발행량
	TMap<FTIHHsBBTicket*, int32> mBlackBoardTickets;
	
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

	//	TODO
	FTIHHsBBRWBase(FTIHHsBBTicket& inTicket):
		mValid(false), mBBTicket(&inTicket),
		mTargetBBSession(nullptr), mBBTicketElement(nullptr),
		mCalculateBeginIndex(0), mCalculateEndIndex(0), mCalculateDataCount(0)
	{
	}

	FTIHHsBBRWBase(UTIHHsBlackBoardSession* inBBSession, FTIHHsBBTicketElement& target):
		mValid(false), mTargetBBSession(inBBSession), mBBTicketElement(&target),
		mBBTicket(nullptr),
		mCalculateBeginIndex(0), mCalculateEndIndex(0), mCalculateDataCount(0)
	{
		if (inBBSession != nullptr)
		{
			mValid = true;
		}
		else
		{
			mValid = false;
		}
	}

	bool IsValid() const
	{
		return mValid;
	}

	const FTIHHsBBTicketElement& GetTargetElement() const
	{
		return *mBBTicketElement;
	}

	const UTIHHsBlackBoardSession* GetTargetSessionConst() const
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

protected:
	bool mValid;
	FTIHHsBBTicket* mBBTicket;

	UTIHHsBlackBoardSession* mTargetBBSession;
	FTIHHsBBTicketElement* mBBTicketElement;
	int32 mCalculateBeginIndex;
	int32 mCalculateEndIndex;
	int32 mCalculateDataCount;
};

//	TODO: 유효한지 확인하는 코드가 필요.
USTRUCT(BlueprintType)
struct FTIHHsBBTicket
{
	GENERATED_BODY()
	friend class UTIHHsBlackBoardSubsystem;
	
	UPROPERTY()
	int32 SessionID;

	UPROPERTY()
	int32 TicketID;
	
	TSharedRef<TArray<FTIHHsBBTicketElement>> TicketElements;

	FTIHHsBBTicket() = default;

	FTIHHsBBTicket(int32 InSessionID, int32 InTicketID)
		: SessionID(InSessionID),
		  TicketID(InTicketID)
	{
	}
};

USTRUCT(BlueprintType)
struct FTIHHsBlackBoardElementForm
{
	GENERATED_BODY()

	UPROPERTY()
	FString ElementName;
	UPROPERTY()
	ETIHHsBlackBoardDataType ElementType;
	UPROPERTY()
	int32 ElementCount;

	FTIHHsBlackBoardElementForm()
		: ElementName(""),
		  ElementType(ETIHHsBlackBoardDataType::EInvalid),
		  ElementCount(0)
	{
	}

	FTIHHsBlackBoardElementForm(const FString& InElementName, const ETIHHsBlackBoardDataType& InElementType,
	                            const int32& InElementCount)
		: ElementName(InElementName),
		  ElementType(InElementType),
		  ElementCount(InElementCount)
	{
	}

	FTIHHsBlackBoardElementForm(const FTIHHsBlackBoardElementForm& InElement)
		: ElementName(InElement.ElementName),
		  ElementType(InElement.ElementType),
		  ElementCount(InElement.ElementCount)
	{
	}

	FTIHHsBlackBoardElementForm& operator=(const FTIHHsBlackBoardElementForm& InElement)
	{
		ElementName = InElement.ElementName;
		ElementType = InElement.ElementType;
		ElementCount = InElement.ElementCount;
		return *this;
	}

	FTIHHsBlackBoardElementForm& operator=(FTIHHsBlackBoardElementForm&& InElement)
	{
		ElementName = MoveTemp(InElement.ElementName);
		ElementType = MoveTemp(InElement.ElementType);
		ElementCount = MoveTemp(InElement.ElementCount);
		return *this;
	}
};
