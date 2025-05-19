#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Templates/TypeHash.h"
#include "TIHHsBlackboardDefines.generated.h"

class UTIHHsBBSession;
struct FTIHHsBlackBoard;

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
	EFVariant,
	EInvalid,
	EDefaultMax
};
/*	==========================================================================
 *								X-Macro
 */

/*	------------------------------------------------------------------------------
 *	@brief	블랙보드 데이터 타입을 정의하는 매크로
 *	@note	앞으로 데이터를 추가할땐 여기에다가 추가하면 된다.
 */
#ifndef TIHMACRO_BB_XCODE_DATATYPE_LIST
#define TIHMACRO_BB_XCODE_DATATYPE_LIST(TIHMACRO_Apply_Macro)\
TIHMACRO_Apply_Macro(bool, Bool, false)\
TIHMACRO_Apply_Macro(int8, Int8, 0)\
TIHMACRO_Apply_Macro(int16, Int16, 0)\
TIHMACRO_Apply_Macro(int32, Int32, 0)\
TIHMACRO_Apply_Macro(int64, Int64, 0)\
TIHMACRO_Apply_Macro(uint8, Uint8, 0)\
TIHMACRO_Apply_Macro(uint16, Uint16, 0)\
TIHMACRO_Apply_Macro(uint32, Uint32, 0)\
TIHMACRO_Apply_Macro(uint64, Uint64, 0)\
TIHMACRO_Apply_Macro(float, Float, 0.0f)\
TIHMACRO_Apply_Macro(double, Double, 0.0)\
TIHMACRO_Apply_Macro(FString, String, TEXT(""))\
TIHMACRO_Apply_Macro(FVector, Vector, FVector::ZeroVector)\
TIHMACRO_Apply_Macro(FVector2D, Vector2D, FVector2D::ZeroVector)\
TIHMACRO_Apply_Macro(FVector4, Vector4, FVector4::Zero())\
TIHMACRO_Apply_Macro(FQuat, Quat, FQuat::Identity)\
TIHMACRO_Apply_Macro(FTransform, Transform, FTransform::Identity)\
TIHMACRO_Apply_Macro(FLinearColor, LinearColor, FLinearColor::White)\
TIHMACRO_Apply_Macro(FColor, Color, FColor::White)\
TIHMACRO_Apply_Macro(FName, Name, FName(TEXT("")))\
TIHMACRO_Apply_Macro(UObject*, UObject, nullptr)\
TIHMACRO_Apply_Macro(UClass*, UClass, nullptr)\
TIHMACRO_Apply_Macro(UActorComponent*, UActorComponent, nullptr)\
TIHMACRO_Apply_Macro(FVariant,FVariant, FVariant())\

#endif// TIHMACRO_BB_XCODE_DATATYPE_LIST

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

	static TArray<TIHTemplateType>* GetBBArrayFromSession(UTIHHsBBSession* blackBoardSession)
	{
		return nullptr;
	}
	static TArray<TIHTemplateType>* GetBBArray(FTIHHsBlackBoard& blackBoardSession)
	{
		return nullptr;
	}
	static const TArray<TIHTemplateType>* GetBBArrayConst(const FTIHHsBlackBoard& blackBoardSession)
	{
		return nullptr;
	}
	static bool CheckType(uint16 dataType)
	{
		return false;
	}
};
/*	------------------------------------------------------------------------------
 *	@brief	블랙보드 데이터 타입을 정의하는 매크로
 *	@note TIHMACRO_BB_XCODE_DATATYPE_LIST 를 사용하여 템플릿 특수화
 *	@note 왜 템플릿을 썼는가? 공통으로 캐스팅을 하지만 특수화를 해야해줘야한다. 그걸 일괄적으로 해주기 위해서 진행
 */
#define TIHMACRO_BB_DEFAULT_TYPE_TRAIT_CLASS(type,enumName,defalutValue) \
	template<> \
	class TTIHHsDefaultTypeTrait<type> \
	{ \
		private: static type gDefaultValue;\
		public: \
		static type GetDefaultValue()\
		{\
		return gDefaultValue;\
		}\
		static constexpr size_t GetTypeSize()\
		{\
		return sizeof(type);\
		}\
		static constexpr ETIHHsBlackBoardDataType GetType() \
		{ \
		return ETIHHsBlackBoardDataType::E##enumName; \
		}\
		static TArray<type>* GetBBArrayFromSession(UTIHHsBBSession* blackBoardSession)\
		{\
		return (blackBoardSession != nullptr) ?	&(blackBoardSession->GetBlackBoard()).BBValue_##enumName : nullptr;	\
		}\
		static TArray<type>* GetBBArray(FTIHHsBlackBoard& blackBoard)\
		{\
			return &blackBoard.BBValue_##enumName;\
		}\
		static const TArray<type>* GetBBArrayConst(const FTIHHsBlackBoard& blackBoard)\
		{\
			return &blackBoard.BBValue_##enumName;\
		}\
		static int32 GetBBArrayNum(UTIHHsBBSession* blackBoardSession)\
		{\
		return (blackBoardSession != nullptr) ?	(blackBoardSession->GetBlackBoard()).BBValue_##enumName.Num() : -1;	\
		}\
		static bool CheckType(uint16 dataType) \
		{ \
		return (dataType == (uint16)ETIHHsBlackBoardDataType::E##enumName); \
		} \
	};

#define TIHMACRO_BB_DEFAULT_TYPE_TRAIT_CLASS_EXTERNAL_DEFINE(type,defalutValue) type TTIHHsDefaultTypeTrait<type>::gDefaultValue = defalutValue;


/*	------------------------------------------------------------------------------
 *	@brief	블랙보드 데이터 타입을 정의하는 매크로
 *	@note TIHMACRO_Blackboard_Datatype_List 를 사용하여 템플릿 특수화
 *	@note 왜 템플릿을 썼는가? 공통으로 캐스팅을 하지만 특수화를 해야해줘야한다. 그걸 일괄적으로 해주기 위해서 진행
 */
#define TIHMACRO_BB_APPLY_DEFAULT_TYPE_TRAIT(type, enumName, defaultValue) \
TIHMACRO_BB_DEFAULT_TYPE_TRAIT_CLASS(type, enumName, defaultValue)

#define TIHMACRO_BB_APPLY_DEFAULT_TYPE_TRAIT_CLASS_EXTERNAL(type, enumName, defaultValue) \
TIHMACRO_BB_DEFAULT_TYPE_TRAIT_CLASS_EXTERNAL_DEFINE(type, defaultValue) 

#define TIHMACRO_BB_CREATE_WRITER_CASE(type, enumName, defaultValue) \
case static_cast<uint16>(ETIHHsBlackBoardDataType::E##enumName): \
return new TTIHHsBBTicketElementWriter<type>(inTicketHandle, targetElement);

#define TIHMACRO_BB_CREATE_READER_CASE(type, enumName, defaultValue) \
case static_cast<uint16>(ETIHHsBlackBoardDataType::E##enumName): \
return new TTIHHsBBTicketElementReader<type>(inBBTicketHandle, targetElement);

#define TIHMACRO_BB_GET_TYPE_CASE_FROM_INT(type, enumName, defaultValue) \
case static_cast<int16>(ETIHHsBlackBoardDataType::E##enumName): \
reValue = ETIHHsBlackBoardDataType::E##enumName; \
break;

#define TIHMACRO_BB_GET_INT_CASE_FROM_ENUM(type, enumName, defaultValue) \
case ETIHHsBlackBoardDataType::E##enumName: \
reValue = static_cast<int16>(ETIHHsBlackBoardDataType::E##enumName); \
break;

#define TIHMACRO_BB_GET_TYPE_SIZE_CASE(type, enumName, defaultValue) \
case static_cast<int16>(ETIHHsBlackBoardDataType::E##enumName): \
reValue = sizeof(type); \
break;

#define TIHMACRO_BB_GET_TYPE_NAME_CASE(type, enumName, defaultValue) \
case static_cast<int16>(ETIHHsBlackBoardDataType::E##enumName): \
reValue = TEXT(#enumName); /* 또는 TEXT("E") TEXT(#enumName) */ \
break;

#define TIHMACRO_BB_ADD_TYPE_NAME_CHECK_ELSE_IF(type, enumName, defaultValue) \
else if (inTypeNameLower == FString(TEXT(#enumName)).ToLower() || \
inTypeNameLower == FString(TEXT("e") TEXT(#enumName)).ToLower() || \
inTypeNameLower == FString(TEXT("etiihhsblackboarddatatype::e") TEXT(#enumName)).ToLower()) \
{ \
reValue = ETIHHsBlackBoardDataType::E##enumName; \
}
#define TIHMACRO_BB_FIRST_TYPE_NAME_CHECK_IF(type, enumName, defaultValue) \
if (inTypeNameLower == FString(TEXT(#enumName)).ToLower()) \
{ \
reValue = ETIHHsBlackBoardDataType::E##enumName; \
}
#define TIHMACRO_BB_OTHER_TYPE_NAME_CHECK_ELSE_IF(type, enumName, defaultValue) \
else if (inTypeNameLower == FString(TEXT(#enumName)).ToLower()) \
{ \
reValue = ETIHHsBlackBoardDataType::E##enumName; \
}
#define TIHMACRO_BB_GENERATE_GETTYPEFROMNAME_IF_ELSE_BLOCK(type, enumName, defaultValue) \
if (inTypeNameLower == FString(TEXT(#enumName)).ToLower()) \
{ \
return ETIHHsBlackBoardDataType::E##enumName; \
}
#define TIHMACRO_BB_TYPE_FROM_NAME_IF_CLAUSE(type, enumName, defaultValue) \
if (inTypeName == FString(TEXT(#enumName)).ToLower()) { return ETIHHsBlackBoardDataType::E##enumName; }

#define TIHMACRO_BB_GENERATE_GETTYPEFROMNAME_ELSE_IF(type, enumName, defaultValue) \
else if (inTypeNameLower == FString(TIHMACRO_CASE_TEXT(ETIHHsBlackBoardDataType::E##enumName)).ToLower()) \
{ \
reValue = ETIHHsBlackBoardDataType::E##enumName; \
}

#define TIHMACRO_BB_RESERVE_CASE(type, enumName, defaultValue) \
case static_cast<int16>(ETIHHsBlackBoardDataType::E##enumName): \
BBValue_##enumName.Reserve(count); \
break;

#define TIHMACRO_BB_EMPTY_CASE(type, enumName, defaultValue) \
case static_cast<int16>(ETIHHsBlackBoardDataType::E##enumName): \
BBValue_##enumName.Empty(); \
break;

#define TIHMACRO_BB_ADD_DEFAULTED_CASE(type, enumName, defaultValue) \
case static_cast<int16>(ETIHHsBlackBoardDataType::E##enumName): \
reValue = BBValue_##enumName.AddDefaulted(count); \
break;