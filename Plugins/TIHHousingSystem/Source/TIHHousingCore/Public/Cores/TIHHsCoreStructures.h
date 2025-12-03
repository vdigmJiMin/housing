#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "TIHHsCoreEnums.h"
#include "JsonObjectWrapper.h"
#include "PropertyBag.h"

#include "UObject/Object.h"
#include "TIHHsCoreStructures.generated.h"

struct FTIHHsTriggerVisitor;
class ITIHHsBaseObject;
class ITIHHsInteraction;
struct FJsonObjectWrapper;

namespace TIHUtils
{
	inline constexpr TCHAR GizmoTriggerPrefabTypeStr[] = TEXT("GizmoTriggerPrefab");
	inline const FPrimaryAssetType& GetGizmoTriggerPrefabType()
	{
		static FPrimaryAssetType T(GizmoTriggerPrefabTypeStr);
		/*
		 *	읽어오는걸 해야함. 
		 * 
		 */
		
		return T;
	}
	
}



/*
Tag 란 외부에서 확인하거나 할때 사용하는것이다
collisionChannel,
GameplayTagContainer
	TAG_Housing_Interactable <- TAG_Housing_LineTraceable이거와 통합. 라인트레이스가 가능하다고 봐야함.
		TAG_Housing_Hoverable
		TAG_Housing_Focusable
		TAG_Housing_Holdable <- hold 기능이 있는 놈일때
	TAG_Housing_Attachable
		TAG_Housing_Deckable
		TAG_Housing_Placementable <- 이건 배치가 가능한 놈일때
State...인데 이건 지금의 상태를 설명하고 싶음. 과정이라든가 그런거.
*			
 *			 UE_DECLARE_GAMEPLAY_TAG_EXTERN()
 *			
 * 
 */
UENUM(BlueprintType, Meta = (Bitflags))
enum class ETIHHsStateMachineNodeMaskFlag :uint8
{
	EAny = 0x00,	//	모든 노드에 대해 적용
	EClassType = 0x01,	
	EStateActor = 0x02,
	EVisualState = 0x04,	//	시각적 상태
	EProcessState = 0x08,	//	처리 상태
	EProcessStatePhase = 0x10,	//	처리 상태 단계
	EGlobalCondition = 0x20,	//	전역 조건
};
ENUM_CLASS_FLAGS(ETIHHsStateMachineNodeMaskFlag);

//
// TArray<FName> FTIHHsTags = {
// 	"Hovable",
// 	"Focusable",
// 	"LineTraceable",
// 	"Holdable",
// 	"Stackable",
// 	"Attachable",
// 	"Interactable",
// 	
// };
// 상호작용 관련 태그들
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Interactable);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Interactable_Hover);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Interactable_Focus);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Interactable_Hold);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Interactable_Tap);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Interactable_DoubleTap);

// 부착 관련 태그들
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Attachable);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Attachable_Decked);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Attachable_Decked_Gridable);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Attachable_Placed);

// 보장된 기능
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Guarantee);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Guarantee_HasInteraction);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Guarantee_HasIndividuality);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Guarantee_MaintainedMetadata);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Guarantee_HasCollision);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Guarantee_HasDeck);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Guarantee_HasPlacement);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Guarantee_HasTrigger);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Guarantee_HasStateTree);

// 분류 관련
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Classification);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Classification_Stuff);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Classification_ForBuilding);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Classification_ForInterior);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TIH_Classification_System);

// 가능한 동작을 물어볼까?

/*
 *	states
 *		ETIHHsStateVisual
*				Normal,
				Preview,
				Ghosted,
				Invalid,
				Highlighted,
				Selected,
				Hidden,
 *		ETIHHsStateActor
*			None = 0,
			User,                    // 사용자
			System,                  // 시스템
			Other,                   // 다른 객체
			Self                     // 자체
 *		ETIHHsProcessState
*			Created,				// 생성만 됨.<- 안쓸수도 있음 
			Idle,                    // 아무것도 하지 않음
			Ready,                   // 상호작용 가능 대기
		
			Initiating,
			Processing,
			Finalizing,
		
			Interacting,             // 상호작용 중
			Holding,
			Placing,                // 배치 중
		
			Paused,
			Error,
			Destoying,
 *		ETIHHsProcessStatePhase
*			None,
			Starting,
			InProgress,
			Verifying,  
			Waiting,
			Completing,
			Error,
 * 
 */

UENUM(BlueprintType)
enum class ETIHHsStateVisual : uint8
{
	Normal,
	Preview,
	Ghosted,
	Invalid,
	Highlighted,
	Selected,
	Hidden,
};
UENUM(BlueprintType)
enum class ETIHHsStateActor : uint8
{
	None = 0,
	User,                    // 사용자
	System,                  // 시스템
	Other,                   // 다른 객체
	Self                     // 자체
};
UENUM(BlueprintType)
enum class ETIHHsProcessState  : uint8
{
	Created,				// 생성만 됨.<- 안쓸수도 있음 
	Idle,                    // 아무것도 하지 않음
	Ready,                   // 상호작용 가능 대기

	Initiating,
	Processing,
	Finalizing,

	Interacting,             // 상호작용 중
	Holding,
	Placing,                // 배치 중

	Paused,
	Error,
	Destoying,
};
UENUM(BlueprintType)
enum class ETIHHsProcessStatePhase  : uint8
{
	None,
	Starting,
	InProgress,
	Verifying,  
	Waiting,
	Completing,
	Error,
};
/*
 *	flag가 필요한거같다
 *		초기화를 진행했다.
 *		
 *
 * 
 */
//
// class TIHHOUSINGCORE_API FTIHHsKeyConverter
// {
// public:
// 	template<typename T>
// 	static uint64 ToHash(const T& key)
// 	{
// 		if constexpr (std::is_same_v<T, FString>)
// 		{
// 			return GetTypeHash(key);
// 		}
// 		else if constexpr (std::is_same_v<T, FName>)
// 		{
// 			return GetTypeHash(key);
// 		}
// 		else if constexpr (std::is_same_v<T, int32>)
// 		{
// 			return static_cast<uint64>(key);
// 		}
// 		else if constexpr (std::is_same_v<T, uint32>)
// 		{
// 			return static_cast<uint64>(key);
// 		}
// 		else if constexpr (std::is_same_v<T, int64>)
// 		{
// 			return static_cast<uint64>(key);
// 		}
// 		else if constexpr (std::is_same_v<T, uint64>)
// 		{
// 			return key;
// 		}
// 		else
// 		{
// 			static_assert(false, "Unsupported key type for hashing");
// 			return 0;
// 		}
// 	}
// 	template<typename T>
//    static FString ToString(const T& Key)
// 	{
// 		if constexpr (std::is_same_v<T, FString>)
// 		{
// 			return Key;
// 		}
// 		else if constexpr (std::is_same_v<T, FName>)
// 		{
// 			return Key.ToString();
// 		}
// 		else
// 		{
// 			return FString::Printf(TEXT("%llu"), (uint64)Key);
// 		}
// 	}
// };
//
// constexpr uint64 INVALID_RULELIST_KEY = UINT64_MAX;
//
// template<typename TIHTemplateTypeKey,typename TIHTemplateTypeValue>
// class FTIHHsRuleListNodePair
// {
// private:
// 	uint64 mSelfKey;
// 	uint64 mPrevKey;
// 	uint64 mNextKey;
// 	
// 	TIHTemplateTypeKey mOriginalKey;
// 	TIHTemplateTypeValue mValue;
// public:
// 	FTIHHsRuleListNodePair(const TIHTemplateTypeKey& inKey, const TIHTemplateTypeValue& inValue);
// 	
// 	bool operator==(const FTIHHsRuleListNodePair& other) const
// 	{
// 		return mSelfKey == other.mSelfKey && mOriginalKey == other.mOriginalKey;
// 	}
// 	bool operator!=(const FTIHHsRuleListNodePair& other) const
// 	{
// 		return !(*this == other);
// 	}
// 	uint64 GetPrevKey()const { return mPrevKey; }
// 	void SetPrevKey(uint64 inPrevKey) { mPrevKey = inPrevKey; }
// 	uint64 GetNextKey()const { return mNextKey; }
// 	void SetNextKey(uint64 inNextKey) { mNextKey = inNextKey; }
//
// 	bool HasPrev()const { return mPrevKey != INVALID_RULELIST_KEY; }
// 	bool HasNext()const { return mNextKey != INVALID_RULELIST_KEY; }
// 	bool IsAlone()const { return !HasPrev() && !HasNext(); }
// 	bool IsHead()const { return !HasPrev() && HasNext(); }
// 	bool IsTail()const { return HasPrev() && !HasNext(); }
// 	FTIHHsRuleListNodePair* ClearLinks() { mPrevKey = INVALID_RULELIST_KEY; mNextKey = INVALID_RULELIST_KEY; return this; }
// 	bool IsValid()const { return mSelfKey != INVALID_RULELIST_KEY; }
// 	FTIHHsRuleListNodePair* Invalidate() { mSelfKey = INVALID_RULELIST_KEY; return this; }
// 	
// 	uint64 GetHashedSelfKey()const { return mSelfKey; }
// 	const TIHTemplateTypeKey& GetOriginalKey()const { return mOriginalKey; }
//
// 	TIHTemplateTypeValue& GetValue() { return mValue; }
// 	const TIHTemplateTypeValue& GetValue()const { return mValue; }
// 	TIHTemplateTypeValue& GetValueMutable() { return mValue; }
//
// 	template<typename TOtherKeyType>
// 	bool MatchesKey(const TOtherKeyType& otherKey) const
// 	{
// 		if constexpr (std::is_same_v<TOtherKeyType, TIHTemplateTypeKey>)
// 		{
// 			return mOriginalKey == otherKey;
// 		}
// 		else
// 		{
// 			return mSelfKey == FTIHHsKeyConverter::ToHash(otherKey);
// 		}
// 	}
// };
//
// template <typename TIHTemplateTypeKey, typename TIHTemplateTypeValue>
// FTIHHsRuleListNodePair<TIHTemplateTypeKey, TIHTemplateTypeValue>::FTIHHsRuleListNodePair(
// 	const TIHTemplateTypeKey& inKey, const TIHTemplateTypeValue& inValue): mPrevKey(INVALID_RULELIST_KEY), mNextKey(INVALID_RULELIST_KEY), mOriginalKey(inKey), mValue(inValue)
// {
// 	mSelfKey = FTIHHsKeyConverter::ToHash(mOriginalKey);
// }
//
// UCLASS()
// class TIHHOUSINGCORE_API UTIHHsRuledList : public UObject
// {
// 	GENERATED_BODY()
// 	using TIHTriggerRuleType = FTIHHsRuleListNodePair<FString,TScriptInterface<ITIHHsBaseObject>>;
// 	
// 	const uint64 CommandTopHash = FTIHHsKeyConverter::ToHash(TEXT("top"));
// 	const uint64 CommandFirstHash = FTIHHsKeyConverter::ToHash(TEXT("first"));
// 	const uint64 CommandFrontHash = FTIHHsKeyConverter::ToHash(TEXT("front"));
// 	const uint64 CommandTailHash = FTIHHsKeyConverter::ToHash(TEXT("tail"));
// 	const uint64 CommandBackHash = FTIHHsKeyConverter::ToHash(TEXT("back"));
// 	const uint64 CommandEndHash = FTIHHsKeyConverter::ToHash(TEXT("end"));
// 	const uint64 CommandVoidHash = FTIHHsKeyConverter::ToHash(TEXT(""));
// 	
// 	using RuleMapKeyType = uint64;
// 	using RuleMapIndexType = int32;
// public:
// 	// bool IsEmpty() const
// 	// {
// 	// 	return mRuledNodeArray.Num() == 0;
// 	// }
// 	// bool IsFrontCommand(const uint64& cmdKey) const
// 	// {
// 	// 	return cmdKey == CommandVoidHash || cmdKey == CommandTopHash || cmdKey == CommandFrontHash || cmdKey == CommandFirstHash;
// 	// }
// 	// void PushFront(const FString& cmdKey,TScriptInterface<ITIHHsBaseObject> inValue)
// 	// {
// 	// 	const RuleMapKeyType toHash = FTIHHsKeyConverter::ToHash(cmdKey.ToLower());
// 	// 	bool isContained = mRuledListIndexMap.Contains(toHash);
// 	// 	bool isCommanded = IsFrontCommand(toHash);
// 	//
// 	// 	if (isContained && isCommanded)
// 	// 	{
// 	// 		//	보류
// 	// 	}
// 	// 	else if (isContained && not isCommanded)
// 	// 	{
// 	// 		
// 	// 	}
// 	// 	
// 	//
// 	// 	
// 	// }
// 	// const TIHTriggerRuleType& Head() const
// 	// {
// 	// 	if (mRuledNodeArray.IsValidIndex(mHeadIndex))
// 	// 	{
// 	// 		return mRuledNodeArray[mHeadIndex];
// 	// 	}
// 	// 	return EmptyErrorRulePair;
// 	// }
// 	// const TIHTriggerRuleType& Front() const
// 	// {
// 	// 	return Head();
// 	// }
// 	// const TIHTriggerRuleType& First() const
// 	// {
// 	// 	return Head();
// 	// }
// 	// const TIHTriggerRuleType& Tail() const
// 	// {
// 	// 	if (mRuledNodeArray.IsValidIndex(mTailIndex))
// 	// 	{
// 	// 		return mRuledNodeArray[mTailIndex];
// 	// 	}
// 	// 	return EmptyErrorRulePair;
// 	// }
// 	// const TIHTriggerRuleType& Back() const
// 	// {
// 	// 	return Tail();
// 	// }
// 	// const TIHTriggerRuleType& Last() const
// 	// {
// 	// 	return Back();
// 	// }
// 	
// protected:
//
// 	RuleMapIndexType mHeadIndex;
// 	RuleMapIndexType mTailIndex;
// 	
// 	TMap<RuleMapKeyType,RuleMapIndexType> mRuledListIndexMap;
// 	TArray<TIHTriggerRuleType> mRuledNodeArray;
// 	
// 	static TIHTriggerRuleType EmptyErrorRulePair;
// };

USTRUCT(BlueprintType)
struct FTIHHsObjectState
{
	GENERATED_BODY()

	static constexpr int32 BitMaskVisual = 0xFF000000; // 8 bits for VisualState
	static constexpr int32 BitMaskActor = 0x00FF0000; // 8 bits for StateActor
	static constexpr int32 BitMaskProcessState = 0x0000FF00; // 8 bits for ProcessState
	static constexpr int32 BitMaskProcessStatePhase = 0x000000FF; // 8 bits for ProcessStatePhase

	static constexpr int32 BitShiftVisual = 24; // Shift for VisualState
	static constexpr int32 BitShiftActor = 16; // Shift for StateActor
	static constexpr int32 BitShiftProcessState = 8; // Shift for ProcessState
	static constexpr int32 BitShiftProcessStatePhase = 0; // Shift for ProcessStatePhase
	
	UPROPERTY(BlueprintReadWrite, Category = "TIHHsObjectState")
	ETIHHsStateVisual VisualState = ETIHHsStateVisual::Normal;
	UPROPERTY(BlueprintReadWrite, Category = "TIHHsObjectState")
	ETIHHsStateActor StateActor = ETIHHsStateActor::None;
	UPROPERTY(BlueprintReadWrite, Category = "TIHHsObjectState")
	ETIHHsProcessState ProcessState = ETIHHsProcessState::Idle;
	UPROPERTY(BlueprintReadWrite, Category = "TIHHsObjectState")
	ETIHHsProcessStatePhase ProcessStatePhase = ETIHHsProcessStatePhase::None;

	FTIHHsObjectState()= default;
	FTIHHsObjectState(const FTIHHsObjectState& other) = default;
	FTIHHsObjectState(FTIHHsObjectState&& other) noexcept = default;

	explicit FTIHHsObjectState(int32 bitmask)
	{
		VisualState = static_cast<ETIHHsStateVisual>((bitmask & BitMaskVisual) >> BitShiftVisual);
		StateActor = static_cast<ETIHHsStateActor>((bitmask & BitMaskActor) >> BitShiftActor);
		ProcessState = static_cast<ETIHHsProcessState>((bitmask & BitMaskProcessState) >> BitShiftProcessState);
		ProcessStatePhase = static_cast<ETIHHsProcessStatePhase>(bitmask & BitMaskProcessStatePhase);
	}
	FTIHHsObjectState(ETIHHsStateVisual InVisualState, ETIHHsProcessState InProcessState, ETIHHsProcessStatePhase InProcessStatePhase, ETIHHsStateActor InStateActor)
		: VisualState(InVisualState), StateActor(InStateActor), ProcessState(InProcessState), ProcessStatePhase(InProcessStatePhase)
	{
	}
	FTIHHsObjectState(ETIHHsProcessState InProcessState, ETIHHsProcessStatePhase InProcessStatePhase)
		: ProcessState(InProcessState), ProcessStatePhase(InProcessStatePhase)
	{
		switch (InProcessState) {
		case ETIHHsProcessState::Idle:
			VisualState = ETIHHsStateVisual::Normal;
			StateActor = ETIHHsStateActor::None;
			break;
		case ETIHHsProcessState::Ready:
			VisualState = ETIHHsStateVisual::Normal;
			StateActor = ETIHHsStateActor::User;
			break;
		default:
			VisualState = ETIHHsStateVisual::Normal;
			StateActor = ETIHHsStateActor::System;
			break;
		}
	}

	explicit FTIHHsObjectState(ETIHHsProcessState InProcessState)
		: ProcessState(InProcessState), ProcessStatePhase(ETIHHsProcessStatePhase::None)
	{
		switch (InProcessState) {
		case ETIHHsProcessState::Idle:
			VisualState = ETIHHsStateVisual::Normal;
			StateActor = ETIHHsStateActor::None;
			break;
		case ETIHHsProcessState::Ready:
			VisualState = ETIHHsStateVisual::Normal;
			StateActor = ETIHHsStateActor::User;
			break;
		default:
			VisualState = ETIHHsStateVisual::Normal;
			StateActor = ETIHHsStateActor::System;
			break;
		}
	}
	FORCEINLINE int32 GetCompact() const
	{
		return static_cast<int32>(VisualState) << BitShiftVisual|
				static_cast<int32>(StateActor) << BitShiftActor |
				static_cast<int32>(ProcessState) << BitShiftProcessState |
				static_cast<int32>(ProcessStatePhase);
	}
	FORCEINLINE void SetCompact(int32 inCompact)
	{
		VisualState = static_cast<ETIHHsStateVisual>((inCompact & BitMaskVisual) >> BitShiftVisual);
		StateActor = static_cast<ETIHHsStateActor>((inCompact & BitMaskActor) >> BitShiftActor);
		ProcessState = static_cast<ETIHHsProcessState>((inCompact & BitMaskProcessState) >> BitShiftProcessState);
		ProcessStatePhase = static_cast<ETIHHsProcessStatePhase>(inCompact & BitMaskProcessStatePhase);
	}

	FORCEINLINE FTIHHsObjectState& operator=(const FTIHHsObjectState& other)
	{
		if (this != &other)
		{
			VisualState = other.VisualState;
			ProcessState = other.ProcessState;
			ProcessStatePhase = other.ProcessStatePhase;
			StateActor = other.StateActor;
		}
		return *this;
	}
	FORCEINLINE FString ToString() const
	{
		FString jsonString = FString::Printf(
			TEXT("{ \"VisualState\": \"%s\", \"ProcessState\": \"%s\", \"ProcessStatePhase\": \"%s\", \"StateActor\": \"%s\" }"),
			*UEnum::GetValueAsString(VisualState),
			*UEnum::GetValueAsString(ProcessState),
			*UEnum::GetValueAsString(ProcessStatePhase),
			*UEnum::GetValueAsString(StateActor)
		);
		return jsonString;
	}
	FTIHHsObjectState& operator=(FTIHHsObjectState&& other) noexcept
	{
		if (this != &other)
		{
			VisualState = MoveTemp(other.VisualState);
			ProcessState = MoveTemp(other.ProcessState);
			ProcessStatePhase = MoveTemp(other.ProcessStatePhase);
			StateActor = MoveTemp(other.StateActor);
		}
		return *this;
	}
	
	FORCEINLINE bool operator==(const FTIHHsObjectState& other) const
	{
		bool result = true;
		result &= VisualState == other.VisualState;
		result &= ProcessState == other.ProcessState;
		result &= ProcessStatePhase == other.ProcessStatePhase;
		result &= StateActor == other.StateActor;
		return result;
	}
	FORCEINLINE bool operator!=(const FTIHHsObjectState& other) const	{return not(*this == other);	}
	FORCEINLINE bool operator==(const ETIHHsStateVisual& other) const	{return VisualState == other;	}
	FORCEINLINE bool operator!=(const ETIHHsStateVisual& other) const	{return VisualState != other;	}
	FORCEINLINE bool operator==(const ETIHHsProcessState& other) const	{return ProcessState == other;	}
	FORCEINLINE bool operator!=(const ETIHHsProcessState& other) const	{return ProcessState != other;	}
	FORCEINLINE bool operator==(const ETIHHsProcessStatePhase& other) const	{return ProcessStatePhase == other;	}
	FORCEINLINE bool operator!=(const ETIHHsProcessStatePhase& other) const	{return ProcessStatePhase != other;	}
	FORCEINLINE bool operator==(const ETIHHsStateActor& other) const	{	return StateActor == other;	}
	FORCEINLINE bool operator!=(const ETIHHsStateActor& other) const	{	return StateActor != other;	}
	
	FORCEINLINE bool IsValid() const
	{
		return VisualState != ETIHHsStateVisual::Invalid &&
			   ProcessState != ETIHHsProcessState::Idle &&
			   ProcessStatePhase != ETIHHsProcessStatePhase::None &&
			   StateActor != ETIHHsStateActor::None;
	}
	FORCEINLINE bool IsIdle() const
	{
		return ProcessState == ETIHHsProcessState::Idle;
	}
	FORCEINLINE bool IsReady() const
	{
		return ProcessState == ETIHHsProcessState::Ready;
	}
	FORCEINLINE bool IsInteracting() const
	{
		return ProcessState == ETIHHsProcessState::Interacting;
	}
	FORCEINLINE bool IsHolding() const
	{
		return ProcessState == ETIHHsProcessState::Holding;
	}
	FORCEINLINE bool IsPlacing() const
	{
		return ProcessState == ETIHHsProcessState::Placing;
	}
	FORCEINLINE bool IsPaused() const
	{
		return ProcessState == ETIHHsProcessState::Paused;
	}
	FORCEINLINE bool IsError() const
	{
		return ProcessState == ETIHHsProcessState::Error;
	}
	FORCEINLINE bool IsDestroying() const
	{
		return ProcessState == ETIHHsProcessState::Destoying;
	}
	FORCEINLINE bool IsValidState() const
	{
		bool result = true;
		result &= VisualState != ETIHHsStateVisual::Invalid;
		result &= VisualState != ETIHHsStateVisual::Hidden;
		result &= ProcessState != ETIHHsProcessState::Idle;
		result &= ProcessStatePhase != ETIHHsProcessStatePhase::None;
		result &= StateActor != ETIHHsStateActor::None;
		return result;
	}
	
	FORCEINLINE bool IsValidStateActor() const
	{
		return StateActor != ETIHHsStateActor::None;
	}
	FORCEINLINE bool IsValidProcessState() const
	{
		return ProcessState != ETIHHsProcessState::Idle;
	}
	FORCEINLINE bool IsValidProcessStatePhase() const
	{
		return ProcessStatePhase != ETIHHsProcessStatePhase::None;
	}
	FORCEINLINE bool IsValidVisualState() const
	{
		bool result = true;
		result &= VisualState != ETIHHsStateVisual::Invalid;
		result &= VisualState != ETIHHsStateVisual::Hidden;
		return result; 
	}
	FORCEINLINE bool IsPreview() const
	{
		bool result = true;
		result &= VisualState == ETIHHsStateVisual::Preview;
		result &= VisualState == ETIHHsStateVisual::Ghosted;
		return result;
	}
	FORCEINLINE bool IsGhosted() const
	{
		return VisualState == ETIHHsStateVisual::Ghosted;
	}
	bool CheckMask(FTIHHsObjectState other,ETIHHsStateMachineNodeMaskFlag mask) const
	{
		bool result = true;
		if ( EnumHasAnyFlags(mask,ETIHHsStateMachineNodeMaskFlag::EVisualState))
		{
			result &= other.VisualState == VisualState;
		}
		if (EnumHasAnyFlags(mask , ETIHHsStateMachineNodeMaskFlag::EStateActor))
		{
			result &= other.ProcessState == ProcessState;
		}
		if (EnumHasAnyFlags(mask , ETIHHsStateMachineNodeMaskFlag::EProcessState))
		{
			result &= other.ProcessState == ProcessState;
		}
		if (EnumHasAnyFlags(mask , ETIHHsStateMachineNodeMaskFlag::EProcessStatePhase))
		{
			result &=other.ProcessStatePhase == ProcessStatePhase;
		}
		return result;
	}
};

inline uint32 GetTypeHash(const FTIHHsObjectState& state)
{
	return static_cast<uint32>(state.GetCompact());
}

USTRUCT(BlueprintType)
struct FTIHHsObjectIndividuality
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGameplayTagContainer GamePlayTags;
	UPROPERTY()
	FTIHHsObjectState ObjectState;
	UPROPERTY()
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	UPROPERTY()
	int8 AdditionalObjectFlag[3] = {0, 0, 0}; // 0: None, 1: Interactable, 2: Attachable

	FTIHHsObjectIndividuality():
		CollisionChannel(ECC_Visibility)
	{
		FMemory::Memzero(AdditionalObjectFlag, sizeof(AdditionalObjectFlag));
	}
	FTIHHsObjectIndividuality(const FTIHHsObjectIndividuality& other) = default;
	FTIHHsObjectIndividuality(FTIHHsObjectIndividuality&& other) noexcept = default;

	FORCEINLINE FTIHHsObjectIndividuality& operator=(const FTIHHsObjectIndividuality& other)
	{
		if (this != &other)
		{
			GamePlayTags = other.GamePlayTags;
			ObjectState = other.ObjectState;
			CollisionChannel = other.CollisionChannel;
			FMemory::Memcpy(AdditionalObjectFlag, other.AdditionalObjectFlag, sizeof(AdditionalObjectFlag));
		}
		return *this;
	}
	FORCEINLINE FTIHHsObjectIndividuality& operator=(FTIHHsObjectIndividuality&& other) noexcept
	{
		if (this != &other)
		{
			GamePlayTags = MoveTemp(other.GamePlayTags);
			ObjectState = MoveTemp(other.ObjectState);
			CollisionChannel = MoveTemp(other.CollisionChannel);
			FMemory::Memcpy(AdditionalObjectFlag, other.AdditionalObjectFlag, sizeof(AdditionalObjectFlag));
		}
		return *this;
	}
	FORCEINLINE bool operator==(const FTIHHsObjectIndividuality& other) const
	{
		return GamePlayTags == other.GamePlayTags &&
			   ObjectState == other.ObjectState &&
			   CollisionChannel == other.CollisionChannel &&
			   FMemory::Memcmp(AdditionalObjectFlag, other.AdditionalObjectFlag, sizeof(AdditionalObjectFlag)) == 0;
	}
	FORCEINLINE bool operator!=(const FTIHHsObjectIndividuality& other) const	{		return !(*this == other);	}
	FORCEINLINE bool operator==(const FGameplayTagContainer& other) const	{		return GamePlayTags == other;	}
	FORCEINLINE bool operator==(const FTIHHsObjectState& other) const	{		return ObjectState == other;	}
	FORCEINLINE bool operator==(const TEnumAsByte<ECollisionChannel>& other) const	{		return CollisionChannel == other;	}
	FORCEINLINE bool operator==(const int8 (&other)[3]) const
	{
		bool reValue = true;
		reValue &= AdditionalObjectFlag[0] == other[0];
		reValue &= AdditionalObjectFlag[1] == other[1];
		reValue &= AdditionalObjectFlag[2] == other[2];
		return reValue;
	}

	FORCEINLINE bool HasGPTag(const FGameplayTag& tag) const	{		return GamePlayTags.HasTag(tag);	}
	FORCEINLINE bool HasGPTagExact(const FGameplayTag& tag) const	{		return GamePlayTags.HasTagExact(tag);	}
	FORCEINLINE bool HasAnyGPTags(const FGameplayTagContainer& tags) const	{		return GamePlayTags.HasAny(tags);	}
	FORCEINLINE bool HasAnyGPTagExact(const FGameplayTagContainer& tags) const	{		return GamePlayTags.HasAnyExact(tags);	}
	FORCEINLINE bool HasAllGPTags(const FGameplayTagContainer& tags) const	{		return GamePlayTags.HasAll(tags);	}
	FORCEINLINE bool HasAllGPTagsExact(const FGameplayTagContainer& tags) const	{		return GamePlayTags.HasAllExact(tags);	}
	
	FORCEINLINE bool IsGPInteractable() const	{		return GamePlayTags.HasTag(TIH_Interactable);	}
	FORCEINLINE bool IsGPAttachable() const	{		return GamePlayTags.HasTag(TIH_Attachable);	}
	FORCEINLINE bool CanBeGPHovered() const	{		return GamePlayTags.HasTag(TIH_Interactable_Hover);	}
	FORCEINLINE bool CanBeGPHeld() const	{		return GamePlayTags.HasTag(TIH_Interactable_Hold);	}
	FORCEINLINE bool CanBeGPFocused() const	{		return GamePlayTags.HasTag(TIH_Interactable_Focus);	}
	FORCEINLINE bool CanBeGPDecked() const	{		return GamePlayTags.HasTag(TIH_Attachable_Decked);	}
	FORCEINLINE bool CanBeGPPlaced() const	{		return GamePlayTags.HasTag(TIH_Attachable_Placed);	}
	
	FORCEINLINE bool IsValidGPTag() const	{		return GamePlayTags.Num() > 0;	}
	FORCEINLINE bool IsEmptyGPTag() const	{		return GamePlayTags.IsEmpty();	}

	FORCEINLINE void AddGPTag(const FGameplayTag& tag)
	{
		if (not GamePlayTags.HasTag(tag))
		{
			GamePlayTags.AddTag(tag);
		}
	}
	FORCEINLINE void RemoveGPTag(const FGameplayTag& tag)
	{
		
		if (GamePlayTags.HasTag(tag))
		{
			GamePlayTags.RemoveTag(tag);
		}
	}
	FORCEINLINE void AddGPTags(const FGameplayTagContainer& tags)	{		GamePlayTags.AppendTags(tags);	}
	FORCEINLINE void AppendGPTags(const FGameplayTagContainer& tags)	{		AddGPTags(tags);	}
	FORCEINLINE void RemoveGPTags(const FGameplayTagContainer& tags)
	{
		for (const FGameplayTag& tag : tags)
		{
			GamePlayTags.RemoveTag(tag);
		}
	}
	FORCEINLINE void RemoveGPTags(const TArray<FGameplayTag>& tags)
	{
		FGameplayTagContainer tagContainer = FGameplayTagContainer::CreateFromArray(tags);
		RemoveGPTags(tagContainer);
	}
	FORCEINLINE const FGameplayTagContainer& GetGPTagContainerConst() const	{		return GamePlayTags;	}
	FORCEINLINE FGameplayTagContainer& GetGPTagContainer()	{		return GamePlayTags;	}
	FORCEINLINE void ClearTags()	{		GamePlayTags.Reset();	}

	FORCEINLINE bool IsBlockingCollision() const	{		return CollisionChannel == ECC_WorldStatic || CollisionChannel == ECC_WorldDynamic;	}
	FORCEINLINE bool IsOverlapCollision() const	{		return CollisionChannel == ECC_OverlapAll_Deprecated;	}
	FORCEINLINE bool IsVisibilityCollision() const	{		return CollisionChannel == ECC_Visibility;	}
	FORCEINLINE bool IsCameraCollision() const	{		return CollisionChannel == ECC_Camera;	}
	FORCEINLINE bool CheckCollisionChannel(const TEnumAsByte<ECollisionChannel>& channel) const	{		return CollisionChannel == channel;	}
	
	FORCEINLINE bool CheckCollisionChannels(const TArray<TEnumAsByte<ECollisionChannel>>& channels) const
	{
		for (const auto& channel : channels)
		{
			if (CollisionChannel == channel)
			{
				return true;
			}
		}
		return false;
	}
	
	FORCEINLINE ECollisionResponse GetDefaultCollisionResponse() const
	{
		switch (CollisionChannel.GetValue())
		{
		case ECC_WorldStatic:
		case ECC_WorldDynamic:
			return ECR_Block;
		case ECC_Visibility:
			return ECR_Ignore;
		case ECC_Camera:
			return ECR_Ignore;
		default:
			return ECR_Overlap;
		}
	}
	FORCEINLINE bool CanBeLineTraced() const
	{
		return IsGPInteractable() && 
			   (CollisionChannel == ECC_Visibility || 
				CollisionChannel == ECC_WorldStatic ||
				CollisionChannel == ECC_WorldDynamic);
	}
	// ObjectState
	FORCEINLINE bool IsInIdleState() const	{		return ObjectState.IsIdle();	}
	FORCEINLINE bool IsProcessing() const
	{
		return ObjectState.ProcessState == ETIHHsProcessState::Processing ||
			   ObjectState.ProcessState == ETIHHsProcessState::Initiating ||
			   ObjectState.ProcessState == ETIHHsProcessState::Finalizing;
	}
	FORCEINLINE bool IsInErrorState() const	{		return ObjectState.IsError();	}
	FORCEINLINE bool IsInValidState() const	{		return ObjectState.IsValidState();	}
	FORCEINLINE bool IsInValidVisualState() const	{		return ObjectState.VisualState != ETIHHsStateVisual::Invalid;	}
	FORCEINLINE bool IsBeingPlaced() const	{		return ObjectState.ProcessState == ETIHHsProcessState::Placing;	}
	FORCEINLINE bool IsBeingHeld() const	{		return ObjectState.ProcessState == ETIHHsProcessState::Holding;	}
	FORCEINLINE bool CanInteract() const
	{
		bool result = true;
		result &= IsInValidState();
		result &= not IsProcessing();
		result &= not IsInErrorState();
		result &= not ObjectState.IsDestroying();
		return result;
	}
	FORCEINLINE bool IsInPreviewMode() const
	{
		return ObjectState.VisualState == ETIHHsStateVisual::Preview ||
			   ObjectState.VisualState == ETIHHsStateVisual::Ghosted;
	}

	FORCEINLINE bool IsHightlighted() const
	{
		return ObjectState.VisualState == ETIHHsStateVisual::Highlighted;
	}
	FORCEINLINE bool IsVisuallyValid() const
	{
		return ObjectState.IsValidVisualState();
	}
	FORCEINLINE bool IsSelected() const
	{
		return ObjectState.VisualState == ETIHHsStateVisual::Selected;
	}
	FORCEINLINE void SetVisualState(ETIHHsStateVisual NewVisualState)
	{
		ObjectState.VisualState = NewVisualState;
	}
	
	
	FORCEINLINE FString ToString() const
	{
		FString Result = TEXT("{\n");
		
		// Gameplay Tags
		Result.Append(TEXT("  \"gameplay_tags\": ["));
		if (GamePlayTags.Num() > 0)
		{
			Result.Append(TEXT("\n"));
			TArray<FGameplayTag> TagArray;
			GamePlayTags.GetGameplayTagArray(TagArray);
			
			for (int32 i = 0; i < TagArray.Num(); i++)
			{
				Result.Append(TEXT("    \""));
				Result.Append(TagArray[i].ToString());
				Result.Append(TEXT("\""));
				
				if (i < TagArray.Num() - 1)
				{
					Result.Append(TEXT(","));
				}
				Result.Append(TEXT("\n"));
			}
			Result.Append(TEXT("  ]"));
		}
		else
		{
			Result.Append(TEXT("]"));
		}
		
		Result.Append(TEXT(",\n"));
		
		// Object State
		Result.Append(TEXT("  \"object_state\": "));
		Result.Append(ObjectState.ToString());
		
		Result.Append(TEXT(",\n"));
		
		// Collision Channel
		Result.Append(TEXT("  \"collision_channel\": \""));
		Result.Append(UEnum::GetValueAsString(CollisionChannel.GetValue()));
		Result.Append(TEXT("\",\n"));

		// Additional Object Flags
		Result.Append(TEXT("  \"additional_object_flags\": ["));
		for (int32 i = 0; i < 3; ++i)
		{
			Result.Append(FString::Printf(TEXT("%d"), AdditionalObjectFlag[i]));
			if (i < 2) Result.Append(TEXT(", "));
		}
		Result.Append(TEXT("]\n"));

		Result.Append(TEXT("}"));
		
		return Result;
	}
};

USTRUCT()
struct FTIHHsIndirectObject
{
	GENERATED_BODY()
	
	FTIHHsObjectIndividuality* Individuality;	//	개체의 개별성
	
	TObjectPtr<AActor> Actor;

	TScriptInterface <class ITIHHsBaseObject> TIHBaseObjInterface;
	TScriptInterface <class ITIHHsInteraction> TIHInteraction;

	TUniquePtr<FJsonObject> JsonObject;
	
		FTIHHsIndirectObject()
		: Individuality(nullptr), Actor(nullptr), TIHBaseObjInterface(nullptr), TIHInteraction(nullptr)
	{
	}
	FTIHHsIndirectObject(AActor* actor);
	FTIHHsIndirectObject(const FTIHHsIndirectObject& other)
		: Individuality(other.Individuality), Actor(other.Actor), 
		  TIHBaseObjInterface(other.TIHBaseObjInterface), TIHInteraction(other.TIHInteraction),
		  JsonObject(nullptr)
	{
	}
	FTIHHsIndirectObject(FTIHHsIndirectObject&& other) noexcept
		: Individuality(other.Individuality), Actor(other.Actor), 
		  TIHBaseObjInterface(MoveTemp(other.TIHBaseObjInterface)), 
		  TIHInteraction(MoveTemp(other.TIHInteraction)),
		  JsonObject(MoveTemp(other.JsonObject))
	{
		other.Individuality = nullptr;
		other.Actor = nullptr;
	}
	

	FORCEINLINE bool IsValid() const
	{
		bool result = true;
		result &= Individuality != nullptr;
		result &= Actor != nullptr;
		result &= (TIHBaseObjInterface.GetInterface() != nullptr ||TIHInteraction.GetInterface() != nullptr);
		return result;
	}
	FTIHHsIndirectObject& operator=(const FTIHHsIndirectObject& other)
	{
		if (this != &other)
		{
			Individuality = other.Individuality;
			Actor = other.Actor;
			TIHBaseObjInterface = other.TIHBaseObjInterface;
			TIHInteraction = other.TIHInteraction;
			JsonObject = MakeUnique<FJsonObject>(*other.JsonObject);
		}
		return *this;
	}
	FTIHHsIndirectObject& operator=(FTIHHsIndirectObject&& other) noexcept
	{
		if (this != &other)
		{
			Individuality = other.Individuality;
			Actor = other.Actor;
			TIHBaseObjInterface = MoveTemp(other.TIHBaseObjInterface);
			TIHInteraction = MoveTemp(other.TIHInteraction);
			JsonObject = MoveTemp(other.JsonObject);
			
			other.Individuality = nullptr;
			other.Actor = nullptr;
		}
		return *this;
	}
	
	void StoreJSonData(const FString& JsonString)
	{
		if (JsonObject == nullptr)
		{
			JsonObject = MakeUnique<FJsonObject>();
		}
			
		if (JsonObject)
		{
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
			TSharedPtr<FJsonObject> TempObject;
			if (FJsonSerializer::Deserialize(Reader, TempObject) && TempObject.IsValid())
			{
				// 4) 파싱된 내용을 TUniquePtr<FJsonObject>로 복사
				*JsonObject = *TempObject;  
				// (FJsonObject 은 operator=가 내부 맵을 복사해줍니다)
			}
			else
			{
				// 파싱 실패 시 로깅
				UE_LOG(LogTemp, Warning, TEXT("StoreJSonData: JSON 파싱 실패. 오류: %s"), *Reader->GetErrorMessage());
			}
		}
	}
};

UENUM()
enum class ETIHHsStateInteractTrigger : uint8
{
	ENone = 0,	//	아무것도 없음
	EHoverStart,	//	마우스 오버
	EHoverOngoing,
	EHoverEnd,
	EFocusStart,	//	포커스 시작
	EFocusOngoing,	//	포커스 중
	EFocusEnd,	//	포커스 끝
	EHoldStart,	//	홀드 시작
	EHoldOngoing,	//	홀드 중
	EHoldEnd,	//	홀드 끝
	ETap,	//	탭
	EDoubleTap,	//	더블탭
	EMoveStart,	//	이동 시작
	EMoveOngoing,
	EMoveEnd,	//	이동 끝
	EPlacementStart,	//	배치 시작
	EPlacementOngoing,	//	배치 중
	EPlacementEnd,	//	배치 끝
	EPlacementCancel,	//	배치 취소
	EPlacementConfirm,	//	배치 확정
	EPlacementInvalid,	//	배치 불가능
	EPlacementValid,	//	배치 가능
	ECustom,
};



USTRUCT(BlueprintType)
struct FTIHHsFiniteTriggerPair
{
	GENERATED_BODY()

	//UPROPERTY()
	ETIHHsStateInteractTrigger TriggerType;	//	트리거 종류

	FString TriggerName;	//	트리거 이름

	bool operator==(const FTIHHsFiniteTriggerPair& other) const
	{
		return TriggerType == other.TriggerType && TriggerName == other.TriggerName;
	}
	
};

inline uint32 GetTypeHash(const FTIHHsFiniteTriggerPair& triggerPair)
{
	return GetTypeHash(triggerPair.TriggerType) ^ GetTypeHash(triggerPair.TriggerName);
}

DECLARE_DELEGATE_TwoParams(FTIHHsFiniteStateSwitchDelegate, FTIHHsFiniteTriggerPair, FTIHHsIndirectObject&);

USTRUCT(BlueprintType)
struct FTIHHsFiniteStateSwitchNode
{
	GENERATED_BODY()
	
	FTIHHsFiniteStateSwitchDelegate StateFunction;	//	상태를 반환하는 함수

	//FTIHHsObjectState operator()(FTIHHsFiniteTriggerPair triggerPair,FTIHHsIndirectObject& indirectObj)
	//{
	//	return StateFunction.ExecuteIfBound(triggerPair,indirectObj);
	//}
	
};

UCLASS()
class UTIHHsFiniteStateTree : public UObject
{
	GENERATED_BODY()
public:
	TMap<FTIHHsFiniteTriggerPair, FTIHHsFiniteStateSwitchNode> StateSwitchNodes;	//	상태 전환 노드들

	void Trigger(const FTIHHsFiniteTriggerPair& triggerPair )
	{
		if (StateSwitchNodes.Contains(triggerPair))
		{
			
		}
	}
};



USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsTag
{
	GENERATED_BODY()

	UPROPERTY()
	TEnumAsByte<ECollisionChannel> StaticCollisionChannel;
	
	UPROPERTY()
	FGameplayTagContainer StaticGamePlayTags;

	UPROPERTY()
	FString DynamicStringTag;

	UPROPERTY()
	TMap<FName,FName> DynamicTagMap;
	
	FTIHHsTag()
		: StaticCollisionChannel(ECC_WorldStatic), DynamicStringTag(TEXT(""))
	{
	}	

	bool operator==(const FTIHHsTag& other) const
	{
		return IsEqualAll(*this, other);
	}
	bool operator==(const TEnumAsByte<ECollisionChannel>& other) const
	{
		return StaticCollisionChannel == other;
	}
	bool operator==(const FGameplayTagContainer& other) const
	{
		return StaticGamePlayTags == other;
	}
	bool operator==(const FString& other) const
	{
		return DynamicStringTag == other;
	}
	

	static bool IsEqualAll(const FTIHHsTag& lhs, const FTIHHsTag& rhs)
	{
		return lhs.StaticCollisionChannel == rhs.StaticCollisionChannel &&
			   lhs.StaticGamePlayTags == rhs.StaticGamePlayTags &&
			   lhs.DynamicStringTag == rhs.DynamicStringTag;
	}
	static bool IsEqualAny(const FTIHHsTag& lhs, const FTIHHsTag& rhs)
	{
		return lhs.StaticCollisionChannel == rhs.StaticCollisionChannel ||
			   lhs.StaticGamePlayTags == rhs.StaticGamePlayTags ||
			   lhs.DynamicStringTag == rhs.DynamicStringTag;
	}

	
	static bool IsEqualStatic(const FTIHHsTag& lhs, const FTIHHsTag& rhs)
	{
		return lhs.StaticCollisionChannel == rhs.StaticCollisionChannel &&
			   lhs.StaticGamePlayTags == rhs.StaticGamePlayTags;
	}
	static bool IsEqualDynamic(const FTIHHsTag& lhs, const FTIHHsTag& rhs)
	{
		return lhs.DynamicStringTag == rhs.DynamicStringTag;
	}

	FString ToString() const
	{
		FString Result = TEXT("{\n");
        
		// Collision Channel
		Result.Append(TEXT("  \"collision_channel\": \""));
		Result.Append(UEnum::GetValueAsString(StaticCollisionChannel.GetValue()));
		Result.Append(TEXT("\",\n"));
        
		// Gameplay Tags
		Result.Append(TEXT("  \"gameplay_tags\": ["));
		if (StaticGamePlayTags.Num() > 0)
		{
			Result.Append(TEXT("\n"));
			TArray<FGameplayTag> TagArray;
			StaticGamePlayTags.GetGameplayTagArray(TagArray);
            
			for (int32 i = 0; i < TagArray.Num(); i++)
			{
				Result.Append(TEXT("    \""));
				Result.Append(TagArray[i].ToString());
				Result.Append(TEXT("\""));
                
				if (i < TagArray.Num() - 1)
				{
					Result.Append(TEXT(","));
				}
				Result.Append(TEXT("\n"));
			}
			Result.Append(TEXT("  ]"));
		}
		else
		{
			Result.Append(TEXT("]"));
		}
		Result.Append(TEXT(",\n"));
        
		// Dynamic String Tag
		Result.Append(TEXT("  \"dynamic_string_tag\": \""));
		Result.Append(DynamicStringTag);
		Result.Append(TEXT("\"\n"));
        
		Result.Append(TEXT("}"));
        
		return Result;
	}
};
USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsState
{
	GENERATED_BODY()

	UPROPERTY()
	ETIHHsStateCategory StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_Default;
	UPROPERTY()
	ETIHHsStateDetail StateDetail = ETIHHsStateDetail::ETIHHsStates_Unknown;
	UPROPERTY()
	int16 StateOption = 0;	//	상세 옵션, 예를 들어서 Initiate 상태에서 Starting, Waiting, Error, CompleteFinalizing 등등의 상태를 나타냄

	FTIHHsState() = default;
	FTIHHsState(const FTIHHsState& other) = default;
	FTIHHsState(FTIHHsState&& other) noexcept = default;
	FTIHHsState(ETIHHsStateCategory InStateCategory, ETIHHsStateDetail InStateDetail, int16 InStateOption = 0)
		: StateCategory(InStateCategory), StateDetail(InStateDetail), StateOption(InStateOption)
	{
	}
	FTIHHsState (ETIHHsStateDetail detail):
		StateDetail(detail), StateOption(0)
	{
		switch (detail) {
		case ETIHHsStateDetail::ETIHHsStates_Unknown:
		case ETIHHsStateDetail::ETIHHsStates_Default_Idle:
		case ETIHHsStateDetail::ETIHHsStates_Default_WaitingAction:
		case ETIHHsStateDetail::ETIHHsStates_Default_Paused:
		case ETIHHsStateDetail::ETIHHsStates_Default_Focus:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_Default;
			break;
		case ETIHHsStateDetail::ETIHHsStates_WithUser_InProgress:
		case ETIHHsStateDetail::ETIHHsStates_WithUser_ProcessWaiting:
		case ETIHHsStateDetail::ETIHHsStates_WithUser_CompleteFinalizing:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_WithUser;
			break;
		case ETIHHsStateDetail::ETIHHsStates_BySelf_Waiting:
		case ETIHHsStateDetail::ETIHHsStates_BySelf_InProgress:
		case ETIHHsStateDetail::ETIHHsStates_BySelf_CompleteFinalizing:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_BySelf;
			break;
		case ETIHHsStateDetail::ETIHHsStates_Initiate_Starting:
		case ETIHHsStateDetail::ETIHHsStates_Initiate_Waiting:
		case ETIHHsStateDetail::ETIHHsStates_Initiate_Error:
		case ETIHHsStateDetail::ETIHHsStates_Initiate_CompleteFinalizing:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_Initiate;
			break;
		case ETIHHsStateDetail::ETIHHsStates_Processing_Waiting:
		case ETIHHsStateDetail::ETIHHsStates_Processing_Starting:
		case ETIHHsStateDetail::ETIHHsStates_Processing_Finalizing:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_Processing;
			break;
		case ETIHHsStateDetail::ETIHHsStates_Error_Stopped:
		case ETIHHsStateDetail::ETIHHsStates_Error_Analyzing:
		case ETIHHsStateDetail::ETIHHsStates_Error_Resolving:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_Error;
			break;
		case ETIHHsStateDetail::ETIHHsStates_Destroy_Waiting:
		case ETIHHsStateDetail::ETIHHsStates_Destroy_Starting:
		case ETIHHsStateDetail::ETIHHsStates_Destroy_WaitingRelease:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_Destroy;
			break;
		case ETIHHsStateDetail::ETIHHsStates_Verification_StartWaiting:
		case ETIHHsStateDetail::ETIHHsStates_Verification_InProgress:
		case ETIHHsStateDetail::ETIHHsStates_Verification_CompleteFinalizing:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_Verification;
			break;
		case ETIHHsStateDetail::ETIHHsStates_WithOther_Waiting:
		case ETIHHsStateDetail::ETIHHsStates_WithOther_InProgress:
		case ETIHHsStateDetail::ETIHHsStates_WithOther_ProcessWaiting:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_WithOther;
			break;
		case ETIHHsStateDetail::ETIHHsStates_Backup_Preparing:
		case ETIHHsStateDetail::ETIHHsStates_Backup_InProgress:
		case ETIHHsStateDetail::ETIHHsStates_Backup_CompleteFinalizing:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_Backup;
			break;
		case ETIHHsStateDetail::ETIHHsStates_Update_Waiting:
		case ETIHHsStateDetail::ETIHHsStates_Update_InProgress:
		case ETIHHsStateDetail::ETIHHsStates_Update_CompleteFinalizing:
			StateCategory = ETIHHsStateCategory::ETIHHsStateCategory_Update;
			break;
		}
		
	}

	FTIHHsState& operator=(const FTIHHsState& other)
	{
		if (this != &other)
		{
			StateCategory = other.StateCategory;
			StateDetail = other.StateDetail;
			StateOption = other.StateOption;
		}
		return *this;
	}
	FTIHHsState& operator=(FTIHHsState&& other) noexcept
	{
		if (this != &other)
		{
			StateCategory = MoveTemp(other.StateCategory);
			StateDetail = MoveTemp(other.StateDetail);
			StateOption = MoveTemp(other.StateOption);
		}
		return *this;
	}
	
	bool operator==(const FTIHHsState& other) const
	{
		return StateCategory == other.StateCategory && StateDetail == other.StateDetail && StateOption == other.StateOption;
	}
	bool operator!=(const FTIHHsState& other) const
	{
		return !(*this == other);
	}

	bool operator==(ETIHHsStateCategory InStateCategory) const
	{
		return StateCategory == InStateCategory;
	}
	bool operator==(ETIHHsStateDetail InStateDetail) const
	{
		return StateDetail == InStateDetail;
	}
	
};
USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsAttachmentRuleWrapper
{
	GENERATED_BODY()

	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules::KeepRelativeTransform;
	
	FTIHHsAttachmentRuleWrapper():	AttachmentTransformRules(FAttachmentTransformRules::KeepRelativeTransform)
	{
	}
	FTIHHsAttachmentRuleWrapper(const FAttachmentTransformRules& InAttachmentTransformRules)
		: AttachmentTransformRules(InAttachmentTransformRules)
	{
	}
	FTIHHsAttachmentRuleWrapper(const FTIHHsAttachmentRuleWrapper& other)
		: AttachmentTransformRules(other.AttachmentTransformRules)
	{
	}
	FTIHHsAttachmentRuleWrapper(EAttachmentRule InRule, bool bInWeldSimulatedBodies)
		: AttachmentTransformRules(InRule, bInWeldSimulatedBodies)
	{
	}
	FTIHHsAttachmentRuleWrapper(EAttachmentRule LocationRule, EAttachmentRule RotationRule, EAttachmentRule ScaleRule,bool bWeldSimulatedBodies = false)
		: AttachmentTransformRules(LocationRule, RotationRule, ScaleRule,bWeldSimulatedBodies)
	{
	}
	FTIHHsAttachmentRuleWrapper(FTIHHsAttachmentRuleWrapper&& other) noexcept
		: AttachmentTransformRules(MoveTemp(other.AttachmentTransformRules))
	{
	}

	FTIHHsAttachmentRuleWrapper& operator=(const FTIHHsAttachmentRuleWrapper& other)
	{
		if (this != &other)
		{
			AttachmentTransformRules = other.AttachmentTransformRules;
		}
		return *this;
	}
	FTIHHsAttachmentRuleWrapper& operator=(FTIHHsAttachmentRuleWrapper&& other) noexcept
	{
		if (this != &other)
		{
			AttachmentTransformRules = MoveTemp(other.AttachmentTransformRules);
		}
		return *this;
	}

	FString ToString() const
	{
		FString jsonString;
		jsonString.Append(TEXT("{\n"));
		jsonString.Append(TEXT("  \"location_rule\": \""));
		jsonString.Append(UEnum::GetValueAsString(AttachmentTransformRules.LocationRule));
		jsonString.Append(TEXT("\",\n"));
		jsonString.Append(TEXT("  \"rotation_rule\": \""));
		jsonString.Append(UEnum::GetValueAsString(AttachmentTransformRules.RotationRule));
		jsonString.Append(TEXT("\",\n"));
		jsonString.Append(TEXT("  \"scale_rule\": \""));
		jsonString.Append(UEnum::GetValueAsString(AttachmentTransformRules.ScaleRule));
		jsonString.Append(TEXT("\",\n"));
		jsonString.Append(TEXT("  \"weld_simulated_bodies\": "));
		jsonString.Append(AttachmentTransformRules.bWeldSimulatedBodies ? TEXT("true") : TEXT("false"));
		jsonString.Append(TEXT("\n}"));
		return jsonString;
	}
};


class ITIHHsBaseObject;
//	콜백을 인터페이스로 했음. 왜냐하면 어차피 호출될 놈이 누구인지 알아야하기때문
//	이제 딜레이를 만들었으니 이걸 저장도 해야할거임.
USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsActionTransformData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Type")
	ETIHHsActionTransformDataExecuteAgent EExecuteAgent ;	// Delay냐 아니면 바로 적용이냐의 차이임.
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Type")
	ETIHHsActionTransformDataType ETransformType = ETIHHsActionTransformDataType::ETIHTransform_TransformWorld;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Type",meta=(Bitmask, BitmaskEnum="ETIHHsActionTransformDataCallBackTypes"))
	int32 ECallBackTypes = static_cast<int32>(ETIHHsActionTransformDataCallBackTypes::ETIHTransform_None);	//	이게 있으면 콜백을 호출함.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Type",meta=(Bitmask, BitmaskEnum="ETIHHsActionTransformDataFinishTypes"))
	int32 EFinishConditionTypes  = static_cast<int32>(ETIHHsActionTransformDataFinishTypes::ETIHTransform_Error);;// 쪼개서 실행할지 정하는거
	
	/*
	 *	Data
	 */
	UPROPERTY()
	FTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform|Finish|Time")
	float FinishTime = 0.0f;	//	이게 있으면 시간으로 끝남.
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform|Finish|Direction")
	FVector TargetLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform|Finish|Direction")
	float AcceptanceRadius = KINDA_SMALL_NUMBER;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform|Finish|CollisionType",meta=(Bitmask, BitmaskEnum="ETIHHsTagFilterRank"))
	int32 CollisionFilterRank = static_cast<int32>(ETIHHsTagFilterRank::ETIHHsTagFilterRank_None);	//	이게 있으면 충돌로 끝남.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform|Finish|CollisionType")
	FTIHHsTag CollisionType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform|Finish|CollisionTarget")
	TArray<TScriptInterface<ITIHHsBaseObject>> CollisionTargets;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform|Target")
	USceneComponent* TargetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform|CallBack")
	TScriptInterface<class ITIHHsInteraction> MoveBeginCallBack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform|CallBack")
	TScriptInterface<class ITIHHsInteraction> MovingCallBack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transform|CallBack")
	TScriptInterface<class ITIHHsInteraction> MoveEndCallBack;
};
USTRUCT(Blueprintable)
struct TIHHOUSINGCORE_API FTIHHsActionParamAdditional
{
	GENERATED_BODY()

	UPROPERTY()
	FString ActionData;	//	추가적인 액션 데이터

	UPROPERTY()
	float ActionTime = 0.0f;	//	추가적인 액션 시간

	UPROPERTY()
	int32 ActionOrder = 0;	//	추가적인 액션 순서

	FTIHHsActionParamAdditional() = default;
	FTIHHsActionParamAdditional(const FString& actionData, float actionTime = 0.0f, int32 actionOrder = 0)
		: ActionData(actionData), ActionTime(actionTime), ActionOrder(actionOrder)
	{
	}
	FTIHHsActionParamAdditional(float actionTime)
		: ActionData(TEXT("")), ActionTime(actionTime), ActionOrder(0)
	{
	}
	FTIHHsActionParamAdditional(int32 actionOrder)
		: ActionData(TEXT("")), ActionTime(0.0f), ActionOrder(actionOrder)
	{
	}
	
	FTIHHsActionParamAdditional(const FTIHHsActionParamAdditional& other) = default;
	FTIHHsActionParamAdditional(FTIHHsActionParamAdditional&& other) noexcept
		: ActionData(MoveTemp(other.ActionData)), ActionTime(other.ActionTime), ActionOrder(other.ActionOrder)
	{
	}

	
	FTIHHsActionParamAdditional& operator=(const FTIHHsActionParamAdditional& other)
	{
		if (this != &other)
		{
			ActionData = other.ActionData;
			ActionTime = other.ActionTime;
			ActionOrder = other.ActionOrder;
		}
		return *this;
	}
	FTIHHsActionParamAdditional& operator=(FTIHHsActionParamAdditional&& other) noexcept
	{
		if (this != &other)
		{
			ActionData = MoveTemp(other.ActionData);
			ActionTime = other.ActionTime;
			ActionOrder = other.ActionOrder;
		}
		return *this;
	}
	void Clear()
	{
		ActionData = TEXT("");
		ActionTime = 0.0f;
		ActionOrder = 0;
	}
};



USTRUCT(Blueprintable)
struct TIHHOUSINGCORE_API FTIHHsActionParamIn
{
	GENERATED_BODY()

	UPROPERTY()
	FString ActionName;	//	액션 이름

	UPROPERTY()
	FString ActionData;	//	액션 데이터

	UPROPERTY()
	TArray<FTIHHsActionParamAdditional> ActionParamAdditionals;	//	추가적인 액션 파라미터

	UPROPERTY()
	FInstancedPropertyBag PropertyBag;	//	추가적인 액션 파라미터 JSON 오브젝트들, 이게 있으면 JSON으로 저장됨.

	UPROPERTY()
	UObject* CallerObject = nullptr; // 액션을 호출한 객체, 예를 들어서 액션을 트리거한 플레이어 캐릭터나 컨트롤러 등
	
	FTIHHsActionParamIn() = default;
	explicit FTIHHsActionParamIn(const FInstancedPropertyBag& propertyBag);
	FTIHHsActionParamIn(UObject* callerObject)
		: ActionName(TEXT("")), ActionData(TEXT("")), ActionParamAdditionals(), PropertyBag(), CallerObject(const_cast<UObject*>(callerObject))
	{
	}

	explicit FTIHHsActionParamIn(const FString& actionName, const FString& actionData = TEXT(""), const TArray<FTIHHsActionParamAdditional>& actionParamAdditionals = {},const FInstancedPropertyBag& propertyBag = FInstancedPropertyBag())
		: ActionName(actionName), ActionData(actionData), ActionParamAdditionals(actionParamAdditionals), PropertyBag(propertyBag)
	{
	}
	FTIHHsActionParamIn( const TArray<FTIHHsActionParamAdditional>& actionParamAdditionals )
		: ActionName(TEXT("")), ActionData(TEXT("")), ActionParamAdditionals(actionParamAdditionals)
	{
	}
	
	FTIHHsActionParamIn(const FString& actionName,bool hasAdditional, const TArray<FTIHHsActionParamAdditional>& actionParamAdditionals)
		: ActionName(actionName), ActionData(TEXT("")), ActionParamAdditionals(hasAdditional ? actionParamAdditionals : TArray<FTIHHsActionParamAdditional>())
	{
	}
	FTIHHsActionParamIn(const FString& actionName,const FString& actionData,float deltaTime)
		: ActionName(actionName), ActionData(actionData)
	{
		ActionParamAdditionals.Add(FTIHHsActionParamAdditional(actionData, deltaTime));
	}
	
	FTIHHsActionParamIn(const FTIHHsActionParamIn& other)	
		: ActionName(other.ActionName), ActionData(other.ActionData), ActionParamAdditionals(other.ActionParamAdditionals)
	{
	}
	FTIHHsActionParamIn(FTIHHsActionParamIn&& other) noexcept
		: ActionName(MoveTemp(other.ActionName)), ActionData(MoveTemp(other.ActionData)), ActionParamAdditionals(MoveTemp(other.ActionParamAdditionals)),
		  PropertyBag(MoveTemp(other.PropertyBag))	//	PropertyBag도 이동
	{
	}
	FTIHHsActionParamIn& operator=(const FTIHHsActionParamIn& other)
	{
		if (this != &other)
		{
			ActionName = other.ActionName;
			ActionData = other.ActionData;
			ActionParamAdditionals = other.ActionParamAdditionals;
			PropertyBag = other.PropertyBag;	//	PropertyBag도 복사
		}
		return *this;
	}
	FTIHHsActionParamIn& operator=(FTIHHsActionParamIn&& other) noexcept
	{
		if (this != &other)
		{
			ActionName = MoveTemp(other.ActionName);
			ActionData = MoveTemp(other.ActionData);
			ActionParamAdditionals = MoveTemp(other.ActionParamAdditionals);
			PropertyBag = MoveTemp(other.PropertyBag);	//	PropertyBag도 이동
		}
		return *this;
	}
	void Clear()
	{
		ActionName = TEXT("");
		ActionData = TEXT("");
		ActionParamAdditionals.Empty();
		PropertyBag.Reset();
	}
	FString ToString() const
	{
		FString Result = TEXT("{\n");
		Result.Append(TEXT("  \"action_name\": \""));
		Result.Append(ActionName);
		Result.Append(TEXT("\",\n"));
		Result.Append(TEXT("  \"action_data\": \""));
		Result.Append(ActionData);
		Result.Append(TEXT("\",\n"));
		Result.Append(TEXT("  \"action_param_additionals\": [\n"));
		for (int32 i = 0; i < ActionParamAdditionals.Num(); ++i)
		{
			Result.Append(TEXT("    {\n"));
			Result.Append(TEXT("      \"action_data\": \""));
			Result.Append(ActionParamAdditionals[i].ActionData);
			Result.Append(TEXT("\",\n"));
			Result.Append(TEXT("      \"action_time\": "));
			Result.Append(FString::SanitizeFloat(ActionParamAdditionals[i].ActionTime));
			Result.Append(TEXT(",\n"));
			Result.Append(TEXT("      \"action_order\": "));
			Result.Append(FString::FromInt(ActionParamAdditionals[i].ActionOrder));
			Result.Append(TEXT("\n    }"));
			if (i < ActionParamAdditionals.Num() - 1)
			{
				Result.Append(TEXT(",\n"));
			}
		}
		Result.Append(TEXT("\n  ]\n"));
		Result.Append(TEXT("}"));
		return Result;
	}
};

USTRUCT()
struct FTIHHsCommandArgumentElement	//	256bit
{
	GENERATED_BODY()

	
	TObjectPtr<UObject> ObjectValue;
	UClass* StructValue = nullptr;	//	UStruct 포인터, 이게 있으면 StructValue로 저장됨
};

USTRUCT()
struct TIHHOUSINGCORE_API FTIHHsCommandArgument // 640bit == int32 * 20 == int64 * 10
{
	GENERATED_BODY()

	TMap<FString,FVariant> VariantValue;
	//TMap<FString,FTIHHsCommandArgumentElement> ArgumentTable;	//	명령어 인자들, 예를 들어서 "key1" : "value1", "key2" : 1234 등등의 형태로 저장됨.
	
	FJsonObject JsonValue;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTIHHsBpCommandFunctionDelegate,FTIHHsCommandArgument&, commandArgument); 

DECLARE_DELEGATE_OneParam(FTIHHsCommandFunctionDelegate, FTIHHsCommandArgument&);

USTRUCT(Blueprintable)
struct TIHHOUSINGCORE_API FTIHHsCommandNode
{
	GENERATED_BODY()

	UPROPERTY()
	int16 CommandOrder = 0;	//	명령 순서, 명령이 여러개 있을 때 순서를 정할 수 있음.
	
	UPROPERTY()
	int16 CommandID = 0;	//	명령 ID, 명령이 여러개 있을 때 구분할 수 있는 ID, 이게 있으면 명령이 호출됨.
	
	UPROPERTY()
	FName FunctionName;	//	함수 이름
	
	FTIHHsCommandFunctionDelegate FunctionDelegate;	//	함수 델리게이트, 이게 있으면 함수가 호출됨.
	
};
/*
 *	용법:
 *		FTIHHsCommandNode mCommandFunction;
 *		mCommandFunction.FunctionName = FName(TEXT("RelationShip"));
 *		mCommandFunction.FunctionDelegate.BindLambda([](FTIHHsCommandArgument& CommandArgument)
 *		{
 *			if(CommandArgument.CommandTarget.GetObject() != nullptr)
 *		});
 * 
 */

USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsRelationshipEdgeChild
{
	GENERATED_BODY()

	UPROPERTY()
	TScriptInterface<ITIHHsBaseObject> Child;
	
	UPROPERTY()
	TArray<FName> RelationshipTags;

	FTIHHsRelationshipEdgeChild() = default;
	FTIHHsRelationshipEdgeChild(const FTIHHsRelationshipEdgeChild& other) = default;
	FTIHHsRelationshipEdgeChild(FTIHHsRelationshipEdgeChild&& other) noexcept = default;
	FTIHHsRelationshipEdgeChild(TScriptInterface<ITIHHsBaseObject> child, const TArray<FName>& relationshipTags = {})
		: Child(child), RelationshipTags(relationshipTags)
	{
	}

	
	bool operator==(const FTIHHsRelationshipEdgeChild& other) const
	{
		return Child.GetObject() == other.Child.GetObject();
	}
	FTIHHsRelationshipEdgeChild& operator=(const FTIHHsRelationshipEdgeChild& other)
	{
		if (this != &other)
		{
			Child = other.Child;
			RelationshipTags = other.RelationshipTags;
		}
		return *this;
	}
	FTIHHsRelationshipEdgeChild& operator=(FTIHHsRelationshipEdgeChild&& other) noexcept
	{
		if (this != &other)
		{
			Child = MoveTemp(other.Child);
			RelationshipTags = MoveTemp(other.RelationshipTags);
		}
		return *this;
	}
};

FORCEINLINE uint32 GetTypeHash(const FTIHHsRelationshipEdgeChild& relationshipChild)
{
	return ::GetTypeHash(relationshipChild.Child.GetObject());
}


USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsRelationshipEdge
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Relationship")
	int64 EdgeId = 0;	//	에지 ID, 이게 있으면 에지가 호출됨. 에지 ID는 유일해야함.
	
	UPROPERTY(BlueprintReadWrite, Category = "Relationship")
	TScriptInterface<ITIHHsBaseObject> Parent;	//	소스 객체 

	UPROPERTY(BlueprintReadWrite, Category = "Relationship")
	TScriptInterface<ITIHHsBaseObject> Child;
	
	UPROPERTY(BlueprintReadWrite, Category = "Relationship")
	TArray<FName> RelationshipTags;

	void Clear()
	{
		EdgeId = 0;
		Parent = nullptr;
		Child = nullptr;
		RelationshipTags.Empty();
	}
	
	FTIHHsRelationshipEdge()
	{
	}
	FTIHHsRelationshipEdge(const FTIHHsRelationshipEdge& other)
		: Parent(other.Parent), Child(other.Child)
	{
		EdgeId = MakeUniqueEdgeId(Parent, Child);
	}
	FTIHHsRelationshipEdge(FTIHHsRelationshipEdge&& other):
		 EdgeId(other.EdgeId),Parent(MoveTemp(other.Parent)), Child(MoveTemp(other.Child)) 
	{
	}
	
	FTIHHsRelationshipEdge(TScriptInterface<ITIHHsBaseObject> parent, TScriptInterface<ITIHHsBaseObject> child, const TArray<FName>& relationshipTags = {})
		: Parent(parent), Child(child), RelationshipTags(relationshipTags)
	{
	}

	bool operator==(const FTIHHsRelationshipEdge& other) const
	{
		return EdgeId == other.EdgeId;
	}

	FTIHHsRelationshipEdge & operator=(const FTIHHsRelationshipEdge& other)
	{
		if (this != &other)
		{
			EdgeId = other.EdgeId;
			Parent = other.Parent;
			Child = other.Child;
		}
		return *this;
	}
	FTIHHsRelationshipEdge & operator=(FTIHHsRelationshipEdge&& other) noexcept
	{
		if (this != &other)
		{
			EdgeId = other.EdgeId;
			other.EdgeId = 0;	
			Parent = MoveTemp(other.Parent);
			Child = MoveTemp(other.Child);
		}
		return *this;
	}
	static int64 MakeUniqueEdgeId(int32 lhsId, int32 rhsId)
	{
		uint32 big = FMath::Max(lhsId, rhsId);
		uint32 small = FMath::Min(lhsId, rhsId);
		
		return (static_cast<int64>(big) << 32) | static_cast<int64>(small);
	}
	static int64 MakeUniqueEdgeId(TScriptInterface<ITIHHsBaseObject> lhs, TScriptInterface<ITIHHsBaseObject> rhs)
	{
		int32 lhsId = (lhs.GetObject() != nullptr) ? lhs.GetObject()->GetUniqueID() : 0;
		int32 rhsId = (rhs.GetObject() != nullptr) ? rhs.GetObject()->GetUniqueID() : 0;

		return MakeUniqueEdgeId(lhsId, rhsId);
	}
	static int64 MakeUniqueEdgeId(TScriptInterface<ITIHHsBaseObject> lhs, int32 rhsId)
	{
		int32 lhsId = (lhs.GetObject() != nullptr) ? lhs.GetObject()->GetUniqueID() : 0;
		return MakeUniqueEdgeId(lhsId, rhsId);
	}
	static int64 MakeUniqueEdgeId(int32 lhsId, TScriptInterface<ITIHHsBaseObject> rhs)
	{
		int32 rhsId = (rhs.GetObject() != nullptr) ? rhs.GetObject()->GetUniqueID() : 0;
		return MakeUniqueEdgeId(lhsId,rhsId);
	}
	static int32 SplitEdgeIdParentId(int64 edgeId)
	{
		return static_cast<int32>(edgeId >> 32);
	}
	static int32 SplitEdgeIdChildId(int64 edgeId)
	{
		return static_cast<int32>(edgeId & 0xFFFFFFFF);
	}
};

USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsRelationshipEdgeRefData
{
	GENERATED_BODY()

	UPROPERTY()
	TScriptInterface<ITIHHsBaseObject> Target;

	UPROPERTY()
	TSet<int32> ChildIds;
	
};

FORCEINLINE uint32 GetTypeHash(const FTIHHsRelationshipEdge& Edge)
{
	return ::GetTypeHash(Edge.EdgeId);
}

template <typename Signature>
struct TIHHOUSINGCORE_API FTIHHsFunctionNode;

template <typename R, typename... Args>
struct TIHHOUSINGCORE_API FTIHHsFunctionNode<R(Args...)>
{
	TFunction<R(Args...)> Function;	//	TODO: 함수포인터 형태라 그리 큰 문제는 아님. 대신 다시한번 확인필요.
	
	TArray<FTIHHsFunctionNode> NextNodes;

	FTIHHsFunctionNode& operator[](int32 idx)
	{
		static FTIHHsFunctionNode InvalidNode;
		return  NextNodes.IsValidIndex(idx) ? NextNodes[idx] : InvalidNode;
	}

	R operator()(Args... args) const
	{
		if (Function)
		{
			return Function(args...);
		}
		return R();
	}
};
template <typename... Args>
struct TIHHOUSINGCORE_API FTIHHsFunctionNode<void(Args...)>
{
	TFunction<void(Args...)> Function;
	
	TArray<FTIHHsFunctionNode> NextNodes;

	FTIHHsFunctionNode()
		: Function(nullptr)
	{}
	
	FTIHHsFunctionNode& operator[](int32 idx)
	{
		static FTIHHsFunctionNode InvalidNode;
		return NextNodes.IsValidIndex(idx) ? NextNodes[idx] : InvalidNode;
	}

	void operator()(Args... args) const
	{
		if (Function)
		{
			Function(args...);
		}
	}
};

USTRUCT()
struct FTIHHsIndividualHelper
{
	GENERATED_BODY()

	static bool IsCollisionPossible(const FTIHHsObjectIndividuality& Individuality)
	{
		bool result = true;
		result &= Individuality.IsValidGPTag();
		result &= (Individuality.IsVisibilityCollision() | Individuality.IsVisuallyValid() );
		return result;
	}
	
};

UENUM()
enum class ETIHHsFilterType
{
	ETagIncludeAnd,
	ETagIncludeOr,
	ETagExcludeAnd,
	ETagExcludeOr,

	EStateIncludeAnd,
	EStateIncludeOr,
	EStateExcludeAnd,
	EStateExcludeOr,

	ECollisionIncludeAnd,
	ECollisionIncludeOr,
	ECollisionExcludeAnd,
	ECollisionExcludeOr,
};
USTRUCT(BlueprintType)
struct FTIHHsFilter
{
	GENERATED_BODY()

	UPROPERTY()
	ETIHHsFilterType FilterType = ETIHHsFilterType::ETagIncludeAnd;	//	필터 타입,
	
	UPROPERTY()
	FGameplayTagContainer Tags;
};
//	TArray<FTIHHsFilter> FTIHHsFilterArray;	//	필터 배열, 이걸로 필터링을 할 수 있음.
//	앞부분이 우선순위임. 앞부터 검색시에 포함인지 아닌지 될거임.

/*
*	객체 생성자 매니저를 만든다면
*		인다이렉트 부터 공간을 만들어주고,
*		상태들을 만들어준다. 참고로 상태의 나머지 페이로드쪽에 뭐 넣든지.(양방향을 위해서)
*		하여튼 여기에서 생성자 만들어주고, spawn데이터들을 가져와준다.
*			누가(대상): 액터,컴포넌트
*			언제(타이밍): 지금,딜레이,특정 이벤트
*			어디서(에): {대상 == nullptr, 트랜스폼} <- 대상이 있으면 상대적으로 적용, nullptr이면 지금 월드에 공통 타겟월드로
*			무엇을: 생성인지,파괴인지
*			어떻게: 검증을 하고 행동할지, 바로 지울지
*			왜: 
*		트리거가 가져야 하는정보
*			어떤 인터렉션을 대체하는지, 
* 
* */

USTRUCT()
struct FTIHHsTriggerRegistInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString TriggerPath;	//	트리거 경로, 예를 들어서 "/Game/Path/To/Trigger.Trigger" 형태로 저장됨
	/*
	 *	해당 트리거가 가져야하는 정보
	 *		
	 * 
	 */
	UPROPERTY()
	FString TriggerInterationEvent;	//	최초 진입을 할 대체 인터렉션 이름
};

UENUM()
enum class ETIHHsTriggerFunctionType : uint8
{
	ELambda = 0,	//	람다 함수
	ECallInterface,
	ECommon,
	EDelegate,
	EName,
};

USTRUCT()
struct FTIHHsTriggerInfo
{
	GENERATED_BODY()

	TFunction<void()> TriggerFunction;	//	트리거 함수, 람다 함수 형태로 저장됨
	TScriptInterface<class ITIHHsInteraction> TriggerInterface;	//	트리거 인터페이스, 인터페이스 형태로 저장됨
	FName TriggerFunctionName;	//	트리거 함수 이름, 일반 함수 형태로 저장됨
	ETIHHsTriggerFunctionType TriggerFunctionType = ETIHHsTriggerFunctionType::ELambda;	//	트리거 함수 타입, 람다 함수, 인터페이스 호출, 일반 함수 등등
};

DECLARE_DELEGATE(FTIHHsActionCallBackDelegate);	//	트리거 콜백 델리게이트, 트리거가 호출되었을 때 실행되는 델리게이트

USTRUCT()
struct FTIHHsTriggerCreator
{
	GENERATED_BODY()

	int32 TriggerId = 0;
	
	FString MeshPath;

	TMap<ETIHHsTriggerActionTypes, FTIHHsActionCallBackDelegate> TriggerActions;
	/*
	 *	FTIHHsFunctionNode<void()>* Tap;
	 *	FTIHHsFunctionNode<void()>* doubleTap;
	 *	FTIHHsFunctionNode<void()>* hoverStart;
	 * 
	 */
	TMap<FName,FTIHHsTriggerInfo> TriggerFunctions;	//	트리거 함수들, 이름과 함수 형태로 저장됨
};




UCLASS()
class UTIHHsTriggerHelper : public UObject
{
	GENERATED_BODY()

public:
	
	TArray<FTIHHsTriggerCreator> mTriggerCreators;
	TMap<FString,TArray<int32>> mTriggerCreatorTableByPath;
	TMap<int32,int32> mTriggerCreatorTableById;	//	트리거 ID로 찾을 수 있는 테이블
	
};

// UCLASS()
// class UTIHHsIndividualHelper : public UObject
// {
// 	GENERATED_BODY()
//
// public:
// 	static bool IsLinetraceCollisionPossible(const FTIHHsObjectIndividuality& Individuality)
// 	{
// 		bool result = true;
// 		result &= Individuality.IsValidGPTag();
// 		result &= (Individuality.IsVisibilityCollision() | Individuality.IsVisuallyValid() );
// 		return result;
// 	}
// };

template<typename T>
struct THasInitFunction
{
private:
	template<typename U>static auto Test(int) -> decltype(std::declval<U>().Init(), std::true_type{});
	template<typename>static auto Test(...) -> std::false_type;
public:
	static constexpr bool Value = decltype(Test<T>(nullptr))::value;
};
template<typename T>
struct THasClearFunction
{
private:
	template<typename U>static auto Test(int) -> decltype(std::declval<U>().Clear(), std::true_type{});
	template<typename>static auto Test(...) -> std::false_type;
public:
	static constexpr bool Value = decltype(Test<T>(nullptr))::value;
};
template<typename TKeyType = FName,typename TValueType = int32>
struct TTIHHsLinkedMultiNode
{
	using KeyType = TKeyType;	//	노드의 키 타입
	using ValueType = TValueType;	//	노드의 값 타입

	int32 SelfIndex = -1;
	int32 NextIndex = -1;
	int32 PrevIndex = -1;

	TKeyType Key;
	TArray<TValueType> Values;

	TTIHHsLinkedMultiNode()
		: SelfIndex(-1), NextIndex(-1), PrevIndex(-1), Key()
	{
		Values.Reserve(1);
	}
	TTIHHsLinkedMultiNode(const TKeyType& key, const TValueType& value)
		: SelfIndex(-1), NextIndex(-1), PrevIndex(-1), Key(key), Values(value)
	{
		Values.Reserve(1);
		Values.Add(value);
	}
	TTIHHsLinkedMultiNode(const TKeyType& key, const TArray<TValueType>& values)
		: SelfIndex(-1), NextIndex(-1), PrevIndex(-1), Key(key), Values(values)
	{
	}
	TTIHHsLinkedMultiNode(const TTIHHsLinkedMultiNode& other)
		: SelfIndex(other.SelfIndex), NextIndex(other.NextIndex), PrevIndex(other.PrevIndex), Key(other.Key), Values(other.Values)
	{
		Values.Reserve(1);
	}
	TTIHHsLinkedMultiNode(TTIHHsLinkedMultiNode&& other) noexcept
		: SelfIndex(other.SelfIndex), NextIndex(other.NextIndex), PrevIndex(other.PrevIndex), Key(MoveTemp(other.Key)), Values(MoveTemp(other.Values))
	{
		other.SelfIndex = -1;
		other.NextIndex = -1;
		other.PrevIndex = -1;
	}
	TTIHHsLinkedMultiNode& operator=(const TTIHHsLinkedMultiNode& other)
	{
		if (this != &other)
		{
			SelfIndex = other.SelfIndex;
			NextIndex = other.NextIndex;
			PrevIndex = other.PrevIndex;
			Key = other.Key;
			Values = other.Values;
		}
		return *this;
	}
	TTIHHsLinkedMultiNode& operator=(TTIHHsLinkedMultiNode&& other) noexcept
	{
		if (this != &other)
		{
			SelfIndex = other.SelfIndex;
			NextIndex = other.NextIndex;
			PrevIndex = other.PrevIndex;
			Key = MoveTemp(other.Key);
			Values = MoveTemp(other.Values);
			
			other.SelfIndex = -1;
			other.NextIndex = -1;
			other.PrevIndex = -1;
		}
		return *this;
	}
	bool operator==(const TTIHHsLinkedMultiNode& other) const
	{
		return SelfIndex == other.SelfIndex && NextIndex == other.NextIndex && PrevIndex == other.PrevIndex && Key == other.Key && Values == other.Values;
	}
	bool operator!=(const TTIHHsLinkedMultiNode& other) const
	{
		return !(*this == other);
	}
	void Init()
	{
		SelfIndex = -1;
		NextIndex = -1;
		PrevIndex = -1;

		if constexpr (THasInitFunction<TKeyType>::Value)
		{
			Key.Init();
		}
		else if (THasClearFunction<TKeyType>::Value)
		{
			Key.Clear();
		}
	}
	bool IsValid() const
	{
		return -1 < SelfIndex ;
	}
	bool IsInvalid() const
	{
		return SelfIndex < 0;
	}
};
template<typename TNodeType = TTIHHsLinkedMultiNode>
struct TTIHHsLinkedNodeList
{
	/*
	 *	하나라도 노드가 있다면 head와 tail은 -1이 아니게 된다.
	 *	반대로 -1이면 비어있는 상태임.
	 * 
	 */
	using NodeKeyType = typename TNodeType::KeyType;	//	노드의 키 타입
	using NodeValueType = typename TNodeType::ValueType;	//	노드의 값 타입
	using NodeIndexType = int32;	//	노드 인덱스 타입, 일반적으로 int32를 사용함

	TArray<TNodeType> Nodes;	//	노드 배열
	TMap<NodeKeyType, NodeIndexType> NodeMap;	//	노드 맵, 키를 인덱스로 매핑함
	
	int32 HeadIndex = -1;	//	헤드 인덱스
	int32 TailIndex = -1;	//	테일 인덱스
	
	bool IsDirty = true;	//	노드가 변경되었는지 여부, 노드가 추가되거나 제거되면 true로 설정됨
	int32 DirtyCount = 0;
	
	TTIHHsLinkedNodeList()
		: HeadIndex(-1), TailIndex(-1)
	{
		Nodes.Reserve(8);	//	노드 배열을 16개 정도로 예약함
	}
	TTIHHsLinkedNodeList(const TTIHHsLinkedNodeList& other)
		: Nodes(other.Nodes), NodeMap(other.NodeMap), HeadIndex(other.HeadIndex), TailIndex(other.TailIndex)
	{
	}
	TTIHHsLinkedNodeList(TTIHHsLinkedNodeList&& other) noexcept
		: Nodes(MoveTemp(other.Nodes)), NodeMap(MoveTemp(other.NodeMap)), HeadIndex(other.HeadIndex), TailIndex(other.TailIndex)
	{
		other.HeadIndex = -1;
		other.TailIndex = -1;
	}
	TTIHHsLinkedNodeList& operator=(const TTIHHsLinkedNodeList& other)
	{
		if (this != &other)
		{
			Nodes = other.Nodes;
			NodeMap = other.NodeMap;
			HeadIndex = other.HeadIndex;
			TailIndex = other.TailIndex;
		}
		return *this;
	}
	TTIHHsLinkedNodeList& operator=(TTIHHsLinkedNodeList&& other) noexcept
	{
		if (this != &other)
		{
			Nodes = MoveTemp(other.Nodes);
			NodeMap = MoveTemp(other.NodeMap);
			HeadIndex = other.HeadIndex;
			TailIndex = other.TailIndex;

			other.HeadIndex = -1;
			other.TailIndex = -1;
		}
		return *this;
	}
	
	void Normalize()
	{
		//	여기 위에 먼저 head가 정상인지 확인해야하는구나.
			/*
			 *	목적:
			 *		제거된 노드나 valid하지 않은 노드를 뒤로 미뤄서 제거하고
			 *		정상적인 노드들의 인덱스를 재정비 하는것이 목표임.
			 *		먼저 값이 있을때 해줘야함.
			 *		정상적인 노드들을 먼저 TArray에 하나씩 순서대로 넣는다.
			 *		복사 비용이 많이 들어간다. 근데 이거 말고없다. 어차피 같은거임.
			 *		크기를 먼저 알 수 있으면 좋긴한데 일단해보자.
			 * 
			 */
		int32 validCount = 0;
		for (int32 i = 0; i < Nodes.Num(); ++i)
		{
			if (Nodes[i].IsValid())
			{
				++validCount;
			}
		}
		TArray<TNodeType> validNodes;
		validNodes.Reserve(validCount);	//	유효한 노드의 개수만큼 예약함

		TNodeType* currentNode = nullptr;
		int32 currIdx = HeadIndex;
		while (true)
		{
			if (not Nodes.IsValid(currIdx))
			{
				break;
			}
			int32 nextIdx = currentNode->NextIndex;
			int32 newOrder = validNodes.Add(Nodes[nextIdx]);
			validNodes[newOrder].SelfIndex = newOrder;
			validNodes[newOrder].PrevIndex = newOrder -1;
			validNodes[newOrder].NextIndex = newOrder +1;
			NodeMap.FindOrAdd(currentNode->Key) = newOrder;	//	노드 맵에 새 인덱스를 추가함
		}
		validNodes.Last().NextIndex = -1;	//	마지막 노드의 다음 인덱스를 -1로 설정
		Nodes = MoveTemp(validNodes);	//	유효한 노드들로 교체함
		IsDirty = false;
	}
	
	void PushBack(const TNodeType& inNode)
	{
		if (NodeMap.Contains(inNode.Key))
		{
			NodeMap[inNode.Key].Values.Append(inNode.Values);
		}
		else
		{
			TNodeType* newNode = AddNewNode(inNode);
			if (TailIndex < 0)
			{
				HeadIndex = TailIndex = newNode->SelfIndex;	//	처음 추가하는 경우, 헤드와 테일이 동일함
			}
			else
			{
				Nodes[TailIndex].NextIndex = newNode->SelfIndex;	//	현재 테일의 다음 인덱스를 새 노드의 인덱스로 설정
				newNode->PrevIndex = TailIndex;	//	새 노드의 이전 인덱스를 현재 테일로 설정]
				TailIndex = newNode->SelfIndex;	//	테일 인덱스를 새 노드의 인덱스로 업데이트
			}
			IsDirty = true;
		}
	}
	void PushFront(const TNodeType& inNode)
	{
		//	앞쪽으로 추가를 하는거임. 기존에 있었을경우 중복으로 추가하게 된다.
		if (NodeMap.Contains(inNode.Key))
		{
			NodeMap[inNode.Key].Values.Append(inNode.Values);
		}
		else
		{
			TNodeType* newNode = AddNewNode(inNode);
			if (HeadIndex < 0)
			{
				HeadIndex = TailIndex = newNode->SelfIndex;	//	처음 추가하는 경우, 헤드와 테일이 동일함
			}
			else
			{
				Nodes[HeadIndex].PrevIndex = newNode->SelfIndex;	//	현재 테일의 다음 인덱스를 새 노드의 인덱스로 설정
				newNode->Next = HeadIndex;	//	새 노드의 이전 인덱스를 현재 테일로 설정]
				HeadIndex = newNode->SelfIndex;	//	테일 인덱스를 새 노드의 인덱스로 업데이트
			}
		}
		IsDirty = true;
	}
	void PopBack()
	{
		if (TailIndex < 0)
		{
			return;	//	비어있으면 아무것도 하지 않음
		}
		
		NodeIndexType oldTailIndex = TailIndex;
		TailIndex = Nodes[TailIndex].PrevIndex;	//	테일 인덱스를 이전 노드로 업데이트
		
		if (TailIndex > -1)
		{
			Nodes[TailIndex].NextIndex = -1;	//	새 테일의 다음 인덱스를 -1로 설정
		}
		else
		{
			HeadIndex = -1; // 만약 Tail이 -1이 되면 Head도 -1로 설정
		}
		NodeMap.Remove(Nodes[oldTailIndex].Key); // 노드 맵에서 제거
		IsDirty = true;
	}
	void PopFront()
	{
		if (HeadIndex < 0)
		{
			return;	//	비어있으면 아무것도 하지 않음
		}
		
		NodeIndexType oldHeadIndex = HeadIndex;
		HeadIndex = Nodes[HeadIndex].NextIndex;	//	헤드 인덱스를 다음 노드로 업데이트
		
		if (HeadIndex > -1)
		{
			Nodes[HeadIndex].PrevIndex = -1; // 새 헤드의 이전 인덱스를 -1로 설정
		}
		else
		{
			TailIndex = -1; // 만약 Head가 -1이 되면 Tail도 -1로 설정
		}
		NodeMap.Remove(Nodes[oldHeadIndex].Key); // 노드 맵에서 제거
		IsDirty = true;
	}
	TNodeType* Find(const NodeKeyType& key)
	{
		TNodeType* result = nullptr;
		if (NodeMap.Contains(key))
		{
			result = &Nodes[NodeMap[key]];
		}
		return result;
	}
	
	TNodeType* GetHead()
	{
		TNodeType* result = nullptr;
		if (Nodes.IsValidIndex(HeadIndex) && NodeMap.Contains(HeadIndex))
		{
			result = &Nodes[HeadIndex];
		}
		return result;
	}
	TNodeType* GetTail()
	{
		TNodeType* result = nullptr;
		if (Nodes.IsValidIndex(TailIndex) && NodeMap.Contains(TailIndex))
		{
			result = &Nodes[TailIndex];
		}
		return result;
	}
	
	void InsertBack(NodeKeyType targetKey, const TNodeType& inNode)
	{
		if (NodeMap.Contains(targetKey))
		{
			if (targetKey != inNode.Key)
			{
				TNodeType* newNode = AddNewNode(inNode);
				
				TNodeType& targetNode = Nodes[NodeMap[targetKey]];
				if (targetNode.NextIndex > -1)
				{
					Nodes[targetNode.NextIndex].PrevIndex = newNode->SelfIndex;	//	타겟 노드의 다음 노드의 이전 인덱스를 새
				}
				else
				{
					TailIndex = newNode->SelfIndex;
				}
				newNode->PrevIndex = targetNode.SelfIndex;	//	새 노드의 이전 인덱스를 타겟 노드로 설정
				newNode->NextIndex = targetNode.NextIndex;
				
				targetNode.NextIndex = newNode->SelfIndex;
			}
			else
			{
				Nodes[NodeMap[targetKey]].Values.Append(inNode.Values);
			}
		}
		else
		{
			PushBack(inNode);
		}
	}

	void InsertFront(NodeKeyType targetKey, const TNodeType& inNode)
	{
		if (NodeMap.Contains(targetKey))
		{
			if (targetKey != inNode.Key)
			{
				TNodeType* newNode = AddNewNode(inNode);
				
				TNodeType& targetNode = Nodes[NodeMap[targetKey]];
				if (targetNode.PrevIndex > -1)
				{
					Nodes[targetNode.PrevIndex].NextIndex = newNode->SelfIndex;	//	타겟 노드의 이전 노드의 다음 인덱스를 새 노드로 설정
				}
				else
				{
					HeadIndex = newNode->SelfIndex;
				}
				newNode->NextIndex = targetNode.SelfIndex;	//	새 노드의 다음 인덱스를 타겟 노드로 설정
				newNode->PrevIndex = targetNode.PrevIndex;
				targetNode.PrevIndex = newNode->SelfIndex;
			}
			else
			{
				Nodes[NodeMap[targetKey]].Values.Append(inNode.Values);
			}
		}
		else
		{
			PushFront(inNode);
		}
	}
	//	사실 이거 순서상의 라는 뜻임. 근데 당장은 필요없음
	//void InsertOrderedAt()

	void ShiftPrevious(NodeKeyType targetKey,int32 shiftCount = 1)
	{
		if (TNodeType* targetNode = Find(targetKey))
		{
			int32 targetPrevIndex = targetNode->PrevIndex;
			int32 targetNextIndex = targetNode->NextIndex;

			bool hasTargetPrev = Nodes.IsValidIndex(targetPrevIndex);
			bool hasTargetNext = Nodes.IsValidIndex(targetNextIndex);
			
			if (hasTargetPrev && hasTargetNext)
			{
				Nodes[targetPrevIndex].NextIndex = Nodes[targetNextIndex].SelfIndex;
				Nodes[targetNextIndex].PrevIndex = Nodes[targetPrevIndex].SelfIndex;
			}
			else
			{
				if (hasTargetPrev)
				{
					//	tail이라는 말임
					Nodes[targetPrevIndex].NextIndex = -1;	//	타겟 노드의 이전 노드의 다음 인덱스를 -1로 설정
					TailIndex = targetPrevIndex;	//	테일 인덱스를 타겟 노드의 이전 노드로 설정
				}
				else
				{
					//	prev 가없다는 말과 isolated 상태라는 말은 동일함
					return;
				}
			}
			
			TNodeType* destNode = targetNode;
			while (destNode->PrevIndex > -1 && shiftCount > 0)
			{
				destNode = &Nodes[destNode->PrevIndex];
				shiftCount--;
			}
			
			if (Nodes.IsValidIndex(destNode->PrevIndex))
			{
				Nodes[destNode->PrevIndex].NextIndex = targetNode->SelfIndex;
			}
			else
			{
				HeadIndex = targetNode->SelfIndex;	//	헤드 인덱스를 타겟 노드로 설정
			}
			targetNode->PrevIndex = destNode->PrevIndex;
			targetNode->NextIndex = destNode->SelfIndex;
			destNode->PrevIndex = targetNode->SelfIndex;
		}
	}
	void ShiftNext(NodeKeyType targetKey,int32 shiftCount = 1)
	{
		if (TNodeType* targetNode = Find(targetKey))
		{
			int32 targetPrevIndex = targetNode->PrevIndex;
			int32 targetNextIndex = targetNode->NextIndex;

			bool hasTargetPrev = Nodes.IsValidIndex(targetPrevIndex);
			bool hasTargetNext = Nodes.IsValidIndex(targetNextIndex);
			
			if (hasTargetPrev && hasTargetNext)
			{
				Nodes[targetPrevIndex].NextIndex = Nodes[targetNextIndex].SelfIndex;
				Nodes[targetNextIndex].PrevIndex = Nodes[targetPrevIndex].SelfIndex;
			}
			else
			{
				if (hasTargetNext)
				{
					//	head라는 말임
					Nodes[targetNextIndex].PrevIndex = -1;	//	타겟 노드의 다음 노드의 이전 인덱스를 -1로 설정
					HeadIndex = targetNextIndex;	//	헤드 인덱스를 타겟 노드의 다음 노드로 설정
				}
				else
				{
					return; // isolated 상태라는 말은 동일함
				}
			}
			
			TNodeType* destNode = targetNode;
			while (destNode->NextIndex > -1 && shiftCount > 0)
			{
				destNode = &Nodes[destNode->NextIndex];
				shiftCount--;
			}
			
			if (Nodes.IsValidIndex(destNode->NextIndex))
			{
				Nodes[destNode->NextIndex].PrevIndex = targetNode->SelfIndex;
			}
			else
			{
				TailIndex = targetNode->SelfIndex; // 테일 인덱스를 타겟 노드로 설정
			}
			targetNode->PrevIndex = destNode->SelfIndex;
			targetNode->NextIndex = destNode->NextIndex;
			destNode->NextIndex = targetNode->SelfIndex;
		}
	}
	//void MoveFront(NodeKeyType startKey,NodeKeyType destKey){}
	void Remove(NodeKeyType target)
	{
		if (TNodeType* targetNode = Find(target))
		{
			int32 targetPrevIndex = targetNode->PrevIndex;
			int32 targetNextIndex = targetNode->NextIndex;

			if (targetPrevIndex > -1)
			{
				Nodes[targetPrevIndex].NextIndex = targetNextIndex;	//	타겟 노드의 이전 노드의 다음 인덱스를 타겟 노드의 다음 인덱스로 설정
			}
			else
			{
				HeadIndex = targetNextIndex; // 헤드 인덱스를 타겟 노드의 다음 노드로 설정
			}
			
			if (targetNextIndex > -1)
			{
				Nodes[targetNextIndex].PrevIndex = targetPrevIndex; // 타겟 노드의 다음 노드의 이전 인덱스를 타겟 노드의 이전 인덱스로 설정
			}
			else
			{
				TailIndex = targetPrevIndex; // 테일 인덱스를 타겟 노드의 이전 노드로 설정
			}
			
			NodeMap.Remove(targetNode->Key); // 노드 맵에서 제거
			
			targetNode->Init(); // 초기화
		}
	}
	
	void Swap(NodeKeyType lhs, NodeKeyType rhs)
	{
		TNodeType* rhsNode = Find(rhs);
		TNodeType* lhsNode = Find(lhs);
		if (rhsNode != nullptr && lhsNode != nullptr)
		{
			int32 lhsPrevIndex = lhsNode->PrevIndex;
			int32 lhsNextIndex = lhsNode->NextIndex;
			int32 rhsPrevIndex = rhsNode->PrevIndex;
			int32 rhsNextIndex = rhsNode->NextIndex;
			if (lhsPrevIndex > -1)
			{
				Nodes[lhsPrevIndex].NextIndex = rhsNode->SelfIndex;
			}
			else
			{
				HeadIndex = rhsNode->SelfIndex; // 헤드 인덱스를 rhs 노드로 설정
			}
			
			if (rhsPrevIndex > -1)
			{
				Nodes[rhsPrevIndex].NextIndex = lhsNode->SelfIndex;
			}
			else
			{
				HeadIndex = lhsNode->SelfIndex; // 헤드 인덱스를 lhs 노드로 설정
			}
			if (lhsNextIndex > -1)
			{
				Nodes[lhsNextIndex].PrevIndex = rhsNode->SelfIndex;
			}
			else
			{
				TailIndex = rhsNode->SelfIndex; // 테일 인덱스를 rhs 노드로 설정
			}
			
			if (rhsNextIndex > -1)
			{
				Nodes[rhsNextIndex].PrevIndex = lhsNode->SelfIndex;
			}
			else
			{
				TailIndex = lhsNode->SelfIndex; // 테일 인덱스를 lhs 노드로 설정
			}
			
			lhsNode->PrevIndex = rhsPrevIndex;
			lhsNode->NextIndex = rhsNextIndex;
			
			rhsNode->PrevIndex = lhsPrevIndex;
			rhsNode->NextIndex = lhsNextIndex;
		}
	}
	
	int32 GetCount() const
	{
		return Nodes.Num();
	}
	
	bool IsEmpty() const
	{
		return Nodes.IsEmpty();
	}
	
private:
	TNodeType* AddNewNode(const TNodeType& nodeValue = TNodeType() )
	{
		TNodeType* result = nullptr;
		int32 newNodeIndex = Nodes.Add(nodeValue);
		Nodes[newNodeIndex].Init();
		Nodes[newNodeIndex].SelfIndex = newNodeIndex;
		Nodes[newNodeIndex].PrevIndex = -1;
		Nodes[newNodeIndex].NextIndex = -1;
		NodeMap.Add(Nodes[newNodeIndex].Key, newNodeIndex);
		result = &Nodes[newNodeIndex];
		return result;
	}
public:
	struct Iterator
	{
	private:
		TTIHHsLinkedNodeList* OwnerList;
		int32 CurrentIndex;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = TNodeType;
		using difference_type = int32;
		using pointer = TNodeType*;
		using reference = TNodeType&;

		Iterator(TTIHHsLinkedNodeList* list, NodeIndexType index)
			: OwnerList(list), CurrentIndex(index)
		{}
		reference operator*() const
		{
			return OwnerList->Nodes[CurrentIndex];
		}
		pointer operator->() const
		{
			return &OwnerList->Nodes[CurrentIndex];
		}
		Iterator& operator++()
		{
			if (CurrentIndex > -1 && OwnerList->Nodes.IsValidIndex(CurrentIndex))
			{
				CurrentIndex = OwnerList->Nodes[CurrentIndex].NextIndex;
			}
			return *this;
		}
		Iterator operator++(int)
		{
			Iterator temp = *this;
			++(*this);
			return temp;
		}
		bool operator==(const Iterator& other) const
		{
			return CurrentIndex == other.CurrentIndex && OwnerList == other.OwnerList;
		}
		bool operator!=(const Iterator& other) const
		{
			return !(*this == other);
		}
		Iterator& operator--()
		{
			if (CurrentIndex > -1 && OwnerList->Nodes.IsValidIndex(CurrentIndex))
			{
				CurrentIndex = OwnerList->Nodes[CurrentIndex].PrevIndex;
			}
			return *this;
		}
		Iterator operator--(int)
		{
			Iterator temp = *this;
			--(*this);
			return temp;
		}
		int32 GetCurrentIndex() const
		{
			return CurrentIndex;
		}
		bool IsValid() const
		{
			return CurrentIndex > -1 && OwnerList->Nodes.IsValidIndex(CurrentIndex);
		}
	};
	Iterator begin()
	{
		return Iterator(this, HeadIndex);
	}
	Iterator end()
	{
		return Iterator(this, -1);	//	-1은 비어있는 상태를 의미함
	}
	Iterator rbegin()
	{
		return Iterator(this, TailIndex);
	}
	Iterator rend()
	{
		return Iterator(this, -1);	//	-1은 비어있는 상태를 의미함
	}
	Iterator cbegin() const
	{
		return Iterator(const_cast<TTIHHsLinkedNodeList*>(this), HeadIndex);
	}
	Iterator cend() const
	{
		return Iterator(const_cast<TTIHHsLinkedNodeList*>(this), -1);	//	-1은 비어있는 상태를 의미함
	}
};

UENUM(BlueprintType)
enum class ETIHHsPrioritySteps : uint8
{
	/*
	 *	firstBefore,first,firstAfter,anyStep,MetaData,MetaDataAfter,MetaDataBefore,Trigger,TriggerAfter,TriggerBefore,Last..
	 *	이렇게 하면 만들수는 있음. 따지고 보면 저걸로 구분하는게 더 빠름. 심지어 저런 자료형도 안만들어도 괜찮음. 그냥 전체 검사하고,그  
	 *
	 * 
	 */
	EFirstBefore = 0,	//	첫번째 노드 이전
	EFirst,	//	첫번째 노드
	EFirstAfter,	//	첫번째 노드 이후
	EAnyStep,	//	어떤 단계든지
	EMetaDataBefore,	//	메타데이터 이전
	EMetaData,	//	메타데이터 단계
	EMetaDataAfter,	//	메타데이터 이후
	ETriggerBefore,	//	트리거 이전
	ETrigger,	//	트리거 단계
	ETriggerAfter,	//	트리거 이후
	ELast,	//	마지막 노드
	EEvaluate,
};
USTRUCT(BlueprintType)
struct FTIHHsPriorityQueryList
{
	GENERATED_BODY()
	
	TTIHHsLinkedNodeList<TTIHHsLinkedMultiNode<ETIHHsPrioritySteps,TScriptInterface<ITIHHsBaseObject>>> NodeList;
};

//	이거 생각보다 유용한듯
template<class E>
constexpr E ToFlags(int32 bits)
{
	using U = std::underlying_type_t<E>;
	return static_cast<E>(static_cast<U>(bits));
}
template <class E>
constexpr int32 ToBits(E e) {
	using U = std::underlying_type_t<E>;
	return static_cast<int32>( static_cast<U>(e) );
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTIHHsGizmoTriggerLoadMulticastDelegate, UObject*, callBack);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTIHHsGizmoTriggerLoadDelegate, UObject*, callBack);

//	이건 trigger를 생성할때만 필요함. 저장했을때는 불필요. 룰에서 제거
UENUM(BlueprintType, meta=(Bitflags))
enum class ETIHHsGizmoTriggerOverrideOptType : uint8
{
	ENone				= 0						UMETA(DisplayName="None",Hidden),
	ELocationOption	= 1 << 0					UMETA(DisplayName="Location "),
	ERotationOption	= 1 << 1					UMETA(DisplayName="Rotation"),
	EScaleOption		= 1 << 2				UMETA(DisplayName="Scale"),
	EInteractionFuncNamesOption = 1 << 3		UMETA(DisplayName="Interaction Function Names"),//	기본은 append
	EGizmoTagsOption	= 1 << 4				UMETA(DisplayName="Gizmo Tags"),				//	기본은 append
	ECollisionChannelOption	= 1 << 5				UMETA(DisplayName="Collision Channel"),
	ECollisionResponseOption	= 1 << 6				UMETA(DisplayName="Collision Response"),
	ECalculateTransformOption	 = 1 << 7				UMETA(DisplayName="Calculate Transform Option"),
	
};
ENUM_CLASS_FLAGS(ETIHHsGizmoTriggerOverrideOptType);
UENUM(BlueprintType)
enum class ETIHHsGizmoTriggerSpawnTransformRule : uint8
{
	ENone,
	EIdentity,	//	그냥 원점
	EReplace,	//	spawn data의 트랜스폼을 그대로 사용
	EMultiply,	//	Offset을 그대로 곱함
	EAdd,	//	Offset을 그대로 더함
	EPrefab,	//	prefab의 트랜스폼을 그대로 사용
	
};

USTRUCT(BlueprintType)
struct FTIHHsGizmoTriggerSpawnRule
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttachmentRule LocationRule = EAttachmentRule::KeepRelative;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttachmentRule RotationRule = EAttachmentRule::KeepRelative;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttachmentRule ScaleRule = EAttachmentRule::KeepRelative;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte< ECollisionChannel> CollisionChannel = ECC_WorldStatic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionResponse> CollisionResponse = ECR_Block;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETIHHsGizmoTriggerSpawnTransformRule LocationTransformRule = ETIHHsGizmoTriggerSpawnTransformRule::EReplace;
	
	FTIHHsGizmoTriggerSpawnRule() = default;
	FTIHHsGizmoTriggerSpawnRule(const FTIHHsGizmoTriggerSpawnRule& other) = default;
	FTIHHsGizmoTriggerSpawnRule(FTIHHsGizmoTriggerSpawnRule&& other) noexcept = default;
	FTIHHsGizmoTriggerSpawnRule& operator=(const FTIHHsGizmoTriggerSpawnRule& other) = default;
	FTIHHsGizmoTriggerSpawnRule& operator=(FTIHHsGizmoTriggerSpawnRule&& other) noexcept = default;
	FTIHHsGizmoTriggerSpawnRule(EAttachmentRule locationRule, EAttachmentRule rotationRule, EAttachmentRule scaleRule,TEnumAsByte<ECollisionChannel> collisionChannel, TEnumAsByte<ECollisionResponse> collisionResponse, ETIHHsGizmoTriggerSpawnTransformRule transformRule)
		: LocationRule(locationRule), RotationRule(rotationRule), ScaleRule(scaleRule),
		  CollisionChannel(collisionChannel), CollisionResponse(collisionResponse), LocationTransformRule(transformRule)
	{}

	static void OverrideRule(ETIHHsGizmoTriggerOverrideOptType overrideRule,const FTIHHsGizmoTriggerSpawnRule& srcRule,FTIHHsGizmoTriggerSpawnRule& outRule)
	{
		if (EnumHasAnyFlags(overrideRule, ETIHHsGizmoTriggerOverrideOptType::ELocationOption))
		{
			outRule.LocationRule = srcRule.LocationRule;
		}
		if (EnumHasAnyFlags(overrideRule, ETIHHsGizmoTriggerOverrideOptType::ERotationOption))
		{
			outRule.RotationRule = srcRule.RotationRule;
		}
		if (EnumHasAnyFlags(overrideRule, ETIHHsGizmoTriggerOverrideOptType::EScaleOption))
		{
			outRule.ScaleRule = srcRule.ScaleRule;
		}
		if (EnumHasAnyFlags(overrideRule, ETIHHsGizmoTriggerOverrideOptType::ECollisionChannelOption))
		{
			outRule.CollisionChannel = srcRule.CollisionChannel;
		}
		if (EnumHasAnyFlags(overrideRule, ETIHHsGizmoTriggerOverrideOptType::ECollisionResponseOption))
		{
			outRule.CollisionResponse = srcRule.CollisionResponse;
		}
		if( EnumHasAnyFlags(overrideRule, ETIHHsGizmoTriggerOverrideOptType::ECalculateTransformOption))
		{
			outRule.LocationTransformRule = srcRule.LocationTransformRule;
		}
	}
};

USTRUCT(BlueprintType)
struct FTIHHsGizmoTriggerSpawnBody	//	스폰할때 사용하는거
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite , meta=(DisplayName="Gizmo Name",ToolTip="기즈모 이름"))
	FString GizmoName = TEXT(""); //	기즈모 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Spawn Index",ToolTip="같은 이름의 기즈모가 여러개 있을수도 있으니 구분하기 위한 인덱스"),AdvancedDisplay)
	int32 SpawnIndex = 0; //	같은 이름의 기즈모가 여러개 있을수도 있으니 구분하기 위한 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Interaction Functions",ToolTip="인터렉션 함수 이름들"),AdvancedDisplay)
	TSet<FName> TriggerInteractionFuncNames;//	override
	UPROPERTY(EditAnywhere, BlueprintReadWrite,DisplayName = "Global Gizmo Tags",meta=(ToolTip="글로벌 기즈모 태그"),AdvancedDisplay)
	FGameplayTagContainer GizmoTags;//	override
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ShowOnlyInnerProperties=true,MakeEditWidget= true))
	FTransform RelativeTransform = FTransform::Identity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,meta=(ShowOnlyInnerProperties=true))
	TSoftClassPtr<UObject> SpawnTargetCls;

	class UTIHHsGizmoTriggerPrefabDataAsset* RefPrefabThema = nullptr; //	이거 넣는 이유. 어차피 이걸로 나중에 찾아야해서 넣은거임 글로벌 용도
	
	FTIHHsGizmoTriggerSpawnBody() = default;
	FTIHHsGizmoTriggerSpawnBody(const FTIHHsGizmoTriggerSpawnBody& other) = default;
	FTIHHsGizmoTriggerSpawnBody(FTIHHsGizmoTriggerSpawnBody&& other) noexcept = default;
	FTIHHsGizmoTriggerSpawnBody& operator=(const FTIHHsGizmoTriggerSpawnBody& other) = default;
	FTIHHsGizmoTriggerSpawnBody& operator=(FTIHHsGizmoTriggerSpawnBody&& other) noexcept = default;

	static void OverrideBody(ETIHHsGizmoTriggerOverrideOptType overrideOpt,const FTIHHsGizmoTriggerSpawnBody& overrideBody,FTIHHsGizmoTriggerSpawnBody& outBody )
	{
		if (EnumHasAnyFlags(overrideOpt, ETIHHsGizmoTriggerOverrideOptType::EInteractionFuncNamesOption))
		{
			outBody.TriggerInteractionFuncNames = overrideBody.TriggerInteractionFuncNames;
		}
		else
		{
			outBody.TriggerInteractionFuncNames.Append(overrideBody.TriggerInteractionFuncNames);
		}
		if (EnumHasAnyFlags(overrideOpt, ETIHHsGizmoTriggerOverrideOptType::EGizmoTagsOption))
		{
			outBody.GizmoTags = overrideBody.GizmoTags;
		}
		else
		{
			outBody.GizmoTags.AppendTags(overrideBody.GizmoTags);
		}
	}
	static FTIHHsGizmoTriggerSpawnBody DefaultSpawnBody()
	{
		FTIHHsGizmoTriggerSpawnBody result;
		result.GizmoName = TEXT("DefaultGizmo");
		result.SpawnIndex = 0;
		result.TriggerInteractionFuncNames = {};
		result.GizmoTags = {};
		result.RelativeTransform = FTransform::Identity;
		result.SpawnTargetCls = nullptr;
		result.RefPrefabThema = nullptr;
		return result;
	}
};

DECLARE_DELEGATE_ThreeParams(FTIHHsGizmoTriggerSpawnDelegate,
	const FTIHHsGizmoTriggerSpawnData&,AActor* ,UObject*);

/*
 *	visitor에도 들어가고 spawn에 대한 직접적인 정보를 제공하는 용도
 * 
 */
USTRUCT(BlueprintType)
struct FTIHHsGizmoTriggerSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ThemaName = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTIHHsGizmoTriggerSpawnRule SpawnRule = FTIHHsGizmoTriggerSpawnRule();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTIHHsGizmoTriggerSpawnBody SpawnBody = FTIHHsGizmoTriggerSpawnBody();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<ITIHHsInteraction> OwnerInteraction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta =(Bitmask,BitmaskEnum="ETIHHsGizmoTriggerOverrideOptType"))
	int32 OverrideOptions = ToBits(ETIHHsGizmoTriggerOverrideOptType::ENone); //	오버라이드 옵션, 기본값은 없음
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<class ATIHHsGizmoPrefabTriggerGeneratorBase> CustomGenerator = nullptr; //	커스텀 생성기, 기본값은 없음
	
	FTIHHsGizmoTriggerSpawnDelegate OnSpawnComplete; //	스폰 완료 콜백, 기본값은 없음
	
	FTIHHsGizmoTriggerSpawnData() = default;
	FTIHHsGizmoTriggerSpawnData(const FTIHHsGizmoTriggerSpawnData& other) = default;
	FTIHHsGizmoTriggerSpawnData(FTIHHsGizmoTriggerSpawnData&& other) noexcept = default;
	FTIHHsGizmoTriggerSpawnData& operator=(const FTIHHsGizmoTriggerSpawnData& other) = default;
	FTIHHsGizmoTriggerSpawnData& operator=(FTIHHsGizmoTriggerSpawnData&& other) noexcept = default;

	FTIHHsGizmoTriggerSpawnData(const FName& themaName, const FTIHHsGizmoTriggerSpawnRule& spawnRule, const FTIHHsGizmoTriggerSpawnBody& spawnBody, TScriptInterface<ITIHHsInteraction> ownerInteraction)
		: ThemaName(themaName), SpawnRule(spawnRule), SpawnBody(spawnBody), OwnerInteraction(ownerInteraction), OverrideOptions(ToBits(ETIHHsGizmoTriggerOverrideOptType::ENone))
	{}

	FTIHHsGizmoTriggerSpawnData& SetThemaName(const FName& themaName)
	{
		ThemaName = themaName;
		return *this;
	}
	FTIHHsGizmoTriggerSpawnRule& OnSetSpawnRule()
	{
		return SpawnRule;
	}
	FTIHHsGizmoTriggerSpawnData& SetSpawnRule(const FTIHHsGizmoTriggerSpawnRule& spawnRule)
	{
		SpawnRule = spawnRule;
		return *this;
	}
	FTIHHsGizmoTriggerSpawnBody& OnSetSpawnBody()
	{
		return SpawnBody;
	}
	FTIHHsGizmoTriggerSpawnData& SetSpawnBody(const FTIHHsGizmoTriggerSpawnBody& spawnBody)
	{
		SpawnBody = spawnBody;
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& SetOwnerInteraction(TScriptInterface<ITIHHsInteraction> ownerInteraction)
	{
		OwnerInteraction = ownerInteraction;
		return *this;
	}
	
	FTIHHsGizmoTriggerSpawnData& SetOwnerInteraction(UObject* ownerObject);

	FTIHHsGizmoTriggerSpawnData& SetOverrideOptions(ETIHHsGizmoTriggerOverrideOptType overrideOptions)
	{
		OverrideOptions = ToBits(overrideOptions);
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& OnOverrideLocation()
	{
		OverrideOptions |= ToBits(ETIHHsGizmoTriggerOverrideOptType::ELocationOption);
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& OnOverrideLocation(const FVector& location)
	{
		OverrideOptions |= ToBits(ETIHHsGizmoTriggerOverrideOptType::ELocationOption);
		SpawnBody.RelativeTransform.SetLocation(location);
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& OnOverrideRotation()
	{
		OverrideOptions |= ToBits(ETIHHsGizmoTriggerOverrideOptType::ERotationOption);
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& OnOverrideRotation(const FRotator& rotation)
	{
		OverrideOptions |= ToBits(ETIHHsGizmoTriggerOverrideOptType::ERotationOption);
		SpawnBody.RelativeTransform.SetRotation(FQuat(rotation));
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& OnOverrideScale()
	{
		OverrideOptions |= ToBits(ETIHHsGizmoTriggerOverrideOptType::EScaleOption);
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& OnOverrideScale(const FVector& scale)
	{
			OverrideOptions |= ToBits(ETIHHsGizmoTriggerOverrideOptType::EScaleOption);
		SpawnBody.RelativeTransform.SetScale3D(scale);
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& OnOverrideInteractionFuncNames()
	{
		OverrideOptions |= ToBits(ETIHHsGizmoTriggerOverrideOptType::EInteractionFuncNamesOption);
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& AddOverrideInteractionFuncName(const FName& funcName)
	{
		if (EnumHasAnyFlags(ToFlags<ETIHHsGizmoTriggerOverrideOptType>(OverrideOptions), ETIHHsGizmoTriggerOverrideOptType::EInteractionFuncNamesOption))
		{
			OverrideOptions |= ToBits(ETIHHsGizmoTriggerOverrideOptType::EInteractionFuncNamesOption);
		}
		SpawnBody.TriggerInteractionFuncNames.Add(funcName);
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& OnOverrideGizmoTags()
	{
		OverrideOptions |= ToBits(ETIHHsGizmoTriggerOverrideOptType::EGizmoTagsOption);
		return *this;
	}
	FTIHHsGizmoTriggerSpawnData& AddOverrideGizmoTag(const FName& tag)
	{
		const FGameplayTag gpTag = UGameplayTagsManager::Get().RequestGameplayTag(tag,false);
		if (gpTag.IsValid())
		{
			if (EnumHasAnyFlags(ToFlags<ETIHHsGizmoTriggerOverrideOptType>(OverrideOptions) , ETIHHsGizmoTriggerOverrideOptType::EGizmoTagsOption) )
			{
				OverrideOptions |= ToBits(ETIHHsGizmoTriggerOverrideOptType::EGizmoTagsOption);
			}
			SpawnBody.GizmoTags.AddTag(gpTag);
		}
		return *this;
	}
};
UCLASS()
class ATIHHsGizmoPrefabTriggerGeneratorBase : public AActor
{
	GENERATED_BODY()
public:
	
	
	UFUNCTION(BlueprintCallable)
	virtual UObject* GenerateGizmoTrigger(const FTIHHsGizmoTriggerSpawnData& spawnData) PURE_VIRTUAL(ATIHHsGizmoPrefabTriggerGeneratorBase::GenerateGizmoTrigger, return nullptr;);
};

/*
 *	prefab을 저장하기 위한 용도
 *
 * 
 */
UCLASS(BlueprintType,EditInlineNew,DefaultToInstanced)
class TIHHOUSINGCORE_API UTIHHsGizmoTriggerPrefabNode : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefab", meta=(DisplayName="Gizmo Name",ToolTip="노드 이름"))
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Spawn", meta=(DisplayName="Inherit Global Rule",ToolTip="글로벌 룰 상속 여부" ))
	bool InheritGlobalRule = true; //	글로벌 룰 상속 여부, 기본값은 true
	/*
		EAttachmentRule LocationRule = EAttachmentRule::KeepRelative;
		EAttachmentRule RotationRule = EAttachmentRule::KeepRelative;
		EAttachmentRule ScaleRule = EAttachmentRule::KeepRelative;
		TEnumAsByte< ECollisionChannel> CollisionChannel = ECC_WorldStatic;
		TEnumAsByte<ECollisionResponse> CollisionResponse = ECR_Block;
		ETIHHsGizmoTriggerSpawnTransformRule LocationTransformRule = ETIHHsGizmoTriggerSpawnTransformRule::EReplace;
	 */
	UPROPERTY(
		EditAnywhere, BlueprintReadWrite,Category = "Spawn",
		meta=(DisplayName="UseGlobal Options",ToolTip="오버라이드 옵션" , Bitmask, BitmaskEnum="/Script/TIHHousingCore.ETIHHsGizmoTriggerOverrideOptType",EditCondition = "InheritGlobalRule == true"))
	int32 InheritGlobalRuleOptions = static_cast<int32>(ETIHHsGizmoTriggerOverrideOptType::ENone);
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Spawn", meta=(DisplayName="Inherit Transform Rule",ToolTip="글로벌 룰 상속시 트랜스폼 룰" , EditCondition = "InheritGlobalRule == true"))
	ETIHHsGizmoTriggerSpawnTransformRule InheritTransformRule = (ETIHHsGizmoTriggerSpawnTransformRule::EReplace); //	글로벌 룰 상속시 트랜스폼 룰
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Spawn", meta=(DisplayName="Spawn Rule",ToolTip="스폰 규칙" , ShowOnlyInnerProperties=true))
	FTIHHsGizmoTriggerSpawnRule SpawnRule;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Spawn", meta=(DisplayName="Spawn Data",ToolTip="스폰 데이터" , ShowOnlyInnerProperties=true))
	FTIHHsGizmoTriggerSpawnBody SpawnBody;

	FTIHHsGizmoTriggerSpawnRule GetGizmoTriggerSpawnRule()
	{
		return SpawnRule;
	}
	FTIHHsGizmoTriggerSpawnBody GetGizmoTriggerSpawnBody()
	{
		return SpawnBody;
	}
	FTIHHsGizmoTriggerSpawnBody GetGizmoTriggerSpawnBodyConditioned(FTIHHsGizmoTriggerPrefabThemaData* themaData) const;
};
/*
 *	DataAsset -> 루트를 저장하고 에디터에서 수정,편집을 하여 데이터를 저장하는 용도
 *	노드 : public UObject -> 실질적인 데이터를 담고 있고, 계층을 구성 하게 해주는 용도
 */
UCLASS(Blueprintable)
class UTIHHsGizmoTriggerPrefabDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,
		Category = "Prefab",DisplayName = "Prefab Name" )
	FString PrefabThema;
	UPROPERTY(BlueprintReadWrite,EditAnywhere,
		Category = "Prefab",DisplayName = "Prefab Description",
		meta = (MultiLine = "true")
		)
	FString PrefabThemeDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Prefab" ,DisplayName = "Global Offset Transform",
		meta=(ShowOnlyInnerProperties=true,MakeEditWidget= true)
		)
	FTransform OffsetTransform;
	UPROPERTY(BlueprintReadWrite,EditAnywhere,
		Category = "Global",DisplayName = "Global Spawn Rule" )
	
	FTIHHsGizmoTriggerSpawnRule GlobalRule;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Global",DisplayName = "Global Interaction Function Names",
		meta=(AdvancedDisplay)
		)
	TSet<FName> GlobalInteractionFunctionNames;//	override
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Global" ,DisplayName = "Global Gizmo Tags")
	FGameplayTagContainer GlobalGizmoTags;//	override
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Custom" ,DisplayName = "Custom Manager Class")
	TSoftClassPtr<UObject> CustomManager;
	UPROPERTY(EditAnywhere,
		Category = "Custom" ,DisplayName = "Custom Properties"
		)
	FInstancedPropertyBag CustomProperties;

	// 규칙공유x, 계층x (독립 추가)
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite,
		Category="Prefab",DisplayName = "Trigger Gizmo Prefabs",meta=(TitleProperty="Name"))
	TArray<TObjectPtr<UTIHHsGizmoTriggerPrefabNode>> Prefabs;

	FTIHHsGizmoTriggerSpawnBody ToSpawnData()const
	{
		FTIHHsGizmoTriggerSpawnBody result;
		result.GizmoName = PrefabThema;
		result.SpawnIndex = 0;
		result.RelativeTransform = OffsetTransform;
		result.SpawnTargetCls = nullptr;
		result.TriggerInteractionFuncNames = GlobalInteractionFunctionNames;
		result.GizmoTags = GlobalGizmoTags;
		return result;
	}

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TIHUtils::GetGizmoTriggerPrefabType(), FName(*PrefabThema));
	}
};

USTRUCT(BlueprintType)
struct FTIHHsGizmoTriggerPrefabThemaData
{
	GENERATED_BODY()
	/*
	 *	UTIHHsGizmoTriggerPrefabDataAsset 여기 안에 있는것을 검색할 수 있게 풀어놓은거임.
	 * 
	 */
	
	UPROPERTY()
	UTIHHsGizmoTriggerPrefabDataAsset* DataAsset;
	
	UPROPERTY()
	TMap<FString, TObjectPtr<UTIHHsGizmoTriggerPrefabNode>> PrefabMap;	//	Prefabs에 대한 매핑 데이터임.

	void operator()(const FString& prefabName, TObjectPtr<UTIHHsGizmoTriggerPrefabNode> node)
	{
		if (!PrefabMap.Contains(prefabName) && node != nullptr)
		{
			PrefabMap.Add(prefabName, node);
		}
	}

	UTIHHsGizmoTriggerPrefabNode* operator()(const FString& prefabName) const
	{
		return TryGetizmoTriggerPrefabNode(prefabName);
	}
	UTIHHsGizmoTriggerPrefabNode* TryGetizmoTriggerPrefabNode(const FString& prefabName) const
	{
		if (const TObjectPtr<UTIHHsGizmoTriggerPrefabNode>* found = PrefabMap.Find(prefabName))
		{
			return *found;
		}
		return nullptr;
	}
};


UCLASS()
class TIHHOUSINGCORE_API ATIHHsGizmoTriggerProbeSpawner : public AActor
{
	GENERATED_BODY()

public:
	virtual UObject* SpawnGizmoTriggerProbe(const FTIHHsGizmoTriggerSpawnRule& spawnRule, const FTIHHsGizmoTriggerSpawnBody& spawnData, TScriptInterface<ITIHHsInteraction> ownerInteraction);
};

/*	prefab loader and prefab instancing and manager
 *	loader: 불러올 프리펩에 대한 정보를 뭐 어디 config에 넣든 뭐든 어쨌든 개발할땐 변수로 만들어 데이터 에셋을 불러온다.
 *		기능: 동기비동기기능
 *	instancing: 불러온 프리펩을 인스턴싱 해주는 역할
 *		기능: instancing, 
 *	manager: 프리펩을 관리하는 역할
 *		기능: 프리팹이 있는지 물어보거나 불러오는건 시발아 loader가 하는거고, 인스턴싱은 instancing이 하는거고
 *		pooling(만들어진걸 저장해두고 재사용하는 기능) 하고 상태 확인하고 물어보는것들은 모두 이곳에서 할거임. 
 *	
 */


USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTIHHsTriggerVisitor
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FTIHHsGizmoTriggerSpawnData> VisitorNodes;	//	방문 노드들
	FTIHHsGizmoTriggerSpawnData& AddVisitorNode(
		const FName& themaName,
		const FTIHHsGizmoTriggerSpawnRule& spawnRule,
		const FTIHHsGizmoTriggerSpawnBody& spawnBody,
		TScriptInterface<ITIHHsInteraction> ownerInteraction
	)
	{
		return VisitorNodes.Add_GetRef(FTIHHsGizmoTriggerSpawnData(themaName, spawnRule, spawnBody, ownerInteraction));
	}
	
	FTIHHsGizmoTriggerSpawnData& AddVisitorNode(const FTIHHsGizmoTriggerSpawnData& node)
	{
		return VisitorNodes.Add_GetRef(node);
	}
	FTIHHsGizmoTriggerSpawnData& operator()(const FTIHHsGizmoTriggerSpawnData& node	)
	{
		return AddVisitorNode(node);
	}
	FTIHHsGizmoTriggerSpawnData& operator()(const FName& themaName, const FTIHHsGizmoTriggerSpawnRule& spawnRule, const FTIHHsGizmoTriggerSpawnBody& spawnBody, TScriptInterface<ITIHHsInteraction> ownerInteraction)
	{
		return AddVisitorNode(themaName, spawnRule, spawnBody, ownerInteraction);
	}
};


















