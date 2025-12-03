// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "Subsystems/WorldSubsystem.h"
#include "TIHHsCore.h"
#include "TIHHsSQLiteHelper.h"
#include "TIHHsHousingStuffSystem.generated.h"


class UTIHHsSQLiteHelper;
class UTIHHsPlacementComponent;
class UTIHHsTriggerComponent;
class UTIHHsMetaDataComponent;
struct FInstancedStruct;

USTRUCT(Blueprintable, BlueprintType)
struct FMooAnDanWater
{
	GENERATED_BODY()

	UPROPERTY()
	FInstancedStruct Water;
	UPROPERTY()
	int32 ReturnType;
};

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam( FMooAnDanWater,FTIHHsMooAnDanWater,FInstancedStruct,Water);

/**
 * 
 */
class UTIHHsHousingStuffSystem;
UCLASS()
class ATIHHsHousingStuffSystemRootActor : public AActor
{
	GENERATED_BODY()
	friend class UTIHHsHousingStuffSystem;
public:

private:
	TArray<AActor*> mManagedActors;
	TArray<AActor*> mSystemActors;
};

USTRUCT()
struct FTIHHsStuffNode
{
	GENERATED_BODY()

	FTransform RelateTransform;

	TWeakPtr<FTIHHsStuffNode> ParentNode;
};


USTRUCT()
struct FTIHHsHousingAttach
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Parent;

	UPROPERTY()
	TObjectPtr<AActor> Child;
};

USTRUCT(BlueprintType)
struct FTIHHsComponentSpawnInfo
{
	GENERATED_BODY()

	// 스폰할 컴포넌트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Prefab")
	TSubclassOf<UActorComponent> ComponentClass;

	// 액터에 붙일 상대 트랜스폼 (SceneComponent 상속 시 이용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Prefab")
	FTransform RelativeTransform;

	// 프로퍼티 오버라이드를 위한 JSON 직렬화 문자열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Prefab")
	FString PropertyJson;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Prefab")
	TArray<FName> Tags;
};

// 2) 프리팹 전체 정의
USTRUCT(BlueprintType)
struct FTIHHsPrefabDefinition
{
	GENERATED_BODY()

	// 프리팹 식별 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Prefab")
	FName Name;

	// Spawn할 액터 클래스 (기본은 ATIHHsBaseObject 계열)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Prefab")
	TSubclassOf<AActor> ActorClass;

	// 액터에 붙일 컴포넌트들 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Prefab")
	TArray<FTIHHsComponentSpawnInfo> Components;
};

USTRUCT(BlueprintType)
struct FTIHHsObjectGroup
{
	GENERATED_BODY()
	FTIHHsObjectGroup()= default;
	FTIHHsObjectGroup(const FTIHHsObjectGroup& other) = default;
	FTIHHsObjectGroup(FTIHHsObjectGroup&& other) noexcept = default;
	
	
	UPROPERTY()
	TArray<TScriptInterface< ITIHHsBaseObject>> Objects;

	FTIHHsObjectGroup& operator=(const FTIHHsObjectGroup& other)
	{
		if (this != &other)
		{
			Objects = other.Objects;
		}
		return *this;
	}
	FTIHHsObjectGroup& operator=(FTIHHsObjectGroup&& other) noexcept
	{
		if (this != &other)
		{
			Objects = MoveTemp(other.Objects);
		}
		return *this;
	}
};

USTRUCT()
struct FTIHHsHousingStuffSystemConfigure
{
	GENERATED_BODY()

	UPROPERTY()
	FString SaveDirDBName = TEXT("prefab.db");

	UPROPERTY()
	FTransform DefaultSpawnTransform;
};

USTRUCT(BlueprintType)
struct FTIHHsStuffPostInitFuncHandler
{
	GENERATED_BODY()
	
	UPROPERTY()
	int32 Order = 0;	
	
	TFunction<void(UTIHHsHousingStuffSystem*,TArray<TScriptInterface<ITIHHsBaseObject>>&)> Func;
};


UCLASS()
class TIHHOUSINGCORE_API UTIHHsHousingStuffSystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	void CreateDatabaseIfNotExists(const FString&, FString& prefabDBPath);
	bool InitializeDatabase();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void EnsureDefinitionsTable();
	void LoadDefinitionsToMemory();

	void EnsureComponentTagsTable();
	void LoadComponentTagsToMemory();
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UFUNCTION(BlueprintCallable, Category="TIHH|Prefab")
	FTIHHsPrefabDefinition CaptureActorAsPrefab(AActor* SourceActor);
	
	//UFUNCTION(BlueprintCallable, Category = "TIHHsHousingStuffSystem|Create",meta=(WorldContext="WorldContextObject"))
	//static AActor* CreateTIHObj(UObject* worldContextObject, const FTransform& transform);
	//UFUNCTION(BlueprintCallable, Category = "TIHHsHousingStuffSystem|Create",meta=(WorldContext="WorldContextObject"))
	//static void DeleteTIHObj(UObject* worldContextObject, AActor* target);

	//UFUNCTION(BlueprintCallable, Category = "TIHHsHousingStuffSystem|Create",meta=(WorldContext="WorldContextObject"))
	//static FTIHHsObjectGroup SpawnPrefabTIHObj(UObject* worldContextObject, FName prefabName, const FTransform& transform);
	
	//	Hierarchy
	//UFUNCTION(BlueprintCallable, Category = "TIHHsHousingStuffSystem|Hierarchy",meta=(WorldContext="WorldContextObject"))
	//static void AttachTIHObjToTIHObj(UObject* worldContextObject, AActor* parent, AActor* child);
	//UFUNCTION(BlueprintCallable, Category = "TIHHsHousingStuffSystem|Hierarchy",meta=(WorldContext="WorldContextObject"))
	//static void DetachTIHObjFromTIHObj(UObject* worldContextObject, AActor* parent, AActor* child);

	/*
	 *	게임이 시작하고나서 그녀석들이 호출되어야하는데 어떻게 해야할까?
	 *	런타임 체인으로 만들자
	 *	델리게이트로 먼저 월드의 전체 액터들을 들고온다.
	 *	람다 연결해서 특정 키워드에 반응해서 처리하는 함수를 등록할 수 있게 만들자.
	 *	트리거나 다른것들도 마찬가지로 말이야.
	 * 
	 */
	void RegisterPostInitializeFunction(const FName& name, FTIHHsStuffPostInitFuncHandler postInitFuncHandler)
	{
		if (not mPostInitFuncsTable.Contains(name))
		{
			if (postInitFuncHandler.Func)
			{
				mPostInitFuncsTable.Add(name, postInitFuncHandler);
			
				mPostInitFuncsTable.ValueSort([](const FTIHHsStuffPostInitFuncHandler& A, const FTIHHsStuffPostInitFuncHandler& B) {
					return A.Order < B.Order;
				});
			}
		}
	}
	
private:
	FTIHHsHousingStuffSystemConfigure mConfigure;
	TStrongObjectPtr<UTIHHsSQLiteHelper> mSQLiteHelper;
	TMap<FName, FTIHHsPrefabDefinition> mPrefabTable;

	UWorld* mWorld;
	TMap<FName,FTIHHsStuffPostInitFuncHandler> mPostInitFuncsTable;
	TArray<TScriptInterface<ITIHHsBaseObject>> mRegistedBaseObjects;
	TMap<FName,TArray<TScriptInterface<ITIHHsBaseObject>>> mRegistedBaseObjectsByTag;	//	태그별로 등록된 액터들
};
