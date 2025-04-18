#pragma once

#include "CoreMinimal.h"
#include "TIHHsBlackboard.h"
#include "TIHBlackBoardSubsystem.h"


template <typename TIHTemplateType>
class TTIHHsDefaultTypeTrait
{
public:
	static constexpr size_t GetTypeSize()
	{
		return 0;
	}

	static constexpr ETIHHsBlackBoardDataType GetType()
	{
		return ETIHHsBlackBoardDataType::EInvalid;
	}

	static TArray<TIHTemplateType>* GetBBArray(UTIHHsBlackBoardSession* blackBoardSession)
	{
		return nullptr;
	}

	static bool CheckType(uint16 dataType)
	{
		return false;
	}
};
class FTIHHsDefaultTypeTraitHelper
{
public:
	static FTIHHsBBRWBase* CreateBBTicketElementWriter(UTIHHsBlackBoardSession* inBBSession,FTIHHsBBTicketElement& targetElement);
	static FTIHHsBBRWBase* CreateBBTicketElementReader(UTIHHsBlackBoardSession* inBBSession,FTIHHsBBTicketElement& targetElement);
	static int16 GetTypeSize(int16 inDataType);
};

#define TIHMACRO_DefaultTypeTrait(type,enumName,defalutValue) \
	template<> \
	class TTIHHsDefaultTypeTrait<type> \
	{ \
		public: \
		static type GetDefaultValue()\
		{\
		return defalutValue;\
		}\
		static constexpr size_t GetTypeSize()\
		{\
		return sizeof(type);\
		}\
		static constexpr ETIHHsBlackBoardDataType GetType() \
		{ \
		return ETIHHsBlackBoardDataType::E##enumName; \
		}\
		static TArray<type>* GetBBArray(UTIHHsBlackBoardSession* blackBoardSession)\
		{\
		return (blackBoardSession != nullptr) ?	&(blackBoardSession->GetBlackBoard()).BBValue_##enumName : nullptr;	\
		}\
		static bool CheckType(uint16 dataType) \
		{ \
		return (dataType == (uint16)ETIHHsBlackBoardDataType::E##enumName); \
		} \
	}

TIHMACRO_DefaultTypeTrait(bool, Bool, false);

TIHMACRO_DefaultTypeTrait(int8, Int8, 0);

TIHMACRO_DefaultTypeTrait(int16, Int16, 0);

TIHMACRO_DefaultTypeTrait(int32, Int32, 0);

TIHMACRO_DefaultTypeTrait(int64, Int64, 0);

TIHMACRO_DefaultTypeTrait(uint8, Uint8, 0);

TIHMACRO_DefaultTypeTrait(uint16, Uint16, 0);

TIHMACRO_DefaultTypeTrait(uint32, Uint32, 0);

TIHMACRO_DefaultTypeTrait(uint64, Uint64, 0);

TIHMACRO_DefaultTypeTrait(float, Float, 0.0f);

TIHMACRO_DefaultTypeTrait(double, Double, 0.0);

TIHMACRO_DefaultTypeTrait(FString, String, TEXT(""));

TIHMACRO_DefaultTypeTrait(FVector, Vector, FVector::ZeroVector);

TIHMACRO_DefaultTypeTrait(FVector2D, Vector2D, FVector2D::ZeroVector);

TIHMACRO_DefaultTypeTrait(FVector4, Vector4, FVector4::Zero());

TIHMACRO_DefaultTypeTrait(FQuat, Quat, FQuat::Identity);

TIHMACRO_DefaultTypeTrait(FTransform, Transform, FTransform::Identity);

TIHMACRO_DefaultTypeTrait(FLinearColor, LinearColor, FLinearColor::White);

TIHMACRO_DefaultTypeTrait(FColor, Color, FColor::White);

TIHMACRO_DefaultTypeTrait(FName, Name, FName(TEXT("")));

TIHMACRO_DefaultTypeTrait(UObject*, UObject, nullptr);

TIHMACRO_DefaultTypeTrait(UClass*, UClass, nullptr);

TIHMACRO_DefaultTypeTrait(UActorComponent*, UActorComponent, nullptr);

template <typename TIHTemplateType>
class TTIHHsBBTicketElementRWBase : public FTIHHsBBRWBase
{
public:
	virtual ~TTIHHsBBTicketElementRWBase() override = default;
	TTIHHsBBTicketElementRWBase(FTIHHsBBTicket& inTicket) = delete;

	virtual ETIHHsBlackBoardProcessorType GetProcessorType() const override final
	{
		return ETIHHsBlackBoardProcessorType::ETicketElement;
	}

	TTIHHsBBTicketElementRWBase(UTIHHsBlackBoardSession* inBBSession,
	                            FTIHHsBBTicketElement& targetElement): FTIHHsBBRWBase(inBBSession, targetElement)
	{
	}
};

template <typename TIHTemplateType>
class TTIHHsBBWriter : public TTIHHsBBTicketElementRWBase<TIHTemplateType>
{
public:
	virtual ~TTIHHsBBWriter() override = default;
	virtual bool IsReadable() const override { return true; };
	virtual bool IsWriteable() const override { return true; };

	TTIHHsBBWriter(UTIHHsBlackBoardSession* inBBSession, FTIHHsBBTicketElement& targetElement):
		TTIHHsBBTicketElementRWBase<TIHTemplateType>(inBBSession, targetElement), mTargetArray(nullptr)
	{
		int16 dataType = this->mBBTicketElement->GetDataType();
		bool& valid = this->mValid;
		if (TTIHHsDefaultTypeTrait<TIHTemplateType>::CheckType(dataType))
		{
			int32& calculateBeginIndex = this->mCalculateBeginIndex;
			int32& calculateEndIndex = this->mCalculateEndIndex;
			int32& calculateDataCount = this->mCalculateDataCount;

			mTargetArray = TTIHHsDefaultTypeTrait<TIHTemplateType>::GetBBArray(inBBSession);
			if (mTargetArray != nullptr)
			{
				calculateBeginIndex = targetElement.GetDataOffset();
				calculateEndIndex = targetElement.GetDataOffset() + targetElement.GetDataCount(); //	less than
				if (not mTargetArray->IsValidIndex(calculateBeginIndex))
				{
					valid = false;
				}
				if (mTargetArray->Num() < calculateEndIndex)
				{
					calculateEndIndex = mTargetArray->Num();
				}
				calculateDataCount = calculateEndIndex - calculateBeginIndex;
				if (calculateDataCount < 1)
				{
					valid = false;
				}
			}
			else
			{
				valid = false;
			}
		}
		else
		{
			valid = false;
		}
	}

	TIHTemplateType& operator[](int32 index)
	{
		static TIHTemplateType errorValue = TTIHHsDefaultTypeTrait<TIHTemplateType>::GetDefaultValue();
		if (this->mValid)
		{
			if (-1 < index && index < this->mCalculateDataCount)
			{
				index += this->mCalculateBeginIndex;
			}
			else
			{
				UE_LOG(LogTemp, Warning,
				       TEXT("TIHHsBBWriter::operator[] index out of range, index = %d, mCalculateDataCount = %d"),
				       index, this->mCalculateDataCount);
				return errorValue;
			}

			return (*(mTargetArray))[index];
		}
		return errorValue;
	}

private:
	TArray<TIHTemplateType>* mTargetArray;
};

template <typename TIHTemplateType>
class TTIHHsBBReader : public TTIHHsBBTicketElementRWBase<TIHTemplateType>
{
public:
	virtual ~TTIHHsBBReader() override = default;
	virtual bool IsReadable() const override { return true; };

	TTIHHsBBReader(FTIHHsBBTicket& inTicket) = delete;

	TTIHHsBBReader(UTIHHsBlackBoardSession* inBBSession, FTIHHsBBTicketElement& targetElement):
		TTIHHsBBTicketElementRWBase<TIHTemplateType>(inBBSession, targetElement), mTargetArray(nullptr)
	{
		int16 dataType = this->mBBTicketElement->GetDataType();
		bool& valid = this->mValid;
		if (TTIHHsDefaultTypeTrait<TIHTemplateType>::CheckType(dataType))
		{
			int32& calculateBeginIndex = this->mCalculateBeginIndex;
			int32& calculateEndIndex = this->mCalculateEndIndex;
			int32& calculateDataCount = this->mCalculateDataCount;

			mTargetArray = TTIHHsDefaultTypeTrait<TIHTemplateType>::GetBBArray(inBBSession);
			if (mTargetArray != nullptr)
			{
				calculateBeginIndex = targetElement.GetDataOffset();
				calculateEndIndex = targetElement.GetDataOffset() + targetElement.GetDataCount(); //	less than
				if (not mTargetArray->IsValidIndex(calculateBeginIndex))
				{
					valid = false;
				}
				if (mTargetArray->Num() < calculateEndIndex)
				{
					calculateEndIndex = mTargetArray->Num();
				}
				calculateDataCount = calculateEndIndex - calculateBeginIndex;
				if (calculateDataCount < 1)
				{
					valid = false;
				}
			}
			else
			{
				valid = false;
			}
		}
		else
		{
			valid = false;
		}
	}

	const TIHTemplateType& operator[](int32 index) const
	{
		static TIHTemplateType errorValue = TTIHHsDefaultTypeTrait<TIHTemplateType>::GetDefaultValue();
		if (this->mValid)
		{
			if (-1 < index && index < this->mCalculateDataCount)
			{
				index += this->mCalculateBeginIndex;
			}
			else
			{
				UE_LOG(LogTemp, Warning,
				       TEXT("TIHHsBBWriter::operator[] index out of range, index = %d, mCalculateDataCount = %d"),
				       index, this->mCalculateDataCount);
				return errorValue;
			}

			return (*(mTargetArray))[index];
		}
		return errorValue;
	}

private:
	TArray<TIHTemplateType>* mTargetArray;
};


class FTIHHsBBTicketTicketWriter : public FTIHHsBBRWBase
{
public:
	virtual ~FTIHHsBBTicketTicketWriter() override
	{
		for (TPair<int16, TArray<FTIHHsBBRWBase*>>& target : mTicketElementMap)
		{
			for (FTIHHsBBRWBase* element :target.Value)
			{
				if (element != nullptr)
				{
					delete element;
					element = nullptr;
				}
			}
			target.Value.Empty(0);
		}
		mTicketElementMap.Empty();
	};
	FTIHHsBBTicketTicketWriter(UTIHHsBlackBoardSession* inBBSession, FTIHHsBBTicketElement& targetElement) = delete;

	FTIHHsBBTicketTicketWriter(FTIHHsBBTicket& inTicket): FTIHHsBBRWBase(inTicket)
	{
		UTIHHsBlackBoardSubsystem* bbSystem = UTIHHsBlackBoardSubsystem::GetBBSystem();

		if (bbSystem != nullptr)
		{
			int16 ticketId = mBBTicket->SessionID;
			UTIHHsBlackBoardSession* bbSession = bbSystem->GetBlackBoardSessionById(ticketId);
			if (bbSession != nullptr)
			{
				for (FTIHHsBBTicketElement& bbTicketElement : mBBTicket->TicketElements)
				{
					FTIHHsBBRWBase* writer = FTIHHsDefaultTypeTraitHelper::CreateBBTicketElementWriter(bbSession, bbTicketElement);
					if (writer == nullptr )
					{
						mValid = false;
					}
					else if (writer->IsValid())
					{
						mTicketElementMap.FindOrAdd(bbTicketElement.GetDataType()).Add(writer);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error,
				       TEXT("TTIHHsBBTicketTicketRWBase::TTIHHsBBTicketTicketRWBase() bbSession is nullptr"));
				mValid = false;
			}
		}
		else
		{
			mValid = false;
		}
	}
	TArray<FTIHHsBBRWBase*>& operator[](int16 type)
	{
		static TArray<FTIHHsBBRWBase*> errorValue;
		if (mTicketElementMap.Contains(type))
		{
			return mTicketElementMap[type];
		}
		return errorValue;
	}
	TArray<FTIHHsBBRWBase*>& operator[](ETIHHsBlackBoardDataType type)
	{
		static TArray<FTIHHsBBRWBase*> errorValue;
		if (mTicketElementMap.Contains(static_cast<int16>(type)))
		{
			return mTicketElementMap[static_cast<int16>(type)];
		}
		return errorValue;
	}
	FTIHHsBBRWBase* GetTicketElementWriter(ETIHHsBlackBoardDataType type, int32 index)
	{
		if (mTicketElementMap.Contains(static_cast<int16>(type)))
		{
			if (mTicketElementMap[static_cast<int16>(type)].IsValidIndex(index))
			{
				return mTicketElementMap[static_cast<int16>(type)][index];
			}
		}
		return nullptr;
	}
	template<typename TIHTemplateType>
	TTIHHsBBWriter<TIHTemplateType>* GetTicketElementWriter(int32 index)
	{
		int16 type = static_cast<int16>(TTIHHsDefaultTypeTrait<TIHTemplateType>::GetType());
		if (type != static_cast<int16>(ETIHHsBlackBoardDataType::EInvalid) && mTicketElementMap.Contains(type))
		{
			TArray<FTIHHsBBRWBase*>& elementArray = mTicketElementMap[type];
			if (elementArray.IsValidIndex(index))
			{
				return 	static_cast<TTIHHsBBWriter<TIHTemplateType>*>(elementArray[index]);
			}
		}
		return nullptr;
	}
private:
	TMap<int16, TArray<FTIHHsBBRWBase*>> mTicketElementMap;
};

class FTIHHsBBTicketTicketReader : public FTIHHsBBRWBase
{
public:
	virtual ~FTIHHsBBTicketTicketReader() override
	{
		for (TPair<int16, TArray<FTIHHsBBRWBase*>>& target : mTicketElementMap)
		{
			for (FTIHHsBBRWBase* element :target.Value)
			{
				if (element != nullptr)
				{
					delete element;
					element = nullptr;
				}
			}
			target.Value.Empty(0);
		}
		mTicketElementMap.Empty();
	};
	FTIHHsBBTicketTicketReader(UTIHHsBlackBoardSession* inBBSession, FTIHHsBBTicketElement& targetElement) = delete;
	FTIHHsBBTicketTicketReader(FTIHHsBBTicket& inTicket): FTIHHsBBRWBase(inTicket)
	{
		UTIHHsBlackBoardSubsystem* bbSystem = UTIHHsBlackBoardSubsystem::GetBBSystem();

		if (bbSystem != nullptr)
		{
			int16 ticketId = mBBTicket->SessionID;
			UTIHHsBlackBoardSession* bbSession = bbSystem->GetBlackBoardSessionById(ticketId);
			if (bbSession != nullptr)
			{
				for (FTIHHsBBTicketElement& bbTicketElement : mBBTicket->TicketElements)
				{
					FTIHHsBBRWBase* reader = FTIHHsDefaultTypeTraitHelper::CreateBBTicketElementReader(
						bbSession, bbTicketElement);
					if (reader != nullptr && reader->IsValid())
					{
						mTicketElementMap.FindOrAdd(bbTicketElement.GetDataType()).Add(reader);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error,
					   TEXT("FTIHHsBBTicketTicketReader::FTIHHsBBTicketTicketReader() bbSession is nullptr"));
				mValid = false;
			}
		}
		else
		{
			mValid = false;
		}
	}
	const TArray<FTIHHsBBRWBase*>& operator[](int16 type)
	{
		static TArray<FTIHHsBBRWBase*> errorValue;
		if (mTicketElementMap.Contains(type))
		{
			return mTicketElementMap[type];
		}
		return errorValue;
	}
	const TArray<FTIHHsBBRWBase*>& operator[](ETIHHsBlackBoardDataType type)
	{
		static TArray<FTIHHsBBRWBase*> errorValue;
		if (mTicketElementMap.Contains(static_cast<int16>(type)))
		{
			return mTicketElementMap[static_cast<int16>(type)];
		}
		return errorValue;
	}
	const FTIHHsBBRWBase* GetTicketElementReader(ETIHHsBlackBoardDataType type, int32 index) const
	{
		if (mTicketElementMap.Contains(static_cast<int16>(type)))
		{
			if (mTicketElementMap[static_cast<int16>(type)].IsValidIndex(index))
			{
				return mTicketElementMap[static_cast<int16>(type)][index];
			}
		}
		return nullptr;
	}
	bool ReadPossible() const
	{
		UTIHHsBlackBoardSubsystem* bbSystem = UTIHHsBlackBoardSubsystem::GetBBSystem();
		if (bbSystem != nullptr)
		{
			int16 ticketId = mBBTicket->SessionID;
			UTIHHsBlackBoardSession* bbSession = bbSystem->GetBlackBoardSessionById(ticketId);
			if (bbSession != nullptr)
			{
				return true;
			}
		}
		return false;
	}
private:
	TMap<int16, TArray<FTIHHsBBRWBase*>> mTicketElementMap;
};





inline FTIHHsBBRWBase* FTIHHsDefaultTypeTraitHelper::CreateBBTicketElementWriter(UTIHHsBlackBoardSession* inBBSession,
	FTIHHsBBTicketElement& targetElement)
{
	switch (targetElement.GetDataType())
	{
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EBool):
		return new TTIHHsBBWriter<bool>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInt8):
		return new TTIHHsBBWriter<int8>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInt16):
		return new TTIHHsBBWriter<int16>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInt32):
		return new TTIHHsBBWriter<int32>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInt64):
		return new TTIHHsBBWriter<int64>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUint8):
		return new TTIHHsBBWriter<uint8>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUint16):
		return new TTIHHsBBWriter<uint16>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUint32):
		return new TTIHHsBBWriter<uint32>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUint64):
		return new TTIHHsBBWriter<uint64>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EFloat):
		return new TTIHHsBBWriter<float>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EDouble):
		return new TTIHHsBBWriter<double>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EString):
		return new TTIHHsBBWriter<FString>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EVector):
		return new TTIHHsBBWriter<FVector>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EVector2D):
		return new TTIHHsBBWriter<FVector2D>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EVector4):
		return new TTIHHsBBWriter<FVector4>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EQuat):
		return new TTIHHsBBWriter<FQuat>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::ETransform):
		return new TTIHHsBBWriter<FTransform>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::ELinearColor):
		return new TTIHHsBBWriter<FLinearColor>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EColor):
		return new TTIHHsBBWriter<FColor>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EName):
		return new TTIHHsBBWriter<FName>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUObject):
		return new TTIHHsBBWriter<UObject*>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUClass):
		return new TTIHHsBBWriter<UClass*>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUActorComponent):
		return new TTIHHsBBWriter<UActorComponent*>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInvalid):
		return nullptr;
	default: return nullptr;
	}
}

inline FTIHHsBBRWBase* FTIHHsDefaultTypeTraitHelper::CreateBBTicketElementReader(UTIHHsBlackBoardSession* inBBSession,
	FTIHHsBBTicketElement& targetElement)
{
		switch (targetElement.GetDataType())
	{
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EBool):
		return new TTIHHsBBReader<bool>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInt8):
		return new TTIHHsBBReader<int8>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInt16):
		return new TTIHHsBBReader<int16>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInt32):
		return new TTIHHsBBReader<int32>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInt64):
		return new TTIHHsBBReader<int64>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUint8):
		return new TTIHHsBBReader<uint8>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUint16):
		return new TTIHHsBBReader<uint16>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUint32):
		return new TTIHHsBBReader<uint32>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUint64):
		return new TTIHHsBBReader<uint64>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EFloat):
		return new TTIHHsBBReader<float>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EDouble):
		return new TTIHHsBBReader<double>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EString):
		return new TTIHHsBBReader<FString>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EVector):
		return new TTIHHsBBReader<FVector>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EVector2D):
		return new TTIHHsBBReader<FVector2D>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EVector4):
		return new TTIHHsBBReader<FVector4>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EQuat):
		return new TTIHHsBBReader<FQuat>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::ETransform):
		return new TTIHHsBBReader<FTransform>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::ELinearColor):
		return new TTIHHsBBReader<FLinearColor>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EColor):
		return new TTIHHsBBReader<FColor>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EName):
		return new TTIHHsBBReader<FName>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUObject):
		return new TTIHHsBBReader<UObject*>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUClass):
		return new TTIHHsBBReader<UClass*>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EUActorComponent):
		return new TTIHHsBBReader<UActorComponent*>(inBBSession, targetElement);
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInvalid):
		return nullptr;
	default: return nullptr;
	}
}

inline int16 FTIHHsDefaultTypeTraitHelper::GetTypeSize(int16 inDataType)
{
	int16 reValue = 0;
		switch (inDataType)
		{
			TIHMACRO_CASE_BEGIN	(Bool)					reValue = sizeof(bool);									break;
			TIHMACRO_CASE_BEGIN	(Int8)					reValue = sizeof(int8);			 											break;
			TIHMACRO_CASE_BEGIN	(Int16)					reValue = sizeof(int16);			 											break;
			TIHMACRO_CASE_BEGIN	(Int32)					reValue = sizeof(int32);			 											break;
			TIHMACRO_CASE_BEGIN	(Int64)					reValue = sizeof(int64);			 											break;
			TIHMACRO_CASE_BEGIN	(Uint8)					reValue = sizeof(uint8);			 											break;
			TIHMACRO_CASE_BEGIN	(Uint16)				reValue = sizeof(uint16);			 											break;
			TIHMACRO_CASE_BEGIN	(Uint32)				reValue = sizeof(uint32);			 											break;
			TIHMACRO_CASE_BEGIN	(Uint64)				reValue = sizeof(uint64);			 											break;
			TIHMACRO_CASE_BEGIN	(Float)					reValue = sizeof(float);			 											break;
			TIHMACRO_CASE_BEGIN	(Double)				reValue = sizeof(double);			 											break;
			TIHMACRO_CASE_BEGIN	(String)				reValue = sizeof(FString);			 											break;
			TIHMACRO_CASE_BEGIN	(Vector)				reValue = sizeof(FVector);			 											break;
			TIHMACRO_CASE_BEGIN	(Vector2D)				reValue = sizeof(FVector2D);			 											break;
			TIHMACRO_CASE_BEGIN	(Vector4)				reValue = sizeof(FVector4);			 											break;
			TIHMACRO_CASE_BEGIN	(Quat)					reValue = sizeof(FQuat);			 											break;
			TIHMACRO_CASE_BEGIN	(Transform)				reValue = sizeof(FTransform);			 											break;
			TIHMACRO_CASE_BEGIN	(LinearColor)			reValue = sizeof(FLinearColor);			 											break;
			TIHMACRO_CASE_BEGIN	(Color)					reValue = sizeof(FColor);			 											break;
			TIHMACRO_CASE_BEGIN	(Name)					reValue = sizeof(FName);			 											break;
			TIHMACRO_CASE_BEGIN	(UObject)				reValue = sizeof(UObject*);			 											break;
			TIHMACRO_CASE_BEGIN	(UClass)				reValue = sizeof(UClass*);			 											break;
			TIHMACRO_CASE_BEGIN	(UActorComponent)		reValue = sizeof(UActorComponent*);			 											break;
		
		}
		return reValue;
}
