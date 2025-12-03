// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UObject/Object.h"
#include "Cores/TIHHsCoreEnums.h"
#include "Cores/TIHHsCoreStructures.h"
#include "Cores/TIHHsCoreInterface.h"

#include "Subsystems/WorldSubsystem.h"
#include "TIHHsPrefabCore.generated.h"

/**
 * 
 */
UCLASS()
class TIHHOUSINGCORE_API UTIHHsPrefabCore : public UObject
{
	GENERATED_BODY()
};

// // 컴포넌트 데이터
// USTRUCT(BlueprintType)
// struct FTIHHsPrefabComponentData
// {
// 	GENERATED_BODY()
//
// 	// 컴포넌트 클래스
// 	UPROPERTY()
// 	TSubclassOf<UActorComponent> ComponentClass;
//     
// 	// 컴포넌트 이름 (액터 내에서 유니크)
// 	UPROPERTY()
// 	FString ComponentName;
//     
// 	// SceneComponent인 경우 트랜스폼
// 	UPROPERTY()
// 	FTransform RelativeTransform;
//     
// 	// 부모 컴포넌트 인덱스 (-1이면 RootComponent)
// 	UPROPERTY()
// 	int32 ParentComponentIndex = -1;
//     
// 	// 자식 컴포넌트 인덱스들
// 	UPROPERTY()
// 	TArray<int32> ChildComponentIndices;
//     
// 	// 직렬화된 프로퍼티
// 	UPROPERTY()
// 	TArray<uint8> SerializedProperties;
//     
// 	// SceneComponent인지 여부
// 	UPROPERTY()
// 	bool bIsSceneComponent = false;
//     
// 	// RootComponent인지 여부
// 	UPROPERTY()
// 	bool bIsRootComponent = false;
// };
//
// // 개선된 액터 데이터
// USTRUCT(BlueprintType)
// struct FTIHHsPrefabActorData
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY()
// 	TSubclassOf<AActor> ActorClass;
//     
// 	UPROPERTY()
// 	FTransform RelativeTransform;
//     
// 	// 액터의 프로퍼티 (컴포넌트 제외)
// 	UPROPERTY()
// 	TArray<uint8> SerializedActorProperties;
//     
// 	// ⭐ 이 액터의 모든 컴포넌트들
// 	UPROPERTY()
// 	TArray<FTIHHsPrefabComponentData> Components;
//     
// 	// 액터 계층
// 	UPROPERTY()
// 	TArray<int32> ChildActorIndices;
//     
// 	UPROPERTY()
// 	int32 ParentActorIndex = -1;
//     
// 	UPROPERTY()
// 	FString ActorName;
// };
//
// // 프리팹 전체 데이터
// USTRUCT(BlueprintType)
// struct FTIHHsPrefabData
// {
// 	GENERATED_BODY()
//
// 	// 모든 액터 데이터 (플랫 배열)
// 	UPROPERTY()
// 	TArray<FTIHHsPrefabActorData> Actors;
//
// 	// 루트 액터 인덱스
// 	UPROPERTY()
// 	int32 RootActorIndex = -1;
//
// 	// 프리팹 메타데이터
// 	UPROPERTY()
// 	FString PrefabName;
//
// 	UPROPERTY()
// 	FDateTime CreatedTime;
// };
//
// // 프리팹 데이터 에셋
// UCLASS(BlueprintType)
// class UTIHHsPrefabDataAsset : public UPrimaryDataAsset
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
// 	FTIHHsPrefabData PrefabData;
//
// 	// 프리팹 썸네일 (에디터용)
// 	UPROPERTY(EditAnywhere)
// 	UTexture2D* ThumbnailImage;
//
// 	// 카테고리/태그
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
// 	TArray<FString> Tags;
// };
//
//
// class FTIHHsPrefabSerializer
// {
// public:
//     static FTIHHsPrefabData SerializeActorHierarchy(TArray<AActor*> actors);
//
// private:
//     // ⭐ 컴포넌트 직렬화 핵심
//     static void SerializeComponents(AActor* Actor, TArray<FTIHHsPrefabComponentData>& OutComponents);
//
//     static TArray<uint8> SerializeObjectProperties(UObject* Object);
// };
//
// class FTIHHsPrefabInstantiator
// {
// public:
//     static AActor* InstantiatePrefab(UWorld* World, const FTIHHsPrefabData& PrefabData, 
//                                      const FTransform& WorldTransform);
// private:
//     // ⭐ 컴포넌트 복원 핵심
//     static void RestoreComponents(AActor* Actor, const TArray<FTIHHsPrefabComponentData>& ComponentsData);
//
//     static void DeserializeObjectProperties(UObject* Object, const TArray<uint8>& Data);
// };
//
// UCLASS()
// class UTIHHsPrefabWrapper : public UObject
// {
// 	GENERATED_BODY()
// public:
// 	UFUNCTION(BlueprintCallable, Category="TIHH|Prefab|Serializer")
// 	static FTIHHsPrefabData SerializeActorHierarchy(TArray<AActor*> actors)
// 	{
// 		return FTIHHsPrefabSerializer::SerializeActorHierarchy(actors);
// 	}
// 	
// 	UFUNCTION(BlueprintCallable, Category="TIHH|Prefab|Instantiator")
// 	static AActor* InstantiatePrefab(UWorld* World, const FTIHHsPrefabData& PrefabData,	 const FTransform& WorldTransform)
// 	{
// 		return FTIHHsPrefabInstantiator::InstantiatePrefab(World, PrefabData, WorldTransform);
// 	}
// };
UCLASS()
class TIHHOUSINGCORE_API UTIHHsGizmoTriggerPrefabSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static FPrimaryAssetType GetPrefabAssetType()
	{
		return TIHUtils::GetGizmoTriggerPrefabType();
	}
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void OnWorldComponentsUpdated(UWorld& World) override;

	UFUNCTION(BlueprintCallable)
	bool HasCachedPrefab(const FPrimaryAssetId& prefabId) const
	{
		return mCachedPrefabDataAssets.Contains(prefabId);
	}
	UFUNCTION(BlueprintCallable)
	UTIHHsGizmoTriggerPrefabDataAsset* GetCachedPrefab(const FPrimaryAssetId& prefabId) const
	{
		if (const TObjectPtr<UTIHHsGizmoTriggerPrefabDataAsset>* found = mCachedPrefabDataAssets.Find(prefabId))
		{
			return *found;
		}
		return nullptr;
	}
	UFUNCTION(BlueprintCallable,Category="loader")
	UTIHHsGizmoTriggerPrefabDataAsset* LoadPrefabSync(const FPrimaryAssetId& prefabId);
	
	UFUNCTION(BlueprintCallable,Category="loader")
	void LoadPrefabAsync(const FPrimaryAssetId& prefabId, FTIHHsGizmoTriggerLoadDelegate onLoaded);
	
	UFUNCTION(BlueprintCallable,Category="loader")
	static FPrimaryAssetId MakePrefabPrimaryAssetId(const FName& prefabName)
	{
		return FPrimaryAssetId(GetPrefabAssetType(), prefabName);
	}
	UFUNCTION(BlueprintCallable,Category="loader")
	static FPrimaryAssetId MakePrefabPrimaryAssetIdByString(const FString& prefabName)
	{
		return FPrimaryAssetId(GetPrefabAssetType(), FName(*prefabName));
	}

	UPROPERTY(BlueprintAssignable, Category="loader")
	FTIHHsGizmoTriggerLoadMulticastDelegate OnPrefabLoaded;
	void LoadPrefabASyncOnBroadcast(const FPrimaryAssetId& prefabId);

		
	UFUNCTION(BlueprintCallable,Category="loader")
	void StorePrefabDataAsset(const FPrimaryAssetId& prefabId, UTIHHsGizmoTriggerPrefabDataAsset* prefabAsset)
	{
		if (prefabAsset != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Storing prefab asset: %s"), *prefabId.ToString());
			mCachedPrefabDataAssets.Add(prefabId, prefabAsset);
			for (auto& i :prefabAsset->Prefabs)
			{
				i->SpawnBody.RefPrefabThema = prefabAsset;
			}
		}
	}
	//	for loader
	void SortPrefabThemaData();
	void PrecachePrefabsSync();
	//UFUNCTION(BlueprintCallable,Category="Instance")
	 //	원래 뭐하려고 했냐면, 어차피 트리거visitor의 노드는 FTIHHsGizmoTriggerSpawnData라서 FTIHHsGizmoTriggerSpawnData 이게 모든 컴포넌트를 방문하고 그 안에 유저나 개발자는 생성에 관한 내용과 생성된후의 처리를 한곳에서 설정할 수 있게 만드는게 목적임.
	UFUNCTION(BlueprintCallable,Category="Instance")
	void ProcessInstanceGizmoTriggerByVisitor(AActor* requestActor, const FTIHHsTriggerVisitor& completedVisitor);
	
	UFUNCTION(BlueprintCallable,Category="Instance")
	UObject* InstancePrefab(AActor* requestActor,const FTIHHsGizmoTriggerSpawnData& spawnData);
	
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Instance")
	FName CurrentPrefabThema = FName("Default");
	
	FTIHHsGizmoTriggerPrefabThemaData* TryGetPrefabThemaData(const FName& themaName)
	{
		if (themaName.IsNone())
			return nullptr;
		return mPrefabThemaMap.Find(themaName);
	}
	
	
private:
	//void PrecessGizmoTriggerOverrideOptType(const ETIHHsGizmoTriggerOverrideOptType& OverrideOptions,s) 
	
	//TObjectPtr<class ATIHHsGizmoTriggerPrefabManager> mPrefabManagerActor = nullptr;
	UPROPERTY(Transient)// precached
	TMap<FPrimaryAssetId, TObjectPtr<UTIHHsGizmoTriggerPrefabDataAsset>> mCachedPrefabDataAssets;
	TMap<FString,FPrimaryAssetId> mPrefabNameToIdMap;

	TMap<FPrimaryAssetId,TMap<FString,UTIHHsGizmoTriggerPrefabNode*> > mPrefabNodeCache; // prefabId -> (prefabName -> node)
	
	//	runtime
	using TIHGizmoPrefabThema = FName;
	FTIHHsGizmoTriggerPrefabThemaData* mCurrentPrefabThemaData = nullptr;

	//	이거 그냥 UTIHHsGizmoTriggerPrefabDataAsset 이거로 할까? 그렇게 가야할듯
	TMap<TIHGizmoPrefabThema,FTIHHsGizmoTriggerPrefabThemaData> mPrefabThemaMap;

	TStrongObjectPtr< ATIHHsGizmoTriggerProbeSpawner> mDefaultProbeSpawner = nullptr;
};