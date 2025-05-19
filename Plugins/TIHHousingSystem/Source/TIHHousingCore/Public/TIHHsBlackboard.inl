#pragma once

#include "CoreMinimal.h"
#include "TIHHsBlackboard.h"

class FTIHHsBBTicketElementHandleBase;

class FTIHHsDefaultTypeTraitHelper
{
public:
	static FTIHHsBBTicketElementHandleBase* CreateBBTicketElementWriter(const FTIHHsBBTicketHandle& ticketHandle,FTIHHsBBTicketElement& targetElement);
	static FTIHHsBBTicketElementHandleBase* CreateBBTicketElementReader(const FTIHHsBBTicketHandle& ticketHandle,FTIHHsBBTicketElement& targetElement);
	static ETIHHsBlackBoardDataType GetTypeFromInt16(int16 inDataType);
	static int16 GetTypeFromEnum(ETIHHsBlackBoardDataType inDataType);
	static int16 GetTypeSize(int16 inDataType);
	static FString GetTypeName(int16 inDataType);
	static ETIHHsBlackBoardDataType GetTypeFromName(FString inTypeName);
};

/*	---------------------------------------------------------------------------
 *	@brief	TIHMACRO_Blackboard_Datatype_List를 통해서 TIHMACRO_DefaultTypeTrait를 특수화한다
 */
TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_APPLY_DEFAULT_TYPE_TRAIT)


/*	---------------------------------------------------------------------------
 *	@brief	티켓 엘리먼트 핸들러 베이스
 */
class FTIHHsBBTicketElementHandleBase : public FTIHHsBBRWBase
{
public:
	explicit FTIHHsBBTicketElementHandleBase(FTIHHsBBTicket& inTicket) = delete;	//	티켓 엘리먼트는 엘리먼트 타겟이 무조건 있어야한다.
	
	explicit FTIHHsBBTicketElementHandleBase(const FTIHHsBBTicketHandle& inTicketHandle,FTIHHsBBTicketElement& target)
		: FTIHHsBBRWBase(inTicketHandle,target), mCalculateBeginIndex(0), mCalculateEndIndex(0), mCalculateDataCount(0)
	{}
	
	virtual ~FTIHHsBBTicketElementHandleBase() override = default;

	virtual int32 Num() const = 0;

	virtual void* GetBaseElementPtr() const = 0
	{
		return nullptr;
	}

protected:
	virtual void* GetBaseElementPtrMutable()
	{
		return nullptr;
	}
	int32 mCalculateBeginIndex;
	int32 mCalculateEndIndex;
	int32 mCalculateDataCount;
};
/*	-----------------------------------------------------------------------------
 *	@brief 티켓 '엘리먼트' 이터레이터
 *	@details 티켓 엘리먼트 핸들러는 티켓 엘리먼트를 읽거나 쓸 수 있는 핸들러입니다.
 *	@note TTIHHsBBTicketElementReader와 TTIHHsBBTicketElementWriter에서 사용한다.
 */
template<typename ElementType,bool IsConst>
class TTIHHsBBTicketElementIteratorImpl
{
public:
	using ResolvedElementType = std::conditional_t<IsConst, const ElementType, ElementType>;
	using value_type        = ElementType;
	using reference         = ResolvedElementType&;
	using pointer           = ResolvedElementType*;
	using difference_type   = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;
private:
	pointer mPtr;
public:
	TTIHHsBBTicketElementIteratorImpl(pointer ptr)
		: mPtr(ptr)
	{
	}

	//	--- Access operators
	reference operator*() const{return *mPtr;}
	pointer operator->() const{return mPtr;}

	//	--- increment/decrement operators
	TTIHHsBBTicketElementIteratorImpl& operator++()	{++mPtr;return *this;}
	TTIHHsBBTicketElementIteratorImpl operator++(int){auto tmp = *this;++mPtr;return tmp;}

	TTIHHsBBTicketElementIteratorImpl& operator--(){--mPtr;	return *this;	}
	TTIHHsBBTicketElementIteratorImpl operator--(int){auto tmp = *this;--mPtr;return tmp;}

	//	--- random access operators
	TTIHHsBBTicketElementIteratorImpl& operator+=(int32 offset)	{mPtr += offset;return *this;}
	TTIHHsBBTicketElementIteratorImpl operator+(difference_type offset) const { return TTIHHsBBTicketElementIteratorImpl(mPtr + offset); }
	TTIHHsBBTicketElementIteratorImpl& operator-=(difference_type offset) { mPtr -= offset; return *this; }
	TTIHHsBBTicketElementIteratorImpl operator-(difference_type offset) const { return TTIHHsBBTicketElementIteratorImpl(mPtr - offset); }

	//	--- const_iterator - iterator
	template<bool OtherIsConst>
	difference_type operator-(const TTIHHsBBTicketElementIteratorImpl<ElementType, OtherIsConst>& other) const
	{
		return this->operator->() - other.operator->();
	}

	//	--- access indexing operators
	reference operator[](difference_type offset) const{	return *(mPtr + offset);}

	//	--- comparison operators
	template <bool OtherIsConst>
	bool operator==(const TTIHHsBBTicketElementIteratorImpl<ElementType, OtherIsConst>& other) const {
		return this->operator->() == other.operator->();
	}

	template <bool OtherIsConst>
	bool operator!=(const TTIHHsBBTicketElementIteratorImpl<ElementType, OtherIsConst>& other) const {
		return !(*this == other);
	}

	template <bool OtherIsConst>
	bool operator<(const TTIHHsBBTicketElementIteratorImpl<ElementType, OtherIsConst>& other) const {
		return this->operator->() < other.operator->();
	}

	template <bool OtherIsConst>
	bool operator>(const TTIHHsBBTicketElementIteratorImpl<ElementType, OtherIsConst>& other) const {
		return other < *this; // < 를 사용하여 구현
	}

	template <bool OtherIsConst>
	bool operator<=(const TTIHHsBBTicketElementIteratorImpl<ElementType, OtherIsConst>& other) const {
		return !(other < *this); // < 와 == 를 사용하여 구현 (또는 !( *this > other ) )
	}

	template <bool OtherIsConst>
	bool operator>=(const TTIHHsBBTicketElementIteratorImpl<ElementType, OtherIsConst>& other) const {
		return !(*this < other); // < 를 사용하여 구현
	}
	
	// --- non-const iterator 에서 const_iterator 로의 변환 ---
	operator TTIHHsBBTicketElementIteratorImpl<ElementType, true>() const {
		return TTIHHsBBTicketElementIteratorImpl<ElementType, true>(mPtr);
	}
	// (C++17 contiguous_iterator_tag를 위한 추가 기능 - 선택 사항)
	// contiguous_iterator_tag를 사용하려면, std::to_address가 이 포인터를 반환해야 합니다.
	// C++20부터는 기본적으로 포인터 타입의 반복자에 대해 std::to_address가 잘 동작합니다.
	// 명시적으로 제공하려면 다음과 같이 할 수 있습니다. (보통은 필요 없을 수 있음)
	// #if __cplusplus >= 201703L // C++17 이상
	// friend pointer std::to_address(const TTIHHSBBTicketElementIteratorImpl& iter) noexcept requires std::is_lvalue_reference_v<reference> {
	//     return iter.mPtr;
	// }
	// #endif
};
template <typename ElementType, bool IsConst>
TTIHHsBBTicketElementIteratorImpl<ElementType, IsConst> operator+
	(
		typename TTIHHsBBTicketElementIteratorImpl<ElementType, IsConst>::difference_type		offset,
		const TTIHHsBBTicketElementIteratorImpl<ElementType, IsConst>&							iterator
	)
{
	return iterator + offset;
}

/*	------------------------------------------------------------------------------
 *	@brief 템플릿 티켓 엘리먼트 핸들러,읽기 전용 
 *	@details 컨테이너 처럼 사용가능한 티켓 핸들러
 *	@method begin() end() 를 사용하여 반복자 처럼 사용가능하다.
 *	@method Num, GetBaseElementPtr(), GetBaseElementPtrMutable(),
 */
template<typename TIHTemplateType>
class TTIHHsBBTicketElementReader : public FTIHHsBBTicketElementHandleBase
{
public:
	using value_type        = TIHTemplateType;
	using reference         = TIHTemplateType&;
	using const_reference   = TIHTemplateType const&;
	using pointer           = TIHTemplateType*;
	using const_pointer     = TIHTemplateType const*;
	using size_type         = std::size_t;
	using difference_type   = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;
	
	using const_iterator = TTIHHsBBTicketElementIteratorImpl<TIHTemplateType, true>;
	using iterator = TTIHHsBBTicketElementIteratorImpl<TIHTemplateType, true>;
	
	const_iterator begin() const
	{
		return iterator(static_cast<const TIHTemplateType*>(GetBaseElementPtr()));
	}
	const_iterator end() const
	{
		return iterator(static_cast<const TIHTemplateType*>(GetBaseElementPtr()) + Num());
	}
	iterator begin()
	{
		return iterator(static_cast<const TIHTemplateType*>(GetBaseElementPtr()));
	}
	iterator end()
	{
		return iterator(static_cast<const TIHTemplateType*>(GetBaseElementPtr())+ Num());
	}
	
	/*	----------------------------------------------------------------------------
	 *	@brief Constructor
	 *	@details 티켓 핸들로 부터 엘리먼트를 읽기 위한 생성자
	 *	@note 앞으로 이것만 사용해야함.
	 */
	TTIHHsBBTicketElementReader(const FTIHHsBBTicketHandle& inTicketHandle,FTIHHsBBTicketElement& target)
		: FTIHHsBBTicketElementHandleBase(inTicketHandle, target), mTargetBBArray(nullptr)
	{
		int32 typeId = this->mBBTicketElement->GetDataType();
		bool& valid = this->mValid;
		
		if (valid && TTIHHsDefaultTypeTrait<TIHTemplateType>::CheckType(typeId))
		{
			int32& calculateBeginIndex = this->mCalculateBeginIndex;
			int32& calculateEndIndex = this->mCalculateEndIndex;
			int32& calculateDataCount = this->mCalculateDataCount;

			mTargetBBArray = TTIHHsDefaultTypeTrait<TIHTemplateType>::GetBBArrayFromSession(mTargetBBSession);
			if (mTargetBBArray != nullptr && not mTargetBBArray->IsEmpty())
			{
				calculateBeginIndex = target.GetDataBeginOffset();
				calculateEndIndex = target.GetDataEndOffset() -1; //	less than
				if (not mTargetBBArray->IsValidIndex(calculateBeginIndex))
				{
					UE_LOG(LogTemp, Error,TEXT("TTIHHsBBElementReader::TTIHHsBBElementReader() - Invalid range: Offset=%d, Count=%d, ArraySize=%d"),
						calculateBeginIndex, target.GetDataCount(), mTargetBBArray->Num());
					SetValid(false);
				}

				if (mTargetBBArray->Num() <= calculateEndIndex)
				{
					calculateEndIndex = mTargetBBArray->Num() -1 ;
				}
				calculateDataCount = calculateEndIndex - calculateBeginIndex + 1;
				if (calculateDataCount <= 0)
				{
					UE_LOG(LogTemp, Error,TEXT("TTIHHsBBElementReader::TTIHHsBBElementReader() - Invalid range: Offset=%d, Count=%d, ArraySize=%d"),
						calculateBeginIndex, target.GetDataCount(), mTargetBBArray->Num());
					valid = false;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("TTIHHsBBTicketElementReader<TIHTemplateType>::TTIHHsBBTicketElementReader() - mTargetBBArray is not valid(nullptr or empty)"));
				SetValid(false);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error,TEXT("TTIHHsDefaultTypeTrait<TIHTemplateType>::CheckType(dataType) not same"));
			SetValid(false);
		}
	}
	
	// TTIHHsBBTicketElementReader(UTIHHsBlackBoardSession* const inBBSession, FTIHHsBBTicketElement& target)
	// 	: FTIHHsBBTicketElementHandleBase(inBBSession, target)
	// {
	// 	int32 typeId = this->mBBTicketElement->GetDataType();
	// 	bool& valid = this->mValid;
	// 	if (TTIHHsDefaultTypeTrait<TIHTemplateType>::CheckType(typeId))
	// 	{
	// 		int32& calculateBeginIndex = this->mCalculateBeginIndex;
	// 		int32& calculateEndIndex = this->mCalculateEndIndex;
	// 		int32& calculateDataCount = this->mCalculateDataCount;
	//
	// 		mTargetBBArray = TTIHHsDefaultTypeTrait<TIHTemplateType>::GetBBArrayFromSession(inBBSession);
	// 		if (mTargetBBArray != nullptr && not mTargetBBArray->IsEmpty())
	// 		{
	// 			calculateBeginIndex = target.GetDataBeginOffset();
	// 			calculateEndIndex = target.GetDataEndOffset() -1; //	less than
	// 			if (not mTargetBBArray->IsValidIndex(calculateBeginIndex))
	// 			{
	// 				UE_LOG(LogTemp, Error,TEXT("TTIHHsBBElementReader::TTIHHsBBElementReader() - Invalid range: Offset=%d, Count=%d, ArraySize=%d"),
	// 					calculateBeginIndex, target.GetDataCount(), mTargetBBArray->Num());
	// 				SetValid(false);
	// 			}
	//
	// 			if (mTargetBBArray->Num() <= calculateEndIndex)
	// 			{
	// 				calculateEndIndex = mTargetBBArray->Num() -1 ;
	// 			}
	// 			calculateDataCount = calculateEndIndex - calculateBeginIndex + 1;
	// 			if (calculateDataCount <= 0)
	// 			{
	// 				UE_LOG(LogTemp, Error,TEXT("TTIHHsBBElementReader::TTIHHsBBElementReader() - Invalid range: Offset=%d, Count=%d, ArraySize=%d"),
	// 					calculateBeginIndex, target.GetDataCount(), mTargetBBArray->Num());
	// 				valid = false;
	// 			}
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("TTIHHsBBTicketElementReader<TIHTemplateType>::TTIHHsBBTicketElementReader() - mTargetBBArray is not valid(nullptr or empty)"));
	// 			SetValid(false);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Error,TEXT("TTIHHsDefaultTypeTrait<TIHTemplateType>::CheckType(dataType) not same"));
	// 		SetValid(false);
	// 	}
	// }
	virtual int32 Num() const override
	{
		return IsAvailable() ? mCalculateDataCount : 0;
	}
	bool IsValidIndex(int32 idx, int32 targetArrayNum) const
	{
		//	큰범위 체크
		if (idx < 0 || idx >= targetArrayNum)
		{
			UE_LOG(LogTemp, Error, TEXT("GetElementPtr - Invalid index,over origin array index=%d, size=%d"), idx, targetArrayNum);
			return true;
		}
		//	작은범위 체크
		if (idx < mCalculateBeginIndex || idx > mCalculateEndIndex)
		{
			UE_LOG(LogTemp, Error, TEXT("GetElementPtr - Invalid index,over ticket element index=%d, size=%d"), idx, mCalculateDataCount);
			return true;
		}
		return false;
	}
	virtual void* GetBaseElementPtr() const override
	{
	    ensureMsgf(IsAvailable(), TEXT("GetBaseElementPtr - Reader is not valid for access (mValid is false)."));
	    ensureMsgf(mTargetBBArray != nullptr, TEXT("GetBaseElementPtr - mTargetBBArray is nullptr."));
	    ensureMsgf(mCalculateDataCount > 0, TEXT("GetBaseElementPtr - mCalculateDataCount is %d. A valid pointer to elements cannot be provided for an empty or invalid logical range."), mCalculateDataCount);
	    ensureMsgf(mTargetBBArray->IsValidIndex(mCalculateBeginIndex),TEXT("GetBaseElementPtr - mCalculateBeginIndex (%d) is out of bounds for mTargetBBArray (Num: %d)."), mCalculateBeginIndex, mTargetBBArray->Num());
	    ensureMsgf(mTargetBBArray->IsValidIndex(mCalculateEndIndex), TEXT("GetBaseElementPtr - The end of the logical slice (physical index %d) is out of bounds for mTargetBBArray (Num: %d)."),mCalculateEndIndex, mTargetBBArray->Num());
		
	    if (not IsAvailable()) return nullptr; // ensure가 non-fatal일 경우를 대비한 방어 코드
	    if (mCalculateDataCount <= 0) return nullptr;
	    if (mTargetBBArray == nullptr) return nullptr;
	    if (not mTargetBBArray->IsValidIndex(mCalculateBeginIndex)) return nullptr;
	    if (not mTargetBBArray->IsValidIndex(mCalculateEndIndex)) return nullptr;
		
	    // 모든 검사를 통과하면 논리적 0번 요소의 주소 반환
	    return &(*mTargetBBArray)[mCalculateBeginIndex];
	}
	const TIHTemplateType& operator[](int32 logicalIdx) const
	{
		static TIHTemplateType defaultValue = TTIHHsDefaultTypeTrait<TIHTemplateType>::GetDefaultValue();
		if (logicalIdx < 0  ||  Num() <= logicalIdx )
		{
			UE_LOG(LogTemp, Error, TEXT("TTIHHsBBTicketElementReader::operator[] - Invalid index: %d, valid range: [%d, %d]"), logicalIdx, mCalculateBeginIndex, mCalculateEndIndex);
			return defaultValue; // ★ 안전한 기본값 참조 반환
		}
		void* rawBasePtr = GetBaseElementPtr();
		
		if (rawBasePtr == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("TTIHHsBBTicketElementReader::operator[] - GetBaseElementPtr() returned nullptr unexpectedly. Returning default."));
			return defaultValue; // ★ 안전한 기본값 참조 반환
		}
		TIHTemplateType* typedBasePtr = static_cast<TIHTemplateType*>(rawBasePtr);
		return typedBasePtr[logicalIdx];
	}

	virtual bool IsReadable() const override{return true;}
	virtual ETIHHsBlackBoardProcessorType GetProcessorType() const override{return ETIHHsBlackBoardProcessorType::ETicketElement;}
	virtual ~TTIHHsBBTicketElementReader() override
	{
		mTargetBBArray = nullptr;
	}

protected:
	TArray<TIHTemplateType>* mTargetBBArray;
};// class TTIHHsBBTicketElementReader

/*	------------------------------------------------------------------------------
 *	@brief 템플릿 티켓 엘리먼트 핸들러,쓰기 가능
 *	@details 컨테이너 처럼 사용가능한 티켓 핸들러
 *	@method begin() end() 를 사용하여 반복자 처럼 사용가능하다.
 *	@method Num, GetBaseElementPtr(), GetBaseElementPtrMutable(),
 */
template<typename TIHTemplateType>
class TTIHHsBBTicketElementWriter : public FTIHHsBBTicketElementHandleBase
{
public:
	using value_type        = TIHTemplateType;
	using reference         = TIHTemplateType&;
	using const_reference   = TIHTemplateType const&;
	using pointer           = TIHTemplateType*;
	using const_pointer     = TIHTemplateType const*;
	using size_type         = std::size_t;
	using difference_type   = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;
	
	using const_iterator = TTIHHsBBTicketElementIteratorImpl<TIHTemplateType, true>;
	using iterator = TTIHHsBBTicketElementIteratorImpl<TIHTemplateType, false>;
	const_iterator cbegin() const
	{
		const TIHTemplateType* base = static_cast<const TIHTemplateType*>(GetBaseElementPtr()); // const 버전 호출
		return base ? const_iterator(base) : const_iterator(nullptr);
	}
	const_iterator cend() const
	{
		const TIHTemplateType* base = static_cast<const TIHTemplateType*>(GetBaseElementPtr());
		return base ? const_iterator(base + Num()) : const_iterator(nullptr);
	}
	iterator begin()
	{
		TIHTemplateType* base = static_cast<TIHTemplateType*>(GetBaseElementPtrMutable());
		return base ? iterator(base) : iterator(nullptr);
	}
	iterator end()
	{
		TIHTemplateType* base = static_cast<TIHTemplateType*>(GetBaseElementPtrMutable());
		return base ? iterator(base + Num()) : iterator(nullptr);
	}
	TTIHHsBBTicketElementWriter(const FTIHHsBBTicketHandle& inTicketHandle,FTIHHsBBTicketElement& target)
		: FTIHHsBBTicketElementHandleBase(inTicketHandle, target), mTargetBBArray(nullptr)
	{
		int32 typeId = this->mBBTicketElement->GetDataType();
		bool& valid = this->mValid;
		if (valid && TTIHHsDefaultTypeTrait<TIHTemplateType>::CheckType(typeId))
		{
			int32& calculateBeginIndex = this->mCalculateBeginIndex;
			int32& calculateEndIndex = this->mCalculateEndIndex;
			int32& calculateDataCount = this->mCalculateDataCount;

			mTargetBBArray = TTIHHsDefaultTypeTrait<TIHTemplateType>::GetBBArrayFromSession(mTargetBBSession);
			if (mTargetBBArray != nullptr && not mTargetBBArray->IsEmpty())
			{
				calculateBeginIndex = target.GetDataBeginOffset();
				calculateEndIndex = target.GetDataEndOffset() -1; //	less than
				if (not mTargetBBArray->IsValidIndex(calculateBeginIndex))
				{
					UE_LOG(LogTemp, Error,TEXT("TTIHHsBBTicketElementWriter::TTIHHsBBTicketElementWriter() - Invalid range: Offset=%d, Count=%d, ArraySize=%d"),
						calculateBeginIndex, target.GetDataCount(), mTargetBBArray->Num());
					SetValid(false);
				}

				if (mTargetBBArray->Num() <= calculateEndIndex)
				{
					calculateEndIndex = mTargetBBArray->Num() -1 ;
				}
				calculateDataCount = calculateEndIndex - calculateBeginIndex + 1;
				if (calculateDataCount <= 0)
				{
					UE_LOG(LogTemp, Error,TEXT("TTIHHsBBTicketElementWriter::TTIHHsBBTicketElementWriter() - Invalid range: Offset=%d, Count=%d, ArraySize=%d"),
						calculateBeginIndex, target.GetDataCount(), mTargetBBArray->Num());
					valid = false;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("TTIHHsBBTicketElementWriter<TIHTemplateType>::TTIHHsBBTicketElementWriter() - mTargetBBArray is not valid(nullptr or empty)"));
				SetValid(false);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error,TEXT("TTIHHsBBTicketElementWriter TTIHHsDefaultTypeTrait<TIHTemplateType>::CheckType(dataType) not same"));
			SetValid(false);
		}
	}
	
	// TTIHHsBBTicketElementWriter(UTIHHsBlackBoardSession* const inBBSession, FTIHHsBBTicketElement& target)
	// 	: FTIHHsBBTicketElementHandleBase(inBBSession, target)
	// {
	// 	int32 typeId = this->mBBTicketElement->GetDataType();
	// 	bool& valid = this->mValid;
	// 	if (valid && TTIHHsDefaultTypeTrait<TIHTemplateType>::CheckType(typeId))
	// 	{
	// 		int32& calculateBeginIndex = this->mCalculateBeginIndex;
	// 		int32& calculateEndIndex = this->mCalculateEndIndex;
	// 		int32& calculateDataCount = this->mCalculateDataCount;
	//
	// 		mTargetBBArray = TTIHHsDefaultTypeTrait<TIHTemplateType>::GetBBArrayFromSession(inBBSession);
	// 		if (mTargetBBArray != nullptr && not mTargetBBArray->IsEmpty())
	// 		{
	// 			calculateBeginIndex = target.GetDataBeginOffset();
	// 			calculateEndIndex = target.GetDataEndOffset() -1; //	less than
	// 			if (not mTargetBBArray->IsValidIndex(calculateBeginIndex))
	// 			{
	// 				UE_LOG(LogTemp, Error,TEXT("TTIHHsBBTicketElementWriter::TTIHHsBBTicketElementWriter() - Invalid range: Offset=%d, Count=%d, ArraySize=%d"),
	// 					calculateBeginIndex, target.GetDataCount(), mTargetBBArray->Num());
	// 				SetValid(false);
	// 			}
	//
	// 			if (mTargetBBArray->Num() <= calculateEndIndex)
	// 			{
	// 				calculateEndIndex = mTargetBBArray->Num() -1 ;
	// 			}
	// 			calculateDataCount = calculateEndIndex - calculateBeginIndex + 1;
	// 			if (calculateDataCount <= 0)
	// 			{
	// 				UE_LOG(LogTemp, Error,TEXT("TTIHHsBBTicketElementWriter::TTIHHsBBTicketElementWriter() - Invalid range: Offset=%d, Count=%d, ArraySize=%d"),
	// 					calculateBeginIndex, target.GetDataCount(), mTargetBBArray->Num());
	// 				valid = false;
	// 			}
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("TTIHHsBBTicketElementWriter<TIHTemplateType>::TTIHHsBBTicketElementWriter() - mTargetBBArray is not valid(nullptr or empty)"));
	// 			SetValid(false);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Error,TEXT("TTIHHsBBTicketElementWriter TTIHHsDefaultTypeTrait<TIHTemplateType>::CheckType(dataType) not same"));
	// 		SetValid(false);
	// 	}
	// }
	virtual int32 Num() const override
	{
		return IsAvailable() ? mCalculateDataCount : 0;
	}
	bool IsValidIndex(int32 idx, int32 targetArrayNum) const
	{
		//	큰범위 체크
		if (idx < 0 || idx >= targetArrayNum)
		{
			UE_LOG(LogTemp, Error, TEXT("GetElementPtr - Invalid index,over origin array index=%d, size=%d"), idx, targetArrayNum);
			return true;
		}
		//	작은범위 체크
		if (idx < mCalculateBeginIndex || idx > mCalculateEndIndex)
		{
			UE_LOG(LogTemp, Error, TEXT("GetElementPtr - Invalid index,over ticket element index=%d, size=%d"), idx, mCalculateDataCount);
			return true;
		}
		return false;
	}
	virtual void* GetBaseElementPtr() const override
	{
	    ensureMsgf(IsAvailable(), TEXT("GetBaseElementPtr - Reader is not valid for access (mValid is false)."));
	    ensureMsgf(mTargetBBArray != nullptr, TEXT("GetBaseElementPtr - mTargetBBArray is nullptr."));
	    ensureMsgf(mCalculateDataCount > 0, TEXT("GetBaseElementPtr - mCalculateDataCount is %d. A valid pointer to elements cannot be provided for an empty or invalid logical range."), mCalculateDataCount);
	    ensureMsgf(mTargetBBArray->IsValidIndex(mCalculateBeginIndex),TEXT("GetBaseElementPtr - mCalculateBeginIndex (%d) is out of bounds for mTargetBBArray (Num: %d)."), mCalculateBeginIndex, mTargetBBArray->Num());
	    ensureMsgf(mTargetBBArray->IsValidIndex(mCalculateEndIndex), TEXT("GetBaseElementPtr - The end of the logical slice (physical index %d) is out of bounds for mTargetBBArray (Num: %d)."),mCalculateEndIndex, mTargetBBArray->Num());
		
	    if (not IsAvailable()) return nullptr; // ensure가 non-fatal일 경우를 대비한 방어 코드
	    if (mCalculateDataCount <= 0) return nullptr;
	    if (mTargetBBArray == nullptr) return nullptr;
	    if (not mTargetBBArray->IsValidIndex(mCalculateBeginIndex)) return nullptr;
	    if (not mTargetBBArray->IsValidIndex(mCalculateEndIndex)) return nullptr;
		
	    // 모든 검사를 통과하면 논리적 0번 요소의 주소 반환
	    return &(*mTargetBBArray)[mCalculateBeginIndex];
	}
	const TIHTemplateType& operator[](int32 logicalIdx) const
	{
		static TIHTemplateType defaultValue = TTIHHsDefaultTypeTrait<TIHTemplateType>::GetDefaultValue();
		if (logicalIdx < mCalculateBeginIndex ||  Num() <= logicalIdx )
		{
			UE_LOG(LogTemp, Error, TEXT("TTIHHsBBTicketElementReader::operator[] - Invalid index: %d, valid range: [%d, %d]"), logicalIdx, mCalculateBeginIndex, mCalculateEndIndex);
			return defaultValue; // ★ 안전한 기본값 참조 반환
		}
		void* rawBasePtr = GetBaseElementPtr();
		
		if (rawBasePtr == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("TTIHHsBBTicketElementReader::operator[] - GetBaseElementPtr() returned nullptr unexpectedly. Returning default."));
			return defaultValue; // ★ 안전한 기본값 참조 반환
		}
		TIHTemplateType* typedBasePtr = static_cast<TIHTemplateType*>(rawBasePtr);
		return typedBasePtr[logicalIdx];
	}
	TIHTemplateType& operator[](int32 logicalIdx)
	{
		// 1. 논리적 인덱스 범위 검사 (매우 중요!)
		//    프로그램 비정상 종료를 막기 위해 check 사용 또는 엄격한 오류 처리 필요
		checkf(logicalIdx >= 0 && logicalIdx < Num(), 
			   TEXT("TTIHHsBBTicketElementWriter::operator[] - Logical index %d is out of bounds (0 to %d)."), 
			   logicalIdx, Num() - 1);

		// 2. 수정 가능한 기본 포인터 가져오기
		TIHTemplateType* typedBasePtr = static_cast<TIHTemplateType*>(GetBaseElementPtrMutable());
        
		// 3. GetBaseElementPtrMutable()가 ensure/check를 통해 nullptr이 아님을 보장한다고 가정
		//    만약 그럼에도 nullptr 가능성이 있다면, 여기서도 checkf 필요
		checkf(typedBasePtr != nullptr, TEXT("TTIHHsBBTicketElementWriter::operator[] - GetBaseElementPtrMutable() returned nullptr."));

		return typedBasePtr[logicalIdx];
	}
protected:
	virtual void* GetBaseElementPtrMutable() override
	{
		ensureMsgf(IsAvailable(), TEXT("GetBaseElementPtr - Reader is not valid for access (mValid is false)."));
		if (not IsAvailable()) return nullptr; // ensure가 non-fatal일 경우를 대비한 방어 코드
		ensureMsgf(mTargetBBArray != nullptr, TEXT("GetBaseElementPtr - mTargetBBArray is nullptr."));
		if (mTargetBBArray == nullptr) return nullptr;
		ensureMsgf(mCalculateDataCount > 0, TEXT("GetBaseElementPtr - mCalculateDataCount is %d. A valid pointer to elements cannot be provided for an empty or invalid logical range."), mCalculateDataCount);
		if (mCalculateDataCount <= 0) return nullptr;
		ensureMsgf(mTargetBBArray->IsValidIndex(mCalculateBeginIndex),TEXT("GetBaseElementPtr - mCalculateBeginIndex (%d) is out of bounds for mTargetBBArray (Num: %d)."), mCalculateBeginIndex, mTargetBBArray->Num());
		if (not mTargetBBArray->IsValidIndex(mCalculateBeginIndex)) return nullptr;
		ensureMsgf(mTargetBBArray->IsValidIndex(mCalculateEndIndex), TEXT("GetBaseElementPtr - The end of the logical slice (physical index %d) is out of bounds for mTargetBBArray (Num: %d)."),mCalculateEndIndex, mTargetBBArray->Num());
		if (not mTargetBBArray->IsValidIndex(mCalculateEndIndex)) return nullptr;

		return &(*mTargetBBArray)[mCalculateBeginIndex];
	}

public:
	virtual bool IsWriteable() const override{return true;}
	virtual ETIHHsBlackBoardProcessorType GetProcessorType() const override{return ETIHHsBlackBoardProcessorType::ETicketElement;}
	virtual ~TTIHHsBBTicketElementWriter() override
	{
		mTargetBBArray = nullptr;
	}

protected:
	TArray<TIHTemplateType>* mTargetBBArray;
};	//	TTIHHsBBTicketElementWriter

/*	-----------------------------------------------------------------------------
 *	@brief TTIHHsBBTicketElementWriter와 TTIHHsBBTicketElementReader의 컨트롤 뷰
 *	@details FTIHHsBBTicketElementHandleBase 이거를 컨트롤 하는것이다. TTIHHsBBTicketElementIteratorImpl와 다른점은 이건 FTIHHsBBTicketElementHandleBase를 캐스팅한것을 컨트롤 하는것이다.
 *	@note FTIHHsBBTicketReader, FTIHHsBBTicketWriter 에서 사용한다.
 */
template<typename TIHTemplateType,bool IsWriter = false >
class TTIHHsBBTicketElementHandleView  {
public:
    // 작성자 뷰인지 읽기 전용 뷰인지에 따라 실제 노출할 핸들 타입 결정
    using HandleType = std::conditional_t<IsWriter, TTIHHsBBTicketElementWriter<TIHTemplateType>, TTIHHsBBTicketElementReader<TIHTemplateType>>;
    // const로 접근할 때는 작성자 뷰에서도 읽기 전용 핸들 타입을 노출
    using ConstHandleType = TTIHHsBBTicketElementReader<TIHTemplateType>;

    // --- 컨테이너 타입 별칭 (이제 핸들 타입을 가리킴) ---
    using value_type = HandleType; // 반복 시 얻게 되는 요소의 타입 (non-const)
    using reference = HandleType&; // non-const 접근 시 얻는 참조 타입
    using const_reference = const ConstHandleType&; // const 접근 시 얻는 참조 타입
    using pointer = HandleType*; // non-const 접근 시 얻는 포인터 타입
    using const_pointer = const ConstHandleType*; // const 접근 시 얻는 포인터 타입
    using size_type = int32; // TArray 크기 타입 사용
    using difference_type = std::ptrdiff_t;

    // 내부적으로 FTIHHsBBTicketElementHandleBase* 배열에 대한 참조 유지
	
    using HandleArray = std::conditional_t<IsWriter, TArray<FTIHHsBBTicketElementHandleBase*>, TArray<FTIHHsBBTicketElementHandleBase*> const>;
    HandleArray& mActualArrayViewed;

    // 생성자
    explicit TTIHHsBBTicketElementHandleView(HandleArray& arr) : mActualArrayViewed(arr) {}

    // 유효성 체크 (예: 유효한 배열 기반인지)
    bool IsValid() const { return mActualArrayViewed.GetData() != nullptr; }

    // --- 반복자 구현 ---
    template <bool IteratorIsConst>
    class InternalIterator
    {
    public:
       // 반복자 역참조 시 얻게 되는 타입: 적절한 핸들 타입에 대한 참조
       using ResolvedHandleType = std::conditional_t<IteratorIsConst, const ConstHandleType, HandleType>;
       using value_type = std::remove_cv_t<ResolvedHandleType>; // iterator_traits를 위한 non-const value type
       using reference = ResolvedHandleType&;
       using pointer = ResolvedHandleType*;

       using difference_type   = std::ptrdiff_t;
       using iterator_category = std::random_access_iterator_tag;

    private:
       // mActualArrayViewed 배열 내부의 포인터에 대한 포인터
       
    	using InternalPtrPtrType = std::conditional_t<
				   IteratorIsConst,
				   FTIHHsBBTicketElementHandleBase *const *, // const iterator는 const 포인터에 대한 포인터를 가짐
				   FTIHHsBBTicketElementHandleBase** // non-const iterator는 non-const 포인터에 대한 포인터를 가짐
			   >;
    	InternalPtrPtrType mCurrentHandleBasePtrPtr; // 내부 포인터 타입 변경
    public:
       // 생성자: 이제 올바르게 const-qualified된 포인터 타입을 받습니다.
       InternalIterator() : mCurrentHandleBasePtrPtr(nullptr) {}
       explicit InternalIterator (InternalPtrPtrType p) : mCurrentHandleBasePtrPtr(p) {}

       // --- 접근 연산자 ---
       // Dereferencing yields a reference to the correct derived handle type
       reference operator*() const {
          FTIHHsBBTicketElementHandleBase* baseHandle = *mCurrentHandleBasePtrPtr; // InternalPtrPtrType을 통해 베이스 포인터 획득
          check(baseHandle != nullptr);
          // 적절한 파생 핸들 타입 포인터로 캐스팅 (Reader 또는 Writer). constness는 여기서 적용됩니다.
          return *static_cast<ResolvedHandleType*>(baseHandle);
       }

       pointer operator->() const {
          FTIHHsBBTicketElementHandleBase* baseHandle = *mCurrentHandleBasePtrPtr; // InternalPtrPtrType을 통해 베이스 포인터 획득
          check(baseHandle != nullptr);
          return static_cast<ResolvedHandleType*>(baseHandle);
       }
       // ... (increment/decrement/arithmetic/comparison operators - mCurrentHandleBasePtrPtr 사용) ...
       InternalIterator& operator++() { ++mCurrentHandleBasePtrPtr; return *this; }
       InternalIterator operator++(int) { auto tmp = *this; ++mCurrentHandleBasePtrPtr; return tmp; }
        InternalIterator& operator--() { --mCurrentHandleBasePtrPtr; return *this; }
        InternalIterator operator--(int) { auto tmp = *this; --mCurrentHandleBasePtrPtr; return tmp; }
        InternalIterator& operator+=(difference_type offset) { mCurrentHandleBasePtrPtr += offset; return *this; }
        InternalIterator operator+(difference_type offset) const { return InternalIterator(mCurrentHandleBasePtrPtr + offset); }
        InternalIterator& operator-=(difference_type offset) { mCurrentHandleBasePtrPtr -= offset; return *this; }
        InternalIterator operator-(difference_type offset) const { return InternalIterator(mCurrentHandleBasePtrPtr - offset); }


       // Difference between iterators (potentially different constness)
       template <bool OtherIsConst>
       friend difference_type operator-(
          const InternalIterator<IteratorIsConst>& lhs, // Use enclosing iterator template param
          const InternalIterator<OtherIsConst>& rhs); // Use friend iterator template param

        // Friend class declaration
        template <bool OtherIsConst>
        friend class TTIHHsBBTicketElementHandleView<TIHTemplateType, IsWriter>::InternalIterator;

       // comparison operators (using mCurrentHandleBasePtrPtr directly)
       template <bool OtherIsConst>
       bool operator==(const InternalIterator<OtherIsConst>& other) const { return mCurrentHandleBasePtrPtr == other.mCurrentHandleBasePtrPtr; }
        template <bool OtherIsConst>
       bool operator!=(const InternalIterator<OtherIsConst>& other) const { return !(*this == other); }
       template <bool OtherIsConst>
       bool operator<(const InternalIterator<OtherIsConst>& other) const { return mCurrentHandleBasePtrPtr < other.mCurrentHandleBasePtrPtr; }
        template <bool OtherIsConst>
       bool operator>(const InternalIterator<OtherIsConst>& other) const { return other < *this; } // Use <
        template <bool OtherIsConst>
       bool operator<=(const InternalIterator<OtherIsConst>& other) const { return !(other < *this); } // Use <
        template <bool OtherIsConst>
       bool operator>=(const InternalIterator<OtherIsConst>& other) const { return !(*this < other); } // Use <


       // Implicit conversion from non-const to const iterator
       operator InternalIterator<true>() const {
          // FTIHHsBBTicketElementHandleBase** 에서 FTIHHsBBTicketElementHandleBase *const * 로의 변환은 안전합니다.
          return InternalIterator<true>(mCurrentHandleBasePtrPtr);
       }
    }; // InternalIterator 끝

    // --- 뷰를 위한 반복자 타입 별칭 ---
    using iterator = InternalIterator<false>; // non-const 뷰는 HandleType에 대한 반복자 제공
    using const_iterator = InternalIterator<true>;  // const 뷰는 ConstHandleType에 대한 반복자 제공

    // --- 반복자 메서드 ---
    iterator begin() {
       auto data = mActualArrayViewed.GetData();
       return data ? iterator(data) : iterator(nullptr);
    }
    iterator end() {
    	
    	
	    if (auto data = mActualArrayViewed.GetData())
    	{
    		// 데이터를 얻었다면, 올바른 끝 반복자 반환 (const_iterator는 InternalIterator<true>)
    		return iterator(data + mActualArrayViewed.Num());
    	}
    	else
    	{
    		// 데이터를 얻지 못했다면 (빈 배열), nullptr을 기반으로 하는 끝 반복자 반환
    		return iterator(nullptr);
    	}
      
    }

    const_iterator begin() const {
       auto data = mActualArrayViewed.GetData();
    	
       return data ? const_iterator(data) : const_iterator(nullptr);
    }
    const_iterator end() const {
	    if (const auto data = mActualArrayViewed.GetData())
    	{
    		// 데이터를 얻었다면, 올바른 끝 반복자 반환 (const_iterator는 InternalIterator<true>)
    		return const_iterator(data + mActualArrayViewed.Num());
    	}
    	else
    	{
    		// 데이터를 얻지 못했다면 (빈 배열), nullptr을 기반으로 하는 끝 반복자 반환
    		return const_iterator(nullptr);
    	}
    }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    // --- 요소 접근 (operator[]) ---
    // [] 연산자 호출 시 핸들 객체에 대한 참조 반환
    reference operator[](size_type idx) { // non-const 접근 (Writer 뷰)
       check(mActualArrayViewed.IsValidIndex(idx)); // 유효한 인덱스인지 확인
       FTIHHsBBTicketElementHandleBase* baseHandlePtr = mActualArrayViewed[idx];
       check(baseHandlePtr != nullptr); // 베이스 포인터가 nullptr이 아닌지 확인
       // 베이스 포인터를 non-const HandleType 포인터로 캐스팅
       return *static_cast<HandleType*>(baseHandlePtr);
    }

    const_reference operator[](size_type idx) const { // const 접근 (Reader 뷰 또는 Writer 뷰의 const 접근)
       check(mActualArrayViewed.IsValidIndex(idx)); // 유효한 인덱스인지 확인
       FTIHHsBBTicketElementHandleBase* baseHandlePtr = mActualArrayViewed[idx];
       check(baseHandlePtr != nullptr); // 베이스 포인터가 nullptr이 아닌지 확인
       // 베이스 포인터를 const ConstHandleType 포인터로 캐스팅
       return *static_cast<const ConstHandleType*>(baseHandlePtr);
    }

    // --- 크기 및 유효성 ---
    bool empty() const { return mActualArrayViewed.IsEmpty(); }
    size_type size() const { return mActualArrayViewed.Num(); }
	int32 Num() const { return mActualArrayViewed.Num(); }
    bool IsValidIndex(size_type idx) const { return mActualArrayViewed.IsValidIndex(idx); }
};

// 클래스 템플릿 바깥에 operator- 정의
// 여기서 <typename TIHTemplateType, bool IsWriter, bool OC1, bool OC2> 는 이 함수 템플릿 자체의 파라미터입니다.
template <typename TIHTemplateType, bool IsWriter, bool OC1, bool OC2>
typename TTIHHsBBTicketElementHandleView<TIHTemplateType, IsWriter>::template InternalIterator<OC1>::difference_type operator-(
	const typename TTIHHsBBTicketElementHandleView<TIHTemplateType, IsWriter>::template InternalIterator<OC1>& lhs,
	const typename TTIHHsBBTicketElementHandleView<TIHTemplateType, IsWriter>::template InternalIterator<OC2>& rhs)
{
	// lhs와 rhs는 InternalIterator의 인스턴스이며, 이들의 mCurrentHandleBasePtrPtr 멤버를 통해 차이 계산
	// lhs와 rhs의 구체적인 타입은 TTIHHsBBTicketElementHandleView<TIHTemplateType, IsWriter>::InternalIterator<OC1> 와
	// TTIHHsBBTicketElementHandleView<TIHTemplateType, IsWriter>::InternalIterator<OC2> 입니다.
	return lhs.mCurrentHandleBasePtrPtr - rhs.mCurrentHandleBasePtrPtr;
}

class FTIHHsBBTicketHandlerBase: public FTIHHsBBRWBase
{
protected:
	TMap<int16, TArray<FTIHHsBBTicketElementHandleBase*>> mTicketElements;
public:
	FTIHHsBBTicketHandlerBase(const FTIHHsBBTicketHandle& inTicketHandle,FTIHHsBBTicketElement& target) = delete;	//	티켓핸들러에서는 엘리먼트 타켓은 필요없다.
	
	explicit FTIHHsBBTicketHandlerBase(const FTIHHsBBTicketHandle& inTicketHandle): FTIHHsBBRWBase(inTicketHandle)
	{}
	
	// FTIHHsBBTicketHandlerBase(FTIHHsBBTicket& inTicket):
	// 	FTIHHsBBRWBase(inTicket),
	// 	mTicketElements()
	// {
	// }
	
	virtual ~FTIHHsBBTicketHandlerBase() override = 0
	{
		
	};
};

/*	-----------------------------------------------------------------------------
 *	@brief 티켓 핸들러,읽기 전용
 *	@details 티켓 핸들러는 읽기 전용으로 사용된다.
 *	@note 엘리먼트 핸들러를 포함한다.
 */
class FTIHHsBBTicketReader :public FTIHHsBBTicketHandlerBase
{
	//	TMap<int16, TArray<FTIHHsBBTicketElementHandleBase*>> mTicketElements;
public:
	explicit FTIHHsBBTicketReader(const FTIHHsBBTicketHandle& inTicketHandle): FTIHHsBBTicketHandlerBase(inTicketHandle)
	{
		if (this->mValid)
		{
			if (mTargetBBSession->IsBBSessionRunning())
			{
				for (FTIHHsBBTicketElement& bbTicketElement : mBBTicket->GetTicketElements().Get())
				{
					FTIHHsBBTicketElementHandleBase* element = FTIHHsDefaultTypeTraitHelper::CreateBBTicketElementReader(inTicketHandle, bbTicketElement);	//	reader 와 writer 가 다른건 이것뿐
					if (element == nullptr )
					{
						UE_LOG(LogTemp,Warning,TEXT("reader is nullptr"));
						SetValid(false);
					}
					else if (element->IsAvailable())
					{
						UE_LOG(LogTemp,Log,TEXT("reader is valid"));
						mTicketElements.FindOrAdd(bbTicketElement.GetDataType()).Add(element);
					}
					else
					{
						UE_LOG(LogTemp,Warning,TEXT("reader is nullptr and not valid"));
						SetValid(false);
					}	
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error,TEXT("TTIHHsBBTicketTicketRWBase::TTIHHsBBTicketTicketRWBase()"));
		}
	}

	// explicit FTIHHsBBTicketReader(FTIHHsBBTicket& inTicket)
	// 	: FTIHHsBBTicketHandlerBase(inTicket)
	// {
	// 	UTIHHsBlackBoardSubsystem* bbSystem = UTIHHsBlackBoardSubsystem::GetBBSystem();
	// 	if (bbSystem != nullptr)
	// 	{
	// 		int16 sessionId = mBBTicket->GetSessionID();
	// 		UTIHHsBlackBoardSession* bbSession = bbSystem->GetBlackBoardSessionById(sessionId);
	// 		if (bbSession != nullptr && bbSession->IsSessionOpen())
	// 		{
	// 			for (FTIHHsBBTicketElement& bbTicketElement : mBBTicket->GetTicketElements().Get())
	// 			{
	// 				FTIHHsBBTicketElementHandleBase* element = FTIHHsDefaultTypeTraitHelper::CreateBBTicketElementReader(bbSession, bbTicketElement);
	// 				if (element == nullptr )
	// 				{
	// 					UE_LOG(LogTemp,Warning,TEXT("reader is nullptr"));
	// 					SetValid(false);
	// 				}
	// 				else if (element->IsValidForAccess())
	// 				{
	// 					UE_LOG(LogTemp,Log,TEXT("reader is valid"));
	// 					mTicketElements.FindOrAdd(bbTicketElement.GetDataType()).Add(element);
	// 				}
	// 				else
	// 				{
	// 					UE_LOG(LogTemp,Warning,TEXT("reader is nullptr and not valid"));
	// 					SetValid(false);
	// 				}	
	// 			}
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogTemp, Error,TEXT("TTIHHsBBTicketTicketRWBase::TTIHHsBBTicketTicketRWBase() bbSession is nullptr"));
	// 			SetValid(false);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Error,TEXT("bbSystem is nullptr"));
	// 		SetValid(false);
	// 	}
	// }
	
	virtual ~FTIHHsBBTicketReader() override;
	virtual bool IsWriteable() const override{return false;}
	virtual bool IsReadable() const override{return true;}
	virtual ETIHHsBlackBoardProcessorType GetProcessorType() const override{return ETIHHsBlackBoardProcessorType::EKnown;}

	template<typename TIHTemplateType>
	auto Elements() const -> TTIHHsBBTicketElementHandleView<TIHTemplateType>
	{
		int16 typeId = static_cast<int16>(TTIHHsDefaultTypeTrait<TIHTemplateType>::GetType());
		if (const TArray<FTIHHsBBTicketElementHandleBase*>* elements = mTicketElements.Find(typeId))
		{
			return TTIHHsBBTicketElementHandleView<TIHTemplateType>(*elements);
		}
		else
		{
			UE_LOG(LogTemp, Error,TEXT("Elements() - No elements found for typeId %d"), typeId);
	       static TArray<FTIHHsBBTicketElementHandleBase*> emptyArray;
	       return TTIHHsBBTicketElementHandleView<TIHTemplateType>(emptyArray);
		}
	}
	//	TMap<int16, TArray<FTIHHsBBTicketElementHandleBase*>> mTicketElements;
	const TArray<FTIHHsBBTicketElementHandleBase*>& operator[](ETIHHsBlackBoardDataType inTypeId) const
	{
		static TArray<FTIHHsBBTicketElementHandleBase*> emptyArray;
		const TArray<FTIHHsBBTicketElementHandleBase*>* reValue = mTicketElements.Find(static_cast<int16>(inTypeId));
		return reValue ? *reValue : emptyArray;
	}
	const TArray<FTIHHsBBTicketElementHandleBase*>& operator[](int32 index) const
	{
		static TArray<FTIHHsBBTicketElementHandleBase*> emptyArray;
		TArray<int16> keys;
		int32 count = mTicketElements.GetKeys(keys);
		if (not keys.IsValidIndex(index))
		{
			UE_LOG(LogTemp, Error,TEXT("operator[] - Invalid index: %d, valid range: [0, %d]"), index, count - 1);
			return emptyArray;
		}
		const TArray<FTIHHsBBTicketElementHandleBase*>* reValue = mTicketElements.Find(keys[index]);
		return reValue ? *reValue : emptyArray;
	}
	
	int32 Num() const
	{
		return mTicketElements.Num();
	}
};

/*	-----------------------------------------------------------------------------
 *	@brief 티켓 핸들러,쓰기 가능
 *	@details 티켓 핸들러는 쓰기를 위한 기능이다.
 *	@note 엘리먼트 핸들러를 포함한다.
 */
class FTIHHsBBTicketWriter: public FTIHHsBBTicketHandlerBase
{
	//	mTicketElements
public:
	explicit FTIHHsBBTicketWriter(const FTIHHsBBTicketHandle& inTicketHandle)
		: FTIHHsBBTicketHandlerBase(inTicketHandle)
	{
		if (this->mValid)
		{
			if (mTargetBBSession->IsBBSessionRunning())
			{
				for (FTIHHsBBTicketElement& bbTicketElement : mBBTicket->GetTicketElements().Get())
				{
					FTIHHsBBTicketElementHandleBase* element = FTIHHsDefaultTypeTraitHelper::CreateBBTicketElementWriter(inTicketHandle, bbTicketElement);
					if (element == nullptr )
					{
						UE_LOG(LogTemp,Warning,TEXT("writer is nullptr"));
						SetValid(false);
					}
					else if (element->IsAvailable())
					{
						UE_LOG(LogTemp,Log,TEXT("writer is valid"));
						mTicketElements.FindOrAdd(bbTicketElement.GetDataType()).Add(element);
					}
					else
					{
						UE_LOG(LogTemp,Warning,TEXT("writer is nullptr and not valid"));
						SetValid(false);
					}	
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error,TEXT("TTIHHsBBTicketTicketRWBase::TTIHHsBBTicketTicketRWBase()"));
		}
	}
	
	// explicit FTIHHsBBTicketWriter(FTIHHsBBTicket& inTicketHandle)
	// 	: FTIHHsBBTicketHandlerBase(inTicketHandle)
	// {
	// 	UTIHHsBlackBoardSubsystem* bbSystem = UTIHHsBlackBoardSubsystem::GetBBSystem();
	// 	if (bbSystem != nullptr)
	// 	{
	// 		int16 sessionId = mBBTicket->GetSessionID();
	// 		UTIHHsBlackBoardSession* bbSession = bbSystem->GetBBSessionById(sessionId);
	// 		if (bbSession != nullptr && bbSession->IsBBSessionRunning())
	// 		{
	// 			for (FTIHHsBBTicketElement& bbTicketElement : mBBTicket->GetTicketElements().Get())
	// 			{
	// 				FTIHHsBBTicketElementHandleBase* element = FTIHHsDefaultTypeTraitHelper::CreateBBTicketElementReader(inTicketHandle, bbTicketElement);
	// 				if (element == nullptr )
	// 				{
	// 					UE_LOG(LogTemp,Warning,TEXT("writer is nullptr"));
	// 					SetValid(false);
	// 				}
	// 				else if (element->IsAvailable())
	// 				{
	// 					UE_LOG(LogTemp,Log,TEXT("writer is valid"));
	// 					mTicketElements.FindOrAdd(bbTicketElement.GetDataType()).Add(element);
	// 				}
	// 				else
	// 				{
	// 					UE_LOG(LogTemp,Warning,TEXT("writer is nullptr and not valid"));
	// 					SetValid(false);
	// 				}	
	// 			}
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogTemp, Error,TEXT("TTIHHsBBTicketTicketRWBase::TTIHHsBBTicketTicketRWBase() bbSession is nullptr"));
	// 			SetValid(false);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Error,TEXT("bbSystem is nullptr"));
	// 		SetValid(false);
	// 	}
	// 	// if (this->GetValid())
	// 	// {
	// 	// 	UTIHHsBlackBoardSession* bbSession = bbSystem->GetBlackBoardSessionById(mBBTicket->GetSessionID());
	// 	// 	if (bbSession)
	// 	// 	{
	// 	// 		bbSession->UseBBTicket(mBBTicket);
	// 	// 	}
	// 	// }
	// 	
	// }
	
	virtual ~FTIHHsBBTicketWriter() override;
	virtual bool IsWriteable() const override{return true;}
	virtual bool IsReadable() const override{return false;}
	virtual ETIHHsBlackBoardProcessorType GetProcessorType() const override{return ETIHHsBlackBoardProcessorType::EKnown;}

	template<typename TIHTemplateType>
	auto Elements() -> TTIHHsBBTicketElementHandleView<TIHTemplateType,true>
	{
		int16 typeId = static_cast<int16>(TTIHHsDefaultTypeTrait<TIHTemplateType>::GetType());
		if (TArray<FTIHHsBBTicketElementHandleBase*>* elements = mTicketElements.Find(typeId))
		{
			return TTIHHsBBTicketElementHandleView<TIHTemplateType,true>(*elements);
		}
		else
		{
			UE_LOG(LogTemp, Error,TEXT("Elements() - No elements found for typeId %d"), typeId);
			static TArray<FTIHHsBBTicketElementHandleBase*> emptyArray;
			return TTIHHsBBTicketElementHandleView<TIHTemplateType,true>(emptyArray);
		}
	}
};//	FTIHHsBBTicketWriter

/*	===============================================================================
 *
 *								Inline Functions
 *
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
 */

inline FTIHHsBBTicketReader::~FTIHHsBBTicketReader()
{
}

inline FTIHHsBBTicketWriter::~FTIHHsBBTicketWriter()
{
}

inline FTIHHsBBTicketElementHandleBase* FTIHHsDefaultTypeTraitHelper::CreateBBTicketElementWriter(const FTIHHsBBTicketHandle& inTicketHandle, FTIHHsBBTicketElement& targetElement)
{
	switch (targetElement.GetDataType())
	{
		TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_CREATE_WRITER_CASE)
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInvalid):
		return nullptr;
	default: return nullptr;
	}
}
inline FTIHHsBBTicketElementHandleBase* FTIHHsDefaultTypeTraitHelper::CreateBBTicketElementReader(const FTIHHsBBTicketHandle& inBBTicketHandle,FTIHHsBBTicketElement& targetElement)
{
		switch (targetElement.GetDataType())
	{
			TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_CREATE_READER_CASE)
	case static_cast<uint16>(ETIHHsBlackBoardDataType::EInvalid):
		return nullptr;
	default: return nullptr;
	}
}

inline ETIHHsBlackBoardDataType FTIHHsDefaultTypeTraitHelper::GetTypeFromInt16(int16 inDataType)
{
	ETIHHsBlackBoardDataType reValue = ETIHHsBlackBoardDataType::EInvalid;
	switch (inDataType)
	{
		TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_GET_TYPE_CASE_FROM_INT)
	default:
		break;
	}
		return reValue;
}

inline int16 FTIHHsDefaultTypeTraitHelper::GetTypeFromEnum(ETIHHsBlackBoardDataType inDataType)
{
	int16 reValue = static_cast<int16>(ETIHHsBlackBoardDataType::EInvalid);
	switch (inDataType)
	{
		TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_GET_INT_CASE_FROM_ENUM)
	default:
		break;
	}
	return reValue;
}

inline int16 FTIHHsDefaultTypeTraitHelper::GetTypeSize(int16 inDataType)
{
	int16 reValue = 0;
		switch (inDataType)
		{
			TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_GET_TYPE_SIZE_CASE)
		default:
			break;
		}
		return reValue;
}

inline FString FTIHHsDefaultTypeTraitHelper::GetTypeName(int16 inDataType)
{
	FString reValue = TEXT("Invalid");
	switch (inDataType)
	{
		TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_BB_GET_TYPE_NAME_CASE)
	default:
		break;
	}
	reValue = reValue.ToLower();
	return reValue;
}

inline ETIHHsBlackBoardDataType FTIHHsDefaultTypeTraitHelper::GetTypeFromName(FString inTypeName)
{
	ETIHHsBlackBoardDataType reValue = ETIHHsBlackBoardDataType::EInvalid;
	inTypeName = inTypeName.ToLower();
	if (inTypeName.Contains(TEXT("etiihhsblackboarddatatype::")))
	{
		inTypeName = inTypeName.Replace(TEXT("etiihhsblackboarddatatype::"), TEXT(""));
	}
	else if (inTypeName.Contains(TEXT("etiihhsblackboarddatatype")))
	{
		inTypeName = inTypeName.Replace(TEXT("etiihhsblackboarddatatype"), TEXT(""));
	}
	return reValue;
}





