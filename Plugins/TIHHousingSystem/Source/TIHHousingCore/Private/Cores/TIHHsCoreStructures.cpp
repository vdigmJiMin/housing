// Fill out your copyright notice in the Description page of Project Settings.


#include "Cores/TIHHsCoreStructures.h"

#include "PropertyBag.h"
//#include "Cores/TIHHsCoreInterface.h"
#include "Engine/AssetManager.h"
#include "Runtime/Engine/Private/AsyncActionLoadPrimaryAsset.h"
#include "TIHComponents/TIHHsTriggerComponent.h"
#include "UObject/PropertyBag.h"

// 상호작용 계층
UE_DEFINE_GAMEPLAY_TAG(TIH_Interactable, "Interactable");
UE_DEFINE_GAMEPLAY_TAG(TIH_Interactable_Hover, "Interactable.Hover");
UE_DEFINE_GAMEPLAY_TAG(TIH_Interactable_Focus, "Interactable.Focus");
UE_DEFINE_GAMEPLAY_TAG(TIH_Interactable_Hold, "Interactable.Hold");
UE_DEFINE_GAMEPLAY_TAG(TIH_Interactable_Tap, "Interactable.Tap");
UE_DEFINE_GAMEPLAY_TAG(TIH_Interactable_DoubleTap, "Interactable.DoubleTap");

// 부착 계층
UE_DEFINE_GAMEPLAY_TAG(TIH_Attachable, "Attachable");
UE_DEFINE_GAMEPLAY_TAG(TIH_Attachable_Decked, "Attachable.Decked");
UE_DEFINE_GAMEPLAY_TAG(TIH_Attachable_Decked_Gridable, "Attachable.Decked.Gridable");
UE_DEFINE_GAMEPLAY_TAG(TIH_Attachable_Placed, "Attachable.Placed");

// 보장된 기능
UE_DEFINE_GAMEPLAY_TAG(TIH_Guarantee, "Guarantee");
UE_DEFINE_GAMEPLAY_TAG(TIH_Guarantee_HasInteraction, "Guarantee.HasInteraction");
UE_DEFINE_GAMEPLAY_TAG(TIH_Guarantee_HasIndividuality, "Guarantee.HasIndividuality");
UE_DEFINE_GAMEPLAY_TAG(TIH_Guarantee_MaintainedMetadata, "Guarantee.MaintainedMetadata");
UE_DEFINE_GAMEPLAY_TAG(TIH_Guarantee_HasCollision, "Guarantee.HasCollision");
UE_DEFINE_GAMEPLAY_TAG(TIH_Guarantee_HasDeck, "Guarantee.HasDeck");
UE_DEFINE_GAMEPLAY_TAG(TIH_Guarantee_HasPlacement, "Guarantee.HasPlacement");
UE_DEFINE_GAMEPLAY_TAG(TIH_Guarantee_HasTrigger, "Guarantee.HasTrigger");
UE_DEFINE_GAMEPLAY_TAG(TIH_Guarantee_HasStateTree, "Guarantee.HasStateTree");


// 정체성 관련
UE_DEFINE_GAMEPLAY_TAG(TIH_Classification, "Classification");
UE_DEFINE_GAMEPLAY_TAG(TIH_Classification_Stuff, "Classification.Stuff");
UE_DEFINE_GAMEPLAY_TAG(TIH_Classification_ForBuilding, "Classification.ForBuilding");
UE_DEFINE_GAMEPLAY_TAG(TIH_Classification_ForInterior, "Classification.ForInterior");
UE_DEFINE_GAMEPLAY_TAG(TIH_Classification_System, "Classification.System");


FTIHHsIndirectObject::FTIHHsIndirectObject(AActor* actor)
{
	FInstancedPropertyBag a;
	a.AddProperty(TEXT("Actor"),EPropertyBagPropertyType::Object,nullptr);
	FJsonObject b;
	b.SetBoolField(TEXT("snap"),false);
	b.SetBoolField(TEXT("check-collision"),false);
	b.SetBoolField(TEXT("check-filter"),false);
	TArray<TPair<FName,bool>> fields;
	fields.Add(TPair<FName,bool>(TEXT("snap"),true));
	
}

FTIHHsActionParamIn::FTIHHsActionParamIn(const FInstancedPropertyBag& propertyBag): ActionName(TEXT("")), ActionData(TEXT("")), PropertyBag(propertyBag)
{
	
}


FTIHHsGizmoTriggerSpawnData& FTIHHsGizmoTriggerSpawnData::SetOwnerInteraction(UObject* ownerObject)
{
	OwnerInteraction.SetObject(ownerObject);
	OwnerInteraction.SetInterface(Cast<ITIHHsInteraction>(ownerObject));
		
	return *this;
}

 FTIHHsGizmoTriggerSpawnBody UTIHHsGizmoTriggerPrefabNode::GetGizmoTriggerSpawnBodyConditioned(
	FTIHHsGizmoTriggerPrefabThemaData* themaData) const
 {
	FTIHHsGizmoTriggerSpawnBody result = SpawnBody;
	bool onCalculateTransfromOpt= EnumHasAnyFlags( ToFlags<ETIHHsGizmoTriggerOverrideOptType>(InheritGlobalRuleOptions), ETIHHsGizmoTriggerOverrideOptType::ECalculateTransformOption);
	
	if (themaData != nullptr
		&& InheritGlobalRule
		&& onCalculateTransfromOpt)
	{
		switch (InheritTransformRule) {
		case ETIHHsGizmoTriggerSpawnTransformRule::ENone:
		case ETIHHsGizmoTriggerSpawnTransformRule::EPrefab:
			break;
		case ETIHHsGizmoTriggerSpawnTransformRule::EIdentity:
			result.RelativeTransform = FTransform::Identity;
			break;
		case ETIHHsGizmoTriggerSpawnTransformRule::EReplace:
			if (themaData->DataAsset)
			{
				result.RelativeTransform = themaData->DataAsset->OffsetTransform;
			}
			break;
		case ETIHHsGizmoTriggerSpawnTransformRule::EMultiply:
			if (themaData->DataAsset)
			{
				result.RelativeTransform = SpawnBody.RelativeTransform* themaData->DataAsset->OffsetTransform;
			}
			break;
		case ETIHHsGizmoTriggerSpawnTransformRule::EAdd:
			if (themaData->DataAsset)
			{
				result.RelativeTransform = SpawnBody.RelativeTransform + themaData->DataAsset->OffsetTransform;
			}
			break;
		}
	}
	return result;
}

UObject* ATIHHsGizmoTriggerProbeSpawner::SpawnGizmoTriggerProbe(const FTIHHsGizmoTriggerSpawnRule& spawnRule, const FTIHHsGizmoTriggerSpawnBody& spawnData, TScriptInterface<ITIHHsInteraction> ownerInteraction)
{
	UObject* result = nullptr;
	UWorld* spawnWorld = GetWorld();
	UClass* cls = spawnData.SpawnTargetCls.LoadSynchronous();
	//spawnWorld->SpawnActor<AActor>(cls,)	//	여기에서 멈춘이유가 UTIHHsGizmoTriggerPrefabDataAsset* 여기 안에 있는 데이터를 모두 필요로 하는데, 지금 나는 따로 빼서 만드느라 DRY를 못지키고 있음.
	
	return result;
}


