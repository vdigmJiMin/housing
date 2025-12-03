// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TIHHsCoreEnums.generated.h"

UENUM(BlueprintType, meta = (Bitflags))
enum class ETIHHsTagFilterRank : uint8
{
	ETIHHsTagFilterRank_None = 0,
	ETIHHsTagFilterRank_CollisionChannle = 1,
	ETIHHsTagFilterRank_GameplayTag = 2,
	ETIHHsTagFilterRank_StaticTag = 3,
	ETIHHsTagFilterRank_DynamicTag = 4,
};
ENUM_CLASS_FLAGS(ETIHHsTagFilterRank);

UENUM(BlueprintType)
enum class ETIHHsStateCategory : uint8
{
	ETIHHsStateCategory_Default = 0,
	ETIHHsStateCategory_WithUser,
	ETIHHsStateCategory_WithOther,
	ETIHHsStateCategory_BySelf,
	ETIHHsStateCategory_Initiate,
	ETIHHsStateCategory_Processing,
	ETIHHsStateCategory_Error,
	ETIHHsStateCategory_Destroy,
	ETIHHsStateCategory_Verification,
	ETIHHsStateCategory_Backup,
	ETIHHsStateCategory_Update,
};
UENUM(BlueprintType)
enum class ETIHHsStateDetail : uint8
{
	ETIHHsStates_Unknown = 0,
	// Default

	ETIHHsStates_Default_Idle,
	ETIHHsStates_Default_WaitingAction,
	ETIHHsStates_Default_Paused,
	ETIHHsStates_Default_Focus,
	ETIHHsStates_Default_CreateCandidate,
	
	// WithUser
	ETIHHsStates_WithUser_InProgress,
	ETIHHsStates_WithUser_ProcessWaiting,
	ETIHHsStates_WithUser_CompleteFinalizing,
    
	// BySelf
	ETIHHsStates_BySelf_Waiting,
	ETIHHsStates_BySelf_InProgress,
	ETIHHsStates_BySelf_CompleteFinalizing,
    
	// Initiate
	ETIHHsStates_Initiate_Starting,
	ETIHHsStates_Initiate_Waiting,
	ETIHHsStates_Initiate_Error,
	ETIHHsStates_Initiate_CompleteFinalizing,
    
	// Processing
	ETIHHsStates_Processing_Waiting,
	ETIHHsStates_Processing_Starting,
	ETIHHsStates_Processing_Finalizing,
    
	// Error
	ETIHHsStates_Error_Stopped,
	ETIHHsStates_Error_Analyzing,
	ETIHHsStates_Error_Resolving,
    
	// Destroy
	ETIHHsStates_Destroy_Waiting,
	ETIHHsStates_Destroy_Starting,
	ETIHHsStates_Destroy_WaitingRelease,
    
	// Verification
	ETIHHsStates_Verification_StartWaiting,
	ETIHHsStates_Verification_InProgress,
	ETIHHsStates_Verification_CompleteFinalizing,
    
	// WithOther
	ETIHHsStates_WithOther_Waiting,
	ETIHHsStates_WithOther_InProgress,
	ETIHHsStates_WithOther_ProcessWaiting,
    
	// Backup
	ETIHHsStates_Backup_Preparing,
	ETIHHsStates_Backup_InProgress,
	ETIHHsStates_Backup_CompleteFinalizing,
    
	// Update
	ETIHHsStates_Update_Waiting,
	ETIHHsStates_Update_InProgress,
	ETIHHsStates_Update_CompleteFinalizing,
};
/*
 *	interaction.user.tap
 *	interaction.system.trace
 *	이럴거면 유저라는 객체가 있는게 편할텐데? 그게 플레이어 컨트롤러가 될거고...
 *
 * 
 */
UENUM(BlueprintType)
enum class ETIHHsActionTransformDataExecuteAgent : uint8
{
	ETIHTransform_System,
	ETIHTransform_User,
};
UENUM(BlueprintType)
enum class ETIHHsActionTransformDataType : uint8
{
	ETIHTransform_TransformWorld,
	ETIHTransform_DirectionWorld,
	ETIHTransform_TransformAdditive,
	ETIHTransform_DirectionAdditive,
};

UENUM(BlueprintType, meta = (Bitflags))
enum class ETIHHsActionTransformDataCallBackTypes : uint8
{
	ETIHTransform_None = 0,
	ETIHTransform_MoveBegin = 0x01,	//	1	0b00000001	:	numberic 1
	ETIHTransform_Moving = 0x02,		//	2	0b00000010	:	numberic 2
	ETIHTransform_MoveEnd = 0x04,		//	4	0b00000010	:	numberic 4
	ETIHTransform_MoveBeginAndMoving = 0x03,	//	3	0b00000011	:	numberic 3
	ETIHTransform_MoveBeginAndMoveEnd = 0x05,	//	5	0b00000101	:	numberic 5
	ETIHTransform_MovingAndMoveEnd = 0x06,	//	6	0b00000110	:	numberic 6
	ETIHTransform_MoveAll = 0x07,	//	7	0b00000111	:	numberic 7
};

UENUM(BlueprintType, meta = (Bitflags))
enum class ETIHHsActionTransformDataFinishTypes : uint8
{
	ETIHTransform_Error = 0x00,	//	0	0b00000000	:	numberic 0
	ETIHTransform_Time = 0x01,	//	1	0b00000001	:	numberic 1
	ETIHTransform_Distance = 0x02,	//	2	0b00000010	:	numberic 2
	ETIHTransform_CollisionType = 0x04,	//	4	0b00000100	:	numberic 4
	ETIHTransform_CollisionTarget = 0x08,	//	8	0b00001000	:	numberic 8
	ETIHTransform_Abort = 0x10,	//	16	0b00010000	:	numberic 16
	ETIHTransform_TargetLocation = 0x20,	//	32	0b00100000	:	numberic 32
};

UENUM(blueprintType)
enum class ETIHHsFunctionRegistOvrrideOption : uint8
{
	ENone = 0,
	EOverride = 1,			
	EAddOrOverride = 2,	
	EFindOrNot = 3,		
};

UENUM(BlueprintType)
enum class ETIHHsActionMoveStateType : uint8
{
	EIdle = 0,	//	아무것도 하지 않는 상태
	EOutCursor = 1,	//	MoveEnd이후 시간을 측정하다가 일정시간이 되면 저장시키는 용도
	EMoveStart = 2,	//	이동 시작
	EMoveOngoing = 3,	//	이동 진행중
	EMoveEnd = 4,	//	이동 종료
	EMax = 5 UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETIHHsActionMoveEventType : uint8
{
	EStop = 0,
	EMove,
	EOutCursor,
	EMax UMETA(Hidden), 
};
UENUM(BlueprintType)
enum ETIHHsTriggerActionTypes : uint8
{
	ETriggerAction_Tap,
	ETriggerAction_DoubleTap,
	ETriggerAction_HoldStart,
	ETriggerAction_Holding,
	ETriggerAction_HoldEnd,
	ETriggerAction_HoverEnter,
	ETriggerAction_Hovering,
	ETriggerAction_HoverExit,
	ETriggerAction_MoveStart,
	ETriggerAction_Moving,
	ETriggerAction_MoveEnd,
	
};
UENUM()
enum class ETIHHsFunctionType
{
	EInvalid,
	EJustOrder,	//	Just order, no function to call.
	EJustOrderDebugLog,
	EDynamicDelegate,
	EUObject,
	
	ETFunction,
	EDelegate,
	EStaticFunction,
	EMemberFunction,
};
/*
 *	------------------------------------------------
 *			Enhanced Action System
 * 
 */
UENUM()
enum class ETHsActionFunctionCategory : uint8
{
	EEvent UMETA(DisplayName="Event", ToolTip="just event. argument is only UTHsActionPoint*"),
	EProcess UMETA(DisplayName="Process", ToolTip="step type. start, ongoing, end. argument is UTHsActionPoint* key, float deltaTime"),
	ECursor UMETA(DisplayName="Cursor", ToolTip="cursor type. start, ongoing, end. argument is ATHsActionCursor* cursor,float deltaTime"),
	// EZCustom0,
	// EZCustom1,
	// EZCustom2,
};

UENUM(Blueprintable)
enum class ETHsActionFunctionType : uint8
{
	EProcess_Pressing,
	EProcess_HoldStart,
	EProcess_HoldOngoing,
	EProcess_HoldEnd,
	ECursor_HoverStart,
	ECursor_HoverOngoing,
	ECursor_HoverEnd,
	EEvent_Click,
	EEvent_DoubleClick,
	EEvent_Down,
	EEvent_Up,
	EEvent_Triggered,
	EEvent_Cancel,
	ECursor_CursorMove,
	EProcess_DragStart,
	EProcess_DragOngoing,
	EProcess_DragEnd,

	EFunction_Selection,
	EFunction_DisSelection,
	
	EZCustom0,
	EZCustom1,
	EZCustom2,
	EZCustom3,
	EZCustom4,
	EZCustom5,
	EZCustom6,
	EZCustom7,
	EZCustom8,
};
UENUM()
enum class ETHsActionKeyBehaviorNodeExecuteType
{
	EFirstOne,
	ELastOne,
	EAll,
	ESequence,
	ESelection,
};
UENUM()
enum class ETHsActionMouseClickDragState
{
	EDragNone,
	EDragStarted,
	EDragOngoing,
	EDragEnded,
};
UENUM(BlueprintType)
enum class ETHsActionCursorHoverType : uint8
{
	ENone,
	EHoverStart,
	EHoverOngoing,
	EHoverEnd,
};
UENUM(BlueprintType)
enum  class  ETHsActionPointEvaluationState : uint8
{
	EWaitingDown				UMETA(DisplayName="WaitingDown"),
	EEvaluateOngoingStart			UMETA(DisplayName="CheckingHoldStart"),
	EOngoing					UMETA(DisplayName="Ongoing"),
	ECheckingDoubleClick		UMETA(DisplayName="CheckingDoubleClick"),
};
UENUM(BlueprintType, meta = (Bitflags))
enum class ETHsActionCursorState : uint8
{
	// UPROPERTY()
	// bool bIsMove = false; //	이동이 있었는지 여부임.
	// UPROPERTY()
	// bool bIsHit = false; //	충돌이 있었는지 여부임.<- 물론 hit.isvalid로도 확인 가능함.
	// UPROPERTY()
	// bool bIsSameHitActor = false;	//	이게 되었다고 해서 같은 컴포넌트라는 보장은 없음. 그리고 무조건 not null 이라는 보장도 없음.
	// UPROPERTY()
	// bool bIsSameComponent = false; //	이게 되었다고 해서 같은 액터 라는 보장은 없음(어지간해서는 같은 액터는 맞음). 그리고 무조건 not null 이라는 보장도 없음.
	// UPROPERTY()
	// bool bIsCastedTIHHsInteractionActor = false;
	// UPROPERTY()
	// bool bIsCastedTIHHsInteractionComponent = false;
	ENone									= 0					,
	EIsMove									= 1<<0				,
	EIsHit									= 1<<1				,
	EIsSameHitActor							= 1<<2				,
	EIsSameComponent						= 1<<3				,
	EIsCastedTIHHsInteractionActor 			= 1<<4				,
	EIsCastedTIHHsInteractionComponent		= 1<<5				,
	EValidDeprojectionValid					= 1<<6				,
};
ENUM_CLASS_FLAGS(ETHsActionCursorState);

UENUM(BlueprintType, meta = (Bitflags))
enum class ETHsActionCursorHoverHitCategory : uint8
{
	ENone			= 0			UMETA(DisplayName="None"),
	EWidget			= 1<<0		UMETA(DisplayName="Widget"),
	EGizmo			= 1<<1		UMETA(DisplayName="Gizmo"),
	EStuff			= 1<<2		UMETA(DisplayName="Stuff"),
	EUnknown		= 1<<3		UMETA(DisplayName="Unknown"),

};

UENUM()
enum class ETHsActionCursorMoveState
{
	EMouseMovePaused,
	EMouseMoveStarted,
	EMouseMoveOngoing,
	EMouseMoveEnded,
};
UENUM(BlueprintType, meta = (Bitflags))
enum class ETHsEnhancedActionFuncFilterFlag : uint8
{
	ENone			= 0			UMETA(DisplayName="None"),
	ECursorMove		= 1<<0		UMETA(DisplayName="CursorMove"),
	EHover			= 1<<1		UMETA(DisplayName="Hover"),
	EClick			= 1<<2		UMETA(DisplayName="Click"),
};
ENUM_CLASS_FLAGS(ETHsEnhancedActionFuncFilterFlag);

UENUM(BlueprintType)
enum class ETHsActionPointPressingStateType : uint8
{
	ENone			UMETA(DisplayName="None"),
	EPressStarted	UMETA(DisplayName="PressStarted"),
	EPressOngoing	UMETA(DisplayName="PressOngoing"),
	EPressEnded		UMETA(DisplayName="PressEnded"),
};
UENUM(BlueprintType)
enum class ETHsGizmoUpdateInputDeviceStateType : uint8
{
	EPressed		UMETA(DisplayName="Pressed"),
	EOngoing		UMETA(DisplayName="Ongoing"),
	EReleased		UMETA(DisplayName="Released"),
	EHovered		UMETA(DisplayName="Hovered"),
};
// UENUM(BlueprintType,meta = (Bitflags))
// enum class ETIHHsMouseState : uint8
// {
// 	//	000 000 00
// 	ETIHHsMouseState_None = 0,
// 	ETIHHsMouseState_Taps_DoubleTap = 0x01,		//	1	0b00000001	:	numberic 1
// 	ETIHHsMouseState_Taps_Tap = 0x02,			//	2	0b00000010	:	numberic 2
// 	ETIHHsMouseState_Taps_Hold = 0x03,			//	3	0b00000011	:	numberic 3
// 	
// 	ETIHHsMouseState_HW_Up = 0x04,				//	4	0b00000100	:	signal
// 	ETIHHsMouseState_HW_Toggle = 0x08,			//	8	0b00001000	:	state
// 	ETIHHsMouseState_HW_Down = 0x0c ,			//	12	0b00001100	:	signal
// 	ETIHHsMouseState_HW_Move = 0x10,			//	16	0b00010000	:	state
// 	ETIHHsMouseState_HW_Block = 0x1c,			//	32	0b00011100	:	state
// 	
// 	ETIHHsMouseState_Hover_Hovering = 0x20,		//	32	0b00100000	:	state
// 	ETIHHsMouseState_Hover_HoverEnter = 0x40,	//	64	0b01000000	:	state
// 	ETIHHsMouseState_Hover_HoverExit = 0x80,	//	128	0b10000000	:	state
// 	ETIHHsMouseState_Hover_Block = 0xE0,		//	128	0b11100000	:	state
// };
// UENUM(BlueprintType)
// enum class ETIHHsStateCategory : uint8
// {
// 	ETIHHsStateCategory_Default,
// 	ETIHHsStateCategory_WithUser,
// 	ETIHHsStateCategory_WithOther,
// 	ETIHHsStateCategory_BySelf,
// 	ETIHHsStateCategory_Initialize,
// 	ETIHHsStateCategory_Process,
// 	ETIHHsStateCategory_Error,
// 	ETIHHsStateCategory_Destroy,
// 	ETIHHsStateCategory_Verify,
// 	ETIHHsStateCategory_Backup,
// 	ETIHHsStateCategory_Update,
// };
// UENUM(BlueprintType)
// enum class ETIHHsStateDetail : uint8
// {
// 	EETIHHsStateDetail_Create,
// 	EETIHHsStateDetail_Idle,
// 	EETIHHsStateDetail_WaitingAction,
// 	EETIHHsStateDetail_Paused,
// 	
// 	EETIHHsStateDetail_Stopped,
// 	EETIHHsStateDetail_Analyzing,
// 	EETIHHsStateDetail_Resolving,
// 	
// 	EETIHHsStateDetail_Waiting,
//    	EETIHHsStateDetail_Starting,
//    	EETIHHsStateDetail_Preparing,
//    	EETIHHsStateDetail_InProgress,
//    	EETIHHsStateDetail_ProcessWaiting,
//    	EETIHHsStateDetail_CompleteFinalizing,
//    	EETIHHsStateDetail_StartWaiting,
//    	EETIHHsStateDetail_Finalizing,
//    	EETIHHsStateDetail_WaitingRelease,
// };