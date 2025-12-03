// Fill out your copyright notice in the Description page of Project Settings.


#include "Prefabs/TIHHsPrefabCore.h"

#include "Engine/AssetManager.h"


void UTIHHsGizmoTriggerPrefabSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTIHHsGizmoTriggerPrefabSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	PrecachePrefabsSync();
}

void UTIHHsGizmoTriggerPrefabSubsystem::OnWorldComponentsUpdated(UWorld& World)
{
	Super::OnWorldComponentsUpdated(World);
}

UTIHHsGizmoTriggerPrefabDataAsset* UTIHHsGizmoTriggerPrefabSubsystem::LoadPrefabSync(const FPrimaryAssetId& prefabId)
{
	if (not prefabId.IsValid())
	{
		return nullptr;
	}
	if (TObjectPtr< UTIHHsGizmoTriggerPrefabDataAsset>* foundPrefab = mCachedPrefabDataAssets.Find(prefabId))
	{
		if (IsValid(foundPrefab->Get()))
		{
			return foundPrefab->Get();
		}
		mCachedPrefabDataAssets.Remove(prefabId);
	}

	UAssetManager& assetManager = UAssetManager::Get();
	assetManager.LoadPrimaryAsset(prefabId);

	if (UObject* asset = assetManager.GetPrimaryAssetObject(prefabId))
	{
		if (UTIHHsGizmoTriggerPrefabDataAsset* prefabAsset = Cast<UTIHHsGizmoTriggerPrefabDataAsset>(asset))
		{
			mCachedPrefabDataAssets.Add(prefabId, prefabAsset);
			mPrefabNameToIdMap.Add(prefabAsset->PrefabThema, prefabId);
			auto& cachedTable = mPrefabNodeCache.Add(prefabId);
			for (TObjectPtr<UTIHHsGizmoTriggerPrefabNode> prefab :prefabAsset->Prefabs)
			{
				if (prefab!= nullptr)
				{
					cachedTable.Add(prefab->Name, prefab);
				}
			}
			return prefabAsset;
		}
	}
	
	return nullptr;
}

void UTIHHsGizmoTriggerPrefabSubsystem::LoadPrefabAsync(const FPrimaryAssetId& prefabId,
	 FTIHHsGizmoTriggerLoadDelegate onLoaded)
{
	if (not prefabId.IsValid())
	{
		onLoaded.Execute(nullptr);
		//onLoaded.Broadcast(nullptr);
		return;
	}
	if (TObjectPtr<UTIHHsGizmoTriggerPrefabDataAsset>* foundPrefab = mCachedPrefabDataAssets.Find(prefabId))
	{
		if (IsValid(foundPrefab->Get()))
		{
			onLoaded.Execute(foundPrefab->Get());
			//onLoaded.Broadcast(foundPrefab->Get());
			return;
		}
		mCachedPrefabDataAssets.Remove(prefabId);
	}
	UAssetManager& assetManager = UAssetManager::Get();
	assetManager.LoadPrimaryAsset(prefabId, {},FStreamableDelegate::CreateLambda(
		[
			weakThis = TWeakObjectPtr<UTIHHsGizmoTriggerPrefabSubsystem>(this),
			id = prefabId,
			loadCallback = onLoaded
			]()
		{
			if (not weakThis.IsValid())
			{
				loadCallback.Execute(nullptr);
				//loadCallback.Broadcast(nullptr);
				return;
			}

			UAssetManager& assetManager = UAssetManager::Get();
			if (UObject* obj = assetManager.GetPrimaryAssetObject(id))
			{
				if (auto* asset = Cast<UTIHHsGizmoTriggerPrefabDataAsset>(obj))
				{
					weakThis->mCachedPrefabDataAssets.Add(id, asset);
					loadCallback.Execute(asset);
					return;
				}
			}
			loadCallback.Execute(nullptr);
		}));
}

void UTIHHsGizmoTriggerPrefabSubsystem::LoadPrefabASyncOnBroadcast(const FPrimaryAssetId& prefabId)
{
	if (not prefabId.IsValid())
	{
		OnPrefabLoaded.Broadcast(nullptr);
		//onLoaded.Execute(nullptr);
		//onLoaded.Broadcast(nullptr);
		return;
	}
	if (TObjectPtr<UTIHHsGizmoTriggerPrefabDataAsset>* foundPrefab = mCachedPrefabDataAssets.Find(prefabId))
	{
		if (IsValid(foundPrefab->Get()))
		{
			OnPrefabLoaded.Broadcast(foundPrefab->Get());
			//onLoaded.Broadcast(foundPrefab->Get());
			return;
		}
		mCachedPrefabDataAssets.Remove(prefabId);
	}
	UAssetManager& assetManager = UAssetManager::Get();
	assetManager.LoadPrimaryAsset(prefabId, {},FStreamableDelegate::CreateLambda(
		[
			weakThis = TWeakObjectPtr<UTIHHsGizmoTriggerPrefabSubsystem>(this),
			id = prefabId
			]()
		{
			if (not weakThis.IsValid())
			{
				weakThis->OnPrefabLoaded.Broadcast(nullptr);
				//loadCallback.Broadcast(nullptr);
				return;
			}

			UAssetManager& assetManager = UAssetManager::Get();
			if (UObject* obj = assetManager.GetPrimaryAssetObject(id))
			{
				if (auto* asset = Cast<UTIHHsGizmoTriggerPrefabDataAsset>(obj))
				{
					weakThis->mCachedPrefabDataAssets.Add(id, asset);
					weakThis->OnPrefabLoaded.Broadcast(asset);
					return;
				}
			}
			weakThis->OnPrefabLoaded.Broadcast(nullptr);
		}));
}


void UTIHHsGizmoTriggerPrefabSubsystem::SortPrefabThemaData()
{
	/*
	 * 이거의 목적이 뭐야. 목적만 생각해. 에셋데이터 이름들을 정리해주는거 아니야?
	 */

}

void UTIHHsGizmoTriggerPrefabSubsystem::PrecachePrefabsSync()
{
	//	데이터 에셋에서 모든 놈들을 불러다가 캐싱시키는거임.
	//	TODO:그래서 이놈을 위한 테마 테이블도 필요함.
	UAssetManager& assetMgr = UAssetManager::Get();
	
	TArray<FPrimaryAssetId> allPrefabs;
	assetMgr.GetPrimaryAssetIdList(GetPrefabAssetType(), allPrefabs);
	
	for (const FPrimaryAssetId& prefabId : allPrefabs)
	{
		UObject* obj = assetMgr.GetPrimaryAssetPath(prefabId).TryLoad();
		if (auto* castedAsset = Cast<UTIHHsGizmoTriggerPrefabDataAsset>(obj))
		{
			StorePrefabDataAsset(prefabId, castedAsset);
		}
	}
}

void UTIHHsGizmoTriggerPrefabSubsystem::ProcessInstanceGizmoTriggerByVisitor(AActor* requestActor, const FTIHHsTriggerVisitor& completedVisitor)
{
	TArray<UObject*> prefabs;
	prefabs.Reserve(completedVisitor.VisitorNodes.Num());
	for (const FTIHHsGizmoTriggerSpawnData& spawnData :completedVisitor.VisitorNodes)
	{
		if (UObject* newPrefab = InstancePrefab(requestActor, spawnData))
		{
			prefabs.Add(newPrefab);
		}
	}
}

UObject* UTIHHsGizmoTriggerPrefabSubsystem::InstancePrefab(AActor* requestActor,const FTIHHsGizmoTriggerSpawnData& spawnData )
{
	UObject* result = nullptr;
	if (spawnData.CustomGenerator != nullptr)
	{
		result = spawnData.CustomGenerator->GenerateGizmoTrigger(spawnData);
	}
	else
	{
		if (FTIHHsGizmoTriggerPrefabThemaData* themaData = TryGetPrefabThemaData( spawnData.ThemaName))
		{
			if (UTIHHsGizmoTriggerPrefabNode* srcPrefabData = themaData->TryGetizmoTriggerPrefabNode(spawnData.SpawnBody.GizmoName))
			{
				
				/*
				 *	srcPrefabData 이거 불러왔을때 글로벌 룰을 상속받는 노드였는지 확인.
				 *	그런 노드였다면 가져올때 SpawnRule 을 복사
				 *	이거도 옵션으로 해서 bool 로 할까? 아님 비트플래그? 글로벌 룰을 전부 가져오는지 아닌지 선택
				 * 
				 */
				FTIHHsGizmoTriggerSpawnRule finalSpawnRule = srcPrefabData->SpawnRule;
				FTIHHsGizmoTriggerSpawnBody finalSpawnBody = srcPrefabData->GetGizmoTriggerSpawnBodyConditioned(themaData);
				
				FTIHHsGizmoTriggerSpawnRule::OverrideRule(ToFlags<ETIHHsGizmoTriggerOverrideOptType>(spawnData.OverrideOptions), spawnData.SpawnRule, finalSpawnRule);
				FTIHHsGizmoTriggerSpawnBody::OverrideBody(ToFlags<ETIHHsGizmoTriggerOverrideOptType>(spawnData.OverrideOptions), spawnData.SpawnBody, finalSpawnBody);

				switch (finalSpawnRule.LocationTransformRule) {
				case ETIHHsGizmoTriggerSpawnTransformRule::EIdentity:
					finalSpawnBody.RelativeTransform = FTransform::Identity;
					break;
				case ETIHHsGizmoTriggerSpawnTransformRule::EReplace:
					finalSpawnBody.RelativeTransform = spawnData.SpawnBody.RelativeTransform;
					break;
				case ETIHHsGizmoTriggerSpawnTransformRule::EMultiply:
					finalSpawnBody.RelativeTransform =  spawnData.SpawnBody.RelativeTransform * finalSpawnBody.RelativeTransform ;
					break;
				case ETIHHsGizmoTriggerSpawnTransformRule::EAdd:
					finalSpawnBody.RelativeTransform =  spawnData.SpawnBody.RelativeTransform + finalSpawnBody.RelativeTransform ;
					break;
				case ETIHHsGizmoTriggerSpawnTransformRule::EPrefab:
				case ETIHHsGizmoTriggerSpawnTransformRule::ENone:
					break;
				}

				if (ITIHHsBaseObject* baseObj = Cast<ITIHHsBaseObject>(spawnData.OwnerInteraction.GetObject()))
				{
					finalSpawnBody.RelativeTransform = baseObj->CalculateRelativeTransform(finalSpawnBody.RelativeTransform);
				}
				if (UClass* cls = spawnData.SpawnBody.SpawnTargetCls.LoadSynchronous())
				{
					if (spawnData.OwnerInteraction.GetObject() != nullptr)
					{
						result = NewObject<UObject>(spawnData.OwnerInteraction.GetObject(),cls);
						spawnData.OnSpawnComplete.Execute(spawnData,requestActor,result);
						spawnData.OwnerInteraction->AttachGizmoTrigger(result,finalSpawnRule,finalSpawnBody);
					}
				}
			}
		}
	} 
	return result;
}


//
// FTIHHsPrefabData FTIHHsPrefabSerializer::SerializeActorHierarchy(TArray<AActor*> actors)
// {
// 	FTIHHsPrefabData Result;
// 	TMap<AActor*, int32> ActorToIndex;
// 	
// 	// 액터 수집
// 	TArray<AActor*> AllActors = actors;
//
// 	// 각 액터와 컴포넌트 직렬화
// 	for (int32 i = 0; i < AllActors.Num(); i++)
// 	{
// 		AActor* Actor = AllActors[i];
// 		ActorToIndex.Add(Actor, i);
//
// 		FTIHHsPrefabActorData ActorData;
// 		ActorData.ActorClass = Actor->GetClass();
// 		ActorData.ActorName = Actor->GetName();
//             
// 		// 트랜스폼
// 		if (Actor->GetAttachParentActor())
// 		{
// 			ActorData.RelativeTransform = Actor->GetActorTransform().GetRelativeTransform(
// 				Actor->GetAttachParentActor()->GetActorTransform()
// 			);
// 		}
// 		else
// 		{
// 			ActorData.RelativeTransform = Actor->GetActorTransform();
// 		}
//
// 		// 액터 프로퍼티
// 		ActorData.SerializedActorProperties = SerializeObjectProperties(Actor);
//             
// 		// ⭐ 컴포넌트들 직렬화
// 		SerializeComponents(Actor, ActorData.Components);
//             
// 		Result.Actors.Add(ActorData);
// 	}
//
// 	// 액터 간 부모-자식 관계
// 	for (int32 i = 0; i < AllActors.Num(); i++)
// 	{
// 		AActor* Actor = AllActors[i];
// 		AActor* Parent = Actor->GetAttachParentActor();
//             
// 		if (Parent && ActorToIndex.Contains(Parent))
// 		{
// 			int32 ParentIdx = ActorToIndex[Parent];
// 			Result.Actors[i].ParentActorIndex = ParentIdx;
// 			Result.Actors[ParentIdx].ChildActorIndices.Add(i);
// 		}
// 	}
//
// 	Result.RootActorIndex = 0;
// 	return Result;
// }
//
// void FTIHHsPrefabSerializer::SerializeComponents(AActor* Actor, TArray<FTIHHsPrefabComponentData>& OutComponents)
// {
// 	TArray<UActorComponent*> AllComponents = Actor->GetComponents().Array();
// 	TMap<UActorComponent*, int32> ComponentToIndex;
//         
// 	// 모든 컴포넌트 직렬화
// 	for (int32 i = 0; i < AllComponents.Num(); i++)
// 	{
// 		UActorComponent* Component = AllComponents[i];
// 		ComponentToIndex.Add(Component, i);
//             
// 		FTIHHsPrefabComponentData CompData;
// 		CompData.ComponentClass = Component->GetClass();
// 		CompData.ComponentName = Component->GetName();
// 		CompData.SerializedProperties = SerializeObjectProperties(Component);
//             
// 		// SceneComponent 처리
// 		if (USceneComponent* SceneComp = Cast<USceneComponent>(Component))
// 		{
// 			CompData.bIsSceneComponent = true;
// 			CompData.RelativeTransform = SceneComp->GetRelativeTransform();
// 			CompData.bIsRootComponent = (SceneComp == Actor->GetRootComponent());
// 		}
//             
// 		OutComponents.Add(CompData);
// 	}
//         
// 	// 컴포넌트 간 계층 관계 설정
// 	for (int32 i = 0; i < AllComponents.Num(); i++)
// 	{
// 		if (USceneComponent* SceneComp = Cast<USceneComponent>(AllComponents[i]))
// 		{
// 			// 부모 컴포넌트 찾기
// 			USceneComponent* ParentComp = SceneComp->GetAttachParent();
// 			if (ParentComp && ComponentToIndex.Contains(ParentComp))
// 			{
// 				int32 ParentIdx = ComponentToIndex[ParentComp];
// 				OutComponents[i].ParentComponentIndex = ParentIdx;
// 				OutComponents[ParentIdx].ChildComponentIndices.Add(i);
// 			}
//                 
// 			// 자식 컴포넌트들 찾기
// 			TArray<USceneComponent*> ChildComps = SceneComp->GetAttachChildren();
// 			for (USceneComponent* Child : ChildComps)
// 			{
// 				if (ComponentToIndex.Contains(Child))
// 				{
// 					OutComponents[i].ChildComponentIndices.Add(ComponentToIndex[Child]);
// 				}
// 			}
// 		}
// 	}
// }
//
// TArray<uint8> FTIHHsPrefabSerializer::SerializeObjectProperties(UObject* Object)
// {
// 	TArray<uint8> Data;
// 	FMemoryWriter Writer(Data);
// 	FObjectAndNameAsStringProxyArchive Archive(Writer, false);
//         
// 	// 컴포넌트 참조는 제외하고 직렬화
// 	Object->Serialize(Archive);
//         
// 	return Data;
// }
//
// AActor* FTIHHsPrefabInstantiator::InstantiatePrefab(UWorld* World, const FTIHHsPrefabData& PrefabData,
// 	const FTransform& WorldTransform)
// {
// 	if (PrefabData.Actors.Num() == 0) return nullptr;
//
// 	TArray<AActor*> CreatedActors;
// 	CreatedActors.SetNum(PrefabData.Actors.Num());
//
// 	// 1단계: 액터 생성 및 컴포넌트 복원
// 	for (int32 i = 0; i < PrefabData.Actors.Num(); i++)
// 	{
// 		const FTIHHsPrefabActorData& ActorData = PrefabData.Actors[i];
//             
// 		// 액터 생성 (기본 컴포넌트만 있는 상태)
// 		FActorSpawnParameters SpawnParams;
// 		SpawnParams.SpawnCollisionHandlingOverride = 
// 			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//             
// 		AActor* NewActor = World->SpawnActor<AActor>(
// 			ActorData.ActorClass.Get(),
// 			FTransform::Identity,
// 			SpawnParams
// 		);
//
// 		if (NewActor)
// 		{
// 			// 액터 프로퍼티 복원
// 			DeserializeObjectProperties(NewActor, ActorData.SerializedActorProperties);
//                 
// 			// ⭐ 컴포넌트 복원
// 			RestoreComponents(NewActor, ActorData.Components);
//                 
// 			CreatedActors[i] = NewActor;
// 		}
// 	}
//
// 	// 2단계: 액터 계층 복원
// 	for (int32 i = 0; i < PrefabData.Actors.Num(); i++)
// 	{
// 		const FTIHHsPrefabActorData& ActorData = PrefabData.Actors[i];
// 		AActor* Actor = CreatedActors[i];
//             
// 		if (!Actor) continue;
//
// 		if (ActorData.ParentActorIndex >= 0)
// 		{
// 			AActor* ParentActor = CreatedActors[ActorData.ParentActorIndex];
// 			if (ParentActor)
// 			{
// 				Actor->AttachToActor(ParentActor, 
// 				                     FAttachmentTransformRules::KeepRelativeTransform);
// 			}
// 		}
//
// 		Actor->SetActorRelativeTransform(ActorData.RelativeTransform);
// 	}
//
// 	// 루트 액터에 월드 트랜스폼 적용
// 	if (PrefabData.RootActorIndex >= 0)
// 	{
// 		AActor* RootActor = CreatedActors[PrefabData.RootActorIndex];
// 		if (RootActor)
// 		{
// 			RootActor->SetActorTransform(WorldTransform);
// 			return RootActor;
// 		}
// 	}
//
// 	return nullptr;
// }
//
// void FTIHHsPrefabInstantiator::RestoreComponents(AActor* Actor, const TArray<FTIHHsPrefabComponentData>& ComponentsData)
// {
// 	TArray<UActorComponent*> RestoredComponents;
// 	RestoredComponents.SetNum(ComponentsData.Num());
//         
// 	// 1. 먼저 모든 컴포넌트 생성
// 	for (int32 i = 0; i < ComponentsData.Num(); i++)
// 	{
// 		const FTIHHsPrefabComponentData& CompData = ComponentsData[i];
//             
// 		// 이미 존재하는 컴포넌트인지 확인 (기본 컴포넌트)
// 		UActorComponent* ExistingComp = Actor->FindComponentByClass(
// 			CompData.ComponentClass.Get()
// 		);
//             
// 		UActorComponent* Component = nullptr;
//             
// 		if (ExistingComp && CompData.bIsRootComponent)
// 		{
// 			// RootComponent는 이미 있을 수 있음
// 			Component = ExistingComp;
// 		}
// 		else
// 		{
// 			// 새 컴포넌트 생성
// 			Component = NewObject<UActorComponent>(
// 				Actor, 
// 				CompData.ComponentClass.Get(),
// 				*CompData.ComponentName
// 			);
//                 
// 			if (Component)
// 			{
// 				Component->RegisterComponent();
// 				Actor->AddInstanceComponent(Component);
// 			}
// 		}
//             
// 		if (Component)
// 		{
// 			// 프로퍼티 복원
// 			DeserializeObjectProperties(Component, CompData.SerializedProperties);
// 			RestoredComponents[i] = Component;
// 		}
// 	}
//         
// 	// 2. 컴포넌트 계층 구조 복원
// 	for (int32 i = 0; i < ComponentsData.Num(); i++)
// 	{
// 		const FTIHHsPrefabComponentData& CompData = ComponentsData[i];
//             
// 		if (!CompData.bIsSceneComponent) continue;
//             
// 		USceneComponent* SceneComp = Cast<USceneComponent>(RestoredComponents[i]);
// 		if (!SceneComp) continue;
//             
// 		// RootComponent 설정
// 		if (CompData.bIsRootComponent)
// 		{
// 			Actor->SetRootComponent(SceneComp);
// 		}
//             
// 		// 부모-자식 관계 설정
// 		if (CompData.ParentComponentIndex >= 0)
// 		{
// 			USceneComponent* ParentComp = Cast<USceneComponent>(
// 				RestoredComponents[CompData.ParentComponentIndex]
// 			);
// 			if (ParentComp)
// 			{
// 				SceneComp->SetupAttachment(ParentComp);
// 			}
// 		}
//             
// 		// 트랜스폼 적용
// 		SceneComp->SetRelativeTransform(CompData.RelativeTransform);
// 	}
//         
// 	// 3. 컴포넌트 재등록 (물리, 렌더링 업데이트)
// 	Actor->ReregisterAllComponents();
// }
//
// void FTIHHsPrefabInstantiator::DeserializeObjectProperties(UObject* Object, const TArray<uint8>& Data)
// {
// 	if (Data.Num() == 0) return;
//         
// 	FMemoryReader Reader(Data);
// 	FObjectAndNameAsStringProxyArchive Archive(Reader, false);
// 	Object->Serialize(Archive);
// }
