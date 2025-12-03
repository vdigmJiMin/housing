// Fill out your copyright notice in the Description page of Project Settings.


#include "Gizmo/THsInteractiveGizmoCore.h"

#include "BaseGizmos/CombinedTransformGizmo.h"
#include "BaseGizmos/GizmoArrowComponent.h"
#include "BaseGizmos/GizmoLineHandleComponent.h"
#include "BaseGizmos/GizmoRectangleComponent.h"
#include "Collision/TIHCollisionCore.h"
#include "Kismet/GameplayStatics.h"


UTHsInteractiveGizmoBridge* UTHsInteractiveGizmoBridgeBucket::CreateEmptyBridge()
{
	UTHsInteractiveGizmoBridge* newBridge = NewObject<UTHsInteractiveGizmoBridge>(mGizmoManager.Get());
	if (newBridge != nullptr)
	{
		mBridgePool.Add(newBridge);
		return newBridge;
	}
	return nullptr;
}

TScriptInterface<ITHsInteractiveGizmoAPI> UTHsInteractiveGizmoBridgeBucket::ActivateInteractiveGizmoBucket(TScriptInterface<ITIHHsInteraction> target,int32& outActiveBridgeIdx)
{
	TScriptInterface<ITHsInteractiveGizmoAPI> result = nullptr;
	TWeakObjectPtr<UTHsInteractiveGizmoBridge> forCheckActiveBridge = nullptr;
	outActiveBridgeIdx = -1;
	
	for (TObjectPtr<UTHsInteractiveGizmoBridge>& bridge :mBridgePool)
	{
		if (bridge != nullptr && not bridge->IsConnected())
		{
			if (bridge->ConnectToTarget(target))
			{
				forCheckActiveBridge = bridge;
				result = bridge->GetGizmoObject();
				break;
			}
		}
	}
	if (result == nullptr)
	{
		UTHsInteractiveGizmoBridge* newBridge = NewObject<UTHsInteractiveGizmoBridge>(mGizmoManager.Get());
		if (newBridge != nullptr)
		{
			forCheckActiveBridge = newBridge;
			if (auto newGizmo = mGizmoManager->CreateGizmoActor(mGizmoClass))
			{
				newBridge->InitBridge(newGizmo);
			}
			newBridge->ConnectToTarget(target);
			mBridgePool.Add(newBridge);
			result = newBridge->GetGizmoObject();
		}
	}
	
	if (forCheckActiveBridge != nullptr)
	{
		bool yet = true;
		for (int32 i =  mActiveBridges.Num() -1; -1 < i ; --i)
		{
			if (mActiveBridges[i] == nullptr)
			{
				mActiveBridges[i] = forCheckActiveBridge;
				forCheckActiveBridge->SetCurrentActivateIdx(i);
				yet = false;
				break;
			}
		}
		if (yet)
		{
			forCheckActiveBridge->SetCurrentActivateIdx(mActiveBridges.Num());
			mActiveBridges.Add(forCheckActiveBridge);
		}
		outActiveBridgeIdx = forCheckActiveBridge->GetCurrentActivateIdx();
	}
	else
	{
		result = nullptr;
	}
	
	return result;
}

void UTHsInteractiveGizmoBridgeBucket::DeActivateInteractiveGizmo(TScriptInterface<ITHsInteractiveGizmoAPI> taget)
{
	for (int32 i = 0; i < mActiveBridges.Num(); ++i )
	{
		TWeakObjectPtr<UTHsInteractiveGizmoBridge>  activeBride = mActiveBridges[i];
		if (activeBride.IsValid() && activeBride->IsConnected())
		{
			if (activeBride->GetGizmoObject() == taget)
			{
				activeBride->DisconnectFromTarget();
				mActiveBridges[i] = nullptr;
				++mDisConnectedGizmoSortCount;
				SortDisconnectedGizmo();
				break;
			}
		}
	}
}

void UTHsInteractiveGizmoBridgeBucket::SortDisconnectedGizmo()
{
	if (mDisConnectedGizmoSortMax <= mDisConnectedGizmoSortCount)
	{
		mActiveBridges.Sort([](
			const TWeakObjectPtr<UTHsInteractiveGizmoBridge>& bridgeA,
			const TWeakObjectPtr<UTHsInteractiveGizmoBridge>& bridgeB)
		{
			/*
			 *	1 0 <- swap, 0 1 <- no swap , 0 0 <- no swap , 1 1 <- no swap
			 */
			bool result = false;
			if (bridgeA == nullptr && bridgeB != nullptr)
			{
				result = true;
			}
			return result;
		});
		
		mDisConnectedGizmoSortCount = 0;
	}
}

TScriptInterface<ITHsInteractiveGizmoAPI> FTHsInteractiveGizmoHandle::TryGetGizmo() const
{
	TScriptInterface<ITHsInteractiveGizmoAPI> result = nullptr;
	if (InteractiveGizmoManager)
	{
		TWeakObjectPtr<UTHsInteractiveGizmoBridge> bridge = InteractiveGizmoManager->TryGetBridge(BridgeBucketIndex,AciveBridgeIndex);
		if (bridge.IsValid() && bridge->IsConnected())
		{
			result = bridge->GetGizmoObject();
		}
		else
		{
			if (bridge.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("FTHsInteractiveGizmoHandle::TryGetGizmo Bridge is not connected"));
			}
			if (not bridge.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("FTHsInteractiveGizmoHandle::TryGetGizmo Bridge is not valid"));
			}
		}
	}
	return result;
}

TScriptInterface<ITIHHsInteraction> FTHsInteractiveGizmoHandle::TryGetTarget() const
{
	TScriptInterface<ITIHHsInteraction> result = nullptr;
	if (InteractiveGizmoManager)
	{
		TWeakObjectPtr<UTHsInteractiveGizmoBridge> bridge = InteractiveGizmoManager->TryGetBridge(BridgeBucketIndex,AciveBridgeIndex);
		if (bridge.IsValid() && bridge->IsConnected())
		{
			result = bridge->GetTargetObject();
		}
	}
	return result;
}

TWeakObjectPtr<UTHsInteractiveGizmoBridge> FTHsInteractiveGizmoHandle::TryGetBridge() const
{
	TWeakObjectPtr<UTHsInteractiveGizmoBridge> result = nullptr;
	if (InteractiveGizmoManager)
	{
		result = InteractiveGizmoManager->TryGetBridge(BridgeBucketIndex,AciveBridgeIndex);
	}
	return result;
}


ATHsInteractiveGizmoManager::ATHsInteractiveGizmoManager()
{
	PrimaryActorTick.bCanEverTick = false;
	
	
}


void ATHsInteractiveGizmoManager::UpdateInteractiveGizmo(const FTHsActionCursorData& cursorData)
{
	mCurrentCursorDataCapture = &cursorData;
	mAnyHitGizmo = false;
	
	for (TObjectPtr<UTHsInteractiveGizmoBridgeBucket>& bucket : mGizmoBuckets)
	{
		if (bucket == nullptr)
		{
			continue;
		}
		bucket->ForEachActiveBridge([
		cursorData,
		weakThis = TWeakObjectPtr<ATHsInteractiveGizmoManager>(this)
		](UTHsInteractiveGizmoBridge* bridge)
		{
			if (weakThis.IsValid())
			{
				if (bridge == nullptr)
				{
					return;
				}
				TScriptInterface<ITHsInteractiveGizmoAPI>& gizmoIF = bridge->GetGizmoObject();
				if (gizmoIF == nullptr)	
				{
					return;
				}
				FHitResult hitResult;

				const bool bHit = gizmoIF->RaycastGizmoHitResult(cursorData,hitResult);
				//	

				if (not bHit )
				{
					return;
				}
				const int32 priority = gizmoIF->GetGizmoPriority();
				if (not weakThis->IsAnyHitGizmo() )
				{
					weakThis->SetBestHitResult(hitResult);
					weakThis->SetBestPriority(priority);
					weakThis->SetBestGizmoBridge(bridge);
					weakThis->SetAnyHitGizmo(true);
				}
			}
			
		});
	}

	//if (mAnyHitGizmo)
	//{
	//	const FVector rayOrigin = cursorData.WorldLocation;
	//	const FVector endPoint = bestHit.HitWorldLocation;
	//	if (Occluded(rayOrigin,endPoint))
	//	{
	//		mAnyHitGizmo = false;
	//		bestBridge = nullptr;
	//		bestHit = {};
	//	}
	//}
	
	mCurrentResult = mAnyHitGizmo ? mBestHit : FHitResult();
}

const FTHsActionCursorData& ATHsInteractiveGizmoManager::GetCurrentCursorDataCapture() 
{
	
	
	return mTempCursorData;
}


UTHsInteractiveGizmoBridgeBucket* ATHsInteractiveGizmoManager::FindInteractiveGizmoBridgeBucket(UClass* gizmoCls)
{
	UTHsInteractiveGizmoBridgeBucket* result = nullptr;
	for ( TObjectPtr<UTHsInteractiveGizmoBridgeBucket> gizmoBucket :mGizmoBuckets)
	{
		if (gizmoBucket->GetGizmoClass() == gizmoCls)
		{
			result = gizmoBucket;
			break;
		}
	}
	return result;
}

bool ATHsInteractiveGizmoManager::LineTraceHitGizmo(const FTHsActionCursorData& cursorData) 
{
	UpdateInteractiveGizmo(cursorData);
	if (mAnyHitGizmo)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Gizmo with Priority %d"), mBestPriority);
	}
	return mAnyHitGizmo;
}

void ATHsInteractiveGizmoManager::LoadGizmoCreators()
{
	UObject* tryLoadDataSetting = mDataSettingPath.IsValid() ? mDataSettingPath.TryLoad() : nullptr;
	if (tryLoadDataSetting)
	{
		if (mCreatorSettingObject = Cast<UTHsInteractiveGizmoCreatorSetting>(tryLoadDataSetting))
		{
			const TArray<UTHsInteractiveGizmoCreateDescriptor*>& gizmoCreatorDescArray = mCreatorSettingObject.Get()->GizmoCreators;
			for (UTHsInteractiveGizmoCreateDescriptor* desc : gizmoCreatorDescArray)
			{
				UTHsInteractiveGizmoCreateDescriptor* clonedDesc = DuplicateObject<UTHsInteractiveGizmoCreateDescriptor>( desc,this);
				if (not desc->bUseOverrideCreatedMethod && desc->OnInteractiveGizmoActorCreated != nullptr)
				{
					clonedDesc->OnInteractiveGizmoActorCreated = MoveTemp(desc->OnInteractiveGizmoActorCreated);
				}
				RegisterGizmoCreator(clonedDesc);
			}
		}
	}
}

void ATHsInteractiveGizmoManager::RegisterGizmoCreator(UTHsInteractiveGizmoCreateDescriptor* descriptor)
{
	if (descriptor )
	{
		UClass* checkCls = descriptor->GizmoActorClass;
		if (checkCls == nullptr || not checkCls->IsChildOf<ATHsInteractiveGizmoActor>())
		{
			return;
		}
		
		if (not mGizmoCreatorDescriptors.Contains(descriptor->GizmoActorClass))
		{
			mGizmoCreatorDescriptors.Add(descriptor->GizmoActorClass,descriptor);
		}
		else
		{
			mGizmoCreatorDescriptors[descriptor->GizmoActorClass] = descriptor;
		}
	}
}


UTHsInteractiveGizmoBridgeBucket* ATHsInteractiveGizmoManager::CreateInteractiveGizmoBridgeBucket(UClass* cls)
{
	UTHsInteractiveGizmoBridgeBucket* result = nullptr;
	if (cls != nullptr && cls->ImplementsInterface(UTHsInteractiveGizmoAPI::StaticClass()))
	{
		result = NewObject<UTHsInteractiveGizmoBridgeBucket>(this);
		
		result->InitBucket(cls,this);
		result->SetBucketIndex(mGizmoBuckets.Num());
		mGizmoBuckets.Add(result);
	}
	return result;
}

void ATHsInteractiveGizmoManager::TransferToGizmoManager(ITHsInteractiveGizmoAPI* gizmo)
{
	if (UObject* gizmoObject =Cast<UObject>(gizmo))
	{
		UClass* checkCls = gizmoObject->GetClass();
		UTHsInteractiveGizmoBridgeBucket* gizmoBucket =  FindInteractiveGizmoBridgeBucket(checkCls);
		if (gizmoBucket == nullptr)
		{
			gizmoBucket = CreateInteractiveGizmoBridgeBucket(checkCls);
		}
		
		if (UTHsInteractiveGizmoBridge* newBridge = gizmoBucket->CreateEmptyBridge())
		{
			newBridge->InitBridge(gizmo);
		}
	}
}

TScriptInterface<ITHsInteractiveGizmoAPI> ATHsInteractiveGizmoManager::ActivateInteractiveGizmo(UClass* gizmoCls,TScriptInterface<ITIHHsInteraction> target,int32& outBucketIdx,int32& outActiveBridgeIdx)
{
	//	[[maybe_unused]] 
	TScriptInterface<ITHsInteractiveGizmoAPI> result = nullptr;
	for ( TObjectPtr<UTHsInteractiveGizmoBridgeBucket> gizmoBucket :mGizmoBuckets)
	{
		if (gizmoBucket->GetGizmoClass() == gizmoCls)
		{
			result = gizmoBucket->ActivateInteractiveGizmoBucket(target,outActiveBridgeIdx);
			outBucketIdx = gizmoBucket->GetBucketIndex();
			break;
		}
	}
	if (result == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Create New Gizmo Bucket for %s"), *gizmoCls->GetName());
		
		UTHsInteractiveGizmoBridgeBucket* newBucket = NewObject<UTHsInteractiveGizmoBridgeBucket>(this);
		if (newBucket != nullptr)
		{
			newBucket->InitBucket(gizmoCls,this);
			newBucket->SetBucketIndex(mGizmoBuckets.Add(newBucket));
			outBucketIdx = newBucket->GetBucketIndex();
			result = newBucket->ActivateInteractiveGizmoBucket(target,outActiveBridgeIdx);
		}
	}
	
	return result;
}

void ATHsInteractiveGizmoManager::BeginPlay()
{
	Super::BeginPlay();

	//ATHsInteractiveGizmoTest* gizmoObj = GetWorld()->SpawnActor<ATHsInteractiveGizmoTest>();
	//gizmoObj->mGizmoManager = this;
	//
	//ATHsInteractiveGizmoTestActor* spawnActor= GetWorld()->SpawnActor<ATHsInteractiveGizmoTestActor>();
	//spawnActor->mGizmoManager = this;
}



void ATHsInteractiveGizmoTest::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(
	timerHandle,
	[this]()
	{
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(),ATHsInteractiveGizmoManager::StaticClass(),foundActors);
		for (AActor* actor :foundActors)
		{
			if (actor->GetClass() == ATHsInteractiveGizmoManager::StaticClass())
			{
				UE_LOG(LogTemp, Warning, TEXT("Gizmo Manager Found!!!!"));
				if (ATHsInteractiveGizmoManager* manager = Cast<ATHsInteractiveGizmoManager>(actor))
				{
					manager->TransferToGizmoManager(this);
					mGizmoManager = manager;
					UE_LOG(LogTemp, Warning, TEXT("Gizmo Manager Transferred!!!!"));
					break;
				}
			}
		}
		
		
	},0.1f,false);

}

ATHsInteractiveGizmoTest::ATHsInteractiveGizmoTest()
{
	PrimaryActorTick.bCanEverTick = false;
	mRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(mRootComponent);

	mLine = CreateDefaultSubobject<UTHsInteractiveGizmoLineComponent>(TEXT("Line"));
	mLine->Length = 100.f;
	mLine->Color = FLinearColor::Blue;
	mLine->SetupAttachment(mRootComponent);
	mLine->SetHiddenInGame(false);
	mLine->SetMobility(EComponentMobility::Movable);
	
	mLine->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mLine->SetCollisionResponseToAllChannels(ECR_Ignore);
	mLine->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	mArrow = CreateDefaultSubobject<UTHsGizmoArrowComponent>(TEXT("Arrow"));
	mArrow->SetupAttachment(mRootComponent);
	mArrow->SetHiddenInGame(false);
	mArrow->SetMobility(EComponentMobility::Movable);

	mArrow->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mArrow->SetCollisionResponseToAllChannels(ECR_Ignore);
	mArrow->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	mRect = CreateDefaultSubobject<UGizmoRectangleComponent>(TEXT("Rect"));
	mRect->SetupAttachment(mRootComponent);
	mRect->SetHiddenInGame(false);
	mRect->SetMobility(EComponentMobility::Movable);
	mRect->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mRect->SetCollisionResponseToAllChannels(ECR_Ignore);
	mRect->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	mLine->Length = 50.f;
	mLine->Direction = FVector(1,0,0);
	mLine->Normal = FVector(0,1,0);
	mLine->PixelHitDistanceThreshold = 6.f;
	//mLine->bIsViewDependent = false; // ViewContext 셋업 전에는 꺼두고 확인
}


bool ATHsInteractiveGizmoTest::RaycastGizmoHitResult(const FTHsActionCursorData& cursor, FHitResult& outHitResult)
{
	const FVector rayStart = cursor.GetRayStart();
	const FVector rayEnd = cursor.GetRayEnd();
	
	if (CastedPrimitiveRaycast(mLine,outHitResult,rayStart,rayEnd))
	{
		return true;
	}
	if (CastedPrimitiveRaycast(mArrow,outHitResult,rayStart,rayEnd))
	{
		return true;
	}
	if (CastedPrimitiveRaycast(mRect,outHitResult,rayStart,rayEnd))
	{
		return true;
	}
	return false;
}

void ATHsInteractiveGizmoTest::ActiveGizmo(TScriptInterface<ITIHHsInteraction> target)
{
	mLine->SetVisibility(true);
	mArrow->SetVisibility(true);
	mRect->SetVisibility(true);
	mTarget = target;
	if (mTarget != nullptr && mTarget.GetInterface() != nullptr && mTarget.GetObject() != nullptr)
	{
		mIsValidTarget = true;
	}
}

void ATHsInteractiveGizmoTest::DeActiveGizmo()
{
	mLine->SetVisibility(false);
	mArrow->SetVisibility(false);
	mRect->SetVisibility(false);
	mTarget = nullptr;
	mIsValidTarget = false;
	UE_LOG(LogTemp, Warning, TEXT("{{De}}ActiveGizmo!!!!!!!!!! who? ATHsInteractiveGizmoTest"));
}

void ATHsInteractiveGizmoTest::OverrideActionPointDown(UTHsActionPoint* actionPoint)
{
	mActionAnyParam.Clear();
	mActionAnyParam.FunctionType = ETHsActionFunctionType::EEvent_Down;
	Interaction(mActionAnyParam);
}

void ATHsInteractiveGizmoTest::OverrideActionPointUp(UTHsActionPoint* actionPoint)
{
	mActionAnyParam.Clear();
	mActionAnyParam.FunctionType = ETHsActionFunctionType::EEvent_Up;
	Interaction(mActionAnyParam);
}

void ATHsInteractiveGizmoTest::OverrideActionPointClick(UTHsActionPoint* actionPoint)
{
	if (mIsValidTarget)
	{
		if (UPrimitiveComponent* comp = mGizmoManager->GetBestHitResult().GetComponent())
		{
			UE_LOG(LogTemp, Warning, TEXT("{{{{{{{{{{{{{{{{{{Clicked Gizmo Component Name : %s"), *comp->GetName());
			if (ITIHHsInteraction* interaction= Cast<ITIHHsInteraction>(comp))
			{
				interaction->OverrideActionPointClick(actionPoint);
			}
		}
	}
}

void ATHsInteractiveGizmoTest::OverrideActionPointDoubleClick(UTHsActionPoint* actionPoint)
{
	mActionAnyParam.Clear();
	mActionAnyParam.FunctionType = ETHsActionFunctionType::EEvent_DoubleClick;
	Interaction(mActionAnyParam);
}

void ATHsInteractiveGizmoTest::OverrideActionPointHoldOngoingStart(UTHsActionPoint* actionPoint, float deltaTime)
{
	mActionAnyParam.Clear();
	mActionAnyParam.FunctionType = ETHsActionFunctionType::EEvent_Down;
	mActionAnyParam.DeltaTime = deltaTime;
	Interaction(mActionAnyParam);
}

void ATHsInteractiveGizmoTest::OverrideActionPointHoldOngoing(UTHsActionPoint* actionPoint, float deltaTime)
{
	mActionAnyParam.Clear();
	mActionAnyParam.FunctionType = ETHsActionFunctionType::EProcess_HoldOngoing;
	mActionAnyParam.DeltaTime = deltaTime;
	Interaction(mActionAnyParam);
}

void ATHsInteractiveGizmoTest::OverrideActionPointHoldOngoingEnd(UTHsActionPoint* actionPoint, float deltaTime)
{
	mActionAnyParam.Clear();
	mActionAnyParam.FunctionType = ETHsActionFunctionType::EProcess_HoldStart;
	mActionAnyParam.DeltaTime = deltaTime;
	Interaction(mActionAnyParam);
}

void ATHsInteractiveGizmoTest::OverrideActionPointHoverStart(ATHsActionCursor* cursor, float deltaTime)
{
	mActionAnyParam.Clear();
	mActionAnyParam.FunctionType = ETHsActionFunctionType::ECursor_HoverStart;
	mActionAnyParam.ActionCursor = cursor;
	Interaction(mActionAnyParam);
}

void ATHsInteractiveGizmoTest::OverrideActionPointHoverOngoing(ATHsActionCursor* cursor, float deltaTime)
{
	mActionAnyParam.Clear();
	mActionAnyParam.FunctionType = ETHsActionFunctionType::ECursor_HoverOngoing;
	mActionAnyParam.ActionCursor = cursor;
	Interaction(mActionAnyParam);
}

void ATHsInteractiveGizmoTest::OverrideActionPointHoverEnd(ATHsActionCursor* cursor, float deltaTime)
{
	mActionAnyParam.Clear();
	mActionAnyParam.FunctionType = ETHsActionFunctionType::ECursor_HoverEnd;
	mActionAnyParam.ActionCursor = cursor;
	Interaction(mActionAnyParam);
}

void ATHsInteractiveGizmoTest::OverrideActionPointClickDragOngoing(UTHsActionPoint* actionPoint,float deltaTime)
{
	mActionAnyParam.Clear();
	mActionAnyParam.FunctionType = ETHsActionFunctionType::EProcess_DragOngoing;
	mActionAnyParam.DeltaTime = deltaTime;
	Interaction(mActionAnyParam);
}

void ATHsInteractiveGizmoTest::Interaction(FTHsActionPointAnyParam& interactionAnyParam)
{
	switch (interactionAnyParam.FunctionType) {
	case ETHsActionFunctionType::EProcess_Pressing:
		break;
	case ETHsActionFunctionType::EProcess_HoldStart:
		break;
	case ETHsActionFunctionType::EProcess_HoldOngoing:
		break;
	case ETHsActionFunctionType::EProcess_HoldEnd:
		break;
	case ETHsActionFunctionType::ECursor_HoverStart:
		break;
	case ETHsActionFunctionType::ECursor_HoverOngoing:
		break;
	case ETHsActionFunctionType::ECursor_HoverEnd:
		break;
	case ETHsActionFunctionType::EEvent_Click:
		
		break;
	case ETHsActionFunctionType::EEvent_DoubleClick:
		break;
	case ETHsActionFunctionType::EEvent_Down:
		break;
	case ETHsActionFunctionType::EEvent_Up:
		break;
	case ETHsActionFunctionType::EEvent_Triggered:
		break;
	case ETHsActionFunctionType::EEvent_Cancel:
		break;
	case ETHsActionFunctionType::ECursor_CursorMove:
		break;
	case ETHsActionFunctionType::EProcess_DragStart:
		break;
	case ETHsActionFunctionType::EProcess_DragOngoing:
		break;
	case ETHsActionFunctionType::EProcess_DragEnd:
		break;
	case ETHsActionFunctionType::EFunction_Selection:
		break;
	case ETHsActionFunctionType::EFunction_DisSelection:
		break;
	case ETHsActionFunctionType::EZCustom0:
		break;
	case ETHsActionFunctionType::EZCustom1:
		break;
	case ETHsActionFunctionType::EZCustom2:
		break;
	case ETHsActionFunctionType::EZCustom3:
		break;
	case ETHsActionFunctionType::EZCustom4:
		break;
	case ETHsActionFunctionType::EZCustom5:
		break;
	case ETHsActionFunctionType::EZCustom6:
		break;
	case ETHsActionFunctionType::EZCustom7:
		break;
	case ETHsActionFunctionType::EZCustom8:
		break;
	}



}

FTHsInteractiveGizmoHandle ATHsInteractiveGizmoManager::ActivateInteractiveGizmoHandle(UClass* gizmoCls, TScriptInterface<ITIHHsInteraction> target)
{
	int32 bucketIdx = -1;
	int32 activeBridgeIdx = -1;
	ATHsInteractiveGizmoManager* qualification = nullptr;
	if (target.GetObject() == nullptr)
	{
		if (target.GetInterface() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ATHsInteractiveGizmoManager::ActivateInteractiveGizmoHandle GetObject 에서 시작 둘다 nullptr"));
			return FTHsInteractiveGizmoHandle::InvalidHandle();
		}
		if (UObject* castObj= Cast<UObject>(target.GetInterface()))
		{
			target.SetObject(castObj);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATHsInteractiveGizmoManager::ActivateInteractiveGizmoHandle GetObject 에서 시작 둘다 nullptr, 그리고 인터페이스를 오브젝트로 캐스트도 못함"));
			return FTHsInteractiveGizmoHandle::InvalidHandle();
		}
		
	}
	
	if (target.GetInterface() == nullptr)
	{
		if (target.GetObject() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ATHsInteractiveGizmoManager::ActivateInteractiveGizmoHandle GetInterface 에서 시작 둘다 nullptr"));
			return FTHsInteractiveGizmoHandle::InvalidHandle();
		}
		if (ITIHHsInteraction* castInterface = Cast<ITIHHsInteraction>(target.GetObject()))
		{
			target.SetInterface(castInterface);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATHsInteractiveGizmoManager::ActivateInteractiveGizmoHandle GetInterface 에서 시작 둘다 nullptr, 그리고 오브젝트를 인터페이스로 캐스트도 못함"));
			return FTHsInteractiveGizmoHandle::InvalidHandle();
		}
		
	}

	if (target.GetInterface() == nullptr || target.GetObject() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATHsInteractiveGizmoManager::ActivateInteractiveGizmoHandle 둘다 거쳤는데도 널이면 false"));
		return FTHsInteractiveGizmoHandle::InvalidHandle();
	}
	
	
	TScriptInterface<ITHsInteractiveGizmoAPI> isNotNull = ActivateInteractiveGizmo(
		gizmoCls, target, bucketIdx, activeBridgeIdx);
	
	if (isNotNull != nullptr)
	{
		return FTHsInteractiveGizmoHandle{this,bucketIdx,activeBridgeIdx};
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATHsInteractiveGizmoManager::ActivateInteractiveGizmoHandle ActivateInteractiveGizmo 가 널을 반환함"));
		return FTHsInteractiveGizmoHandle::InvalidHandle();
	}
	
}


void ATHsInteractiveGizmoManager::DeActivateInteractiveGizmo(TScriptInterface<ITHsInteractiveGizmoAPI> taget)
{
	for (TObjectPtr gizmoBucket :mGizmoBuckets)
	{
		if (gizmoBucket->GetGizmoClass() == taget.GetObject()->GetClass())
		{
			gizmoBucket->DeActivateInteractiveGizmo(taget);
			break;
		}
	}
}



ATHsInteractiveGizmoTestActor::ATHsInteractiveGizmoTestActor()
{
	PrimaryActorTick.bCanEverTick = false;
	mMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UTHsInteractiveGizmoTestStaticMesh"));
	SetRootComponent(mMeshComponent);
	mMeshComponent->SetStaticMesh( LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'")));
	
}

void ATHsInteractiveGizmoTestActor::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle,[this]()
	{
		mGizmoHandle = mGizmoManager->ActivateInteractiveGizmoHandle(ATHsInteractiveGizmoTest::StaticClass(),this);
		UE_LOG(LogTemp, Warning, TEXT(" Active BeginPlay Test %d %d"), mGizmoHandle.BridgeBucketIndex, mGizmoHandle.AciveBridgeIndex);
		if (mGizmoHandle.IsValidHandle())
		{
			UE_LOG(LogTemp, Warning, TEXT("Gizmo Activated in ATHsInteractiveGizmoTestActor"));
			TScriptInterface<ITHsInteractiveGizmoAPI> gizmoIF = mGizmoHandle.TryGetGizmo();
			if (gizmoIF != nullptr)
			{
				if (gizmoIF->IsActiveGizmo())
				{
					UE_LOG(LogTemp, Warning, TEXT("------Gizmo is active!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("------Gizmo is what?!!!!!!!!!!!"));
			}
		}
	},2.5f,false);
	
}

void ATHsInteractiveGizmoTestActor::OverrideActionPointClick(UTHsActionPoint* actionPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("ATHsInteractiveGizmoTestActor Clicked! Deactivating Gizmo."));
}

void ATHsInteractiveGizmoTestActor::OverrideConnectGizmo(TScriptInterface<class ITHsInteractiveGizmoAPI> gizmo)
{
	mGizmo = gizmo;
}
int32 ATHsInteractiveGizmoTestActor::OverrideCommunicateInteractionString(const FString& stringMsgm,const FTHsCommunicationMessage& message)
{
	if (stringMsgm == TEXT("gizmo"))
	{
		
		
	}
	return 0;
}

void UTHsInteractivePlaneMoveProxy::ApplyMoveDelta(const FVector& worldDelta, const FVector& planeNormalWorld)
{
	if (mTargetComponent.IsValid())
	{
		const FVector moveDelta = FVector::VectorPlaneProject(worldDelta, planeNormalWorld);
		//mTargetComponent->AddWorldOffset(moveDelta);
		FTransform transform = mTargetComponent->GetComponentTransform();
		transform.SetLocation(mTargetComponent->GetComponentLocation() + moveDelta);
		AActor* owner = mTargetComponent->GetOwner();
		if (ATHsStuff_Dep* stuff= Cast<ATHsStuff_Dep>(owner))
		{
			transform = stuff->PlaceOnSurface(transform);
		}
		mTargetComponent->SetWorldTransform(transform);
		if (mGizmoComponent && mGizmoComponent->IsActiveGizmo())
		{
			mGizmoComponent->MoveGizmo(FTransform::Identity);
		}
	}
}

UTHsInteractiveGizmoLineComponent::UTHsInteractiveGizmoLineComponent()
{
	PrimaryComponentTick.bCanEverTick
	= false;
}

void UTHsInteractiveGizmoLineComponent::OverrideActionPointClick(UTHsActionPoint* actionPoint)
{
	if (actionPoint->GetActionPointKey() == EKeys::LeftMouseButton)
	{
		if (mGizmoBridge && mGizmoBridge->mAxisProxy.IsValid())
		{
			if (mGizmoBridge->TargetComponent.IsValid())
			{
				mGizmoBridge->mAxisProxy.Get()->Initialize(mGizmoBridge->TargetComponent.Get(),mGizmoBridge->GizmoAPI);
				
			}
			
		}
	}
	
}
void UTHsInteractiveGizmoLineComponent::OverrideActionPointClickDragOngoing(UTHsActionPoint* actionPoint,
	float deltaTime)
{
	if (actionPoint->GetActionPointKey() == EKeys::LeftMouseButton)
	{
		
	}
}

void UTHsInteractiveGizmoLineComponent::BeginPlay()
{
	Super::BeginPlay();
}

ATHsInteractiveTransformGizmo::ATHsInteractiveTransformGizmo()
{
	PrimaryActorTick.bCanEverTick = false;

	mRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(mRootComponent);
	/*
	* UPROPERTY(EditAnywhere, Category = Options)
	FLinearColor Color = FLinearColor::Red;


	UPROPERTY(EditAnywhere, Category = Options)
	float HoverSizeMultiplier = 2.0f;


	UPROPERTY(EditAnywhere, Category = Options)
	float PixelHitDistanceThreshold = 7.0f;
	 */
	UTHsInteractiveGizmoLineComponent* upComponent = CreateDefaultSubobject<UTHsInteractiveGizmoLineComponent>(TEXT("UpComponent"));
	{
		upComponent->Direction = FVector(0,0,1);
		upComponent->Normal = FVector(0,0,1);
		upComponent->Length = 75.f;
		upComponent->PixelHitDistanceThreshold = 6.f;
		upComponent->HandleSize = 5.0f;
		upComponent->Thickness = 2.0f;
		upComponent->bImageScale = true;
		upComponent->Color = FLinearColor::Blue;
		upComponent->HoverSizeMultiplier = 2.0f;
		upComponent->SetGizmoBridge(&mGizmoBridge);
	}
	mUpComponent = upComponent;
	mUpComponent->SetupAttachment(mRootComponent);
	mUpComponent->SetHiddenInGame(false);
	
	// mUpComponent->SetMobility(EComponentMobility::Movable);
	// mUpComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// mUpComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	// mUpComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	UTHsInteractiveGizmoLineComponent* forwardComponent = CreateDefaultSubobject<UTHsInteractiveGizmoLineComponent>(TEXT("ForwardComponent"));
	{
		forwardComponent->Direction = FVector(0,1,0);
		forwardComponent->Normal = FVector(0,1,0);
		forwardComponent->Length = 75.f;
		forwardComponent->PixelHitDistanceThreshold = 6.f;
		forwardComponent->HandleSize = 5.0f;
		forwardComponent->Thickness = 2.0f;
		forwardComponent->bImageScale = true;
		forwardComponent->Color = FLinearColor::Green;
		forwardComponent->HoverSizeMultiplier = 2.0f;
	}
	mForwardComponent = forwardComponent;

	UTHsInteractiveGizmoLineComponent* rightComponent = CreateDefaultSubobject<UTHsInteractiveGizmoLineComponent>(TEXT("RightComponent"));
	{
		rightComponent->Direction = FVector(0,1,0);
		rightComponent->Normal = FVector(0,1,0);
		rightComponent->Length = 75.f;
		rightComponent->PixelHitDistanceThreshold = 6.f;
		rightComponent->HandleSize = 5.0f;
		rightComponent->Thickness = 2.0f;
		rightComponent->bImageScale = true;
		rightComponent->Color = FLinearColor::Green;
		rightComponent->HoverSizeMultiplier = 2.0f;
	}
	mRightComponent = rightComponent;
	
}

void ATHsInteractiveTransformGizmo::ActiveGizmo(TScriptInterface<ITIHHsInteraction> target)
{
	mTarget =target;
	if (mTarget != nullptr)
	{
		mIsValidTarget = true;
		mRootComponent->SetVisibility(true,true);
	}
}

void ATHsInteractiveTransformGizmo::DeActiveGizmo()
{
	mTarget = nullptr;
	mIsValidTarget = false;
	mRootComponent->SetVisibility(false,true);
}

bool ATHsInteractiveTransformGizmo::IsActiveGizmo() const
{
	return mIsValidTarget;
}

bool ATHsInteractiveTransformGizmo::RaycastGizmoHitResult(const FTHsActionCursorData& cursor, FHitResult& outHitResult)
{
	const FVector rayStart = cursor.GetRayStart();
	const FVector rayEnd = cursor.GetRayEnd();
	const FCollisionQueryParams& params = FCollisionQueryParams::DefaultQueryParam;
	
	if (mUpComponent != nullptr&& mUpComponent->LineTraceComponent(outHitResult,rayStart,rayEnd,params))
	{
		return true;
	}
	if (mForwardComponent != nullptr&& mForwardComponent->LineTraceComponent(outHitResult,rayStart,rayEnd,params))
	{
		return true;
	}
	if (mRightComponent != nullptr&& mRightComponent->LineTraceComponent(outHitResult,rayStart,rayEnd,params))
	{
		return true;
	}
	if (mXYPlaneComponent != nullptr&& mXYPlaneComponent->LineTraceComponent(outHitResult,rayStart,rayEnd,params))
	{
		return true;
	}
	if (mXZPlaneComponent != nullptr&& mXZPlaneComponent->LineTraceComponent(outHitResult,rayStart,rayEnd,params))
	{
		return true;
	}
	if (mYZPlaneComponent != nullptr&& mYZPlaneComponent->LineTraceComponent(outHitResult,rayStart,rayEnd,params))
	{
		return true;
	}
	
	return false;
}

int32 ATHsInteractiveTransformGizmo::GetGizmoPriority() const
{
	return 0;
}


