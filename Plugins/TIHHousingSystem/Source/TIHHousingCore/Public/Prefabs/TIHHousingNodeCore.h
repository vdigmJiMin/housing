// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Cores/TIHHsCoreClasses.h"
#include "UObject/Object.h"
#include "TIHHousingNodeCore.generated.h"

class UTHsNode_Dep;
class UTHsNodeAttachPoint;

/*
 *	시발...보류
 */
UENUM(BlueprintType)
enum class ETHsNodeType_Dep : uint8
{
	EUNDEFINED			= 0		UMETA(DisplayName="Undefined"),
	EAttachableSurface	= 1<<0	UMETA(DisplayName="AttachableSurface"),
	EAttachmentPoint	= 1<<1	UMETA(DisplayName="AttachmentPoint"),
	ESocket				= 1<<2	UMETA(DisplayName="Socket"),
	EInteractiveGizmo	= 1<<3	UMETA(DisplayName="InteractiveGizmo"),
	EFunction 			= 1<<4	UMETA(DisplayName="Function"),
	ESurfaceRule		= 1<<5	UMETA(DisplayName="SurfaceRule"),
};

UINTERFACE()
class UTHsNodeFunctions : public UInterface
{
	GENERATED_BODY()
};

class ITHsNodeFunctions
{
	GENERATED_BODY()

public:
	
};


UCLASS()
class UTHsNode_Dep : public UObject,public ITHsNodeFunctions
{
	GENERATED_BODY()

public:
	
};


USTRUCT(BlueprintType)
struct FTHsNodePrefabData_Dep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	ETHsNodeType_Dep NodeType = ETHsNodeType_Dep::EUNDEFINED;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	FName NodeName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	FText NodeDisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	FText NodeDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	FGameplayTagContainer NodeTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	TSubclassOf<class UTHsNode_Dep> NodeClass = nullptr;
};


UCLASS()
class UTHsNodePrefab_Dep : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prefabs")
	FTHsNodePrefabData_Dep PrefabData;
};


UCLASS()
class TIHHOUSINGCORE_API ATIHHousingNodeCore : public AActor
{
	GENERATED_BODY()
public:
	
};

/*
 *	Interactive
 *		
 */
