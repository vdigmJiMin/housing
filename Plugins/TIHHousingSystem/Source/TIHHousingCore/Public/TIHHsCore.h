// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "TIHHsCore.generated.h"

UCLASS()
class UTIHHsGlobalConfigure : public UObject
{
	GENERATED_BODY()
public:
	
	UFUNCTION()
	static bool IsTickEnable(UWorld* world) 
	{
		bool reValue = false;
		if (not gForceDisable)
		{
			if (world == nullptr)
			{
				reValue = false;
			}
			else
			{
				if (world->IsPaused())
				{
					reValue = gTickPauseEnable;
				}
				else
				{
					reValue = gTickRuntimeActive;
				}
			}
		}
		return reValue;
	}
	
	UFUNCTION()
	static void SetTickForceDisable(bool inForceDisable){gForceDisable = inForceDisable;}
	UFUNCTION()
	static bool IsTickForceDisable() {return gForceDisable;}

	UFUNCTION()
	static void SetTickPauseEnable(bool inTickPauseEnable){gTickPauseEnable = inTickPauseEnable;}
	UFUNCTION()
	static bool IsTickPauseEnable() {return gTickPauseEnable;}

	UFUNCTION()
	static void SetTickRuntimeActiveEnable(bool inTickPauseEnable){gTickPauseEnable = inTickPauseEnable;}
	UFUNCTION()
	static bool IsTickRuntimeActive() {return gTickPauseEnable;}

	//	IsTickableInEditor 이거만 다르게
private:
	static bool gForceDisable;
	static bool gTickPauseEnable;
	static bool gTickRuntimeActive;	//	이거는 개별로 해도 되는거임
};


class UTIHHsTapActionSubSystem;
UENUM(BlueprintType,meta = (Bitflags))
enum class ETIHHsMouseState : uint8
{
	//	000 000 00
	ETIHHsMouseState_None = 0,
	ETIHHsMouseState_Taps_DoubleTap = 0x01,		//	1	0b00000001	:	numberic 1
	ETIHHsMouseState_Taps_Tap = 0x02,			//	2	0b00000010	:	numberic 2
	ETIHHsMouseState_Taps_Hold = 0x03,			//	3	0b00000011	:	numberic 3
	
	ETIHHsMouseState_HW_Up = 0x04,				//	4	0b00000100	:	signal
	ETIHHsMouseState_HW_Toggle = 0x08,			//	8	0b00001000	:	state
	ETIHHsMouseState_HW_Down = 0x0c ,			//	12	0b00001100	:	signal
	ETIHHsMouseState_HW_Move = 0x10,			//	16	0b00010000	:	state
	ETIHHsMouseState_HW_Block = 0x1c,			//	32	0b00011100	:	state
	
	ETIHHsMouseState_Hover_Hovering = 0x20,		//	32	0b00100000	:	state
	ETIHHsMouseState_Hover_HoverEnter = 0x40,	//	64	0b01000000	:	state
	ETIHHsMouseState_Hover_HoverExit = 0x80,	//	128	0b10000000	:	state
	ETIHHsMouseState_Hover_Block = 0xE0,		//	128	0b11100000	:	state
};

UINTERFACE()
class UTIHHsCommonObject : public UInterface
{
	GENERATED_BODY()
};

class ITIHHsCommonObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual FGameplayTag GetGameplayTag()PURE_VIRTUAL(GetGameplayTag, return FGameplayTag::EmptyTag;);

	UFUNCTION()
	virtual AActor* GetOwnerBasedOnActor()PURE_VIRTUAL(GetOwnerBasedOnActor, return nullptr;);
	
	UFUNCTION()
	virtual void OnMouseActionDown()PURE_VIRTUAL(OnMouseActionDown,);
	
	UFUNCTION()
	virtual void OnMouseActionUp()PURE_VIRTUAL(OnMouseActionUp,);
	
	//	hover
	UFUNCTION() 
	virtual void OnMouseActionHoverEnter()PURE_VIRTUAL(OnMouseActionHoverEnter,);
	UFUNCTION() 
	virtual void OnMouseActionHovering()PURE_VIRTUAL(OnMouseActionHovering,);
	UFUNCTION() 
	virtual void OnMouseActionHoverExit()PURE_VIRTUAL(OnMouseActionHoverExit,);
	
	UFUNCTION()
	virtual void OnMouseActionDoubleTap(int32 bDoubleTapType)PURE_VIRTUAL(OnMouseActionDoubleTap,);
	
	UFUNCTION()
	virtual void OnMouseActionTap()PURE_VIRTUAL(OnMouseActionTap,);
	//	hold
	UFUNCTION()
	virtual void OnMouseActionHoldStart()PURE_VIRTUAL(OnMouseActionHoldStart,);
	UFUNCTION()
	virtual void OnMouseActionHolding()PURE_VIRTUAL(OnMouseActionHolding,);
	UFUNCTION()
	virtual void OnMouseActionHoldEnd()PURE_VIRTUAL(OnMouseActionHoldEnd,);
	
	virtual bool IsAttachedToActor(ITIHHsCommonObject* requester)PURE_VIRTUAL(IsAttachedToActor, return false;);
	
	
};

UINTERFACE()
class UTIHHsComponentObject : public UInterface
{
	GENERATED_BODY()
};

class ITIHHsComponentObject
{
	GENERATED_BODY()
public:
	virtual void justTest(){};
	
};

UINTERFACE()
class UTIHHsActorObject : public UInterface
{
	GENERATED_BODY()
};

class ITIHHsActorObject
{
	GENERATED_BODY()
public:
	virtual void justTestActor(){};
};

UCLASS()
class UTIHHsBaseComponent : public USceneComponent, public ITIHHsCommonObject, public ITIHHsComponentObject
{
	GENERATED_BODY()
public:
	
};

UCLASS()
class ATIHHsBaseStuff : public AActor, public ITIHHsCommonObject, public ITIHHsActorObject
{
	GENERATED_BODY()
public:
	
};

UCLASS()
class ATIHHsDefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
};

UCLASS()
class ATIHHsStartPawn : public APawn
{
	GENERATED_BODY()
public:
};

UCLASS()
class ATIHHsGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
};

USTRUCT()
struct FTIHHsHousingNode
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 NodeID;
};

USTRUCT()
struct FTIHHsHousingScene
{
	GENERATED_BODY()
	
};

UCLASS()
class TIHHOUSINGCORE_API ATIHHsTestActor : public AActor, public ITIHHsCommonObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATIHHsTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnMouseActionHoverEnter() override;
	virtual void OnMouseActionHovering() override;
	virtual void OnMouseActionHoverExit() override;
};
UCLASS()
class TIHHOUSINGCORE_API UTIHHsTriggerComponent : public UActorComponent, public ITIHHsCommonObject, public ITIHHsComponentObject
{
	GENERATED_BODY()
public:
	
};
