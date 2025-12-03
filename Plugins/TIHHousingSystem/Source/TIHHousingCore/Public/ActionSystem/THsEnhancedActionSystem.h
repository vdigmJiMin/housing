// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "GameplayTagContainer.h"
#include "InteractiveGizmoManager.h"
#include "InteractiveToolsContext.h"
#include "SceneViewExtension.h"
#include "BaseGizmos/GizmoActor.h"
#include "Cores/TIHHsCoreClasses.h"
#include "Cores/TIHHsCoreInterface.h"
//#include "Gizmo/THsInteractiveGizmoCore.h"
#include "Subsystems/WorldSubsystem.h"
#include "THsEnhancedActionSystem.generated.h"

class ATHsInteractiveGizmoManager;
class UTransformProxy;
class UCombinedTransformGizmo;
//	THsH
class UTHsActionPoint;
class UTHsEnhancedActionSystem;
class UTHsEnhancedActionSystemDataSetting;
class ATHsActionCursor;
class UTHsActionMouseClick;
/**
 * 앞으로 줄임말 *THs**** 로한다.
 *	ex) UTHsActionGesture, FTHsActionType
 */


//	[[move]]:  델리게이트의 경우에는 다른 곳으로 옮기기 애매함. 한쪽에 모아두는것이 상책
DECLARE_MULTICAST_DELEGATE_OneParam(FTHsEventDelegate,UTHsActionPoint*)
DECLARE_MULTICAST_DELEGATE_TwoParams(FTHsProcessDelegate,UTHsActionPoint*,float)
DECLARE_MULTICAST_DELEGATE_TwoParams(FTHsCursorDelegate,ATHsActionCursor*,float);

/*
 *	command
 *		selectable
 *		hoverable
 *		placeable
 *		movable
 *	linetrace
 *		
 *		
 *			
 *
 * 
 */
//[[move]]: 태그도 다른곳으로 옮기기 애매함. 한쪽에 모아두는것이 상책
UE_DECLARE_GAMEPLAY_TAG_EXTERN(THS_Action_Trait)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(THS_Action_Trait_Selectable	)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(THS_Action_Trait_Hoverable		)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(THS_Action_Trait_Attachable	)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(THS_Action_Trait_Movable		)

//[[move]]: enum 들도 한쪽에 모아서 core로 해놓자.
// UENUM()
// enum class ETHsActionFunctionCategory : uint8
// {
// 	EEvent UMETA(DisplayName="Event", ToolTip="just event. argument is only UTHsActionPoint*"),
// 	EProcess UMETA(DisplayName="Process", ToolTip="step type. start, ongoing, end. argument is UTHsActionPoint* key, float deltaTime"),
// 	ECursor UMETA(DisplayName="Cursor", ToolTip="cursor type. start, ongoing, end. argument is ATHsActionCursor* cursor,float deltaTime"),
// 	// EZCustom0,
// 	// EZCustom1,
// 	// EZCustom2,
// };
//
// UENUM(Blueprintable)
// enum class ETHsActionFunctionType : uint8
// {
// 	EProcess_Pressing,
// 	EProcess_HoldStart,
// 	EProcess_HoldOngoing,
// 	EProcess_HoldEnd,
// 	ECursor_HoverStart,
// 	ECursor_HoverOngoing,
// 	ECursor_HoverEnd,
// 	EEvent_Click,
// 	EEvent_DoubleClick,
// 	EEvent_Down,
// 	EEvent_Up,
// 	EEvent_Triggered,
// 	EEvent_Cancel,
// 	ECursor_CursorMove,
// 	EProcess_DragStart,
// 	EProcess_DragOngoing,
// 	EProcess_DragEnd,
// 	
// 	EZCustom0,
// 	EZCustom1,
// 	EZCustom2,
// 	EZCustom3,
// 	EZCustom4,
// 	EZCustom5,
// 	EZCustom6,
// 	EZCustom7,
// 	EZCustom8,
// };
// UENUM()
// enum class ETHsActionKeyBehaviorNodeExecuteType
// {
// 	EFirstOne,
// 	ELastOne,
// 	EAll,
// 	ESequence,
// 	ESelection,
// };
// UENUM()
// enum class ETHsActionMouseClickDragState
// {
// 	EDragNone,
// 	EDragStarted,
// 	EDragOngoing,
// 	EDragEnded,
// };
// UENUM(BlueprintType)
// enum class ETHsActionCursorHoverType : uint8
// {
// 	ENone,
// 	EHoverStart,
// 	EHoverOngoing,
// 	EHoverEnd,
// };
// UENUM(BlueprintType)
// enum  class  ETHsActionPointEvaluationState : uint8
// {
// 	EWaitingDown				UMETA(DisplayName="WaitingDown"),
// 	EEvaluateOngoingStart			UMETA(DisplayName="CheckingHoldStart"),
// 	EOngoing					UMETA(DisplayName="Ongoing"),
// 	ECheckingDoubleClick		UMETA(DisplayName="CheckingDoubleClick"),
// };
// UENUM(BlueprintType, meta = (Bitflags))
// enum class ETHsActionCursorState : uint8
// {
// 	// UPROPERTY()
// 	// bool bIsMove = false; //	이동이 있었는지 여부임.
// 	// UPROPERTY()
// 	// bool bIsHit = false; //	충돌이 있었는지 여부임.<- 물론 hit.isvalid로도 확인 가능함.
// 	// UPROPERTY()
// 	// bool bIsSameHitActor = false;	//	이게 되었다고 해서 같은 컴포넌트라는 보장은 없음. 그리고 무조건 not null 이라는 보장도 없음.
// 	// UPROPERTY()
// 	// bool bIsSameComponent = false; //	이게 되었다고 해서 같은 액터 라는 보장은 없음(어지간해서는 같은 액터는 맞음). 그리고 무조건 not null 이라는 보장도 없음.
// 	// UPROPERTY()
// 	// bool bIsCastedTIHHsInteractionActor = false;
// 	// UPROPERTY()
// 	// bool bIsCastedTIHHsInteractionComponent = false;
// 	ENone									= 0					,
// 	EIsMove									= 1<<0				,
// 	EIsHit									= 1<<1				,
// 	EIsSameHitActor							= 1<<2				,
// 	EIsSameComponent						= 1<<3				,
// 	EIsCastedTIHHsInteractionActor 			= 1<<4				,
// 	EIsCastedTIHHsInteractionComponent		= 1<<5				,
// };
// UENUM()
// enum class ETHsActionCursorMoveState
// {
// 	EMouseMovePaused,
// 	EMouseMoveStarted,
// 	EMouseMoveOngoing,
// 	EMouseMoveEnded,
// };
// UENUM(BlueprintType, meta = (Bitflags))
// enum class ETHsEnhancedActionFuncFilterFlag : uint8
// {
// 	ENone			= 0			UMETA(DisplayName="None"),
// 	ECursorMove		= 1<<0		UMETA(DisplayName="CursorMove"),
// 	EHover			= 1<<1		UMETA(DisplayName="Hover"),
// 	EClick			= 1<<2		UMETA(DisplayName="Click"),
// };
// ENUM_CLASS_FLAGS(ETHsEnhancedActionFuncFilterFlag);
//
// UENUM(BlueprintType)
// enum class ETHsActionPointPressingStateType : uint8
// {
// 	ENone			UMETA(DisplayName="None"),
// 	EPressStarted	UMETA(DisplayName="PressStarted"),
// 	EPressOngoing	UMETA(DisplayName="PressOngoing"),
// 	EPressEnded		UMETA(DisplayName="PressEnded"),
// };
// UENUM(BlueprintType)
// enum class ETHsGizmoUpdateInputDeviceStateType : uint8
// {
// 	EPressed		UMETA(DisplayName="Pressed"),
// 	EOngoing		UMETA(DisplayName="Ongoing"),
// 	EReleased		UMETA(DisplayName="Released"),
// 	EHovered		UMETA(DisplayName="Hovered"),
// };
//[[move]]: 여기까지는 코어로 넣어도 됨.
namespace TIHUtils
{
	namespace GlobalConfigDefault
	{
		static const TSet<FKey> ALL_KEYBOARD_KEYS = {
			EKeys::A, EKeys::B, EKeys::C, EKeys::D, EKeys::E, EKeys::F, EKeys::G, EKeys::H,
			EKeys::I, EKeys::J, EKeys::K, EKeys::L, EKeys::M, EKeys::N, EKeys::O, EKeys::P,
			EKeys::Q, EKeys::R, EKeys::S, EKeys::T, EKeys::U, EKeys::V, EKeys::W, EKeys::X,
			EKeys::Y, EKeys::Z,
			EKeys::Zero, EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five,
			EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine,
			EKeys::SpaceBar, EKeys::Enter, EKeys::BackSpace, EKeys::Tab,
			EKeys::LeftShift, EKeys::RightShift, EKeys::LeftControl, EKeys::RightControl,
			EKeys::LeftAlt, EKeys::RightAlt, EKeys::CapsLock, EKeys::Escape,
			EKeys::F1, EKeys::F2, EKeys::F3, EKeys::F4, EKeys::F5, EKeys::F6,
			EKeys::F7, EKeys::F8, EKeys::F9, EKeys::F10, EKeys::F11, EKeys::F12
		};
		static const TSet<FKey> MODIFIER_KEYS = {
			EKeys::LeftShift, EKeys::RightShift, EKeys::LeftControl, EKeys::RightControl,
			EKeys::LeftAlt, EKeys::RightAlt
		};
		static const TSet<FKey> ALPHABET_KEYS = {
			EKeys::A, EKeys::B, EKeys::C, EKeys::D, EKeys::E, EKeys::F, EKeys::G, EKeys::H,
			EKeys::I, EKeys::J, EKeys::K, EKeys::L, EKeys::M, EKeys::N, EKeys::O, EKeys::P,
			EKeys::Q, EKeys::R, EKeys::S, EKeys::T, EKeys::U, EKeys::V, EKeys::W, EKeys::X,
			EKeys::Y, EKeys::Z
		};
		static const TSet<FKey> NUMBER_KEYS = {
			EKeys::Zero, EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five,
			EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine
		};
		static const TSet<FKey> FUNCTION_KEYS = {
			EKeys::F1, EKeys::F2, EKeys::F3, EKeys::F4, EKeys::F5, EKeys::F6,
			EKeys::F7, EKeys::F8, EKeys::F9, EKeys::F10, EKeys::F11, EKeys::F12
		};
		static const TSet<FKey> SPECIAL_KEYS = {
			EKeys::SpaceBar, EKeys::Enter, EKeys::BackSpace, EKeys::Tab,
			EKeys::CapsLock, EKeys::Escape
		};
		// 모든 마우스 입력
		static const TSet<FKey> ALL_MOUSE_KEYS = {
			EKeys::LeftMouseButton, EKeys::RightMouseButton, EKeys::MiddleMouseButton,
			EKeys::ThumbMouseButton, EKeys::ThumbMouseButton2,
			//EKeys::MouseX, EKeys::MouseY, EKeys::MouseWheelAxis
		};

		// 모든 터치 입력
		static const TSet<FKey> ALL_TOUCH_KEYS = {
			EKeys::TouchKeys[0], EKeys::TouchKeys[1], EKeys::TouchKeys[2],
			EKeys::TouchKeys[3], EKeys::TouchKeys[4], EKeys::TouchKeys[5],
			EKeys::TouchKeys[6], EKeys::TouchKeys[7], EKeys::TouchKeys[8],
			EKeys::TouchKeys[9]
		};
		

		// 통합 모든 입력
		static TArray<FKey> GetAllInputKeys()
		{
			TArray<FKey> AllKeys;
			AllKeys.Append(ALL_KEYBOARD_KEYS.Array());
			AllKeys.Append(ALL_MOUSE_KEYS.Array());
			AllKeys.Append(ALL_TOUCH_KEYS.Array());
			return AllKeys;
		}

		namespace InputTiming
		{
			static constexpr float Double_Click_Threashold = 0.3f; // 0.3초 이내에 두 번 클릭하면 더블 클릭으로 간주
			static constexpr float Hold_Threashold = 0.5f; // 0.5초 이상 누르고 있으면 홀드로 간주
		}
	}
}

//[[move]]: 이거는 쓰는곳에 액션 포인트뿐이긴 한데 BindingInteractionInterface 이거때문에라도 액션포인트와 함께 있어야함.
USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTHsActionBindContext
{
	GENERATED_BODY()
	
	FTHsEventDelegate DelegateEventDown;
	FTHsEventDelegate DelegateEventUp;
	FTHsEventDelegate DelegateEventTriggered;
	FTHsEventDelegate DelegateEventCancelled;
	
	FTHsEventDelegate DelegateEventDoubleClick;
	FTHsEventDelegate DelegateEventClick;
	
	FTHsProcessDelegate DelegateProcessPressing;
	FTHsProcessDelegate DelegateProcessHoldStart;
	FTHsProcessDelegate DelegateProcessHoldOngoing;
	FTHsProcessDelegate DelegateProcessHoldEnd;
	
	FTHsEventDelegate DelegateOngoingExpiredCallback;		//	etcEvent 로 분류
	FTHsEventDelegate DelegateDoubleClickExpiredCallback;

	FTHsEventDelegate DelegateJustClickedCallback;

	FTHsProcessDelegate DelegateProcessDragStart;
	FTHsProcessDelegate DelegateProcessDragOngoing;
	FTHsProcessDelegate DelegateProcessDragEnd;

	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	bool bBindKeyActionStarted = true;
	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	bool bBindKeyActionCompleted = true;
	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	bool bBindKeyActionTriggered = true;
	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	bool bBindKeyActionCancelled = false;
	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	bool bBindKeyActionOngoing = false;
	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	bool bTraceHover = false;
	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	bool bTraceMove = false;
	
	void BindingInteractionInterface(UTHsActionPoint* actionPoint,TScriptInterface<ITIHHsInteraction> interaction)
	{
		FObjectKey objKey(interaction.GetObject());
		if (not BindedInteractionIndexMap.Contains(objKey))
		{
			int32 indexValue = BindedInteractions.Add(interaction);
			BindedInteractionIndexMap.Add(objKey,indexValue);
			interaction->BindKey(actionPoint);
			return;
		}
		int32 bindIndex = BindedInteractionIndexMap[ objKey ];
		BindedInteractions[bindIndex]->UnBindKey(actionPoint);
		BindedInteractions[bindIndex] = interaction;
		interaction->BindKey(actionPoint);
	}
	
private:
	TMap<FObjectKey,int32> BindedInteractionIndexMap;
	TArray<TScriptInterface<ITIHHsInteraction>> BindedInteractions;
};
//[[move]]:이거 목적이 액션의 행동들을 쿼리하는건데 이거 필요없음.
UINTERFACE()
class UTHsActionKeyBehaviorQuery : public UInterface
{
	GENERATED_BODY()
};

class ITHsActionKeyBehaviorQuery
{
	GENERATED_BODY()

public:
	virtual bool TryExecute(struct FTHsActionKeyBehaviorNodeContainer& queries, UTHsEnhancedActionSystem* actionSystem){ return true;} 
};


USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTHsActionKeyBehaviorNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETHsActionFunctionType ActionFunctionType = ETHsActionFunctionType::EEvent_Click;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<ITHsActionKeyBehaviorQuery> BehaviorQuery;
};


//[[move]]: 따지고 보면 inputbehavior랑 비슷한 원리임 그래서 필요없음
USTRUCT(BlueprintType)
struct TIHHOUSINGCORE_API FTHsActionKeyBehaviorNodeContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETHsActionKeyBehaviorNodeExecuteType ExecuteType = ETHsActionKeyBehaviorNodeExecuteType::EFirstOne;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTHsActionKeyBehaviorNode> Nodes;

	bool ExecuteNodes(UTHsEnhancedActionSystem* actionSystem);

	
};
//[[move]]: 데이터 에셋용도라 시스템과 떨어져 있어도 괜찮음. 다른 곳을 파서 옮기는걸로
USTRUCT(BlueprintType)
struct FTHsEnhancedActionSystemDataSettingData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bUseAllInputKeys = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bExploreInputMappingContext = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input" ,meta = (ToolTip= "bExploreAllInputKeys 가 true 일때는 excludeKeys 로 동작합니다."))
	TArray<FKey> KeyKeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (ToolTip= "bExploreAllInputKeys 가 true 일때는 excludeKeys 로 동작합니다."))
	TArray<FSoftObjectPath> InputMappingContextPaths;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float InputTickInterval = 1.0f / 30.0f;	//	30fps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TMap<FKey,TSubclassOf<UTHsActionPoint>> OverrideActionPointPathMap; //	FKey
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Timing")
	float DoubleClickThreshold = TIHUtils::GlobalConfigDefault::InputTiming::Double_Click_Threashold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Timing")
	float DefaultHoldStartThreshold = TIHUtils::GlobalConfigDefault::InputTiming::Hold_Threashold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Timing")
	float DefaultHoldOngoingInterval = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Timing")
	float TripleClickThreshold = 0.4f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Action")
	FTHsActionBindContext DefaultActionBindContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	bool bShowCursor = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	float MouseMoveSensitive = 0.00001f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	float MouseMoveTimeThreshold = 1.0f / 30.0f;	//	30fps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	float LineTraceDistance = 10000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	int32 ReserveRecordCount = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TMap<FName,FGameplayTagContainer> CommandGameplayTagMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TMap<FName,FTHsActionKeyBehaviorNodeContainer> KeyBehaviorNodeMap; //	FName
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gizmo|Material")
	TMap<FName,UMaterialInterface*> DefaultMaterialMap;	
	
	FTHsEnhancedActionSystemDataSettingData() = default;
	FTHsEnhancedActionSystemDataSettingData(const FTHsEnhancedActionSystemDataSettingData& other) = default;
	FTHsEnhancedActionSystemDataSettingData(FTHsEnhancedActionSystemDataSettingData&& other) noexcept = default;
	FTHsEnhancedActionSystemDataSettingData& operator=(const FTHsEnhancedActionSystemDataSettingData& other) = default;
	FTHsEnhancedActionSystemDataSettingData& operator=(FTHsEnhancedActionSystemDataSettingData&& other) noexcept = default;
};
//[[move]]: 마찬가지
UCLASS()
class UTHsEnhancedActionSystemDataSetting : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = "Input")
	FTHsEnhancedActionSystemDataSettingData Data;
	
	FORCEINLINE bool IsUseAllInputKeys() const {			return Data.bUseAllInputKeys; }
	FORCEINLINE bool IsExploreInputMappingContext() const { return Data.bExploreInputMappingContext; }
	FORCEINLINE const TArray<FKey>& GetKeyKeys() const {	return Data.KeyKeys; }
	FORCEINLINE const TArray<FSoftObjectPath>& GetInputMappingContextPaths() const { return Data.InputMappingContextPaths; }
	FORCEINLINE float GetDefaultDoubleClickThreshold() const {		return Data.DoubleClickThreshold; }
	FORCEINLINE float GetDefaultHoldStartThreshold() const {		return Data.DefaultHoldStartThreshold; }
	FORCEINLINE float GetHoldOngoingInterval() const {		return Data.DefaultHoldOngoingInterval; }
	FORCEINLINE float GetTripleClickThreshold() const {		return Data.TripleClickThreshold; }
	
	FORCEINLINE TSubclassOf<UTHsActionPoint>* GetOverrideActionPointPath(const FKey& key) 
	{
		return Data.OverrideActionPointPathMap.Find(key);
	}
	FORCEINLINE float GetMouseMoveSensitive() const { return Data.MouseMoveSensitive; }
	FORCEINLINE float GetMouseMoveTimeThreshold() const { return Data.MouseMoveTimeThreshold; }
	FORCEINLINE float GetLineTraceDistance() const { return Data.LineTraceDistance; }
	FORCEINLINE int32 GetReserveRecordCount() const { return Data.ReserveRecordCount; }
	UMaterialInterface* GetGizmoDefaultMaterial(FName Name){
		if (Data.DefaultMaterialMap.Contains(Name))
		{
			return Data.DefaultMaterialMap[Name];
		}
		return nullptr;
	}
};



/*
	raw Input key - enhanced Input action
	----
	UTHsConfigSubsystem : UGameInstanceSubsystem
		- 모든 입력키를 관리한다.
		- 기본적으로 제공하는 키는 모두 포함한다.
		- 커스텀 키를 추가할 수 있다.
		- 커스텀 키를 제거할 수 있다.
		- 모든 키를 반환할 수 있다.
		- 기본적으로 에셋을 검색해서 커스텀 키를 등록한다.
		- 제외키도 설정 할 수 있다.
*/

//[[move]]: 이거 쓰이는 곳을 보면 인핸스액션시스템밖에 없음. 그리고 대부분 그냥 들고오는 용도로 쓰는거임. 딱히 필요가 없음.

USTRUCT(BlueprintType)
struct FTHsSettingBoolean
{
	GENERATED_BODY()

	struct
	{
		int32 WholeValue = 0;
	}SettingValueUnion;
	
	FORCEINLINE bool IsTrue() const { return SettingValueUnion.WholeValue != 0; }
	FORCEINLINE bool IsFalse() const { return not IsTrue(); }
	
	FORCEINLINE void SetBool(bool bInTrue = true) { SettingValueUnion.WholeValue = bInTrue ? 1 : 0; }
	FORCEINLINE void SetTrue() { SetBool(true); }
	FORCEINLINE void SetFalse() { SetBool(false); }

	FORCEINLINE bool GetBoolean() const { return IsTrue(); }
	
};

UCLASS()
class UTHsSettingCaller : public UObject
{
	GENERATED_BODY()
public:
	virtual void CallFunc(){}
	
};
/*
 *	world subsystem 이 필요함.
 *		시동을 걸어줄 놈이며, 모든 액터나 설정들이 모두 beginplay에서 등록을 마치고 난 후에 
 *
 * 
 */

UCLASS()
class TIHHOUSINGCORE_API UTHsSettingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		InitializeDefaultKeys();
	}
	virtual void Deinitialize() override
	{
		
	}
	
	UFUNCTION()
	TArray<FKey> GetAllInputKeys() const
	{
		TArray<FKey> AllKeys;
		AllKeys.Append(KeyboardKeys);
		AllKeys.Append(MouseKeys);
		AllKeys.Append(TouchKeys);
		return AllKeys;
	}
	UFUNCTION()
	void AddCustomKey(const FKey& key,const FString& category = "keyboard");
	UFUNCTION()
	void RemoveCustomKey(const FKey& key);
	UFUNCTION()
	void InitializeDefaultKeys();

	bool bIsEnhancedActionTickableInEditor = true;
	bool bIsTickableWhenPaused = false;
	
	
	UPROPERTY(BlueprintReadWrite, Category = "Input|Keyboard")
	TArray<FKey> KeyboardKeys;
	UPROPERTY(BlueprintReadWrite, Category = "Input|Mouse")
	TArray<FKey> MouseKeys ;
	UPROPERTY(BlueprintReadWrite, Category = "Input|Touch")
	TArray<FKey> TouchKeys ;
	
	UPROPERTY(BlueprintReadWrite, Category = "Input|Timing")
	float DoubleClickThreshold = 0.3f;
	UPROPERTY(BlueprintReadWrite, Category = "Input|Timing")
	float HoldStartThreshold = 0.5f;
	UPROPERTY(BlueprintReadWrite, Category = "Input|Timing")
	float HoldOngoingInterval = 0.1f;
	UPROPERTY(BlueprintReadWrite, Category = "Input|Timing")
	float TripleClickThreshold = 0.4f;

	UPROPERTY(BlueprintReadWrite, Category = "GameState")
	bool bCompleteInputKeySetup = false;
	UPROPERTY(BlueprintReadWrite, Category = "GameState")
	bool bCompleteCursorInit = false;
	UPROPERTY(BlueprintReadWrite, Category = "GameState")
	bool bCompleteGizmoInit;
	//	TODO: 나중에 세이브로 저장할 수 있도록 해야함.나중에 로드로 불러올 수 있도록 해야함.
	//	TODO: 상태가 완료되었을때 델리게이트도 발생시켜야함. 근데 지금은 안함.
	
	UFUNCTION()
	void CompleteInputKeySetup()
	{
		bCompleteInputKeySetup = true;
	}
	UFUNCTION()
	void CompleteCursorInit()
	{
		bCompleteCursorInit = true;
	}
	UFUNCTION()
	void CompleteGizmoInit()
	{
		bCompleteGizmoInit = true;
	}
	bool IsValidDependency() const
	{
		bool result = false;
		if (UTHsDependencyBase* di= mGizmoDependency.Get())
		{
			result = di->GetWorld() != nullptr;
			result &= (di->GetGizmoManager() != nullptr);
			result &= (di->GetActionSystem() != nullptr);
		}
		
		return result;
	}
	UTHsDependencyBase* TryGetGizmoDependency() const
	{
		if (mGizmoDependency.Get() == nullptr)
		{
			mGizmoDependency.Reset( NewObject<UTHsDependencyBase>());
		}
		return mGizmoDependency.Get();
	}
	bool IsExistBooleanSetting(FName name) const
	{
		return mBooleanSettings.Contains(name);
	}
	bool IsTrueBooleanSetting(FName name) const
	{
		const FTHsSettingBoolean* booleanSetting = mBooleanSettings.Find(name);
		if (booleanSetting == nullptr)
		{
			return false;
		}
		return booleanSetting->IsTrue();
	}
	
	void RegisterCallerOrBoolean(FName name,UTHsSettingCaller* caller = nullptr)
    {
        if (caller != nullptr)
        {
	        mCallerSettings.FindOrAdd(name,caller);
        }
		OnSettingBooleanFalse(name);
    }
	
	FTHsSettingBoolean* FindOrAddBooleanSetting(FName name)
	{
		FTHsSettingBoolean* booleanSetting = mBooleanSettings.Find(name);
		if (booleanSetting == nullptr)
		{
			booleanSetting = &mBooleanSettings.Add(name);
		}
		return booleanSetting;
	}
	void OnSettingBooleanTrue(FName name)
	{
		FTHsSettingBoolean* booleanSetting = FindOrAddBooleanSetting(name);
		booleanSetting->SetTrue();
		CallBackCaller(name);
	}
	void OnSettingBooleanFalse(FName name)
	{
		FTHsSettingBoolean* booleanSetting = FindOrAddBooleanSetting(name);
		booleanSetting->SetFalse();
	}
	void CallBackCaller(FName name)
	{
		if (mCallerSettings.Contains(name))
		{
			mCallerSettings[name]->CallFunc();
		}
	}
	
private:
	TMap<FName,FTHsSettingBoolean> mBooleanSettings;
	UPROPERTY()
	TMap<FName,UTHsSettingCaller*> mCallerSettings;
	mutable TStrongObjectPtr<UTHsDependencyBase> mGizmoDependency = nullptr;
};

namespace TIHUtils
{
	inline UTHsSettingSubsystem* GetSettingSubsystem(UObject* ueObj = nullptr)
	{
		UTHsSettingSubsystem* result = nullptr;
		UWorld* world = nullptr;
		if (ueObj == nullptr)
		{
			world = GetTHsWorldSimple();
			if (world)
			{
				world = GEngine->GetWorldFromContextObject(ueObj, EGetWorldErrorMode::LogAndReturnNull);
			}
		}
		else
		{
			world = ueObj->GetWorld();
		}
		
		if (world)
		{
			if (UGameInstance* gameInstance = world->GetGameInstance())
			{
				result = gameInstance->GetSubsystem<UTHsSettingSubsystem>();
			}
			//result = world->GetSubsystem<UTHsSettingSubsystem>();
		}
		
		return result;
	}
	inline EInputDevices GetInputKeyType(const FKey& key)
	{
		if (GlobalConfigDefault::ALL_KEYBOARD_KEYS.Contains(key))
		{
			return EInputDevices::Keyboard;
		}
		else if (GlobalConfigDefault::ALL_MOUSE_KEYS.Contains(key))
		{
			return EInputDevices::Mouse;
		}
		else if (GlobalConfigDefault::ALL_TOUCH_KEYS.Contains(key))
		{
			return EInputDevices::TabletFingers;
		}
		return EInputDevices::None;
	}
}



USTRUCT()
struct TIHHOUSINGCORE_API FTHsActionPointDelegateFilterLayer
{
	GENERATED_BODY()
	
	FTHsEventDelegate Delegate;
};



//[[move]]: 이거는 핵심인데, 액션포인트들을 묶어주는 카테고리 같은거임. 아이디어는 ITF에서 따옴. 이거는 좀 앞선곳에 있어도 됨. 코어는 너무 앞이고 그 이후에 넣는식으로 core - {inter} - actionpoint - actionsystem 이런식인데, 아마 core와 actionpoint 사이에 넣어야할듯.
UINTERFACE()
class UTHsActionPointAPI : public UInterface
{
	GENERATED_BODY()
};

class ITHsActionPointAPI
{
	GENERATED_BODY()

public:
	virtual ETHsEnhancedActionFuncFilterFlag GetEnhancedActionFilterFlags()
	PURE_VIRTUAL(ITHsActionPointAPI::GetEnhancedActionFilterFlags, return ETHsEnhancedActionFuncFilterFlag::ENone;);
	
	UFUNCTION()
	virtual void PostInitActionPoint(UTHsEnhancedActionSystem* actionSystem,const UInputAction* inputAction, const FKey& key,int32 index){}
	UFUNCTION()
	virtual void PostEvaluateActionPoint(float CurrentTime, float frameDeltaTime){}

	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointDown(){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPressing(UTHsActionPoint* actionPoint, float deltaTime){}
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointUp(){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointClick(){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointDoubleClick(){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointOngoingStart(const float deltaTime){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointOngoing(const float deltaTime){};
	//UFUNCTION(Category="ActionPoint|Override")
	UFUNCTION()
	virtual void OverrideActionPointOngoingEnd(const float deltaTime){};
	
	UFUNCTION()
	virtual void OverrideActionPointHoverStart(ATHsActionCursor* cursor){};
	UFUNCTION()
	virtual void OverrideActionPointHoverOngoing(ATHsActionCursor* cursor){};
	UFUNCTION()
	virtual void OverrideActionPointHoverEnd(ATHsActionCursor* cursor){};

	UFUNCTION()
	virtual void OverrideActionPointClickDragStart(float deltaTime){};
	UFUNCTION()
	virtual void OverrideActionPointClickDragOngoing( float deltaTime){};
	UFUNCTION()
	virtual void OverrideActionPointClickDragEnd( float deltaTime){};
	//	ActionMouseMove
	UFUNCTION()
	virtual void OverrideActionPointMouseMove(ATHsActionCursor* cursor,float deltaTime){};
	UFUNCTION()
	virtual void OverrideActionPointSafeEnd(TScriptInterface<ITHsActionPointAPI> actionPoint){};
	
};

//	여기도 개별 델리게이트를 가질 수 있게 해야함. bindAction 이라는 이름으로 있기는 한데, 이걸로는 부족함.
UCLASS(BlueprintType)
class TIHHOUSINGCORE_API UTHsActionPoint : public UObject, public ITHsActionPointAPI
{
	GENERATED_BODY()
friend class UTHsSettingSubsystem;
public:
	
	UFUNCTION()
	void SetIndexInSystem(const int32 index)
	{
		mIndexInSystem = index;
	}

	//	bind 단계에서 콜링됨, playerController 가 세팅된 이후에 콜링됨.
	UFUNCTION()
	void InitActionPoint(UTHsEnhancedActionSystem* actionSystem,const UInputAction* inputAction, const FKey& key);
	UFUNCTION()
	virtual void PostInitActionPoint(UTHsEnhancedActionSystem* actionSystem,const UInputAction* inputAction, const FKey& key,int32 index){}

	UFUNCTION()
	FString GetActionPointKeyString() const
	{
		return mKey.ToString();
	}
	UFUNCTION()
	const FKey& GetActionPointKey() const
	{
		return mKey;
	}
	
	//	HWInput -> ActionSystem.Func{OverrideActionPoint -> ActionPointDelegate} -> ActionSystem.CommonInput(actionPointHandle)   
	UFUNCTION(BlueprintCallable,Category="ActionPoint|HW")
	void HwInputKeyActionDown(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable,Category="ActionPoint|HW")
	void HwInputKeyActionUp(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable,Category="ActionPoint|HW")
	void HwInputKeyActionTriggered(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable,Category="ActionPoint|HW")
	void HwInputKeyActionCancel(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable,Category="ActionPoint|HW")
	void HwInputKeyActionOngoing(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable,Category="ActionPoint|System")
	void ActivateActionPointBegin();
	UFUNCTION(BlueprintCallable,Category="ActionPoint|System")
	void ActivateActionPointEnd();
	
	UFUNCTION(BlueprintCallable,Category="ActionPoint|System")
	int32 GetIndexInSystem() const { return mIndexInSystem; }

	void ClearTimeData();
	
	UFUNCTION()
	void SetActionSystem(UTHsEnhancedActionSystem* actionSystem);
	UFUNCTION()
	void EvaluateActionPoint(float CurrentTime, float frameDeltaTime);


	//	인터페이스를 넣는거임.
	UFUNCTION()
	void BindingInteractionInterface(TScriptInterface<ITIHHsInteraction> interaction)
	{
		mActionBindContext.BindingInteractionInterface(this,interaction);
	}
	UFUNCTION()
	bool IsBindKeyActionStarted() const { return mActionBindContext.bBindKeyActionStarted; }
	UFUNCTION()
	bool IsBindKeyActionCompleted() const { return mActionBindContext.bBindKeyActionCompleted; }
	UFUNCTION()
	bool IsBindKeyActionTriggered() const { return mActionBindContext.bBindKeyActionTriggered; }
	UFUNCTION()
	bool IsBindKeyActionCancelled() const { return mActionBindContext.bBindKeyActionCancelled; }
	UFUNCTION()
	bool IsBindKeyActionOngoing() const { return mActionBindContext.bBindKeyActionOngoing; }
	UFUNCTION()
	ETHsActionPointEvaluationState GetEvaluationState() const { return mEvaluationState; }
	UFUNCTION()
	float GetActivationTime() const { return mActivationTime; }
	UFUNCTION()
	float GetOngoingElapseTime() const { return mOngoingElapseTime; }
	UFUNCTION()
	const FInputActionValue& GetStartedValue() const { return mStartedValue; }
	UFUNCTION()
	const FInputActionValue& GetCompletedValue() const { return mCompletedValue; }
	UFUNCTION()
	const FInputActionValue& GetTriggeredValue() const { return mTriggeredValue; }
	UFUNCTION()
	const UInputAction* GetInputAction() const { return mInputAction; }
	UFUNCTION()
	UTHsEnhancedActionSystem* GetActionSystem() const { return mActionSystem; }
	
	UFUNCTION()
	FTHsActionBindContext& GetActionBindContext() { return mActionBindContext; }

	UFUNCTION()
	bool IsOngoing() const { return mStateOngoing; }

protected:
	//	TODO
	void TransferenceState(ETHsActionPointEvaluationState newState)
	{
		switch (mEvaluationState) {
		case ETHsActionPointEvaluationState::EWaitingDown:
			switch (newState) {
			case ETHsActionPointEvaluationState::EWaitingDown:
				break;
			case ETHsActionPointEvaluationState::EEvaluateOngoingStart:
				/*	
				 *	버튼이 클릭되어 평가에 들어가는 첫 순간
				 * 
				 */
					mEvaluationState = newState;
				break;
			case ETHsActionPointEvaluationState::EOngoing:
				break;
			case ETHsActionPointEvaluationState::ECheckingDoubleClick:
				break;
			}
			break;
		case ETHsActionPointEvaluationState::EEvaluateOngoingStart:
			break;
		case ETHsActionPointEvaluationState::EOngoing:
			break;
		case ETHsActionPointEvaluationState::ECheckingDoubleClick:
			break;
		}
	
	}

public:
	virtual ETHsEnhancedActionFuncFilterFlag GetEnhancedActionFilterFlags() override
	{
		return ETHsEnhancedActionFuncFilterFlag::EClick;
	}
	ETHsActionPointPressingStateType GetPressingState() const
	{
		return mPressingStateType;
	}
	UWorld* GetWorldContext() const
	{
		return mWorldContext;
	}
protected:
	ETHsActionPointEvaluationState mEvaluationState = ETHsActionPointEvaluationState::EWaitingDown;
	
	FTHsActionBindContext mActionBindContext;

	float mHoldingStartThreshold = 0.3f;
	float mDoubleClickThreshold = 0.15f;

	//	runtime
	float mCurrentDeltaTime = 0.0;//	초기화 시점은  EWaitingDown -> EEvaluateOngoingStart 로 변경될때, EEvaluateOngoingStart -> ECheckingDoubleClick 로 변경될때. 이거는 double의 expire든 ongoing의 expire든 공통으로 캐싱하는 용도임
	float mActivationTime = 0.0;	//	활성화가 되었을때 흐르는 전체 시간
	
	float mOngoingElapseTime = 0.0;

	//	record
	float mRecordedFirstDownTime = 0.0; //	첫번째 다운이 기록된 시간
	float mRecordedEnterOngoingTime = 0.0; //	홀드 상태로 진입한 시간
	float mRecordedDoubleClickTime = 0.0;
	
	int32 mIndexInSystem;
	FKey mKey;
	
	FInputActionValue mStartedValue;
	FInputActionValue mCompletedValue;
	FInputActionValue mTriggeredValue;
	
	const UInputAction* mInputAction = nullptr;
	class UTHsEnhancedActionSystem* mActionSystem = nullptr;

	bool mStateOngoing = false;
	int64 mOngoingTickCount = 0;

	bool mStatePressing = false;
	ETHsActionPointPressingStateType mPressingStateType;	//	0 wait, 1 down, 2 ongoing, 3 up
	UWorld* mWorldContext;
};
//ENUM_CLASS_FLAGS(ETHsActionCursorState);


//	record


//	이놈이 호버도 대신 해주고 있음. 물체 호버는 이놈이 담당할
UCLASS()
class TIHHOUSINGCORE_API ATHsActionCursor: public AActor,public ITHsActionPointAPI
{
	GENERATED_BODY()
public:
	void InitActionCursor(UTHsEnhancedActionSystem* actionSystem,APlayerController* playerController,UTHsEnhancedActionSystemDataSetting* dataSetting);
	
	void DebugCursorInfo(float RayLength, float Duration, bool bDoTrace, TEnumAsByte<ECollisionChannel> TraceChannel, bool bOnScreenText);
	void DebugCursorVisualization();
	void ExecuteCursorHoverActions(const FTHsActionCursorData& currRecordTarget,const  FTHsActionCursorData& prevRecordTarget,float deltaTime);

	FTHsActionCursorData& GetCurrentCursorData()
	{
		return mRecords[mCurrentIndex];
	}
	FTHsActionCursorData& GetPreviousCursorData()
	{
		return mRecords[mRecords[mCurrentIndex].PrevIndex];
	}
	const FTHsActionCursorData& GetPreviousCursorDataConst()
	{
		return mRecords[mRecords[mCurrentIndex].PrevIndex];
	}
	
	FORCEINLINE void ActionCursorHitCategoryStuff()
	{
		mRecords[mCurrentIndex].ActionCursorHitCategory = static_cast<int32>(ETHsActionCursorHoverHitCategory::EStuff);
	}
	FORCEINLINE void ActionCursorHitCategoryGizmo()
	{
		mRecords[mCurrentIndex].ActionCursorHitCategory = static_cast<int32>(ETHsActionCursorHoverHitCategory::EGizmo);
	}
	FORCEINLINE void ActionCursorHitCategoryWidget()
	{
		mRecords[mCurrentIndex].ActionCursorHitCategory = static_cast<int32>(ETHsActionCursorHoverHitCategory::EWidget);
	}
	void ProcessHitActor(FTHsActionCursorData& currCursorRecord,const FHitResult& HitResult)
	{
		if (currCursorRecord.IsActionDeprojectionValid())
		{
			if (HitResult.bBlockingHit)
			{
				currCursorRecord.Hit = HitResult;
				currCursorRecord.OnActionStateHit();
				ActionCursorHitCategoryStuff();
			}
		}
	}
	FORCEINLINE void AdvanceRecordIndex()
	{
		mCurrentIndex = GetCurrentCursorData().NextIndex;
	}

	UGameViewportClient* mGameViewportClient = nullptr;



	/*
	 * update GetMousePosition
	 * check OnActionStateMove
	 * update worldLocation && worldDirection, update WorldRayEnd check OnActionDeprojectionValid
	 * Calculate Delta
	 * UpdateCursorMovementState
	 */
	void UpdateCursorMoveForRecord(float deltaTime, FTHsActionCursorData& curRecordTarget, const FTHsActionCursorData& prevRecordTarget);

	void UpdateCursorMovementState(FTHsActionCursorData& currRecordTarget, float deletaTime);
	
	void VerifyHitForRecord();

	void UpdateHitActorForRecord(FTHsActionCursorData& curRecordTarget, const FTHsActionCursorData& prevRecordTarget) const;

	void UpdateHitComponentForRecord(FTHsActionCursorData& curRecordTarget, const FTHsActionCursorData& prevRecordTarget);

	//	deprecated
	void UpdateCursorMovement(float deltaTime);

	FTHsActionCursorData mCaptureCursorData;
	void GetCursorCaptureRef(FTHsActionCursorData& cursorData)
	{
		cursorData.Clear();
		const FTHsActionCursorData& recentRecord = GetPreviousRecord();
		FVector2D rawPixelPos;
		mPlayerController->GetMousePosition(rawPixelPos.X, rawPixelPos.Y);
		cursorData.ScreenPosition = rawPixelPos;
		
		FVector2D deltaScreenPos = cursorData.CalculatedDeltaScreenPosition(recentRecord);
		if (deltaScreenPos.SizeSquared() > mMoveSensitive)
		{
			cursorData.OnActionStateMove();
		}
		if (mPlayerController->DeprojectScreenPositionToWorld(cursorData.ScreenPosition.X, cursorData.ScreenPosition.Y, cursorData.WorldLocation, cursorData.WorldDirection))
		{
			cursorData.OnActionDeprojectionValid();
			cursorData.WorldRayEnd = cursorData.WorldLocation + (cursorData.WorldDirection * mLineTraceDistance);
		}
		UpdateCursorMovementState(cursorData, 0.0f);
	}
	const FTHsActionCursorData& GetCursorCapture()
	{
		GetCursorCaptureRef(mCaptureCursorData);
		return mCaptureCursorData;
	}
	
	void UpdateCursorRecordBegin(FTHsActionCursorData& curRecordTarget, const FTHsActionCursorData& preRecordTarget, float deltaTime);

	void UpdateCursorRecordEnd(FTHsActionCursorData& currRecordTarget, FTHsActionCursorData& prevRecordTarget, float deltaTime);

	void DisplayDeltalMoveInfo()
	{
		if (mDeltaScreenPosition.SizeSquared() > KINDA_SMALL_NUMBER)
		{
			UE_LOG(LogTemp, Log, TEXT("DeltaScreenPosition: %s"), *mDeltaScreenPosition.ToString());
		}
		if (mDeltaWorldLocation.SizeSquared() > KINDA_SMALL_NUMBER)
		{
			UE_LOG(LogTemp, Log, TEXT("DeltaWorldLocation: %s"), *mDeltaWorldLocation.ToString());
		}
		if (mDeltaWorldDirection.SizeSquared() > KINDA_SMALL_NUMBER)
		{
			UE_LOG(LogTemp, Log, TEXT("DeltaWorldDirection: %s"), *mDeltaWorldDirection.ToString());
		}

		
	}
	
	UFUNCTION()
	ECollisionChannel GetTraceChannel() const
	{
		return mTraceChannel;
	}
	UFUNCTION()
	const FHitResult& GetCurrentHitResult() const
	{
		if (mRecords.IsValidIndex(mCurrentIndex))
		{
			return mRecords[mCurrentIndex].Hit;
		}
		static FHitResult emptyHit;
		return emptyHit;
	}
	UFUNCTION(BlueprintCallable)
	const FTHsActionCursorData& GetCurrentRecord() const
	{
		if (mRecords.IsValidIndex(mCurrentIndex))
		{
			return mRecords[mCurrentIndex];
		}
		static FTHsActionCursorData emptyRecord;
		return emptyRecord;
	}
	UFUNCTION(BlueprintCallable)
	const FTHsActionCursorData& GetPreviousRecord() const
	{
		return mRecords[mRecords[mCurrentIndex].PrevIndex];
	}

	void DebugLogRecord();

	UFUNCTION(BlueprintCallable)
	const FTHsActionCursorData& GetRegressRecord(int32 prevCount = 1) const
	{
		int32 cur = GetCurrentRecord().SelfIndex;	//	무조건 현재에서 이전을 얻긴하지만 현재에서 
		while (prevCount)
		{
			int32 prev = mRecords[cur].PrevIndex;
			if (prev == GetCurrentRecord().SelfIndex)
			{
				//	한바퀴 돈거임.
				break;
			}
			cur = prev;
			--prevCount;
		}
		return mRecords[cur];
	}
	
	// Tag used to provide extra information or filtering for debugging of the trace (e.g. Collision Analyzer)
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryTraceTag(const FName& tag)	
	{
		
		mQueryParams.TraceTag = tag;
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryOwnerTag(const FName& tag)
	{
		mQueryParams.OwnerTag = tag;
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryTraceComplex(bool bTraceComplex)
	{
		mQueryParams.bTraceComplex = bTraceComplex;
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryFindInitialOverlaps(bool bFindInitialOverlaps)
	{
		mQueryParams.bFindInitialOverlaps = bFindInitialOverlaps;
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryReturnFaceIndex(bool bReturnFaceIndex)
	{
		mQueryParams.bReturnFaceIndex = bReturnFaceIndex;
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryReturnPhysicalMaterial(bool bReturnPhysicalMaterial)
	{
		mQueryParams.bReturnPhysicalMaterial = bReturnPhysicalMaterial;
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryIgnoreBlocks(bool inParam)
	{
		mQueryParams.bIgnoreBlocks = inParam;
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryIgnoreTouches(bool inParam)
	{
		mQueryParams.bIgnoreTouches = inParam;
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQuerySkipNarrowPhase(bool inParam)
	{
		mQueryParams.bSkipNarrowPhase = inParam;
	}
	//	Whether to ignore traces to the cluster union and trace against its children instead. bTraceIntoSubComponents
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryTraceIntoSubComponents(bool inParam)
	{
		mQueryParams.bTraceIntoSubComponents = inParam;
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryReplaceHitWithSubComponents(bool inParam)
	{
		mQueryParams.bReplaceHitWithSubComponents = inParam;
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryMobilityType(int32 mobilityType)
	{
		switch (mobilityType)
		{
			case 0:
				mQueryParams.MobilityType = EQueryMobilityType::Any;
				break;
			case 1:
				mQueryParams.MobilityType = EQueryMobilityType::Static;
				break;
			default:
				mQueryParams.MobilityType = EQueryMobilityType::Dynamic;
				break;
		}
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryIgnoreActors(const TArray<AActor*>& ignoreActors)
	{
		mQueryParams.AddIgnoredActors(ignoreActors);
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryIgnoreComponents(const TArray<UPrimitiveComponent*>& ignoreComponents)
	{
		mQueryParams.AddIgnoredComponents(ignoreComponents);
	}
	UFUNCTION(BlueprintCallable)
	void SetCollisionQueryIgnoreMaskFilter(uint8 ignoreMask)
	{
		mQueryParams.IgnoreMask = ignoreMask;
	}
	
	UFUNCTION(BlueprintCallable)
	void SetCollisionChannel(ECollisionChannel channel)
	{
		mTraceChannel = channel;
	}
	
	UFUNCTION(BlueprintCallable)
	FVector2D GetDeltaScreenPosition() const
	{
		return mDeltaScreenPosition;
	}
	UFUNCTION(BlueprintCallable)
	FVector2D GetDeltaScreenDirection() const
	{
		return mDeltaScreenDirection;
	}
		
	UFUNCTION(BlueprintCallable)
	FVector2D GetScreenPosition() 
	{
		return GetCurrentRecord().ScreenPosition;
	}
	
	void UpdateHoverStateForRecord(FTHsActionCursorData& currRecordTarget,FTHsActionCursorData& prevRecordTarget,float deltaTime) const;
	
	virtual void OverrideActionPointHoverStart(ATHsActionCursor* cursor)override;
	virtual void OverrideActionPointHoverOngoing(ATHsActionCursor* cursor)override;
	virtual void OverrideActionPointHoverEnd(ATHsActionCursor* cursor)override;
	virtual void OverrideActionPointMouseMove(ATHsActionCursor* cursor, float deltaTime) override;
	
	
	ETHsActionCursorHoverType GetHoverType() const
	{
		return mHoverType;
	}
	ETHsActionCursorMoveState GetMoveState() const
	{
		return mMoveState;
	}
	bool IsMoving() const
	{
		return mMoveState == ETHsActionCursorMoveState::EMouseMoveOngoing ||
			   mMoveState == ETHsActionCursorMoveState::EMouseMoveStarted;
	}
	bool IsPaused() const
	{
		return mMoveState == ETHsActionCursorMoveState::EMouseMovePaused;
	}

	FRay GetCurrentWorldRay() const
	{
		if (mRecords.IsValidIndex(mCurrentIndex))
		{
			const auto& record = GetCurrentRecord();
			return FRay(record.WorldLocation,record.WorldDirection);
		}
		return FRay();
		
		
	}

	virtual ETHsEnhancedActionFuncFilterFlag GetEnhancedActionFilterFlags() override
	{
		return ETHsEnhancedActionFuncFilterFlag::ECursorMove | ETHsEnhancedActionFuncFilterFlag::EHover;
	}

	//	not Used
	void CursorLineTrace(FHitResult& outHitResult) const
	{
		const auto& record = GetCurrentRecord();
		FVector start = record.WorldLocation;
		FVector end = start + record.WorldDirection * mLineTraceDistance;
		GetWorld()->LineTraceSingleByChannel(outHitResult, start, end, mTraceChannel, mQueryParams);
	}

	const FVector GetCurrentStartWorldLocation() const
	{
		if (mRecords.IsValidIndex(mCurrentIndex))
		{
			return GetCurrentRecord().WorldLocation;
		}
		return FVector::ZeroVector;
	}
	const FVector GetCurrentWorldDirection() const
	{
		if (mRecords.IsValidIndex(mCurrentIndex))
		{
			return GetCurrentRecord().WorldDirection;
		}
		return FVector::ZeroVector;
	}
	const FVector GetCurrentEndWorldLocation() const
	{
		if (mRecords.IsValidIndex(mCurrentIndex))
		{
			const auto& record = GetCurrentRecord();
			return record.WorldLocation + record.WorldDirection * mLineTraceDistance;
		}
		return FVector::ZeroVector;
	}
	//	TODO: 이거 변경하는 코드 꼭 넣기
	const ECollisionChannel GetCurrentTraceChannel() const
	{
		return mTraceChannel;
	}
	
private:
	int32 mCurrentIndex = 0;
	
	float mMoveSensitive = 0.00001f;
	//int32 mMoveTimeThresholdFrame = 30.0f;
	float mMoveTimeThreshold = 1.0f / 30.0f;
	float mLineTraceDistance = 10000.0f;

	ECollisionChannel mTraceChannel = ECC_Visibility;
	
	UTHsEnhancedActionSystem* mActionSystem;
	APlayerController* mPlayerController;
	TArray<FTHsActionCursorData> mRecords;
	FCollisionQueryParams mQueryParams;

	FTIHHsActionParamIn mActionParamIn;

	ETHsActionCursorHoverType mHoverType = ETHsActionCursorHoverType::ENone;
	
	FVector2D mDeltaScreenPosition = FVector2D::ZeroVector;
	FVector2D mDeltaScreenDirection = FVector2D::ZeroVector;
	FVector mDeltaWorldLocation = FVector::ZeroVector;
	FVector mDeltaWorldDirection = FVector::ZeroVector;
	float mElapsedMoveTime = 0.0f;
	float mPauseMoveTime = 0.0f;

	//	TODO: 나중에 정확도를 올려줄때 만들자. 원래라면 이걸 record에 넣어서 지금의 상태를 알려줘야함.
	bool mVertifyMoveRecord = false;
	
	TScriptInterface<ITIHHsInteraction> mCastedTIHHsInteractionActor;

	TStrongObjectPtr<UTHsFilter> mHoverFilter;


	
	ETHsActionCursorMoveState mMoveState = ETHsActionCursorMoveState::EMouseMovePaused;


	//bool mHoverProcessed = false;
};


/*
================================================================================================================
                                    Gizmo 
 */
class UTHsGizmoManagerWrapper;
class FTHsToolsContextBase
{
public:
	
	FTHsToolsContextBase(UTHsEnhancedActionSystem* actionSystem, UTHsGizmoManagerWrapper* owner);
	virtual ~FTHsToolsContextBase() = default;

	UTHsEnhancedActionSystem* GetActionSystem() const
	{
		return mActionSubsystem.Get();
	}
	UTHsGizmoManagerWrapper* GetOwner() const
	{
		return mOwner.Get();
	}
	UTHsGizmoManagerWrapper* GetGizmoManagerWrapper() const
	{
		return GetOwner();
	}
	UWorld* GetWorldContext() const
	{
		return mWorldContext;
	}
	void InitToolsContext(UWorld* worldContext, const UTHsEnhancedActionSystemDataSetting* dataSetting)
	{
		mWorldContext = worldContext;
		PostInitToolsContext(dataSetting);
	}
	
protected:
	virtual void PostInitToolsContext(const UTHsEnhancedActionSystemDataSetting* dataSetting)
	{}
	
	TWeakObjectPtr<UTHsEnhancedActionSystem> mActionSubsystem;
	TWeakObjectPtr<UTHsGizmoManagerWrapper> mOwner;
	UWorld* mWorldContext = nullptr;
};




class FTHsToolsContextQueriesAPI : public FTHsToolsContextBase, public IToolsContextQueriesAPI
{
public:
	FTHsToolsContextQueriesAPI(UTHsEnhancedActionSystem* actionSystem, UTHsGizmoManagerWrapper* owner);

	virtual UWorld* GetCurrentEditingWorld() const override
	{
		return GetWorldContext();
	}
	virtual void GetCurrentSelectionState(FToolBuilderState& StateOut) const override;
	virtual void GetCurrentViewState(FViewCameraState& StateOut) const override;
	virtual UMaterialInterface* GetStandardMaterial(EStandardToolContextMaterials MaterialType) const override;
	virtual FViewport* GetHoveredViewport() const override;
	virtual FViewport* GetFocusedViewport() const override
	{
		return GetHoveredViewport();
	}

protected:
	virtual void PostInitToolsContext(const UTHsEnhancedActionSystemDataSetting* dataSetting) override;
private:
	
	TArray<AActor*>* mSelectedActorsArrPtr;
	TArray<UActorComponent*>* mSelectedComponentsArrPtr;
};

class FTHsToolsContextTransactionsAPI :  public FTHsToolsContextBase, public IToolsContextTransactionsAPI
{
public:
	FTHsToolsContextTransactionsAPI(UTHsEnhancedActionSystem* const actionSystem,UTHsGizmoManagerWrapper* const owner);

	virtual void DisplayMessage(const FText& Message, EToolMessageLevel Level) override;
	virtual void PostInvalidation() override;
	virtual void BeginUndoTransaction(const FText& Description) override;
	virtual void EndUndoTransaction() override;
	virtual void AppendChange(UObject* TargetObject, TUniquePtr<FToolCommandChange> Change,
		const FText& Description) override;
	virtual bool RequestSelectionChange(const FSelectedObjectsChangeList& SelectionChange) override;
protected:
	virtual void PostInitToolsContext(const UTHsEnhancedActionSystemDataSetting* dataSetting) override;
};

/*
 *	목적, THsGizmoManagerWrapper 에서 액션 포인트 특히 FInputDeviceState 를 관리하기 위해서임. 내부에 FDelegateHandle 도 가지고 있음.
 *	이거하다가 체인으로 넘어간거임. 특정 키의 특정 인풋 조합이 발생하면
 *	호출하라는것. 즉 이거에서 UTHsActionPoint 는 빼도 된다는 말임.
 *	이거 하라고 원래 enhancedInput이 modifier랑 trigger를 나눠놓은거임.
 *	그런데 나는 따로 시스템을 만들어 놨으니깐 저걸 못쓰는거임.
 *	특정 트리의 구조가 있고, 인풋들이 들어왔을때 한번에 호출해야 성능이 좋은거임.
 *	즉 그냥 일단은 검사 object만들어서 액션시스템에 현재 키입력 상태 만들고, 그 입력을 위하 컴바인 인풋 만들어서 혹은 상태 만드는게 좋음.
 *	그럼 일단 키 + 이벤트 일때 함수 호출되도록 해야함
 *		해당 키가 hw상태에 따라 다르게 호출을 함.
 *		복합키는 일단 cursor 나 key에 맞춰서 만들었을 뿐임 UTHsActionMouseClick 가 검사중. 그러면 애초에 그냥 액션 포인터로 만드는게 맞지 않나? 이걸 왜 따로 하려고 하지? 예를 들어서 컨트롤이라고 이름을 지어보자. 어차피 이걸 따로 오브젝트를 만들기로 했으니 그거대로 가야지 왜 어려운 구조 만들고 앉아있어. 룰베이스 아웃, 
 * 
 */
USTRUCT()
struct FTHsGizmoManagerActionHandler
{
	GENERATED_BODY()
	
	UPROPERTY()
	FInputDeviceState DeviceState = {};
	UPROPERTY()
	UTHsActionPoint* ActionPoint = nullptr;
	
	FDelegateHandle DelegateHandle;
	UPROPERTY()
	ETHsActionFunctionCategory FunctionCategory = ETHsActionFunctionCategory::EEvent;
	UPROPERTY()
	ETHsActionFunctionType FunctionType = ETHsActionFunctionType::EEvent_Click;
	

	FTHsGizmoManagerActionHandler() = default;
	FTHsGizmoManagerActionHandler(const FTHsGizmoManagerActionHandler& other)
	{
		DelegateHandle = other.DelegateHandle;
		DeviceState = other.DeviceState;
		FunctionCategory = other.FunctionCategory;
		FunctionType = other.FunctionType;
	}
	FTHsGizmoManagerActionHandler(FTHsGizmoManagerActionHandler&& other) noexcept
	{
		DelegateHandle = other.DelegateHandle;
		DeviceState = other.DeviceState;
		FunctionCategory = other.FunctionCategory;
		FunctionType = other.FunctionType;
	}
	~FTHsGizmoManagerActionHandler() = default;
	FTHsGizmoManagerActionHandler& operator=(const FTHsGizmoManagerActionHandler& other)
	{
		if (this != &other)
		{
			DelegateHandle = other.DelegateHandle;
			DeviceState = other.DeviceState;
			FunctionCategory = other.FunctionCategory;
			FunctionType = other.FunctionType;
		}
		return *this;
	}
	FTHsGizmoManagerActionHandler& operator=(FTHsGizmoManagerActionHandler&& other) noexcept
	{
		if (this != &other)
		{
			DelegateHandle = other.DelegateHandle;
			DeviceState = other.DeviceState;
			FunctionCategory = other.FunctionCategory;
			FunctionType = other.FunctionType;
		}
		return *this;
	}

	FTHsGizmoManagerActionHandler& operator=(FDelegateHandle other)
	{
		DelegateHandle = other;
		return *this;
	}
	FTHsGizmoManagerActionHandler& operator=(UTHsActionPoint* other)
	{
		ActionPoint = other;
		return *this;
	}
	FTHsGizmoManagerActionHandler& operator=(FInputDeviceState other)
	{
		DeviceState = other;
		return *this;
	}
	FTHsGizmoManagerActionHandler& operator=(ETHsActionFunctionCategory other)
	{
		FunctionCategory = other;
		return *this;
	}
	FTHsGizmoManagerActionHandler& operator=(ETHsActionFunctionType other)
	{
		FunctionType = other;
		return *this;
	}
	
};
FORCEINLINE uint32 GetTypeHash(const FTHsGizmoManagerActionHandler& Key)
{
	uint32 result = 0;
	result = HashCombine(result,static_cast<uint32>(Key.FunctionCategory));
	result = HashCombine(result,static_cast<uint32>(Key.FunctionType));
	return result;
}
UCLASS()
class TIHHOUSINGCORE_API ATHsTestCube : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	
	UPROPERTY()
	UStaticMeshComponent* mStaticMeshComponent;
};


UCLASS()
class UTHsGizmoManagerWrapper : public UObject, public ITHsActionPointAPI
{
	GENERATED_BODY()
public:
	bool IsValidUpdate() const
	{
		return mValidationUpdate;
	}

	void DebugTestCube();
	UFUNCTION(BlueprintCallable)
	void InitGizmoManager(UTHsEnhancedActionSystem* actionSystem,UWorld* worldContext);

	UFUNCTION(BlueprintCallable)
	UInteractiveGizmoManager* GetInteractiveGizmoManager() const
	{
		return mGizmoManager;
	}
	UFUNCTION(BlueprintCallable)
	UInputRouter* GetInputRouter() const
	{
		return mInputRouter;
	}
	UFUNCTION(BlueprintCallable)
	UToolTargetManager* GetToolTargetManager() const
	{
		return mTargetManager;
	}
	UFUNCTION(BlueprintCallable)
	UInteractiveToolManager* GetInteractiveToolManager() const
	{
		return mToolManger;
	}

	virtual void BeginDestroy() override;
	void UpdateAutoValidation(float deltaTime);
	void UpdateMouseInputStateForGizmo(FInputDeviceState& deviceState );

	UFUNCTION(BlueprintCallable)
	UInteractiveToolsContext* GetToolsContext() const
	{
		return mToolsContext;
	}
	
	UFUNCTION(BlueprintCallable)
	TArray<AActor*>& GetSelectedActors()
	{
		return mSelectedActors;
	}
	
	UFUNCTION(BlueprintCallable)
	TArray<UActorComponent*>& GetSelectedComponents()
	{
		return mSelectedComponents;
	}
	ETHsGizmoUpdateInputDeviceStateType mUpdateInputLeftMouseStateType = ETHsGizmoUpdateInputDeviceStateType::EHovered;

	FString ToStringInputDeviceState() const
	{
		FString result;
		switch (mUpdateInputLeftMouseStateType) {
		case ETHsGizmoUpdateInputDeviceStateType::EPressed:	result = TEXT("Pressed");		break;
		case ETHsGizmoUpdateInputDeviceStateType::EOngoing:	result = TEXT("Ongoing");		break;
		case ETHsGizmoUpdateInputDeviceStateType::EReleased: result = TEXT("Released");	break;
		case ETHsGizmoUpdateInputDeviceStateType::EHovered: result = TEXT("Hovered");		break;
		}
		return result;
	}
	
	UFUNCTION(BlueprintCallable)
	void UpdateGizmo(float deltaTime);

	UTHsActionPoint* mLeftMouseActionPoint = nullptr;
	
	UFUNCTION(BlueprintCallable)
	bool ValidationToolsContext();

	int32 mInputActionType = 0;	//	0: Mouse, 1: KeyBoard, 2: GamePad, 3: Touch
	
	virtual void OverrideActionPointDown() override
	{
		
	}
	virtual void OverrideActionPressing(UTHsActionPoint* actionPoint, float deltaTime) override
	{
		
	}
	virtual void OverrideActionPointUp() override
	{
		
	}

private:
	UPROPERTY()
	UInteractiveToolsContext* mToolsContext;
	UPROPERTY()
	UInteractiveGizmoManager* mGizmoManager;
	UPROPERTY()
	UInputRouter* mInputRouter;
	UPROPERTY()
	UToolTargetManager* mTargetManager;
	UPROPERTY()
	UInteractiveToolManager* mToolManger;

	TSharedPtr<class FTHsGizmoViewExtension> mGizmoViewExtension;
	
	
	UPROPERTY()
	FTransform mOriginTransform;
	
	TUniquePtr<FTHsToolsContextQueriesAPI> mToolsContextQueriesAPI;
	TUniquePtr<FTHsToolsContextTransactionsAPI> mToolsContextTransactionsAPI;

	UWorld* mWorldContext;
	UTHsEnhancedActionSystem* mActionSystem = nullptr;

	UPROPERTY()
	TArray<AActor*> mSelectedActors;
	UPROPERTY()
	TArray<UActorComponent*> mSelectedComponents;

	float mAutoValidationTime = 0.0f;
	float mAutoValidationMax = 0.0f;

	bool mValidationUpdate = false;

	TMap<FKey,FTHsGizmoManagerActionHandler> mActionHandlers;

	UCombinedTransformGizmo* mCombinedTransformGizmo = nullptr;
	UTransformProxy* mTransformProxy = nullptr;
	//	cosmos reasoning
	int32 mCurrentInputType = 0;	//	0 check , 1 press, 2 down 3 up
	FInputDeviceState mCurrentInputState;
	
	UPROPERTY()
	ATHsTestCube* mTestCube = nullptr;
};


/*

 */
template<ETHsActionFunctionCategory FuncCategory>
struct TTHsActionBinder;
/*
 사용법
auto handle = BindInputKeyActionPointByName<ETHsActionFunctionCategory::EEvent>(ETHsActionFunctionType::ECancel,TEXT("dd"),[](UTHsActionPoint* test)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cancel dd"));
	});
 */
UCLASS()
class UTHsEnhancedActionSystemState : public UObject
{
	GENERATED_BODY()
public:

	
private:
	
};

/*
 *	TODO: 꼭....옮긴다.
 */
UCLASS()
class TIHHOUSINGCORE_API ATHsEnhancedActionActor : public AActor
{
	GENERATED_BODY()
public:
	
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
};

USTRUCT()
struct FTHsEnhancedActionHitQuery
{
	GENERATED_BODY()

	UPROPERTY()
	bool IsHitPossible = false;
	UPROPERTY()
	bool IsHoverPossible = false;
	UPROPERTY()
	bool IsActionPossible = false;
	
	ECollisionChannel TraceChannel = ECC_Visibility;
		
};

DECLARE_MULTICAST_DELEGATE_OneParam(FTHsActionSystemPostBindAllAction, UTHsEnhancedActionSystem* /*actionSystem*/);

UCLASS()
class TIHHOUSINGCORE_API UTHsEnhancedActionSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	UWorld* GetWorldContext() const
	{
		return mWorldContext;
	}

	bool LineTraceHitGizmo(const FTHsActionCursorData& cursorData) const;
	void UpdateInteractiveGizmo(const FTHsActionCursorData& cursorData);

	TScriptInterface<ITIHHsInteraction> LineTraceSelectableInteraction(FHitResult& outHitResult) const;
	TScriptInterface<ITIHHsInteraction> LineTraceSelectableGizmo(FHitResult& outHitResult) const;
	TScriptInterface<ITIHHsInteraction> LineTraceSelectableAny(FHitResult& outHitResult) const;
	static EInputDevices GetInputKeyType(const FKey& key)
	{
		return TIHUtils::GetInputKeyType(key);
	}
	static EInputDevices GetInputKeyTypeByActionPoint(const UTHsActionPoint* actionPoint)
	{
		if (actionPoint)
		{
			return GetInputKeyType(actionPoint->GetActionPointKey());
		}
		return EInputDevices::None;
	}
	FORCEINLINE void ClearGlobalHit()
	{
		mGlobalHitResult.Reset();
	}
	static bool IsInputKeyTypeKeyboard(const FKey& key)
	{
		return GetInputKeyType(key) == EInputDevices::Keyboard;
	}
	static bool IsInputKeyTypeMouse(const FKey& key)
	{
		return GetInputKeyType(key) == EInputDevices::Mouse;
	}
	static bool IsInputKeyTypeGamepad(const FKey& key)
	{
		return GetInputKeyType(key) == EInputDevices::Gamepad;
	}
	static bool IsInputKeyTypeTouch(const FKey& key)
	{
		return GetInputKeyType(key) == EInputDevices::TabletFingers;
	}
	static bool IsInputKeyTypeModifier(const FKey& key)
	{
		return TIHUtils::GlobalConfigDefault::MODIFIER_KEYS.Contains(key);
	}
	
	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Conditional;
	}
	virtual bool IsTickable() const override
	{
		return true;
	}
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
//	################	[	모든것의 시작	]	################
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	FORCEINLINE void UpdateActionPoints()
	{
		float currentTime = FPlatformTime::Seconds();
		int32 numActivated =0;
		for (UTHsActionPoint* actionPoint:mActivatedActionPoints)
		{
			if (actionPoint == nullptr || not mEnableKeys.Contains(actionPoint->GetActionPointKey()))
			{
				continue;
			}
			
			actionPoint->EvaluateActionPoint(currentTime,mInputTickElapsedTime);
			
			numActivated++;
		}
	}


	static bool IsCursorOnWorld()
	{
		const FVector2D ScreenPos = FSlateApplication::Get().GetCursorPos();
		const TArray<TSharedRef<SWindow>>& TopWindows = FSlateApplication::Get().GetInteractiveTopLevelWindows();

		// 2) 커서 아래 위젯 경로(FWidgetPath)
		FWidgetPath Path = FSlateApplication::Get().LocateWindowUnderMouse(
			ScreenPos, TopWindows, /*bIgnoreEnabledStatus=*/false, /*UserIndex=*/INDEX_NONE);

		if (!Path.IsValid())
		{
			// 어떤 위젯도 안 맞음 → 월드
			return true;
		}

		// 3) 리프가 뷰포트면 월드. (타입명 비교: SViewport)
		const TSharedPtr<SWidget> Leaf = Path.Widgets.Last().Widget;
		if (Leaf.IsValid() && Leaf->GetType() == FName("SViewport"))
		{
			return true; // 월드
		}

		// 4) 경로 어딘가에 UMG 위젯(SObjectWidget)이 있으면 UI로 간주
		for (int32 i = Path.Widgets.Num() - 1; i >= 0; --i)
		{
			const TSharedPtr<SWidget> W = Path.Widgets[i].Widget;
			if (W.IsValid() && W->GetType() == FName("SObjectWidget"))
			{
				return false; // UI 위
			}
		}

		// 5) 그 외(예: 오버레이만 존재): 월드로 처리
		return true;
	}
	
	bool GlobalHit(FHitResult& hitResult) const;

	//	----------------------------------------------------------------
	virtual void Tick(float DeltaTime) override;
	virtual void Deinitialize() override;
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UTIHHsEnhancedAction, STATGROUP_Tickables);
	}
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	TArray<FEnhancedActionKeyMapping> CrawlInputMappingContext();
	bool ProcessAllInputKeys(const TArray<FEnhancedActionKeyMapping>& existingMappings);
	
	void BindAllActionsToPlayerController();
	
	UFUNCTION()
	void ActivateActionPoint(UTHsActionPoint* actionPoint);
	UFUNCTION()
	void DeActivateActionPoint(UTHsActionPoint* actionPoint);

	//	action event
	UFUNCTION()
	void ActionDoubleClicked(UTHsActionPoint* actionPoint);
	UFUNCTION()
	void ActionClicked(UTHsActionPoint* actionPoint);
	UFUNCTION()
	void ActionDown(UTHsActionPoint* actionPoint);

	//	pressing은 down이후에 계속 눌리고 있는 상태임. 즉 down이후에 눌리고 있는 상태임. 계속 tick 타임에 계속 호출되는 것임. 
	UFUNCTION()
	void ActionPressing(UTHsActionPoint* actionPoint,float deltaTime);
	
	UFUNCTION()
	void ActionUp(UTHsActionPoint* actionPoint);
	UFUNCTION()
	void ActionOngoingStart(UTHsActionPoint* actionPoint,float deltaTime);
	UFUNCTION()
	void ActionOngoing(UTHsActionPoint* actionPoint,float deltaTime);
	UFUNCTION()
	void ActionOngoingEnd(UTHsActionPoint* actionPoint,float deltaTime);
	UFUNCTION()
	void ActionDragingStart(UTHsActionPoint* actionPoint,float deltaTime);
	UFUNCTION()
	void ActionDraging(UTHsActionPoint* actionPoint,float deltaTime);
	UFUNCTION()
	void ActionDragingEnd(UTHsActionPoint* actionPoint,float deltaTime);
	UFUNCTION()
	void ActionMouseMove(ATHsActionCursor* cursor,float deltaTime);

	UFUNCTION()
	void ActionHoverStart(ATHsActionCursor* cursor,float deltaTime);
	UFUNCTION()
	void ActionHoverOngoing(ATHsActionCursor* cursor,float deltaTime);
	UFUNCTION()
	void ActionHoverEnd(ATHsActionCursor* cursor,float deltaTime);
	
	UFUNCTION()
	bool IsActionPointActivatedByIndex(int32 systemIdx) const
	{
		bool result = false;
		if (mActivatedActionPoints.IsValidIndex(systemIdx) && mActivatedActionPoints[systemIdx] != nullptr)
		{
			result = true;
		}
		return result;
	}
	
	UFUNCTION()
	bool IsActionPointActivatedByKey(const FKey& key) const
	{
		bool result = false;
		if (mAllActionPoints.Contains(key))
		{
			result = IsActionPointActivatedByIndex(mAllActionPoints[key]->GetIndexInSystem());
		}
		return result;
	}
	UFUNCTION()
	UTHsActionPoint* TryGetActiveActionPointByKey(const FKey& key)
	{
		UTHsActionPoint* result = nullptr;
		if (UTHsActionPoint** found = mCurrentActionPoint.Find(key))
		{
			result = *found;
		}
		return result;
	}
	
	UFUNCTION()
	UTHsActionPoint* GetActivateActionPointByIndex(int32 systemIdx) const
	{
		if (mActivatedActionPoints.IsValidIndex(systemIdx))
		{
			return mActivatedActionPoints[systemIdx];
		}
		return nullptr;
	}
	UFUNCTION()
	UTHsActionPoint* GetActionPointByKey(FKey key) const
	{
		if (mAllActionPoints.Contains(key))
		{
			return mAllActionPoints[key];
		}
		return nullptr;
	}
	FKey GetKeyByName(const FName& keyName) const
	{
		for (auto& Pair : mAllKeyActions)
		{
			if (Pair.Key.GetFName() == keyName)
			{
				return Pair.Key;
			}
		}
		return FKey();
	}
	UTHsActionPoint* GetActivateActionPointByName(const FName& keyName) const
	{
		UTHsActionPoint* result = nullptr;
		if (mAllKeyNames.Contains(keyName))
		{
			if (mAllActionPoints.Contains(mAllKeyNames[keyName]))
			{
				result = mAllActionPoints[mAllKeyNames[keyName]];
			}
		}
		return result;
	}
	UFUNCTION()
	UTHsActionPoint* GetActivateActionPointByKey(FKey key) const
	{
		UTHsActionPoint* result = nullptr;
		if (mAllActionPoints.Contains(key))
		{
			int32 systemIdx = mAllActionPoints[key]->GetIndexInSystem();
			result = GetActivateActionPointByIndex(systemIdx);
		}
		return result;
	}

	//	[weakThis = TWeakObjectPtr<UTHsEnhancedActionSystem>(this)]
	//	(UTHsActionPoint* inActionPoint ,float frameDeltaTime)
	template<typename T>
	FDelegateHandle BindInputKeyActionPointOngoingByName( const FName& keyName, T&& InCallable )
	{
		if (mAllKeyNames.Contains(keyName))
		{
			FKey key = mAllKeyNames[keyName];
			if (mAllActionPoints.Contains(key))
			{
				// .AddLambda()는 람다를 직접 받아서 내부적으로 FDelegate를 만들어 추가합니다.
				return mAllActionPoints[key]->GetActionBindContext().DelegateProcessHoldOngoing.AddLambda(Forward<T>(InCallable ));
			}
		}
		return FDelegateHandle();
	}

	FTHsActionSystemPostBindAllAction OnPostBindAllAction;
	
	template <ETHsActionFunctionCategory THsFuncCategory,typename T>
	FDelegateHandle BindInputKeyActionPointByName(ETHsActionFunctionType funcType, const FName& keyName, T&& InCallable );
	
	template <ETHsActionFunctionCategory THsFuncCategory,ETHsActionFunctionType THsFuncType,typename T>
	FDelegateHandle BindInputKeyByName(const FName& keyName, T&& InCallable )
	{
		return BindInputKeyActionPointByName<THsFuncCategory>(THsFuncType,keyName,Forward<T>(InCallable));
	}
	/*
	[weakThis = TWeakObjectPtr<UTHsEnhancedActionSystem>(this), actionSystem = TWeakObjectPtr<UTHsEnhancedActionSystem>(actionSystem)]
	 	(UTHsActionPoint* inActionPoint){}
	 */
	template <typename T>
	FDelegateHandle BindInputKeyEventByName(ETHsActionFunctionType funcType,
	const FName& keyName, T&& InCallable)
	{
		FDelegateHandle result = TTHsActionBinder<ETHsActionFunctionCategory::EEvent>::Bind(this,funcType,keyName,Forward<T>(InCallable));
		return result;
	}
	/*
	[weakThis = TWeakObjectPtr<UTHsEnhancedActionSystem>(this), actionSystem = TWeakObjectPtr<UTHsEnhancedActionSystem>(actionSystem)]
		 (UTHsActionPoint* inActionPoint,float deltaTime){}
	 */
	template <typename T>
	FDelegateHandle BindInputKeyProcessByName(ETHsActionFunctionType funcType,
	const FName& keyName, T&& InCallable)
	{
		FDelegateHandle result = TTHsActionBinder<ETHsActionFunctionCategory::EProcess>::Bind(this,funcType,keyName,Forward<T>(InCallable));
		return result;
	}
	/*
	[weakThis = TWeakObjectPtr<cls>(this), actionSystem = TWeakObjectPtr<UTHsEnhancedActionSystem>(actionSystem)]
		 (ATHsActionCursor* cursor,float deltaTime){}
	 */
	template <typename T>
	FDelegateHandle BindInputKeyCursorByName(ETHsActionFunctionType funcType,
	const FName& keyName, T&& InCallable)
	{
		FDelegateHandle result = TTHsActionBinder<ETHsActionFunctionCategory::ECursor>::Bind(this,funcType,keyName,Forward<T>(InCallable));
		return result;
	}

	//bool CheckHoverable(ATHsActionCursor* ATHsActionCursor, const FTHsActionCursorData& CurRecordTarget);

	// #define THSMACRO_BIND_INPTUT_KEY(FuncCategory,FuncType) \
// 	template <typename T> FDelegateHandle BindInputKey##FuncType(\
// 	const FName& keyName, T&& InCallable){\
// 		return TTHsActionBinder<FuncCategory>(this,FuncType,keyName,Forward<T>(InCallable));\
// 	}

	// template <typename T>
	// FDelegateHandle BindInputKeyProcessHoldStartByName(
	// const FName& keyName, T&& InCallable)
	// {
	// 	FDelegateHandle result = TTHsActionBinder<ETHsActionFunctionCategory::ECursor>::Bind(this,ETHsActionFunctionType::EProcess_HoldStart,keyName,Forward<T>(InCallable));
	// 	return result;
	// }
	//

	

	template<typename FunctorType, typename... VarTypes>
	static FDelegateHandle BindInputKeyActionPointOngoing(UTHsActionPoint* actionPoint,FunctorType&& InFunctor, VarTypes&&... Vars )
	{
		return actionPoint->GetActionBindContext().DelegateProcessHoldOngoing.AddLambda(
			[InFunctor, Vars...](UTHsActionPoint* ap, float deltaTime)
			{
				InFunctor(ap,deltaTime,Vars...);
			}
		);
	}
	
	APlayerController* GetPlayerController()
	{
		return mPlayerController;
	}

	ATHsActionCursor* GetActionCursor()
	{
		return mActionCursor;
	}

	mutable struct
	{
		FVector OutLocation;
		FRotator OutRotation;
		ULocalPlayer* LocalPlayer;
		const UGameViewportClient* ViewportClient;
		FSceneView* SceneView;
	}mPlayerCameraCaches;

	bool bInitGizmoManager;


	void GetCurrentViewState(FViewCameraState& stateOut) const
	{
		if (!mPlayerController || !mPlayerController->PlayerCameraManager)
		{
			return;
		}
		FVector OutLocation;
		FRotator OutRotation;
		mPlayerController->GetPlayerViewPoint(OutLocation,OutRotation);
		stateOut.Position = OutLocation;
		stateOut.Orientation = OutRotation.Quaternion();
		stateOut.bIsVR = false;
		if (UGameViewportClient* viewportClient = mPlayerController->GetWorld()->GetGameViewport())
		{
			FVector2D viewportSize;
			viewportClient->GetViewportSize(viewportSize);

			if (viewportSize.Y > 0)
			{
				// PlayerCameraManager와 GameViewportClient에서 직접 값을 가져와 통일시킵니다.
				// 이것이 CalcSceneView보다 더 직접적이고 신뢰성 있을 수 있습니다.
				stateOut.HorizontalFOVDegrees = mPlayerController->PlayerCameraManager->GetFOVAngle();
				stateOut.AspectRatio = viewportSize.X / viewportSize.Y;
				stateOut.bIsOrthographic = (mPlayerController->PlayerCameraManager->GetCameraCacheView().ProjectionMode == ECameraProjectionMode::Orthographic);
			}
		}
		// if (ULocalPlayer* localPlayer = mPlayerController->GetLocalPlayer())
		// {
		// 	if (const UGameViewportClient* viewportClient = localPlayer->ViewportClient)
		// 	{
		// 		FSceneViewFamily::ConstructionValues viewConstructValue =
		// 			FSceneViewFamily::ConstructionValues(
		// 			/*const FRenderTarget* renderTarget*/
		// 			viewportClient->Viewport,
		// 			/*FSceneInterface* sceneInterface*/
		// 			mWorldContext->Scene,
		// 			/*const FEngineShowFlags& engineShowFlags*/
		// 			viewportClient->EngineShowFlags
		// 		);
		// 		FSceneViewFamilyContext viewFamily(viewConstructValue.SetRealtimeUpdate(true));
		// 		if (FSceneView* sceneView = localPlayer->CalcSceneView(&viewFamily,OutLocation,OutRotation,viewportClient->Viewport))
		// 		{
		// 			stateOut.HorizontalFOVDegrees = sceneView->FOV;
		// 			stateOut.AspectRatio = sceneView->UnscaledViewRect.Width() / static_cast<float>(sceneView->UnscaledViewRect.Height());
		// 			stateOut.bIsOrthographic = not sceneView->IsPerspectiveProjection();
		// 			/*
		// 			 * 직교투영시 처리
		// 			 */
		// 		}
		// 	}
		// }
	}
	
	TSet<FKey> ExcludeKeys;
	FKey GetKeyFromAction(UInputAction* action) const
	{
		for (auto& Pair : mAllKeyActions)
		{
			if (Pair.Value == action)
			{
				return Pair.Key;
			}
		}
		return FKey();
	}

	
	bool mIsHitFlag = true;
	bool mIsHoverFlag = true;
	
	bool mIsGizmoHitFlag = true;
	bool mIsActorHitFlag = true;
	
	bool mIsActionKeyFlag = true;
	bool mIsSpecialActionFlag = true;

	bool mIsCursorMoveFlag = true;

	bool mProcessTick = true;

	void OnProcessTickChanged(bool tick)
	{
		mProcessTick = tick;
	}
	bool IsProcessTick() const
	{
		return mProcessTick;
	}
	
	int32 mCurrentHitType = 0;
	void OnCurrentHitTypeChanged(ETHsActionCursorHoverHitCategory newHitType)
	{
		mCurrentHitType = static_cast<int32>(newHitType);
	}
	
	ETHsActionCursorHoverHitCategory GetCurrentHitType() const
	{
		return static_cast<ETHsActionCursorHoverHitCategory>(mCurrentHitType);
	}
	//	전체 액션 hit 가능 여부
	void SetActionHitPossible(bool possible)
	{
		mIsHitFlag = possible;
	}
	bool IsActionHitPossible() const
	{
		return mIsHitFlag;
	}
	
	//	tick 에서 hover 가능 여부
	void SetHoverHitPossible(bool possible)
	{
		mIsHoverFlag = possible;
	}
	bool IsHoverHitPossible() const
	{
		return mIsHoverFlag;
	}
	
	//	어떤 actionKey든 이걸로 확인
	void SetActionKeyPossiblePossible(bool possible)
	{
		mIsActionKeyFlag = possible;
	}
	bool IsActionKeyPossiblePossible() const
	{
		return mIsActionKeyFlag;
	}
	
	void SetCursorMovePossible(bool possible)
	{
		mIsCursorMoveFlag = possible;
	}
	bool IsCursorMovePossible() const
	{
		return mIsCursorMoveFlag;
	}
	
	//	mAllActionPoints 이거 쓰면 되는데 왜 분리한거냐? 그건 활성화된 키만 관리하기 위해서임.
	TSet<FKey> mEnableKeys;

	void SetEnableKey(const FKey& key,bool enable)
	{
		if (enable)
		{
			if (mAllActionPoints.Contains(key))
			{
				mEnableKeys.Add(key);
			}
		}
		else
		{
			if (mEnableKeys.Contains(key))
			{
				mEnableKeys.Remove(key);
			}
		}
		if (mEnableKeys.IsEmpty())
		{
			mIsActionKeyFlag = false;
		}
	}
	
	bool IsEnableKey(const FKey& key) const
	{
		return mEnableKeys.Contains(key);
	}
	

	
	FTHsEnhancedActionHitQuery mGizmoHitQuery;
	FTHsEnhancedActionHitQuery mActorHitQuery;
	
	/*
	 * 
	 */
	FTHsCursorDelegate DelegateActionCursorMove;
	
	FTHsCursorDelegate DelegateCursorHoverStart;
	FTHsCursorDelegate DelegateCursorHoverOngoing;
	FTHsCursorDelegate DelegateCursorHoverEnd;

	FTHsCursorDelegate DelegateCursorDragging;
protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE ;
	}

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		FGameplayTagContainer GameplayTags;
		
		mSubsystemOuter = Outer;
		return true;
	}
	
	int32 AddActivatedActionPoints(UTHsActionPoint* actionPoint)	
	{
		return mActivatedActionPoints.Add(actionPoint);
	}

	bool bInitCursor = false;
	bool bInitEnhancedActionSystem = false;
	bool bInitClickActions = false;

	bool OnTickEnable()const
	{
		return bInitCursor & bInitEnhancedActionSystem & bInitClickActions;
	}

	const UTHsEnhancedActionSystemDataSetting* GetDataSetting() const
	{
		return mDataSetting;
	}

	const FTHsActionCursorData& GetCurrentCursorRecord() const
	{
		if (mActionCursor)
		{
			return mActionCursor->GetCurrentRecord();
		}
		static FTHsActionCursorData emptyRecord;
		return emptyRecord;
	}
	const FHitResult& GetCurrentCursorRecordHitResult() const
	{
		return GetCurrentCursorRecord().Hit;
	}
	TScriptInterface<ITIHHsInteraction> GetCurrentCursorRecordHitInteractionActor() const
	{
		return GetCurrentCursorRecord().CacheInteractionActor;
	}
	UMaterialInterface* GetGizmoDefaultMaterial(const FName& matName) const
	{
		return mDataSetting ? mDataSetting->GetGizmoDefaultMaterial(matName) : nullptr;
	}
	UMaterialInterface* GetCurrentGizmoDefaultMaterial() const
	{
		return GetGizmoDefaultMaterial(mGizmoMaterialName);
	}
	void TestInit();
private:
	bool mUseFilterExcludeTags = false;
	bool mUseFilterIncludeTags = false;
	
	FGameplayTagContainer mCurrentExcludeTags;
	FGameplayTagContainer mCurrentIncludeTags;

	//	TODO: rebind Process 부분 만들기
	
	//	/Script/TIHHousingCore.THsEnhancedActionSystemDataSetting'/Game/Test/DataSetting.DataSetting'
	//	G:/Project/UE54/Housing/hss/Hss/Content/Test/DataSetting.uasset
	FSoftObjectPath mDataSettingPath = FSoftClassPath("/Script/TIHHousingCore.THsEnhancedActionSystemDataSetting'/Game/Test/DataSetting.DataSetting'");
	//	이거는 Game 폴더의 최상단에 에셋을 만들어 두고 불러오자.
	TObjectPtr<UTHsEnhancedActionSystemDataSetting> mDataSetting = nullptr;
	
	mutable UObject* mSubsystemOuter = nullptr;
	
	UPROPERTY()
	TMap<FKey,const UInputAction*> mAllKeyActions;
	
	UPROPERTY()
	TObjectPtr<UInputMappingContext> mUniversalInputMappingContext;
	
	UWorld* mWorldContext = nullptr;
	
	UPROPERTY()
	TMap<FKey,TObjectPtr<UTHsActionPoint>> mAllActionPoints;
	UPROPERTY()
	TMap<FName,FKey> mAllKeyNames;
	UPROPERTY()
	TArray<UTHsActionPoint*> mActivatedActionPoints;
	
	UPROPERTY()
	TObjectPtr<ATHsActionCursor> mActionCursor;

	float mInputTickInterval = 1.0f / 30.0f;
	float mInputTickElapsedTime = 0.0f;
	float mInputTickThreshold = 1.0f / 30.0f;
	UPROPERTY()
	APlayerController* mPlayerController = nullptr;

	//	gizmo
	//UPROPERTY(Category="Gizmo")
	FName mGizmoMaterialName = FName("DefaultMeshMaterial");
	TMap<FName,UMaterialInterface*> mGizmoDefaultMaterials;

	UPROPERTY()
	TObjectPtr<UTHsGizmoManagerWrapper> mGizmoManagerWrapper = nullptr;

	UPROPERTY()
	TArray<UTHsActionMouseClick*> mAllMouseClickActions;

	UPROPERTY()
	TMap<FName,FTHsActionKeyBehaviorNodeContainer> mAllKeyBehaviorNodes;

	UPROPERTY()
	TArray<FName> mCurrentKeyBehaviorNodeNames;

	void UpdateQueryKeyBehaviorNodes()
	{
		for (const FName& curName :mCurrentKeyBehaviorNodeNames)
		{
			if (FTHsActionKeyBehaviorNodeContainer* behaviorContainer = mAllKeyBehaviorNodes.Find(curName))
			{
				behaviorContainer->ExecuteNodes(this);
			}
		}
	}
public:
	
	void SetModifierKey(const FKey& key,bool clicked)
	{
		if (clicked)
		{
			mCurrentModifierKeys.Add(key);
		}
		else
		{
			if (mCurrentModifierKeys.Contains(key))
			{
				mCurrentModifierKeys.Remove(key);
			}
		}
	}
	bool IsModifierKeyActivate(const FKey& key) const
	{
		return mCurrentModifierKeys.Contains(key);
	}
	bool IsModifierKeyActivateByName(const FName& keyName) const
	{
		FKey key = GetKeyByName(keyName);
		return IsModifierKeyActivate(key);
	}
	const FInputDeviceState& GetCurrentInputDeviceState() const
	{
		return mCurrentGizmoInputDeviceState;
	}
	
	

private:
	TSet<FKey> mCurrentModifierKeys;
	FInputDeviceState mCurrentGizmoInputDeviceState;
public:
	
	void ChangeFilterFuncFlag(ETHsEnhancedActionFuncFilterFlag flag, bool on)
	{
		if (on)
		{
			mCurrentActionFuncFilterFlags |= static_cast<int32>(flag);
		}
		else
		{
			mCurrentActionFuncFilterFlags &= ~static_cast<int32>(flag);
		}
		
	}
	void OnChangeFilterFuncFlag(ETHsEnhancedActionFuncFilterFlag flag)
	{
		ChangeFilterFuncFlag(flag,true);
	}
	void OffChangeFilterFuncFlag(ETHsEnhancedActionFuncFilterFlag flag)
	{
		ChangeFilterFuncFlag(flag,false);
	}

	bool mInputConsumed = false;
	/*
	 * widget - gizmo - actor 순임.
	 * 라인 트레이스도 지금 기록하는게 액터만 있음.
	 */
	
	bool CheckActionFuncFilterFlag(ETHsEnhancedActionFuncFilterFlag targetFlag) const
	{
		return  EnumHasAllFlags(ToFlags<ETHsEnhancedActionFuncFilterFlag>(mCurrentActionFuncFilterFlags),targetFlag);
	}

	bool CheckEnableAction(ITHsActionPointAPI* actionPointAPI) const
	{
		bool result = true;
		if (actionPointAPI)
		{
			result &= CheckActionFuncFilterFlag(actionPointAPI->GetEnhancedActionFilterFlags());
		}
		else
		{
			result = false;
		}

		return result;
	}

	FORCEINLINE void CaptureMouseDeltaInputState(FInputDeviceState& outInputDeviceState) const
	{
		FVector2D screenPos;
		mPlayerController->GetMousePosition(screenPos.X,screenPos.Y);
		outInputDeviceState.InputDevice = EInputDevices::Mouse;
		outInputDeviceState.Mouse.Delta2D = mActionCursor->GetDeltaScreenPosition();
		outInputDeviceState.Mouse.Position2D = screenPos;
		outInputDeviceState.Mouse.WorldRay = mActionCursor->GetCurrentWorldRay();
		UE_LOG(LogTemp,Warning,TEXT("CaptureMouseDeltaInputState Mouse Delta2D : %s, Position2D : %s"),*outInputDeviceState.Mouse.Delta2D.ToString(),*outInputDeviceState.Mouse.Position2D.ToString());
	}

	FVector2D GetPosition2D () const
	{
		return mActionCursor->GetScreenPosition();
	}
	
	FVector2D GetDeltaScreenPosition() const
	{
		return mActionCursor->GetDeltaScreenPosition();
	}
	FRay GetWorldRay() const
	{
		return mActionCursor->GetCurrentWorldRay();
	}

	
	/*
	 *
	 * 
	 */
	FORCEINLINE bool TryCaptureMouseDownForGizmo(FInputDeviceState& outInputDeviceState) const
	{
		bool isMouseDown = false;
		if ( IsActiveActionPointByKey( EKeys::LeftMouseButton))
		{
			outInputDeviceState.Mouse.Left.Button = EKeys::LeftMouseButton;
			//outInputDeviceState.Mouse.Left.bDown = true;
			outInputDeviceState.Mouse.Left.bPressed = true;
			isMouseDown = true;
		}
		else if (IsActiveActionPointByKey(EKeys::RightMouseButton))
		{
			outInputDeviceState.Mouse.Right.Button = EKeys::RightMouseButton;
			//outInputDeviceState.Mouse.Right.bDown = true;
			outInputDeviceState.Mouse.Right.bPressed = true;
			isMouseDown = true;
		}
		
		if (isMouseDown)
		{
			CaptureMouseDeltaInputState(outInputDeviceState);	
		}
		
		return isMouseDown;
	}
	FORCEINLINE bool TryCaptureMousePressingForGizmo(FInputDeviceState& outInputDeviceState) const
	{
		bool result = false;
		if ( IsActiveActionPointByKey( EKeys::LeftMouseButton))
		{
			outInputDeviceState.Mouse.Left.Button = EKeys::LeftMouseButton;
			outInputDeviceState.Mouse.Left.bDown = true;
			result = true;
		}
		else if (IsActiveActionPointByKey(EKeys::RightMouseButton))
		{
			outInputDeviceState.Mouse.Right.Button = EKeys::RightMouseButton;
			outInputDeviceState.Mouse.Right.bDown = true;
			result = true;
		}
		if (result)
		{
			CaptureMouseDeltaInputState(outInputDeviceState);	
		}
		return result;
	}
	FORCEINLINE void CaptureMouseUpForGizmo(FInputDeviceState& outInputDeviceState) const
	{
		bool result = false;
		if ( IsActiveActionPointByKey( EKeys::LeftMouseButton))
		{
			outInputDeviceState.Mouse.Left.Button = EKeys::LeftMouseButton;
			outInputDeviceState.Mouse.Left.bReleased = true;
			result = true;
		}
		else if (IsActiveActionPointByKey(EKeys::RightMouseButton))
		{
			outInputDeviceState.Mouse.Right.Button = EKeys::RightMouseButton;
			outInputDeviceState.Mouse.Right.bReleased = true;
			result = true;
		}
		CaptureMouseDeltaInputState(outInputDeviceState);
	}

	FORCEINLINE void CaptureModifyKeyForGizmo(FInputDeviceState& outInputDeviceState) const
	{
		if (IsActiveActionPointByKey(EKeys::LeftControl) || IsActiveActionPointByKey(EKeys::RightControl))
		{
			outInputDeviceState.bCtrlKeyDown = true;
		}
		if ( IsActiveActionPointByKey(EKeys::LeftShift)|| IsActiveActionPointByKey(EKeys::RightShift))
		{
			outInputDeviceState.bShiftKeyDown = true;
		}
		if ( IsActiveActionPointByKey(EKeys::LeftAlt) || IsActiveActionPointByKey(EKeys::RightAlt))
		{
			outInputDeviceState.bAltKeyDown = true;
		}
		if ( IsActiveActionPointByKey(EKeys::LeftCommand) || IsActiveActionPointByKey(EKeys::RightCommand))
		{
			outInputDeviceState.bCmdKeyDown = true;
		}
	}
	
	bool OnEventMouseDownForGizmo(UTHsActionPoint* inputAction)
	{
		bool result = false;
		// if (mGizmoManagerWrapper->IsValidUpdate())
		// {
		// 	FInputDeviceState inputDeviceStateSnap = {};
		// 	if (TryCaptureMouseDownForGizmo(inputDeviceStateSnap))
		// 	{
		// 		CaptureModifyKeyForGizmo(inputDeviceStateSnap);
		// 		if (mGizmoManagerWrapper->GetInputRouter()->PostInputEvent(inputDeviceStateSnap))
		// 		{
		// 			mGizmoPressed = true;
		// 			result = true;
		// 			UE_LOG(LogTemp,Warning,TEXT("OnEventMouse{{{	Down	}}}ForGizmo PostInputEvent true"));
		// 		}
		// 	}
		// }
		return result;
	}
	bool OnEventMousePressingForGizmo(UTHsActionPoint* inputAction)
	{
		bool result = false;
		// if (mGizmoManagerWrapper->IsValidUpdate())
		// {
		// 	FInputDeviceState inputDeviceStateSnap = {};
		// 	if (TryCaptureMousePressingForGizmo(inputDeviceStateSnap))
		// 	{
		// 		CaptureModifyKeyForGizmo(inputDeviceStateSnap);
		// 		if (mGizmoManagerWrapper->GetInputRouter()->PostInputEvent(inputDeviceStateSnap))
		// 		{
		// 			mGizmoPressed = true;
		// 			result = true;
		// 			UE_LOG(LogTemp,Warning,TEXT("OnEventMouse{{{	Pressing}}}ForGizmo PostInputEvent true"));
		// 		}
		// 	}
		// }

		return result;
	}
	bool mGizmoPressed = false;
	bool IsGizmoPressed() const
	{
		return mGizmoPressed;
	}
	void OnEventMouseUpForGizmo(UTHsActionPoint* inputAction)
	{
		// if (mGizmoManagerWrapper->IsValidUpdate())
		// {
		// 	FInputDeviceState inputDeviceStateSnap = {};
		// 	CaptureMouseUpForGizmo(inputDeviceStateSnap);
		// 	CaptureModifyKeyForGizmo(inputDeviceStateSnap);
		// 	mGizmoPressed = false;
		// 	if (mGizmoManagerWrapper->GetInputRouter()->PostInputEvent(inputDeviceStateSnap))
		// 	{
		// 		
		// 		UE_LOG(LogTemp,Warning,TEXT("OnEventMouse{{{	Up		}}}ForGizmo PostInputEvent true"));
		// 	}
		// }
		
	}
	
	

	bool IsActiveActionPointByKey(const FKey& key) const
	{
		return mCurrentActionPoint.Contains(key);
	}
	bool IsActiveActionPointByKeyName(const FName& keyName) const
	{
		FKey key = GetKeyByName(keyName);
		return IsActiveActionPointByKey(key);
	}
	bool IsActiveActionPoint(UTHsActionPoint* actionPoint) const
	{
		if (actionPoint)
		{
			return IsActiveActionPointByKey(actionPoint->GetActionPointKey());
		}
		return false;
	}
	TArray<UClass*> mConsignmentUObjectClasses;
	
	UPROPERTY()
	TArray<TObjectPtr<UObject>> mConsignmentUObjects;
	
private:
	bool mOnEventTime = false;
	bool mOnProcessTime = false;

	TMap<FKey,UTHsActionPoint*> mCurrentActionPoint;
	
	UPROPERTY( meta = (Bitmask, BitmaskEnum = "ETHsEnhancedActionFuncFilterFlag"))
	int32 mCurrentActionFuncFilterFlags = ToBits(ETHsEnhancedActionFuncFilterFlag::ENone);

	TSharedPtr<class FTHsGizmoViewExtension> mGizmoViewExtension;

public:
	const FTHsActionCursorData& GetCursorCapture()const
	{
		return mActionCursor->GetCursorCapture();
	}
	void GetCursorCaptureRef(FTHsActionCursorData& cursorData)const
	{
		mActionCursor->GetCursorCaptureRef(cursorData);
	}
private:
	mutable FHitResult mGlobalHitResult;
	UPROPERTY()
	ATHsInteractiveGizmoManager* mInteractiveGizmoManager;

	UPROPERTY()
	TObjectPtr<class UTHsActionSystemSelector> mActionSystemSelector;
};// end of UTHsEnhancedActionSystem



//	deprecated: 이거 말고 그냥 액션포인트를 상속받아 만들거임.
UCLASS(Blueprintable)
class UTHsActionKeyBehaviorForDeviceState : public UObject , public ITHsActionKeyBehaviorQuery
{
	GENERATED_BODY()

public:
	virtual bool TryExecute(FTHsActionKeyBehaviorNodeContainer& queries,
		UTHsEnhancedActionSystem* actionSystem) override
	{
		bool result = queries.ExecuteNodes(actionSystem);
		if (result)
		{
			

			
		}
		return result;
	}
};

template <ETHsActionFunctionCategory THsFuncCategory, typename T>
FDelegateHandle UTHsEnhancedActionSystem::BindInputKeyActionPointByName(ETHsActionFunctionType funcType,
	const FName& keyName, T&& InCallable)
{
	FDelegateHandle result = TTHsActionBinder<THsFuncCategory>::Bind(this,funcType,keyName,Forward<T>(InCallable));
	return result;
}
/*
 *	mouseClick
 *
 * 
 */

UCLASS()
class TIHHOUSINGCORE_API UTHsActionMouseClick : public UTHsActionPoint
{
	GENERATED_BODY()
public:
	
	virtual void PostInitActionPoint(UTHsEnhancedActionSystem* actionSystem, const UInputAction* inputAction,
		const FKey& key, int32 index) override;
	virtual void OverrideActionPointClick() override;
	virtual void OverrideActionPointDoubleClick() override;
	virtual void OverrideActionPointOngoingStart(const float deltaTime) override;
	virtual void OverrideActionPointOngoing(const float deltaTime) override;
	virtual void OverrideActionPointOngoingEnd(const float deltaTime) override;

	//virtual void DraggingEvaluate(ATHsActionCursor* cursorObj, float deltaTime);

	void EvaluateDragging(ATHsActionCursor* cursorObj, float deltaTime);
	//virtual void PostEvaluateDragging(ATHsActionCursor* cursorObj, float deltaTime);
	
	//virtual void OverrideActionPointClickDragStart(float deltaTime){};
	//virtual void OverrideActionPointClickDragOngoing( float deltaTime){};
	//virtual void OverrideActionPointClickDragEnd( float deltaTime){};
	
	void HandleDragEnd(float frameDeltaTime);
	void HandleDragOngoing(float frameDeltaTime);
	void HandleDragStart(float frameDeltaTime);
	
	virtual void PostEvaluateActionPoint(float CurrentTime, float frameDeltaTime) override;
	virtual void OverrideActionPressing(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual void OverrideActionPointDown() override;
	virtual void OverrideActionPointUp() override;
	virtual ETHsEnhancedActionFuncFilterFlag GetEnhancedActionFilterFlags() override
	{
		return ETHsEnhancedActionFuncFilterFlag::EClick;
	}

	//FTHsProcessDelegate DelegateProcessDragStart;
	//FTHsProcessDelegate DelegateProcessDragOngoing;
	//FTHsProcessDelegate DelegateProcessDragEnd;
	
private:
	ETHsActionMouseClickDragState mDragState;
	ATHsActionCursor* mActionCursor;
	FTIHHsActionParamIn mActionParamIn;
	APlayerController* mPlayerController;
	TStrongObjectPtr<UTHsFilter> mClickFilter;
	TScriptInterface<ITIHHsInteraction> mSelectedInteraction;

	FDelegateHandle mActionMoveDelegateHandle;
};

inline void UTHsActionMouseClick::HandleDragEnd(float frameDeltaTime)
{
	mDragState = ETHsActionMouseClickDragState::EDragEnded;
	mActionSystem->ActionDragingEnd(this,frameDeltaTime);
}

inline void UTHsActionMouseClick::HandleDragOngoing(float frameDeltaTime)
{
	mDragState = ETHsActionMouseClickDragState::EDragOngoing;
	mActionSystem->ActionDraging(this,frameDeltaTime);
}

inline void UTHsActionMouseClick::HandleDragStart(float frameDeltaTime)
{
	mDragState = ETHsActionMouseClickDragState::EDragStarted;
	mActionSystem->ActionDragingStart(this,frameDeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("HandleDragStart %s"), *GetActionPointKeyString());
}

/*
 *	시발....생각해보니깐 저것도 필요 없음. 아...물론 빠른 호출을 위해서라면 있긴해야하니 하자. 내가 만든건데 시발 써먹어야지...
 * 
 */
UCLASS()
class TIHHOUSINGCORE_API UTHsActionModifierClick : public UTHsActionPoint
{
	GENERATED_BODY()

public:
	virtual void OverrideActionPointDown() override;
	virtual void OverrideActionPointUp() override;
	
private:
	
};

template<>
struct TTHsActionBinder<ETHsActionFunctionCategory::EEvent>
{
	template<typename T>
	static FDelegateHandle Bind(UTHsEnhancedActionSystem* actionSystem,ETHsActionFunctionType functionType, const FName& actionPointKeyName,T&& InCallable)
	{
		FDelegateHandle result = FDelegateHandle();
		if (actionSystem == nullptr)
		{
			return result;
		}
		if (UTHsActionPoint* actionPoint = actionSystem->GetActivateActionPointByName(actionPointKeyName))
		{
			switch (functionType) {
			case ETHsActionFunctionType::EEvent_Click:
				result = actionPoint->GetActionBindContext().DelegateEventClick.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::EEvent_DoubleClick:
				result = actionPoint->GetActionBindContext().DelegateEventDoubleClick.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::EEvent_Down:
				result = actionPoint->GetActionBindContext().DelegateEventDown.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::EEvent_Up:
				result = actionPoint->GetActionBindContext().DelegateEventUp.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::EEvent_Triggered:
				result = actionPoint->GetActionBindContext().DelegateEventTriggered.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::EEvent_Cancel:
				result = actionPoint->GetActionBindContext().DelegateEventCancelled.AddLambda(Forward<T>(InCallable ));
				break;
			default:
				break;
			}
		}
		return result;
	}
};
template<>
struct TTHsActionBinder<ETHsActionFunctionCategory::EProcess>
{
	template<typename T>
	static FDelegateHandle Bind(UTHsEnhancedActionSystem* actionSystem,ETHsActionFunctionType functionType, const FName& actionPointKeyName,T&& InCallable)
	{
		FDelegateHandle result = FDelegateHandle();
		if (actionSystem == nullptr)
		{
			return result;
		}
		if (UTHsActionPoint* actionPoint = actionSystem->GetActivateActionPointByName(actionPointKeyName))
		{
			switch (functionType) {
			case ETHsActionFunctionType::EProcess_Pressing:
				result = actionPoint->GetActionBindContext().DelegateProcessPressing.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::EProcess_HoldStart:
				result = actionPoint->GetActionBindContext().DelegateProcessHoldStart.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::EProcess_HoldOngoing:
				result = actionPoint->GetActionBindContext().DelegateProcessHoldOngoing.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::EProcess_HoldEnd:
				result = actionPoint->GetActionBindContext().DelegateProcessHoldEnd.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::EProcess_DragStart:
				if (UTHsActionMouseClick* mouseClick = Cast<UTHsActionMouseClick>(actionPoint))
				{
					result = mouseClick->GetActionBindContext().DelegateProcessDragStart.AddLambda(Forward<T>(InCallable ));
				}
				break;
			case ETHsActionFunctionType::EProcess_DragOngoing:
				if (UTHsActionMouseClick* mouseClick = Cast<UTHsActionMouseClick>(actionPoint))
				{
					result = mouseClick->GetActionBindContext().DelegateProcessDragOngoing.AddLambda(Forward<T>(InCallable ));
				}
				break;
			case ETHsActionFunctionType::EProcess_DragEnd:
				if (UTHsActionMouseClick* mouseClick = Cast<UTHsActionMouseClick>(actionPoint))
				{
					result = mouseClick->GetActionBindContext().DelegateProcessDragEnd.AddLambda(Forward<T>(InCallable ));
				}
				break;
			default:
				break;
			}
		}
		return result;
	}
};
template<>
struct TTHsActionBinder<ETHsActionFunctionCategory::ECursor>
{
	template<typename T>
	static FDelegateHandle Bind(UTHsEnhancedActionSystem* actionSystem,ETHsActionFunctionType functionType, const FName& actionPointKeyName,T&& InCallable)
	{
		FDelegateHandle result = FDelegateHandle();
		{
			switch (functionType) {
			case ETHsActionFunctionType::ECursor_CursorMove:
				result = actionSystem->DelegateActionCursorMove.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::ECursor_HoverStart:
				result = actionSystem->DelegateCursorHoverStart.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::ECursor_HoverOngoing:
				result = actionSystem->DelegateCursorHoverOngoing.AddLambda(Forward<T>(InCallable ));
				break;
			case ETHsActionFunctionType::ECursor_HoverEnd:
				result = actionSystem->DelegateCursorHoverEnd.AddLambda(Forward<T>(InCallable ));
				break;
			default:
				break;
			}
		}
		return result;
	}
};//

class FTHsGizmoViewExtension final : public FSceneViewExtensionBase
{
public:
	explicit FTHsGizmoViewExtension(const FAutoRegister& AutoRegister)
		: FSceneViewExtensionBase(AutoRegister)
	{
	}
	void InitGizmoViewExtension( UTHsGizmoManagerWrapper* inGizmoWrapper);

	FTHsGizmoViewExtension(const FAutoRegister& AutoRegister,TWeakObjectPtr<UGizmoViewContext> inViewCtx): FSceneViewExtensionBase(AutoRegister), mViewContext(inViewCtx){}
	FTHsGizmoViewExtension(const FAutoRegister& AutoRegister,TWeakObjectPtr<UGizmoViewContext> inViewCtx, UTHsGizmoManagerWrapper* inGizmoWrapper): FSceneViewExtensionBase(AutoRegister), mViewContext(inViewCtx), mGizmoWrapper(inGizmoWrapper){}

	static inline bool IsPrimaryView(const FSceneView& InView)
	{
		//return  (InView.StereoPass == eSSP_FULL || InView.StereoPass == eSSP_PRIMARY);
		return (InView.StereoPass == EStereoscopicPass::eSSP_FULL || InView.StereoPass == EStereoscopicPass::eSSP_PRIMARY);
	}
	
	
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
private:
	TWeakObjectPtr<UGizmoViewContext> mViewContext;
	UTHsGizmoManagerWrapper* mGizmoWrapper = nullptr;
};

UCLASS()
class UTHsActionSystemSelector : public UObject , public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	void InitSelector(UTHsEnhancedActionSystem* actionSystem);

	void OnConnetInteraction(TScriptInterface<ITIHHsInteraction> interactionActor)
	{
		
	}
	void OnDisconnetInteraction(TScriptInterface<ITIHHsInteraction> interactionActor)
	{
		
	}
	
	bool OnSelectInteractionActor(ITIHHsInteraction* interactionActor)
	{
		bool result = false;
		if (interactionActor)
		{
			const FGameplayTagContainer& tag = interactionActor->GetInteractionTags();
			if (tag.HasTag(THS_Action_Trait_Selectable))
			{
				if (AActor* actorBase = Cast<AActor>(interactionActor))
				{
					mSelectedInteraction.SetObject(actorBase);
					mSelectedInteraction.SetInterface( interactionActor);
					result = true;
				}
			}
		}
		return result;
	}
	
	virtual void OverrideActionPointClick(UTHsActionPoint* actionPoint) override;
	virtual void OverrideActionPointDoubleClick(UTHsActionPoint* actionPoint) override;
	
	virtual void OverrideActionPointHoldOngoingStart(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual void OverrideActionPointHoldOngoing(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual void OverrideActionPointHoldOngoingEnd(UTHsActionPoint* actionPoint, float deltaTime) override;

	virtual void OverrideActionPointMouseMove(ATHsActionCursor* cursor, float deltaTime) override;
	virtual void OverrideActionPointSafeEnd(TScriptInterface<ITHsActionPointAPI> actionPoint) override;
	virtual int32 OverrideCommunicateInteraction(const FTHsCommunicationMessage& message) override;
	virtual int32
	OverrideCommunicateInteractionString(const FString& stringMsgm, const FTHsCommunicationMessage& message) override;
	virtual void OverrideConnectGizmo(TScriptInterface<class ITHsInteractiveGizmoAPI> gizmo) override;
	virtual void OverrideDisconnectGizmo(TScriptInterface<class ITHsInteractiveGizmoAPI> gizmo) override;
	virtual void OverrideActionPointClickDragStart(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual void OverrideActionPointClickDragOngoing(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual void OverrideActionPointClickDragEnd(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual FGameplayTagContainer GetInteractionTags() override
	{
		return FGameplayTagContainer(THS_Action_Trait_Selectable);
	}

	virtual void OverrideActionPressing(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual void OverrideActionPointDown(UTHsActionPoint* actionPoint) override;
	virtual void OverrideActionPointUp(UTHsActionPoint* actionPoint) override;
	virtual void OverrideActionPointHoverStart(ATHsActionCursor* cursor, float deltaTime) override;
	virtual void OverrideActionPointHoverEnd(ATHsActionCursor* cursor, float deltaTime) override;

	UTHsEnhancedActionSystem* GetActionSystem() const
	{
		return mActionSystem;
	}
	TScriptInterface<ITIHHsInteraction> GetSelectedInteraction() const
	{
		return mSelectedInteraction;
	}
	void OnSelectedInteraction(TScriptInterface<ITIHHsInteraction> interaction)
	{
		mSelectedInteraction = interaction;
		interaction->OverrideSelection(true);
	}
	void OffSelectedInteraction(TScriptInterface<ITIHHsInteraction> interaction)
	{
		if (interaction.GetInterface() != nullptr)
		{
			interaction->OverrideSelection(false);
		}
		mSelectedInteraction = nullptr;
	}
private:
	UWorld* mWorldContext = nullptr;
	TWeakObjectPtr< UTHsActionMouseClick> mLeftMouseClickAction = nullptr;
	UTHsEnhancedActionSystem* mActionSystem;
	TScriptInterface<ITIHHsInteraction> mSelectedInteraction;
	FHitResult mLastHitResult;
};


//
// UCLASS()
// class UTHsGizmoBehaviorAxisDrag : public UInputBehavior
// {
// 	GENERATED_BODY()
//
// public:
// 	virtual FInputCaptureRequest WantsCapture(const FInputDeviceState& InputState) override;
// 	virtual FInputCaptureUpdate BeginCapture(const FInputDeviceState& InputState, EInputCaptureSide eSide) override;
// 	virtual FInputCaptureUpdate
// 	UpdateCapture(const FInputDeviceState& InputState, const FInputCaptureData& CaptureData) override;
// 	virtual void ForceEndCapture(const FInputCaptureData& CaptureData) override;
// };
