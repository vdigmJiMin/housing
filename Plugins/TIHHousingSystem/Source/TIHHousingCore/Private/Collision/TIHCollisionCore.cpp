// Fill out your copyright notice in the Description page of Project Settings.


#include "Collision/TIHCollisionCore.h"

#include "DiffUtils.h"
#include "JsonObjectConverter.h"
#include "ActionSystem/THsEnhancedActionSystem.h"
#include "Components/Button.h"
#include "DSP/AudioDebuggingUtilities.h"
#include "Kismet/GameplayStatics.h"

TWeakObjectPtr<ATHsStuffManager_desperate> ATHsStuffManager_desperate::gStuffManagerInstance = nullptr;

TWeakObjectPtr<ATHsPresetManager> ATHsPresetManager::gPresetManagerInstance = nullptr;

void UTHsStuffSearchSphere_Dep::AddLinkedConnectPoint(UTHsStuffConnectPoint_Dep* connectPoint)
{
	FVector center = FVector::ZeroVector;
	
	bool bInserted = false;
	int32 realValidCount = 0;
	
	for (int32 i = mLinkedConnectPoints.Num()-1; -1 < i; --i)
	{
		if (mLinkedConnectPoints[i].IsValid())
		{
			center += mLinkedConnectPoints[i].Get()->GetWorldEndLocationByLinkedComponent();
			++realValidCount;
			continue;
		}
		else
		{
			if (not bInserted)
			{
				bInserted = true;
				mLinkedConnectPoints[i] = connectPoint;
				center += connectPoint->GetWorldEndLocationByLinkedComponent();
			}
		}
	}
	if (not bInserted)
	{
		bInserted = true;
		mLinkedConnectPoints.Add(connectPoint);
		center += connectPoint->GetWorldEndLocationByLinkedComponent();
	}
	mLinkedConnectPoints.Sort([](const TWeakObjectPtr<UTHsStuffConnectPoint_Dep>& a, const TWeakObjectPtr<UTHsStuffConnectPoint_Dep>& b)->bool
		{
			if (not a.IsValid() && b.IsValid())
			{
				return true;
			}
			return false;
		});
	if (bInserted)
	{
		++realValidCount;
		center = center / static_cast<float>(realValidCount);
		float maxDistance = 0.0f;
		for (TWeakObjectPtr<UTHsStuffConnectPoint_Dep> linkedConnectPoint :mLinkedConnectPoints)
		{
			if (not linkedConnectPoint.IsValid())
			{
				break;
			}
			const FVector endPoint = linkedConnectPoint.Get()->GetWorldEndLocationByLinkedComponent();
			const float distance = FVector::Distance(center, endPoint);
			maxDistance = FMath::Max(distance, maxDistance);
		}
		mRecentWorldCenterLocation = center;
		mCurrentMinRadius = maxDistance;
		mCurrentMaxRadius = maxDistance * mExtendRadiusScale;
		mCollisionShape = FCollisionShape::MakeSphere(mCurrentMaxRadius);
	}
}

void UTHsStuffSearchSphere_Dep::TryUpdateRecentSurfaces(const FTransform& reserveWorldTransform)
{
	if (CheckRefreshNeed(reserveWorldTransform.GetLocation()))
	{
		RefreshSearchSphere(reserveWorldTransform);

		//	서페이스 갱신
		mRecentSurfaces.Empty();
		for (const FOverlapResult& overlapResult :mRecentOverlapResults)
		{
			if (overlapResult.bBlockingHit)
			{
				if (ITHsHousingFamily_Dep* stuffFunc= Cast<ITHsHousingFamily_Dep>(overlapResult.GetActor()))
				{
					mRecentSurfaces.Append(stuffFunc->GetAllSurfaces());
				}
			}
		}
	}
}

FTHsSurfaceTraceResult UTHsStuffSearchSphere_Dep::TryFindBestSurface(const TScriptInterface<ITHsHousingFamily_Dep>& stuff, const FTransform& reserveWorldTransform)
{
	FTHsSurfaceTraceResult result;
	
	TryUpdateRecentSurfaces(reserveWorldTransform);
	
	bool bFoundBest = false;
	float bestDistanceSq = TNumericLimits<float>::Max();
	FVector bestImpactPoint =FVector::ZeroVector;
	FVector bestImpactNormal = FVector::ZeroVector;
	TWeakObjectPtr<UTHsStuffConnectPoint_Dep> bestPoint = nullptr;
	TWeakObjectPtr<UTHsStuffConnectSurface_Dep> bestSurface = nullptr;
	int32 allowHitDirectionMask = ToBits(ETHsStuffConnectDirection::ENone);
	for (const TWeakObjectPtr<UTHsStuffConnectPoint_Dep>& connectPoint :mLinkedConnectPoints)
	{
		if (not connectPoint.IsValid())
		{
			continue;
		}
		const FVector worldStart = connectPoint->GetWorldStartLocationByLinkedComponent();
		const FVector worldEnd = connectPoint->GetWorldEndLocationByLinkedComponent();
		const int32 connectDirectionMask = connectPoint->GetConnectDirectionMask();
		
		for (const TWeakObjectPtr<UTHsStuffConnectSurface_Dep>& surface : mRecentSurfaces)
		{
			if (surface.IsValid())
			{
				FHitResult hitResult;
				
				const int32 surfaceMask = surface->GetConnectDirectionMask();
				if (surface->LineTraceSingle(hitResult, worldStart, worldEnd,mTraceChannel))
				{
					int32 directionMask = connectDirectionMask & surfaceMask;
					if (directionMask != 0)
					{
						const FVector impactPoint = hitResult.ImpactPoint;
						const float distanceSq = FVector::DistSquared(impactPoint, worldStart);
						
						if ( distanceSq < bestDistanceSq)
						{
							allowHitDirectionMask = directionMask;
							bestDistanceSq = distanceSq;
							bestImpactPoint = impactPoint;
							bestImpactNormal = hitResult.ImpactNormal;
							bestSurface = surface;
							bestPoint = connectPoint;
							bFoundBest = true;
						}
					}
				}
			}
		}
		
	}
	if (bFoundBest)
	{
		result.BestConnectDirection = allowHitDirectionMask;
		
		result.BestConnectPoint = bestPoint;
		result.BestConnectSurface = bestSurface;
		
		result.RecentImpactPoint = bestImpactPoint;
		result.RecentImpactNormal = bestImpactNormal;
	}
	return result;
}

void UTHsStuffSearchSphere_Dep::RefreshSearchSphere(const FTransform& reserveWorldTransform) 
{
	if (mWorldContext == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTHsStuffSearchSphere::RefreshSearchSphere mWorldContext is nullptr"));
		return;
	}
	ReCalculateSphere(reserveWorldTransform.GetLocation());
	
	mRecentOverlapResults.Empty();
	mWorldContext->OverlapMultiByChannel(
		mRecentOverlapResults,
		reserveWorldTransform.GetLocation(),
		FQuat::Identity,
		mTraceChannel,
		mCollisionShape
	);
	
}

bool UTHsStuffSearchSphere_Dep::CheckRefreshNeed(const FVector& reserveWorldLocation) const
{
	bool result = false;
	
	float threshold = mCurrentMinRadius * mRefreshDistanceThresholdScale;
	
	if ((mRecentWorldCenterLocation - reserveWorldLocation).SizeSquared() >= threshold*threshold)
	{
		result = true;
	}
	
	return result;
}

bool UTHsStuffSearchSphere_Dep::CalculateCenterFromValidConnectPoints( FVector& refCenter)
{
	bool result = false;
	int32 realValidCount = 0;
	FVector center = (mLinkedStuffRootComponent.IsValid())?mLinkedStuffRootComponent->GetComponentLocation() : FVector::ZeroVector;
	for (int32 i = 0; i < mLinkedConnectPoints.Num(); ++i)
	{
		const TWeakObjectPtr<UTHsStuffConnectPoint_Dep>& linkedConnectPoint = mLinkedConnectPoints[i];
		if (linkedConnectPoint.IsValid())
		{
			center += linkedConnectPoint.Get()->GetWorldEndLocationByLinkedComponent();
			++realValidCount;
		}
		else
		{
			mLinkedConnectPoints[i].Reset();
			continue;
		}
	}
	
	if (0 < realValidCount)
	{
		refCenter = center/ static_cast<float>(realValidCount);
		result = true;
	}

	return result;
}

void UTHsStuffSearchSphere_Dep::CalculateRadiusFromLinkedPoints(const FVector& reserveWorldLocation, const FVector& center,float& currentMaxRadius)
{
	float maxDistance = 0.0f;
	for (const TWeakObjectPtr<UTHsStuffConnectPoint_Dep>& linkedConnectPoint:mLinkedConnectPoints)
	{
		if (not linkedConnectPoint.IsValid())
		{
			continue;
		}
		const FVector endPoint = linkedConnectPoint.Get()->GetWorldEndLocationByLinkedComponent() + reserveWorldLocation;
		const float distance = FVector::Distance(center, endPoint);
		maxDistance = FMath::Max(distance, maxDistance);
	}
	currentMaxRadius = maxDistance * mExtendRadiusScale;
}

const TArray<FOverlapResult>& UTHsStuffSurfaceFinder::CaptureOverlapResults(const FTransform& reserveWorldTransform,
	const TArray<FVector>& pointsSnapshot/* = TArray<FVector>() */)
{
	static const TArray<FOverlapResult> emptyResults;

	if (not IsValidFinder())
	{
		return emptyResults;
	}

	if (CheckRefreshNeed(reserveWorldTransform.GetLocation()))
	{
		RefreshSearchRange(pointsSnapshot);
		
		mRecentOverlapResults.Empty();
		mWorldContext->OverlapMultiByChannel(
			mRecentOverlapResults,
			mRecentCaptureLocation,
			FQuat::Identity,
			mTraceChannel,
			mCollisionShape
		);
	}
	
	return mRecentOverlapResults;
}

ATHsStuff_Dep* UTHsStuffConnectPoint_Dep::GetOwningStuff() const
{
	if (mLinkedComponent.IsValid())
	{
		if (ATHsStuff_Dep* ownerStuff= Cast<ATHsStuff_Dep>(mLinkedComponent->GetOwner()))
		{
			return ownerStuff;
		}
	}
	return nullptr;
}

bool FTHsSurfaceTraceResult::IsSameOwnerStuff() const
{
	return IsValid() && BestConnectPoint.Get()->GetOwningStuff() == BestConnectSurface.Get()->GetOwningStuff();
}

bool FTHsSurfaceTraceResult::IsAttachPossible() const
{
	return IsValid() && IsSameOwnerStuff();
}

ATHsStuff_Dep* UTHsStuffConnectSurface_Dep::GetOwningStuff() const
{
	ATHsStuff_Dep* owningStuff = nullptr;
	if (mLinkedComponent.IsValid())
	{
		owningStuff = Cast<ATHsStuff_Dep>(mLinkedComponent->GetOwner());
	}
	return owningStuff;
}

bool UTHsStuffConnectPlaneSurface::LineTraceSingle(FHitResult& OutHit, const FVector& Start, const FVector& End,
                                                   ECollisionChannel TraceChannel, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam)
{
	FVector RayDir = (End - Start).GetSafeNormal();
	float RayLength = FVector::Dist(Start, End);
    
	float Denominator = FVector::DotProduct(RayDir, FVector(mPlane.X, mPlane.Y, mPlane.Z));
	
	if (FMath::IsNearlyZero(Denominator))
	{
		return false;
	}
    
	float T = -(FVector::DotProduct(Start, FVector(mPlane.X, mPlane.Y, mPlane.Z)) + mPlane.W) / Denominator;
	
	if (T < 0.0f || T > RayLength)
	{
		return false;
	}
    
	FVector HitPoint = Start + RayDir * T;
    
	FVector PlaneOrigin = mPlane.GetOrigin();
	FVector PlaneNormal = FVector(mPlane.X, mPlane.Y, mPlane.Z);
    
	FVector PlaneRight = FVector::CrossProduct(PlaneNormal, FVector::UpVector).GetSafeNormal();
	if (PlaneRight.IsNearlyZero())
	{
		PlaneRight = FVector::CrossProduct(PlaneNormal, FVector::ForwardVector).GetSafeNormal();
	}
	FVector PlaneUp = FVector::CrossProduct(PlaneRight, PlaneNormal).GetSafeNormal();
    
	FVector LocalHit = HitPoint - PlaneOrigin;
	float LocalX = FVector::DotProduct(LocalHit, PlaneRight);
	float LocalY = FVector::DotProduct(LocalHit, PlaneUp);
    
	float HalfWidth = mSize.X * 0.5f;
	float HalfHeight = mSize.Y * 0.5f;
    
	if (FMath::Abs(LocalX) > HalfWidth || FMath::Abs(LocalY) > HalfHeight)
	{
		return false;
	}
    
	OutHit.bBlockingHit = true;
	OutHit.Location = HitPoint;
	OutHit.ImpactPoint = HitPoint;
	OutHit.Normal = PlaneNormal;
	OutHit.ImpactNormal = PlaneNormal;
	OutHit.Distance = T;
	OutHit.Time = T / RayLength;
    
	return true;
}

FTransform UTHsStuffConnectPlaneSurface::PlaceOnSurface(const FTransform& targetTransform)
{
	return Super::PlaceOnSurface(targetTransform);
}

bool UTHsStuffConnectPlaneSurface::AttachToSurface(const FTHsSurfaceTraceResult& traceResult, const FTransform& targetTransform)
{
	//	rule 처리 이후임.
	bool result = false;
	if (traceResult.IsValid())
	{
		if (ATHsStuff_Dep* ownerStuff = traceResult.BestConnectSurface->GetOwningStuff())
		{
			result = ownerStuff->AttachToSurface(traceResult,targetTransform);
		}
	}
	return result;
	
}

ATHsStuff_Dep::ATHsStuff_Dep()
{
	PrimaryActorTick.bCanEverTick = false;

	mRootComponent = CreateDefaultSubobject<UTHsRootSceneComponent_Dep>(TEXT("RootComponent"));
	SetRootComponent(mRootComponent);
	
}

void ATHsStuff_Dep::TryDetach()
{
	if (mParentStuff.IsValid())
	{
		mParentStuff->DetachFromSurface(this);
		this->UpdateHierarchy(nullptr,mRootComponent->GetComponentTransform());
	}
}

UTHsRootSceneComponent_Dep* ATHsStuff_Dep::RenewRootSceneComponent(TSubclassOf<UTHsRootSceneComponent_Dep> rootCompClass)
{
	if (rootCompClass)
	{
		
		UTHsRootSceneComponent_Dep* newRoot = NewObject<UTHsRootSceneComponent_Dep>(this, rootCompClass);
		if (newRoot)
		{
			newRoot->RegisterComponent();
			SetRootComponent(newRoot);
			mRootComponent = newRoot;
			return newRoot;
		}
	}

	if (GetRootComponent() == nullptr)
	{
		mRootComponent = NewObject<UTHsRootSceneComponent_Dep>(this, UTHsRootSceneComponent_Dep::StaticClass());
		SetRootComponent(mRootComponent);
		return mRootComponent;
	}

	return nullptr;
}

void ATHsStuff_Dep::BeginPlay()
{
	Super::BeginPlay();
	
	//THS_Action_Trait_Selectable
	//THS_Action_Trait_Hoverable
	
	mStuffTags.AddTag(THS_Action_Trait_Attachable);
	mStuffTags.AddTag(THS_Action_Trait_Selectable);
	mStuffTags.AddTag(THS_Action_Trait_Hoverable);
}

UTHsStuffSearchSphere_Dep* ATHsStuff_Dep::GetSearchSphere() const
{
	return mRootComponent ? mRootComponent->GetSearchSphere() : nullptr;
}

const FTransform ATHsStuff_Dep::PlaceOnSurface(const FTransform& targetTransform)
{
	FTransform result = targetTransform;
	if (UTHsStuffSearchSphere_Dep* searchSphere = GetSearchSphere())
	{
		FTHsSurfaceTraceResult bestSurface = searchSphere->TryFindBestSurface(this, targetTransform);
		if (bestSurface.IsValid())
		{
			result = bestSurface.BestConnectSurface->ExecuteRule(result);
			bestSurface.BestConnectSurface->AttachToSurface(bestSurface,result);
		}
	}
	
	return result;
}

void ATHsStuff_Dep::OverrideActionPointClick(UTHsActionPoint* actionPoint)
{
	if (actionPoint->GetActionPointKey() == EKeys::LeftMouseButton)
	{
		if (mStuffSelectState)
		{
			UE_LOG(LogTemp, Warning, TEXT("ATHsStuff: Deselecting Stuff"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATHsStuff: Selecting Stuff"));
		}
	}
}

void ATHsStuff_Dep::OverrideSelection(bool bSelect)
{
	if (bSelect)
	{
		mStuffSelectState = true;
	}
	else
	{
		mStuffSelectState = false;
	}
}

void UTHsRootSceneComponent_Dep::BeginPlay()
{
	Super::BeginPlay();

	//mSearchSphere = NewObject<UTHsStuffSearchSphere>(this, UTHsStuffSearchSphere::StaticClass());
	//mSearchSphere->InitStuffSearchSphere(GetOwner()->GetWorld(),this);
}

void ATHsTestExecuteActor::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world=GetWorld();
	world->SpawnActor<ATHsStuff_Dep>();
	UTHsEnhancedActionSystem* worldSystem = world->GetSubsystem<UTHsEnhancedActionSystem>();
	worldSystem->OnPostBindAllAction.AddLambda([](UTHsEnhancedActionSystem* as)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATHsTestExecuteActor: Bound action point for spawning ATHsStuff"));
		
		as->BindInputKeyEventByName(
		ETHsActionFunctionType::EEvent_Click,
		TEXT("1"),
		[](UTHsActionPoint* actionPoint)
		{
			actionPoint->GetWorldContext()->SpawnActor<ATHsStuff_Dep>();
			UE_LOG(LogTemp, Warning, TEXT("ATHsTestExecuteActor: Spawned ATHsStuff via action point"));
		});
	});
}


UTHsPresetStuffHierarchyNode_Dep* UTHsPresetStuffHierarchyNode_Dep::DefaultRootSceneCompHierarchyData(UObject* outer)
{
	UTHsPresetStuffHierarchyNode_Dep* result = nullptr;
	UObject* owner = outer;
	if (owner == nullptr)
	{
		owner = ATHsStuffManager_desperate::gStuffManagerInstance.Get();
	}
	
	if (ATHsStuffManager_desperate::gStuffManagerInstance.IsValid())
	{
		result = NewObject<UTHsPresetStuffHierarchyNode_Dep>(owner, UTHsPresetStuffHierarchyNode_Dep::StaticClass());
		result->SetDefault();
		result->HierarchyUseType = ETHsPresetCompHierarchyUseType::EUsePreset;
		result->PresetComponentName = GetTHsRootSceneComponentName();
		UTHsRootSceneComponent_Dep* newRoot = NewObject<UTHsRootSceneComponent_Dep>(
			owner, UTHsRootSceneComponent_Dep::StaticClass(), GetTHsRootSceneComponentName(),
			RF_Public | RF_Transactional
			);
		result->OverrideCustomComponent = newRoot;
	}

	verify(result != nullptr);
	
	return result;
}

bool UTHsPresetStuffHierarchyNode_Dep::CheckRegisterValid(TWeakObjectPtr<ATHsStuffManager_desperate> stuffMgr) 
{
	bool result = true;

	//	stuffMgr 이 valid 해야함.
	if (not stuffMgr.IsValid())
	{
		return false;
	}
	
	switch (HierarchyUseType) {
	case ETHsPresetCompHierarchyUseType::EUsePreset://	이름과 등록이 가능한지만 확인
		if (PresetComponentName == NAME_None)
		{
			return false;
		}
		if (stuffMgr->FindPresetStuffComponent(PresetComponentName) != nullptr)
		{
			return false;
		}
	case ETHsPresetCompHierarchyUseType::EUseCustomComponent://	생성할거니깐 이게 클래스 정보가 있는지만 확인
		if (OverrideCustomComponent == nullptr)
		{
			return false;
		}
		break;
	case ETHsPresetCompHierarchyUseType::EForRegisterPreset://	등록을 위한거니깐 다있어함.
		if (PresetComponentName == NAME_None)
		{
			return false;
		}
		if (stuffMgr->FindPresetStuffComponent(PresetComponentName) != nullptr)
		{
			return false;
		}
		if (OverrideCustomComponent == nullptr)
		{
			return false;
		}
		break;
	}
	
	switch (HierarchyProcType) {
	case ETHsPresetCompHierarchyProcType::EProgressive:
		break;
	case ETHsPresetCompHierarchyProcType::EConditional:
		if (ConditionFunctor == nullptr)
		{
			return false;
		}
		break;
	case ETHsPresetCompHierarchyProcType::EIgnore:
		break;
	}
	
	for (const TObjectPtr<UTHsPresetStuffHierarchyNode_Dep>& child :ChildComponents)
	{
		if (child != nullptr && not child->CheckRegisterValid(stuffMgr.Get()))
		{
			return false;
		}
	}
	
	return true;
}

TWeakObjectPtr<UTHsPresetStuffHierarchyNode_Dep> UTHsPresetStuffHierarchyNode_Dep::AddChildHierarchyNode(
	const FName& presetName)
{
	TObjectPtr<UTHsPresetStuffHierarchyNode_Dep> newNode = nullptr;
	if (ATHsStuffManager_desperate* stuffMgr = ATHsStuffManager_desperate::GetStuffManagerInstance())
	{
		//	지금 하고 있는게 뭐냐. 필요한 기능들을 추가하는데, 필요할거 같아서 추가하는거지 진짜로 필요한가? 하면 지금 '당장'은 아닌거 같음.
		//stuffMgr->TryGetPresetStuffComponentByName(presetName)
		if (stuffMgr->ValidatePresetStuffComponentName(presetName) == false)
		{
			newNode = NewObject<UTHsPresetStuffHierarchyNode_Dep>(this);
			newNode->SetDefault();
			newNode->PresetComponentName = presetName;
			ChildComponents.Add(newNode);
		}
	}
	return newNode;
}

void UTHsPresetStuffHierarchyNode_Dep::BeginDestroy()
{
	UObject::BeginDestroy();
}

bool UTHsPresetStuffComponent_Dep::CheckRegisterValid()
{
	if (PresetName == NAME_None)
	{
		return false;
	}

	if (ComponentData == nullptr)
	{
		return false;
	}

	if (ATHsStuffManager_desperate::GetWeakStuffManagerInstance().IsValid())
	{
		ATHsStuffManager_desperate* stuffMgr = ATHsStuffManager_desperate::GetWeakStuffManagerInstance().Get();
		for (const TObjectPtr<UTHsPresetStuffHierarchyNode_Dep>& child:ChildHierarchies)
		{
			if (child != nullptr && not child->CheckRegisterValid(stuffMgr))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

// USceneComponent* UTHsPresetStuffComponent::CreatePresetComponent(ATHsStuffManager* stuffMgr,UObject* outer, const FName& OverridePresetName,bool useHierarchy) const
// {
// 	USceneComponent* resultComponent = nullptr;
// 	if (outer)
// 	{
// 		UClass* componentClass = ComponentClassPath->GetClass();
// 		FTransform componentTransform = RelativeTransform;
// 		FGameplayTagContainer componentTags = ComponentTags;
// 		
// 		if (OverridePresetName != NAME_None)
// 		{
// 			if (UTHsPresetStuffComponent* overrideComp = FindOverridePreset(OverridePresetName))
// 			{
// 				componentClass = overrideComp->ComponentClassPath->GetClass();
// 				componentTransform = overrideComp->RelativeTransform;
// 				componentTags = overrideComp->ComponentTags;
// 			}
// 		}
// 		
// 		resultComponent = NewObject<USceneComponent>(outer, componentClass);
// 		
// 		if (resultComponent)
// 		{
// 			if (useHierarchy)
// 			{
// 				for (const TObjectPtr<UTHsPresetStuffHierarchyData>& hierChild :ComponentHierarchies)
// 				{
// 					if (USceneComponent* newScene = hierChild->CreateHierarchyComponent(
// 						stuffMgr, outer, resultComponent))
// 					{
// 						newScene->AttachToComponent(resultComponent, FAttachmentTransformRules::KeepRelativeTransform);
// 						
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return	resultComponent;
// }



bool ATHsStuffManager_desperate::ApplyOverridePropertiesForComponent(USceneComponent* targetComponent,const TMap<FName,FString>& propertyOverrides)
{
	if (targetComponent == nullptr)
	{
		return false;
	}

	TSharedPtr<FJsonObject> rootObj = MakeShareable(new FJsonObject());
	for (const auto& Elem : propertyOverrides)
	{
		// FString 값을 FJsonValue로 변환 시도
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Elem.Value);
		TSharedPtr<FJsonValue> JsonValue;
		const FString filedName = Elem.Key.ToString();
		if (FJsonSerializer::Deserialize(Reader, JsonValue) && JsonValue.IsValid())
		{
			rootObj->SetField(filedName, JsonValue);
		}
		else // Deserialize 실패 시 일반 문자열로 취급
		{
			rootObj->SetStringField(filedName, Elem.Value);
		}
	}
	FText errorMsg;
	FJsonObjectConverter::JsonObjectToUStruct(rootObj.ToSharedRef(),targetComponent->GetClass(),targetComponent,0,0,false,&errorMsg);
	if (errorMsg.IsEmpty())
	{
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTHsStuffComponentPreset::ApplyOverrideProperties Error: %s"), *errorMsg.ToString());
		return false;
		
	}
	
}

void ATHsStuffManager_desperate::CheckGlobalStuffManager()
{
	if (not gStuffManagerInstance.IsValid())
	{
		UWorld* worldContext = TIHUtils::GetTHsWorldSimple();
		if (worldContext)
		{
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(worldContext, ATHsStuffManager_desperate::StaticClass(), FoundActors);
			if (FoundActors.Num() > 0)
			{
				gStuffManagerInstance = Cast<ATHsStuffManager_desperate>(FoundActors[0]);
			}
		}
		
		checkf(gStuffManagerInstance != nullptr,TEXT("ATHsStuffManager::SpawnStuffFromPreset - No valid ATHsStuffManager instance found in the world."));
	}
}

UTHsPresetStuffActor* ATHsStuffManager_desperate::ToStuffActorPreset(const FName& newPresetName, ATHsStuff_Dep* sourceStuff)
{
	//	이거 목적이 무조건 UTHsPresetStuffActor* 화하는건가? 그럼 이거 필요없고.
	// if (UTHsPresetStuffActor* exist = FindPresetStuffActor(newPresetName))
	// {
	// 	//	
	// 	return exist;	
	// }
	
	if (sourceStuff)
	{
		UTHsPresetStuffActor* newPreset = NewObject<UTHsPresetStuffActor>(this, UTHsPresetStuffActor::StaticClass(),
			newPresetName,
			RF_Public | RF_Transactional);
		
		newPreset->PresetName = newPresetName;
		newPreset->StuffClass = sourceStuff->GetClass();
		if (ITHsHousingFamily_Dep* hf = Cast<ITHsHousingFamily_Dep>(sourceStuff))
		{
			newPreset->StuffTags = hf->GetStuffTags();
		}
		newPreset->IsRuntimePreset = true;
		newPreset->bAutoDefaultRootSceneComponent = true;
		newPreset->RootComponentPreset = UTHsPresetStuffHierarchyNode_Dep::DefaultRootSceneCompHierarchyData(this);
		return newPreset;
	}
	return nullptr;
}

UTHsPresetStuffComponent_Dep* ATHsStuffManager_desperate::ToStuffComponentPreset(const FName& newPresetName,	USceneComponent* sourceComponent)
{
	/*
	 * 
	 */
	if (sourceComponent)
	{
		
		
		UTHsPresetStuffComponent_Dep* newPreset = NewObject<UTHsPresetStuffComponent_Dep>(this, UTHsPresetStuffComponent_Dep::StaticClass(),newPresetName,
			RF_Public | RF_Transactional);
		
		newPreset->PresetName = newPresetName;
		newPreset->ComponentData = DuplicateObject( sourceComponent,this);
		newPreset->RelativeTransform = sourceComponent->GetRelativeTransform();
		
		if (ITHsHousingFamily_Dep* hf = Cast<ITHsHousingFamily_Dep>(sourceComponent))
		{
			newPreset->ComponentTags = hf->GetStuffTags();
		}
		/*
		 *	TODO
		 * 이름으로 하는건데, 이거 일단 해놓고 나중에 생각하자.
		 * 생각해옴: 클래스 이름으로 그냥 만들면 된다.
		 *	
		 *	장점: 쉬움
		 *	단점: 이름이 같은데 내용이 다를 경우가 생김
		 *		해결책: 그럴땐 node의 타입을 기본씬으로 바꾸기
		 *	이게 왜 이렇게 되는가?
		 *		1. UScene 에는 preset 정보가 없음
		 *		2. stuff -> ue component 로는 정보가 있는데, 반대는 없음
		 *		3. 개별 정보를 검증하는 프로세스가 없음
		 *	그래서
		 *		1. 없어도 됨. 이거는 그냥 객체에 불과하니깐. 만약 써주고 싶다면 해당 Scene 의 태그에 넣어주던가
		 *		2. 이것도 마찬가진데 UScene에 검색하려는 기본 컴포넌트 이름 프리셋이 있는지 검증후 넣어보는것도?
		 *		3. 개별 정보 검증 프로세스
		 *			1. 원본 정보와 달라진 정보가 있는지 검증
		 *			2. 다르면 이건 새로운 것으로 생각하고, 프리셋으로 만들어줌.
		 *			3. node 에 그냥 component 타입으로 넣던가
		 */
		
		const TArray<TObjectPtr<USceneComponent>>& childArray = sourceComponent->GetAttachChildren();
		AActor* owner = sourceComponent->GetOwner();
		
		
		for (const TObjectPtr<USceneComponent>& childComp : childArray)
		{
			
			if (owner != childComp->GetOwner())
			{
				continue;
			}
			UTHsPresetStuffHierarchyNode_Dep* newNode = ToStuffHierarchyNodePreset(childComp->GetClass()->GetFName(),childComp.Get());
			// TODO: 그냥 모든 하위 Scene 들을 
		}
	}
	return nullptr;
}

UTHsPresetStuffHierarchyNode_Dep* ATHsStuffManager_desperate::ToStuffHierarchyNodePreset(const FName& newPresetName,USceneComponent* sourceComponent)
{
	return nullptr;
}

void ATHsStuffManager_desperate::RegisterPresetStuffActor(UTHsPresetStuffActor* newPreset)
{
	if (newPreset!= nullptr && newPreset->CheckRegisterValid())
	{
		const int32 newIndex = mAllPresetDatas.Add(newPreset);
		mPresetStuffActorIndexTable.Add(newPreset->PresetName,newIndex);

		RegisterPresetStuffHierarchyNode(newPreset->RootComponentPreset);
	}
}

void ATHsStuffManager_desperate::RegisterPresetStuffHierarchyNode(UTHsPresetStuffHierarchyNode_Dep* newHierarchyNode)
{
	if (newHierarchyNode!= nullptr && newHierarchyNode->CheckRegisterValid(this))
	{
		switch (newHierarchyNode->HierarchyUseType) {
		case ETHsPresetCompHierarchyUseType::EUsePreset:
			if (newHierarchyNode->OverrideCustomComponent != nullptr)
			{
				RegisterPresetStuffComponent(
					ToStuffComponentPreset(
						newHierarchyNode->PresetComponentName, newHierarchyNode->OverrideCustomComponent));
			}
			break;
		case ETHsPresetCompHierarchyUseType::EUseCustomComponent:
			break;
		case ETHsPresetCompHierarchyUseType::EForRegisterPreset:
			RegisterPresetStuffComponent(
				ToStuffComponentPreset(
					newHierarchyNode->PresetComponentName, newHierarchyNode->OverrideCustomComponent));
			break;
		}
	}
}

void ATHsStuffManager_desperate::RegisterPresetStuffComponent(UTHsPresetStuffComponent_Dep* newPreset)
{
	if (newPreset != nullptr && newPreset->CheckRegisterValid())
	{
		const int32 newIndex = mAllPresetDatas.Add(newPreset);
		mPresetStuffComponentIndexTable.Add(newPreset->PresetName,newIndex);
	}
}

void ATHsStuffManager_desperate::MakeDefaultPresetStuffActor()
{
	if (DefaultPresetStuffActor == nullptr)
	{
		DefaultPresetStuffActor = NewObject<UTHsPresetStuffActor>(this, UTHsPresetStuffActor::StaticClass());
		
		DefaultPresetStuffActor->PresetName = FName(TEXT("Default_StuffActor"));
		DefaultPresetStuffActor->StuffClass = ATHsStuff_Dep::StaticClass();
		
		DefaultPresetStuffActor->StuffTags.AddTag(THS_Action_Trait_Attachable);
		DefaultPresetStuffActor->StuffTags.AddTag(THS_Action_Trait_Hoverable);
		DefaultPresetStuffActor->StuffTags.AddTag(THS_Action_Trait_Selectable);
		DefaultPresetStuffActor->StuffTags.AddTag(THS_Action_Trait_Movable);

		RegisterPresetStuffActor(DefaultPresetStuffActor);
		UE_LOG(LogTemp, Warning, TEXT("ATHsStuffManager::MakeStuffPreset called"));
	}
	
	
}

void ATHsStuffManager_desperate::MakeStuffPreset()
{
	MakeDefaultPresetStuffActor();

}

void ATHsStuffManager_desperate::SpawnStuffPresetEditorWidget()
{
	if (DefaultPresetStuffActor)
	{
		if (ATHsStuff_Dep* newStuff = mPresetSpawnAgentInstance->CreatePresetStuff(DefaultPresetStuffActor->PresetName,FTransform::Identity))
		{
			
		}
	}
}

void ATHsStuffManager_desperate::BeginDestroy()
{
	Super::BeginDestroy();
	gStuffManagerInstance = nullptr;
}

void UTHsFrameworkSubsystem_Dep::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	mWorldContext = &InWorld;
	
	//mWorldContext->SpawnActor(ATHsStuffManager::StaticClass());
	if (AActor* temp= mWorldContext->SpawnActor(ATHsPresetManager::StaticClass()))
	{
		if (ATHsPresetManager* presetMgr = Cast<ATHsPresetManager>(temp))
		{
			presetMgr->SetFrameworkSubsystem(this);
			UE_LOG(LogTemp, Warning, TEXT("UTHsFrameworkSubsystem::OnWorldBeginPlay - Started Preset Manager"));
			presetMgr->StartPresetManager();
		}
	}

	
}

UTHsPresetNodeAppearance* UTHsPresetNodeAppearance::GetDefaultAppearance(UObject* outer)
{
	UTHsPresetNodeAppearance* defaultAppearance = NewObject<UTHsPresetNodeAppearance>(outer);
	defaultAppearance->Name = TEXT("DefaultPresetAppearance");
	defaultAppearance->Description = TEXT("This is the default preset appearance.");
	////Script/Engine.Texture2D'/TIHHousingSystem/resources/DefaultsResources/defaultThumbnail.defaultThumbnail'
	// TODO: 페이퍼 버전을 쓸수 있는 방법을 찾기.
	//defaultAppearance->Thumbnail = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(),nullptr,TEXT("/Script/Engine.Texture2D'/TIHHousingSystem/resources/DefaultsResources/defaultThumbnail.defaultThumbnail'")));
	return defaultAppearance;
}


void UTHsPresetStuffActor::SetDefault()
{
	PresetName = NAME_None;
	StuffClass = nullptr;
	StuffTags.Reset();
	IsRuntimePreset = false;
	bAutoDefaultRootSceneComponent = true;
	RootComponentPreset = nullptr;
	if (bAutoDefaultRootSceneComponent)
	{
		RootComponentPreset = UTHsPresetStuffHierarchyNode_Dep::DefaultRootSceneCompHierarchyData();
	}
	else
	{
		RootComponentPreset = nullptr;
	}
	
}

void UTHsPresetStuffActor::TraversalHierarchyNodes(ATHsStuffManager_desperate* stuffMgr,
	TFunctionRef<void(ATHsStuffManager_desperate*, TWeakObjectPtr<UTHsPresetStuffHierarchyNode_Dep>)> procFunc)
{
	RootComponentPreset->TraversalNodes(stuffMgr,procFunc);
}

bool UTHsPresetStuffActor::VerifySettingNotPreset(bool bPropagate) const
{
	bool result = true;
	
	if (PresetName == NAME_None)
	{
		result = false;
	}
	if (StuffClass == nullptr )
	{
		result = false;
	}
	if (not bAutoDefaultRootSceneComponent)
	{
		if (RootComponentPreset == nullptr)
		{
			result = false;
		}
	}
	
	if (RootComponentPreset != nullptr && bPropagate)
	{
		result = RootComponentPreset->VerifySettingNotPreset(bPropagate);
	}
	
	return result;
}

bool UTHsPresetStuffActor::CheckRegisterValid()
{
	//	절대 이름이 NAME_None 이면 등록하면 안됨.
	if (PresetName == NAME_None)	
	{
		return false;
	}
	
	//	TSubclassOf<ATHsStuff> 이게 없으면 절대금지
	if (StuffClass == nullptr)
	{
		return false;
	}
	
	//	자동 등록도 아닌데 루트 컴포넌트 프리셋이 없으면 절대 금지
	if (not bAutoDefaultRootSceneComponent) 
	{
		if (RootComponentPreset == nullptr)
		{
			return false;
		}
	}//	else 인데 RootComponentPreset 가 있을 수 있음 그럼 그냥 덮어 씌우는거임.
	
	if (ATHsStuffManager_desperate::GetWeakStuffManagerInstance().IsValid())
	{
		ATHsStuffManager_desperate* stuffMgr = ATHsStuffManager_desperate::GetWeakStuffManagerInstance().Get();

		//	이름이 위에서 Name_none을 걸렀으니 중복만 체크하면 됨.
		if (stuffMgr->FindPresetStuffActor(PresetName) != nullptr)
		{
			return false;
		}
		
		//	자동 등록인데 루트 컴포넌트 프리셋이 없으면 기본값으로 설정
		if (bAutoDefaultRootSceneComponent && RootComponentPreset == nullptr)
		{
			// 그런데 만약 런타임 객체가 아니라 프리셋 객체라면 기본값도 못넣음.
			if (ATHsStuffManager_desperate::IsPresetAsset(this))
			{
				return false;
			}
			else
			{
				RootComponentPreset = UTHsPresetStuffHierarchyNode_Dep::DefaultRootSceneCompHierarchyData();
			}
		}
		
		//	어쨌든 위에서 다 거르고 bAutoDefaultRootSceneComponent 든 아니든 검사 시작
		if (RootComponentPreset != nullptr)
		{
			//	루트 노드를 돌면서 검사 한 결과를 리턴.
			if (not RootComponentPreset->CheckRegisterValid(stuffMgr))
			{
				//	하나라도 틀려먹었으면 false
				return false;
			}
		}
	}
	else
	{
		//	매니저가 없으면 무조건 실패
		return false;
	}
	
	return true;
}

void UTHsPresetSpawnAgent::InitPresetSpawnAgent(ATHsStuffManager_desperate* manager)
{
	if (manager == nullptr)
	{
		mIsValidAgent = false;
		return;
	}
	mOwnerManager = manager;
	mWorldContext = manager->GetWorld();
	mPresetDataAsset = manager->GetLoadedPresetDataAsset();
	if (mPresetDataAsset.IsValid())
	{
		mIsValidAgent = true;
	}
	OverrideInitPresetDataAsset(manager,mPresetDataAsset.Get());
}

ATHsStuff_Dep* UTHsPresetSpawnAgent::CreatePresetStuff(const FName& presetName, const FTransform& spawnTransform)
{
	ATHsStuff_Dep* result= nullptr;
	FName currPresetName = presetName;
	
	if (mIsValidAgent)
	{
		if (currPresetName != NAME_None)
		{
			 if (const UTHsPresetStuffActor* actorPreset = mOwnerManager->TryGetPresetStuffActorByName(currPresetName))
			 {
			 	UWorld* worldContext = mOwnerManager->GetWorld();
			 	UClass* cls = mOwnerManager->LoadClassFromPath(actorPreset->StuffClass);
			 	
			 	FActorSpawnParameters spawnParams;
			 	spawnParams.Name = FName(*FString::Printf(TEXT("Instance_%s"), *currPresetName.ToString()));
			 	
			 	spawnParams.Owner = mOwnerManager.Get();
			 	spawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
			 	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			    if (ATHsStuff_Dep* newStuff = worldContext->SpawnActor<ATHsStuff_Dep>(cls,spawnTransform,spawnParams))
			 	{
			 		result = newStuff;
			 		USceneComponent* rootComp = newStuff->GetRootComponent();
			 		if ( rootComp == nullptr && actorPreset->bAutoDefaultRootSceneComponent)
			 		{
			 			newStuff->RenewRootSceneComponent(mOwnerManager->CurrentRootComponentClass);
			 		}

			 		ITHsHousingFamily_Dep* hf = Cast<ITHsHousingFamily_Dep>(newStuff);
			 		if (hf)
			 		{
			 			hf->AddGameplayTagContainer(actorPreset->StuffTags);
			 		}

			 		mOwnerManager->RegisterSpawnedManagedStuff(newStuff);
			    	
			    	RecursiveHierarchyComponent(mOwnerManager.Get(),newStuff->GetRootComponent(),actorPreset->RootComponentPreset);

			 		if (hf)
			 		{
			 			hf->PostRegisterStuffManager(mOwnerManager.Get());
			 		}
			 	}
			 }
		}
	}
	return result;
}

USceneComponent* UTHsPresetSpawnAgent::RecursiveHierarchyComponent(ATHsStuffManager_desperate* stuffMgr,USceneComponent* parentComponent, UTHsPresetStuffHierarchyNode_Dep* hierarchyData)
{
	if (stuffMgr == nullptr || hierarchyData == nullptr)
	{
		return nullptr;
	}
	USceneComponent* parent = parentComponent != nullptr ? parentComponent: stuffMgr->GetCurrentSelectedComponent();
	USceneComponent* newComponent = nullptr;
	FName presetName = hierarchyData->PresetComponentName;
	bool childCheck = false;
	
	switch (hierarchyData->HierarchyUseType) {
	case ETHsPresetCompHierarchyUseType::EUsePreset:
		if (UTHsPresetStuffComponent_Dep* preset = stuffMgr->TryGetPresetStuffComponentByName(presetName))
		{
			if (USceneComponent* newComp = CreatePresetComponent(stuffMgr,parent,preset))
			{
				newComponent = newComp;
				childCheck = true;
			}
		}
		break;
		
	case ETHsPresetCompHierarchyUseType::EUseCustomComponent:
		if (USceneComponent* custom = hierarchyData->OverrideCustomComponent)
		{
			UClass* customCls = custom->GetClass();
			AActor* ownerActor = parent->GetOwner();
			USceneComponent* newComp = NewObject<USceneComponent>(ownerActor, customCls);
			ownerActor->AddOwnedComponent(newComp);
			newComp->RegisterComponent();
			newComp->AttachToComponent(parent,FAttachmentTransformRules::KeepRelativeTransform);
			newComponent = newComp;
			childCheck = true;
			if (hierarchyData->bUseOverrideTransform)
			{
				newComp->SetRelativeTransform(hierarchyData->OverrideTransform);
			}
		}
		break;
	case ETHsPresetCompHierarchyUseType::EForRegisterPreset:
		if (UTHsPresetStuffComponent_Dep* preset = stuffMgr->TryGetPresetStuffComponentByName(presetName))
		{
			if (USceneComponent* newComp = CreatePresetComponent(stuffMgr,parent,preset))
			{
				newComponent = newComp;
				childCheck = true;
			}
		}
		break;
	}
	
	if (childCheck)
	{
		switch ( hierarchyData->HierarchyProcType) {
		case ETHsPresetCompHierarchyProcType::EProgressive:
			RecursiveChildHierarchy(stuffMgr, hierarchyData->ChildComponents, newComponent);		
			break;
		case ETHsPresetCompHierarchyProcType::EConditional:
			if (hierarchyData->ConditionFunctor != nullptr)
			{
				if (hierarchyData->ConditionFunctor->CheckCondition(stuffMgr,parentComponent,newComponent,hierarchyData))
				{
					RecursiveChildHierarchy(stuffMgr, hierarchyData->ChildComponents, newComponent);		
				}
			}
			break;
		case ETHsPresetCompHierarchyProcType::EIgnore:
			break;
		}
		return newComponent;
	}
	return nullptr;
}

void UTHsPresetSpawnAgent::RecursiveChildHierarchy(ATHsStuffManager_desperate* stuffMgr, const TArray<TObjectPtr<UTHsPresetStuffHierarchyNode_Dep>>& nodes, USceneComponent* parentComponent)
{
	for (const TObjectPtr<UTHsPresetStuffHierarchyNode_Dep>& childInfo :nodes)
	{
		RecursiveHierarchyComponent(stuffMgr,parentComponent,childInfo);
	}
}



// UTHsPresetStuffActor* UTHsPresetWriterAgent::RecordPresetStuffActor(const ATHsStuff* sourceStuff)
// {
// 	UTHsPresetStuffActor* result = nullptr;
// 	if (mOwnerManager.IsValid() && sourceStuff != nullptr)
// 	{
// 		FString jsonString;
// 		if (FJsonObjectConverter::UStructToJsonObjectString(
// 			sourceStuff->GetClass(),
// 			static_cast<const void*>(sourceStuff),
// 			jsonString,
// 			0,0,0,
// 			nullptr,false))
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("UTHsPresetWriterAgent::RecordPresetStuffActor JSON: %s"), *jsonString);
// 		}
// 	}
// 	
// 	return result;
// }

USceneComponent* UTHsPresetSpawnAgent::CreatePresetComponent(ATHsStuffManager_desperate* stuffMgr,USceneComponent* parentComponent, const UTHsPresetStuffComponent_Dep* componentPreset)
{
	if (stuffMgr!= nullptr && componentPreset != nullptr)
	{
		/*
			Green: parentComponent
			Yellow: stuffMgr->GetCurrentSelectedComponent()
			Red: check(parent != nullptr);
		 */
		USceneComponent* parent = parentComponent != nullptr ? parentComponent: stuffMgr->GetCurrentSelectedComponent();
		
		check(parent != nullptr);

		/*
			Green: [ATHsStuff]
			Yellow: [Selected Component's Owner]
			Red: [ATHsStuffManager]
		 */
		AActor* ownerActor = parent->GetOwner();	
		
		UClass* componentClass = ATHsStuffManager_desperate::ExtractClassFromComponentPreset(componentPreset);
		
		USceneComponent* newComp = NewObject<USceneComponent>(ownerActor, componentClass);
		
		ownerActor->AddOwnedComponent(newComp);
		newComp->RegisterComponent();
		newComp->AttachToComponent(parent,FAttachmentTransformRules::KeepRelativeTransform);
		newComp->SetRelativeTransform(componentPreset->RelativeTransform);
		if(ITHsHousingFamily_Dep* hf = Cast<ITHsHousingFamily_Dep>(newComp))
		{
			hf->AddGameplayTagContainer(componentPreset->ComponentTags);
		}
		//	컴포넌트 자체에 달린 child 를 처리하는거임.
		RecursiveChildHierarchy(stuffMgr, componentPreset->ChildHierarchies, newComp);
		
		return newComp;
	}
	
	return nullptr;
}

ATHsStuffManager_desperate::ATHsStuffManager_desperate()
{
	PrimaryActorTick.bCanEverTick = false;
	mManagerRootComponent = CreateDefaultSubobject<UTHsRootSceneComponent_Dep>(TEXT("ManagerRootComponent"));
}

void ATHsStuffManager_desperate::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = GetWorld();
	
	gStuffManagerInstance = this;
	if (UObject* tryLoadedAsset = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/Script/TIHHousingCore.THsStuffPresetDataAsset'/TIHHousingSystem/resources/db/Bp_StuffPrest0.Bp_StuffPrest0'")))
	{
		if (UTHsStuffPresetDataAsset* stuffPresets = Cast<UTHsStuffPresetDataAsset>(tryLoadedAsset))
		{
			mLoadedPresetDataAsset = stuffPresets;

			StoreClonedManagedStuffs(mLoadedPresetDataAsset);
			StoreManagerInfoFromDataAsset(mLoadedPresetDataAsset);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATHsStuffManager: Failed to cast loaded asset to UTHsStuffPresetDataAsset"));
		}
	}

	// if (mLoadedPresetDataAsset != nullptr)
	// {
	// 	CurrentSelectedStuffClass = mLoadedPresetDataAsset->DefaultStuffClass;
	// 	SpawnPointerTransform = FTransform::Identity;
	// 	
	// 	TMap<FName,TScriptInterface<ITHsHousingFamily>> mComponentPresets;
	// 	
	// 	for (const TObjectPtr<UTHsStuffComponentPreset>& componentPreset : mLoadedPresetDataAsset->ComponentPresets)
	// 	{
	// 		if (UClass* compCls = componentPreset->ComponentClassPath->GetClass())
	// 		{
	// 			
	// 		}
	// 		
	// 	}
	// 	
	// }
	
	if (mPresetEditorWidget == nullptr)
	{
		const FString& PathString = TEXT("/Script/UMGEditor.WidgetBlueprint'/TIHHousingSystem/Ui/ForPreset/Bp_PresetEditorWidget.Bp_PresetEditorWidget_C'");
		FSoftClassPath widgetClassPath(PathString);
		
		if (UClass* widgetCls = widgetClassPath.TryLoadClass<UTHsPresetEditorWidget>())
		{
			mPresetEditorWidget = CreateWidget<UTHsPresetEditorWidget>(GetWorld(),widgetCls);
			if (mPresetEditorWidget)
			{
				mPresetEditorWidget->OwnerManager = this;
				mPresetEditorWidget->AddToViewport();
				UE_LOG(LogTemp, Warning, TEXT("ATHsStuffManager: Preset editor widget created and added to viewport"));
				mPresetEditorWidget->ApplyButton->OnClicked.AddDynamic(this,&ATHsStuffManager_desperate::MakeStuffPreset);
				if (UButton* btn = mPresetEditorWidget->TryGetButtonByIndex(0))
				{
					btn->OnClicked.AddDynamic(this, &ATHsStuffManager_desperate::MakeStuffPreset);
				}
				if (UButton* btn = mPresetEditorWidget->TryGetButtonByIndex(1))
				{
					btn->OnClicked.AddDynamic(this, &ATHsStuffManager_desperate::SaveCurrentPresetToJson);
				}
				
				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ATHsStuffManager: Failed to create preset editor widget instance"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATHsStuffManager: Failed to load preset editor widget class from path: %s"), *PathString);
		}
		
	}
	
}


void ATHsStuffManager_desperate::StoreClonedManagedStuffs(UTHsStuffPresetDataAsset* stuffPresets)
{
	if (stuffPresets)
	{
		const int32 clawledStuffNum = stuffPresets->StuffPresets.Num();
		const int32 wholeManagedStuffNum = clawledStuffNum + stuffPresets->RuntimePresetMaxCount;
		//mManagedStuffs.Reserve(wholeManagedStuffNum);
		for (const TObjectPtr<UTHsPresetStuffActor>& stuffPreset : stuffPresets->StuffPresets)
		{
			if (stuffPreset)
			{
				if (UTHsPresetStuffActor* clonedPreset = DuplicateObject<UTHsPresetStuffActor>(stuffPreset,this))
				{
					const FName presetName = clonedPreset->PresetName;
					if (not mPresetStuffActorIndexTable.Contains(presetName))
					{
						const int32 newIndex = mAllPresetDatas.Add(clonedPreset);;
						mPresetStuffActorIndexTable.Add(presetName,newIndex);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("ATHsStuffManager::StoreClonedManagedStuffs: Duplicate preset name %s"), *presetName.ToString());
					}
				}
				
			}
		}
		//	component preset
		for (const TObjectPtr<UTHsPresetStuffComponent_Dep>& compPreset : stuffPresets->ComponentPresets)
		{
			if (compPreset)
			{
				if (UTHsPresetStuffComponent_Dep* clonedPreset = DuplicateObject<UTHsPresetStuffComponent_Dep>(compPreset,this))
				{
					const FName presetName = clonedPreset->PresetName;
					if (not mPresetStuffComponentIndexTable.Contains(presetName))
					{
						const int32 newIndex = mAllPresetDatas.Add(clonedPreset);;
						mPresetStuffComponentIndexTable.Add(presetName,newIndex);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("ATHsStuffManager::StoreClonedManagedStuffs: Duplicate preset name %s"), *presetName.ToString());
					}
				}
			}
		}
		
	}
}
void ATHsStuffManager_desperate::StoreManagerInfoFromDataAsset(UTHsStuffPresetDataAsset* presetDataAsset)
{
	if (presetDataAsset)
	{
		mCurrentThemaName = presetDataAsset->ThemaName;
		mRuntimePresetMaxCount = presetDataAsset->RuntimePresetMaxCount;
		mDefaultStuffClass = presetDataAsset->DefaultStuffClass;
		mPresetSpawnAgentClass = presetDataAsset->PresetSpawnAgentClass;
	}
}

void UTHsStuffSearchSphere_Dep::ReCalculateSphere(const FVector& reserveWorldLocation)
{
	mValidSphere = false;
	if (mLinkedStuffRootComponent.IsValid())
	{
		if (CalculateCenterFromValidConnectPoints(mRecentLocalCenterLocation))
		{
			CalculateRadiusFromLinkedPoints(
				reserveWorldLocation,mRecentLocalCenterLocation,mCurrentMaxRadius );
			if (0.0f < mCurrentMaxRadius)
			{
				mCollisionShape = FCollisionShape::MakeSphere(mCurrentMaxRadius);
				mValidSphere = true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UTHsStuffSearchSphere::ReCalculateSphere mCurrentMaxRadius is zero or negative"));
			}
		}
	}
}

const TArray<TWeakObjectPtr<UTHsStuffConnectSurface_Dep>>& UTHsStuffSearchSphere_Dep::SearchSurfaces()
{
	GetWorld()->OverlapMultiByChannel(mRecentOverlapResults,mRecentWorldCenterLocation,FQuat::Identity,mTraceChannel,mCollisionShape,FCollisionQueryParams::DefaultQueryParam);
	mRecentSurfaces.Empty();
	for (const FOverlapResult& overlapResult : mRecentOverlapResults)
	{
		TScriptInterface<ITHsHousingFamily_Dep> castedInterface(overlapResult.GetActor());
		if (castedInterface.GetInterface() != nullptr)
		{
			mRecentSurfaces.Append(castedInterface->GetAllSurfaces());
		}
	}
	return mRecentSurfaces;
}

bool ATHsStuff_Dep::AttachToSurface(const FTHsSurfaceTraceResult& traceResult, const FTransform& targetTransform)
{
	if (not traceResult.IsValid() || not traceResult.IsAttachPossible())
	{
		UE_LOG(LogTemp, Warning, TEXT("ATHsStuff::AttachToSurface: Invalid trace result or attach not possible"));
		return false;
	}
	ATHsStuff_Dep* newParent = traceResult.BestConnectSurface->GetOwningStuff();
	ATHsStuff_Dep* child = traceResult.BestConnectPoint->GetOwningStuff();
	if (child != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATHsStuff::AttachToSurface: Mismatched owning stuff"));
	}
	
	auto linkedConnectSurface = traceResult.BestConnectSurface->GetLinkedComponentWeakPtr();
	
	child->SetActorTransform(targetTransform);
	if (child->CheckAttachable(newParent))
	{
		
		if (mParentStuff.IsValid())
		{
			mParentStuff->DetachFromSurface(this);
		}
		child->mParentStuff = TWeakObjectPtr<ATHsStuff_Dep>(newParent);
		child->AttachToComponent(linkedConnectSurface.Get(),FAttachmentTransformRules::KeepWorldTransform);
		newParent->AddChildStuff(child);
		child->OverridePostAttachProcess();
		return true;
	}

	return false;
}

bool ATHsStuff_Dep::CheckAttachable(ATHsStuff_Dep* reserveParentStuff)
{
	bool result = false;
	if (reserveParentStuff->CheckContainsChildStuff(this))
	{
		result = reserveParentStuff->GetStuffTags().HasTag(TIH_Attachable);
		/*
		 * TODO: 여기에 규칙을 넣어야함. 지금은 TIH_Attachable 이것만 넣었는데, 뭐 책상위에는 특정한 속성만 올리리 수 있다든지 그런걸 여기에 추가해주면 되는거임.
		 */
	
	}
	return result;
}

void ATHsStuff_Dep::OverridePostAttachProcess()
{
	ITHsConnectInterface_Dep::OverridePostAttachProcess();
}

void ATHsStuff_Dep::BeginDestroy()
{
	Super::BeginDestroy();

	if (mParentStuff.IsValid())
	{
		mParentStuff->DetachFromSurface(this);
	}
	TArray<TWeakObjectPtr<ATHsStuff_Dep>> array = mChildStuffs.Array();
	const int32 reverse = array.Num() - 1;
	for (int32  i = reverse; -1 < i; --i)
	{
		if (array[i].IsValid())
		{
			array[i]->Destroy();
			array[i] = nullptr;
		}
	}
}

FTHsPresetSaveSlotData_Dep& FTHsPresetSaveSlotData_Dep::operator=(const FTHsPresetRuntimeSavingSnapshot& other)
{
	SaveSlotName = other.SaveSlotData.SaveSlotName;
	SaveDateTime = other.SaveSlotData.SaveDateTime;
	SavedPresetCount = other.SaveSlotData.SavedPresetCount;
	FileVersion = other.SaveSlotData.FileVersion;
	SaveFileHash = other.SaveSlotData.SaveFileHash;
	SaveUserName = other.SaveSlotData.SaveUserName;
	SaveUserID = other.SaveSlotData.SaveUserID;
	return *this;
}

void UTHsPresetRuntimeSaveTOC::AddNewSlotData(const FTHsPresetSaveSlotData_Dep newSlotData)
{
	int32 index = SavedPresetSlots.Add(newSlotData);
	ExistingSaveSlotNameMap.FindOrAdd( newSlotData.SaveSlotName,index);
}

void UTHsPresetRuntimeSaveTOC::BeginDestroy()
{
	Super::BeginDestroy();
	ClearRuntimeSaveTOC();
	IsActivated = false;
}

void UTHsPresetRuntimeSaveTOC::OnSavePresetRuntimeSaveTOCAccept()
{
	if (IsPending) // 대기중이었다면
	{
		const FTHsPresetSaveSlotData_Dep newSlotData = PendingSaveSlotData;//	이걸 넣을거니깐 복사본을 가져온다. 사실 이거 복사한 이유가 검증하고 바꾸려고 한건데, 그걸 OverrideSlotData 여기로 넘겨서 그런지 사실 문제가 없어짐. 이거 필요없을듯. 그래도 일단 냅두자.
		//	한번 찾아봄
		if (int32* found = ExistingSaveSlotNameMap.Find(newSlotData.SaveSlotName))
		{
			//	만약 찾았는데, 유효하기까지 했다면
			if (SavedPresetSlots.IsValidIndex(*found))
			{
				//	덮어쓰기를 한다.
				SavedPresetSlots[*found].OverrideSlotData(newSlotData);
				IsPending = false;	//	pending 해제하고 
				OnPresetRuntimeSaveTOCUpdated.Broadcast(this);
				PendingSaveSlotData.Clear();
				OnPresetRuntimeSaveTOCComplete.Broadcast(this);
				return;
			}
		}
	}
	
	AddNewSlotData(PendingSaveSlotData);
	OnPresetRuntimeSaveTOCUpdated.Broadcast(this);
	OnPresetRuntimeSaveTOCComplete.Broadcast(this);
	PendingSaveSlotData.Clear();
}

void UTHsPresetRuntimeSaveTOC::OnSavePresetRuntimeSaveTOCCancel()
{
}

int32 UTHsPresetRuntimeSaveTOC::TryAsyncSavePresetSlotData(const FTHsPresetSaveSlotData_Dep& newSlotData)
{
	if (not IsPending)
	{
		PendingSaveSlotData = newSlotData;
		if (ExistingSaveSlotNameMap.Contains(newSlotData.SaveSlotName))
		{
			IsPending = true;
			OnPresetRuntimeSaveTOCOverridePending.Broadcast(this);
			return 2;
		}
		else
		{
			IsPending = true;
			OnSavePresetRuntimeSaveTOCAccept();
			return 1;
		}
	}
	return 0;
}

void UTHsPresetRuntimeSaveAgent_Dep::InitPresetRuntimeAgent(ATHsPresetManager* presetManager)
{
	RuntimeNodes.Empty();
	mPresetManager = presetManager;
	OnSetSaveAgentState_Invalid();
	mIsInitialized = false;
	if (mPresetManager != nullptr && mPresetManager->VerifyPresetMgrStoreReady())
	{
		//	최초의 TOC는 매니저의 DataAsset에서 가져온다.
		mLoadedSaveTocName = mPresetManager->GetRuntimePresetTocName();
		mRuntimePresetCount = mPresetManager->GetRuntimePresetMaxCount();
		if (mLoadedSaveTocName.IsEmpty())
		{
			mLoadedSaveTocName = TEXT("DefaultRuntimePresetTOC");
		}
		if (mRuntimePresetCount < 1)
		{
			mRuntimePresetCount = 128;
		}
		RuntimeNodes.Reserve(mRuntimePresetCount);
		//	이거 세이브 목록을 불러오는거임. 초기화 하고 불러오는거라 reload도 고려해야함
		if (mLoadedTOC == nullptr)
		{
			const FString& tocName = mLoadedSaveTocName;
			const int32 slotIndex = 0;
			if (UGameplayStatics::DoesSaveGameExist(tocName,slotIndex))
			{
				if (UTHsPresetRuntimeSaveTOC* loadedTOC = Cast<UTHsPresetRuntimeSaveTOC>(UGameplayStatics::LoadGameFromSlot(tocName,slotIndex)))
				{
					mLoadedTOC = loadedTOC;
					if (mLoadedTOC)
					{
						mLoadedTOC->InitRuntimeSaveTOC();//	save 목록을 불러옴
					}
				}
			}
			else // at first time
			{
				NewTOC();
				UGameplayStatics::SaveGameToSlot(mLoadedTOC,tocName,slotIndex);
				DisplayMessage = TEXT("UTHsPresetRuntimeSaveAgent::InitPresetRuntimeAgent: Created Default and !First! TOC save game slot. name is ") + tocName;
			}
		}
		else
		{
			/*
			 * todo: InitPresetRuntimeAgent 이게 다시 불리는게 맞는지 검증후 리로드 프로세스 짜기
			 */
		}
		mIsInitialized = true;
		OnSetSaveAgentState_Waiting();
	}
	else
	{
		DisplayMessage = TEXT("UTHsPresetRuntimeSaveAgent::InitPresetRuntimeAgent: Preset Manager is not ready.");
	}
}



void UTHsPresetRuntimeSaveAgent_Dep::AsyncLoadPresetRuntimeSaveTOCBegin(const FTHsPresetRuntimeLoadingSnapshot& loadingSnapshot)
{
	CurrentLoadingSnapshot = loadingSnapshot;
	if (IsReadyToSaveLoad())
	{
		OnSetSaveAgentState_Saving();
		if (not RuntimeNodes.IsEmpty())
		{
			BroadcastPresetRuntimeBlocking();
		}
		else
		{
			if (LoadSaveGameSlot(loadingSnapshot.SaveSlotIndex,loadingSnapshot.UserIndex))
			{
				BroadcastPresetRuntimeLoadedComplete();
			}
			else
			{
				BroadcastPresetRuntimeLoadedFailed();
			}
			OnSetSaveAgentState_Waiting();
		}
	}
	else
	{
		DisplayMessage = TEXT("UTHsPresetRuntimeSaveAgent::AsyncLoadPresetRuntimeSaveTOCBegin: Not ready to save/load.");
	}
}

void UTHsPresetRuntimeSaveAgent_Dep::OnLoadPresetRuntimeSaveTOCAccept()
{
}

void UTHsPresetRuntimeSaveAgent_Dep::OnLoadPresetRuntimeSaveTOCCancel()
{
}


bool UTHsPresetRuntimeSaveAgent_Dep::LoadSaveGameSlot(int32 saveSlotIndex,int32 userIndex)
{
	mIsLoadedRuntimePresets = false;
	
	if (IsReadyToSaveLoad() )
	{
		if (mLoadedTOC->IsExistSaveSlotData())
		{
			if (FTHsPresetSaveSlotData_Dep* loadedSlotData = mLoadedTOC->GetSavedPresetSlotDataAt(saveSlotIndex))
			{
				CurrentSelectedSlotIndex = saveSlotIndex;
				CurrentSelectedUserIndex = userIndex;
			
				if(UTHsPresetRuntimeSaveAgent_Dep* saveGame = Cast<UTHsPresetRuntimeSaveAgent_Dep>(UGameplayStatics::LoadGameFromSlot( loadedSlotData->SaveSlotName,userIndex)))
				{
					mLoadedFileVersion = loadedSlotData->FileVersion;
					if (DeepCopyRuntimePresets(saveGame))
					{
						RecentLoadedFileHash = loadedSlotData->SaveFileHash;
						
						mIsLoadedRuntimePresets = true;
					}
				}
			}
		}
	}
	else
	{
		DisplayMessage = TEXT("UTHsPresetRuntimeSaveAgent::LoadSaveGameSlot: Not ready to save/load.");
	}
	return mIsLoadedRuntimePresets;
}

void UTHsPresetRuntimeSaveAgent_Dep::SavePreset()
{
	//	그냥 일단 저장을 시도한다. 
	if (IsReadyToSaveLoad())	//	저장이 가능한지 지금 다른걸 하고 있는지 확인한다.
	{
		UpdateSaveSnapshotData();	//	현재상태를 저장할거다
		FTHsPresetSaveSlotData_Dep tempSlotData = {};
		tempSlotData = SaveSnapshot;
		if (mLoadedTOC == nullptr)	//	TOC가 없으면 그냥 새로 만든다.
		{
			NewTOC();	//	사실 있으면 ClearRuntimeSaveTOC 를 해서 클리어한다.
		}
		//
		int32 result = mLoadedTOC->TryAsyncSavePresetSlotData(tempSlotData);
		switch (result)//	누가 OnPresetRuntimeSaveTOCComplete 를 호출해야함.
		{
		case 0:	//	not accepted
			
			break;
		case 1:	//	write accepted
			mLoadedTOC->OnSavePresetRuntimeSaveTOCAccept();
			OnSetSaveAgentState_Waiting();
			break;
		case 2:	//	pending override
			OnSetSaveAgentState_Saving();
			break;
		default:
		
			break;
		}
	}
	else
	{
		DisplayMessage = TEXT("UTHsPresetRuntimeSaveAgent::SavePreset: Not ready to save/load.");
	}
}



bool UTHsPresetNodeBuilder::IsValidBuilder() const
{
	return TargetNode.IsValid() && PresetManager.IsValid() && WorldContext != nullptr;
}

UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::BeginBuild(ATHsPresetManager* InPresetManager, UTHsPresetNode* targetNode)
{
	PresetManager = InPresetManager;
	TargetNode = targetNode;
	if (targetNode)
	{
		targetNode->SetDefault();
	}
	WorldContext = InPresetManager ? InPresetManager->GetWorld() : nullptr;
	return *this;
}
UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::OnNodeNameTypeNumbering(const FString& inPrefix)
{
	if (IsValidBuilder())
	{
		FString baseName = inPrefix + PresetManager->CurrentDataAssetThemaName.ToString();
		switch (TargetNode->NodeType) {
		case ETHsPresetNodeType_Dep::EJustNode:
			break;
		case ETHsPresetNodeType_Dep::EGroup:
			baseName += TEXT("_Group_") + FString::FromInt(PresetManager->GetGroupPresetNodeCount());
			break;
		case ETHsPresetNodeType_Dep::EActor:
			baseName += TEXT("_Actor_") + FString::FromInt(PresetManager->GetActorPresetNodeCount());
			break;
		case ETHsPresetNodeType_Dep::EComponent:
			baseName += TEXT("_Component_") + FString::FromInt(PresetManager->GetComponentPresetNodeCount());
			break;
		}
		SetNodeName(FName(*baseName));
	}
	return *this;
}

UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::OnNodeNameCheckOverlapNumbering(const FString& presetName)
{
	if (IsValidBuilder())
	{
		TWeakObjectPtr<UTHsPresetNode> preset = PresetManager->GetPresetNodeWeakByName(FName(*presetName));
		if (preset.IsValid())
		{
			int32 nodeCount =PresetManager->GetPresetNodeCountByType(preset->NodeType);
			if (0 < nodeCount)
			{
				 SetNodeName(FName( *(presetName + FString::FromInt(nodeCount))));
			}
			// else <- 이거 말도 안되는거임. GetPresetNodeWeakByName 이걸로 검색도 안되는데, nodeCount 가 0 이라니. 문맥상 안맞아서 고치지는 않는데, 이러면 다시 새로 등록을 해줘야함.
		}
		else
		{
			SetNodeName(FName(*presetName));
		}
	}
	return *this;
}


UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::OnNodeTag_ActionTraitAttachable()
{
	if (IsValidBuilder())
	{
		OnNodeTagsAdd(THS_Action_Trait_Attachable);
	}
	return *this;
}

UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::OffNodeTag_ActionTraitAttachable()
{
	if (IsValidBuilder())
	{
		OnNodeTagsRemove(THS_Action_Trait_Attachable);
	}
	return *this;
}

UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::OnNodeTag_ActionTraitHoverable()
{
	if (IsValidBuilder())
	{
		OnNodeTagsAdd(THS_Action_Trait_Hoverable);
	}
	return *this;
}

UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::OffNodeTag_ActionTraitHoverable()
{
	if (IsValidBuilder())
	{
		OnNodeTagsRemove(THS_Action_Trait_Hoverable);
	}
	return *this;
}

UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::OnNodeTag_ActionTraitSelectable()
{
	if (IsValidBuilder())
	{
		OnNodeTagsAdd(THS_Action_Trait_Selectable);
	}
	return *this;
}

UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::OffNodeTag_ActionTraitSelectable()
{
	if (IsValidBuilder())
	{
		OnNodeTagsRemove(THS_Action_Trait_Selectable);
	}
	return *this;
}

UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::OnNodeTag_ActionTraitMovable()
{
	if (IsValidBuilder())
	{
		OnNodeTagsAdd(THS_Action_Trait_Movable);
	}
	return *this;
}

UTHsPresetNodeBuilder& UTHsPresetNodeBuilder::OffNodeTag_ActionTraitMovable()
{
	if (IsValidBuilder())
	{
		OnNodeTagsRemove(THS_Action_Trait_Movable);
	}
	return *this;
}

UTHsPresetNode* UTHsPresetNodeBuilder::DefaultPresetNodeActorType(UObject* outer)
{
	if (ATHsPresetManager::GetPresetManagerInstance().IsValid())
	{
		
		ATHsPresetManager* presetMgr = ATHsPresetManager::GetPresetManagerInstance().Get();
		
		UTHsPresetNode* newNode = NewObject<UTHsPresetNode>(outer);
		TObjectPtr<UTHsPresetNodeBuilder> tempBuilder = UTHsPresetNodeBuilder::CreatePresetNodeBuilder(presetMgr);
		
		 bool result = tempBuilder->BeginBuild(presetMgr, newNode)
		.OnActorType()
		.OnNodeNameTypeNumbering(TEXT("Preset"))
		.OnOptSetRuntimePreset()
		.OnOptUseDefaultRootScene()
		.OnNodeTagAddAllActionTraits()
		.FinalizeBuild();

		tempBuilder = nullptr;
		
		if (result)
		{
			return newNode;
		}
		else
		{
			newNode = nullptr;
		}
	}
	return nullptr;
}

ATHsPresetManager::ATHsPresetManager()
{
	PrimaryActorTick.bCanEverTick = false;
	mDefaultRootScene = CreateDefaultSubobject<UTHsRootSceneComponent_Dep>(TEXT("PresetManagerRootScene"));
	SetRootComponent(mDefaultRootScene);
}

void ATHsPresetManager::StartPresetManager()
{
	UE_LOG(LogTemp, Warning, TEXT("ATHsPresetManager::StartPresetManager called"));
}


void ATHsPresetManager::BeginDestroy()
{
	Super::BeginDestroy();
	if (gPresetManagerInstance.IsValid())
	{
		gPresetManagerInstance = nullptr;
	}
}

bool ATHsPresetManager::LoadDefaultPresets(const FSoftObjectPath& assetPath)
{
	mIsLoadedDataAsset = false;
	

	
	if ( assetPath != DefaultPresetDataAssetPath)
	{
		if (assetPath.IsValid() && assetPath.IsAsset())
		{
			DefaultPresetDataAssetPath = assetPath;
			if (UObject* loadObj= assetPath.TryLoad())
			{
				if (UTHsPresetDataAsset_Dep* presetDataAsset = Cast<UTHsPresetDataAsset_Dep>(loadObj))
				{
					mLoadedPresetDataAsset = presetDataAsset;
				}
			}
		}
	}
	return VerifyLoadedDataAsset();
	
}

void ATHsPresetManager::ApplyPresetManagerSettingsFromDataAsset()
{
	if (IsValid(mLoadedPresetDataAsset))
	{	
		CurrentDataAssetThemaName = mLoadedPresetDataAsset->ThemaName;
		CurrentSpawnTransform= mLoadedPresetDataAsset->DefaultSpawnTransform;
		DefaultStuffClass = mLoadedPresetDataAsset->DefaultStuffClass;
		DefaultRootComponentClass = mLoadedPresetDataAsset->DefaultRootSceneComponentClass;
		RuntimePresetSaveTocName = mLoadedPresetDataAsset->RuntimePresetSaveTocName;
		ReserveRuntimePresetCount = mLoadedPresetDataAsset->RuntimePresetCount;
	}
	VerifyAppliedPresetManagerSettings();
}

void ATHsPresetManager::StoreDefaultPresets()
{
	mIsStoredDefaultPresets = false;
	if (IsValid(mLoadedPresetDataAsset))
	{
		mPresetNameIndexTable.Append(
			GetAllPresetMetaDataMap(
				mLoadedPresetDataAsset->AllPresetNodesByType,
				true) );
		mIsStoredDefaultPresets = true;
	}
}

void ATHsPresetManager::StoreRuntimePresets()
{
	if (mRuntimePresetAgent == nullptr)
	{
		mRuntimePresetAgent = NewObject<UTHsPresetRuntimeSaveAgent_Dep>(this);
		mRuntimePresetAgent->InitPresetRuntimeAgent(this);
	}

	if (mRuntimePresetAgent->IsReadyToSaveLoad())
	{
		FTHsPresetRuntimeLoadingSnapshot loadingSnapshot = {};
		loadingSnapshot.SaveSlotIndex =
				(CurrentSelectedSaveSlotIndexPtr!= nullptr) ?
					*CurrentSelectedSaveSlotIndexPtr:
					mRuntimePresetAgent->CurrentSelectedSlotIndex;
			
		loadingSnapshot.UserIndex =
			(CurrentSelectedUserIndexPtr!= nullptr) ?
				*CurrentSelectedUserIndexPtr:
				mRuntimePresetAgent->CurrentSelectedUserIndex;
		
		if (mLoadedPresetDataAsset->bUseAsyncLoadForRuntimePresets)
		{
			SnapshotWidgetData();
			mRuntimePresetAgent->AsyncLoadPresetRuntimeSaveTOCBegin(loadingSnapshot);
		}
		else
		{
			if (mRuntimePresetAgent->LoadSaveGameSlotBySnapshot(loadingSnapshot))
			{
				mRuntimePresetAgent->BroadcastPresetRuntimeLoadedComplete();
				//mRuntimePresetAgent->OnPresetRuntimeLoadedCompleteInnerCallBack.Execute(this);
				//mRuntimePresetAgent->OnPresetRuntimeLoadedCompleteCallBack.Broadcast(mRuntimePresetAgent);
			}
			else
			{
				DisplayMessage = TEXT("ATHsPresetManager::StoreRuntimePresets: Failed to load runtime presets.");
				mRuntimePresetAgent->BroadcastPresetRuntimeLoadedFailed();
			}
		}
	}
}

UTHsPresetNode* ATHsPresetManager::CreatePresetNode(const FName& presetName, ETHsPresetNodeType_Dep nodeType)
{
	UTHsPresetNode* newNode = NewObject<UTHsPresetNode>(this);
	TObjectPtr<UTHsPresetNodeBuilder> tempBuilder = UTHsPresetNodeBuilder::CreatePresetNodeBuilder(this);
	bool result = tempBuilder->BeginBuild(this,newNode)
	.SetNodeType(nodeType)
	.OnOptSetRuntimePreset()
	.OnOptEditingMode()
	.OnNodeNameCheckOverlapNumbering(presetName.ToString())
	.OnNodeTagAddAllActionTraits()
	.FinalizeBuild();
	if (result)
	{
		CurrentSelectedPresetNode = newNode;
		return newNode;
	}
	newNode = nullptr;
	return nullptr;
}

ATHsStuff_Dep* ATHsPresetManager::SpawnPresetStuffActorByPresetName(const FName& presetName,const FTransform& spawnTransform)
{
	ATHsStuff_Dep* result = nullptr;

	

	return result;
}

TMap<FName, FTHsPresetMetaData_Dep> ATHsPresetManager::GetAllPresetMetaDataMap(
	const TMap<ETHsPresetNodeType_Dep, FTHsPresetNodeCollection>& allPresetNodesByType, const bool isDefaultAsset)
{
	TMap<FName,FTHsPresetMetaData_Dep> result;
	constexpr int32 start =  static_cast<int32>(ETHsPresetNodeType_Dep::EJustNode);
	constexpr int32 max = static_cast<int32>(ETHsPresetNodeType_Dep::EMax);
	int32 reserveCount = 0;
	TArray<const FTHsPresetNodeCollection*> allCollections;
	allCollections.SetNum(max);
		
	for (int32 i = start; i < max; ++i)
	{
		allCollections[i] = nullptr;
		if (const FTHsPresetNodeCollection* found = allPresetNodesByType.Find(ToFlags<ETHsPresetNodeType_Dep>(i)))
		{
			allCollections[i] = found;
			reserveCount += found->PresetNodes.Num();
		}
	}
	result.Reserve(reserveCount);
	for (const FTHsPresetNodeCollection* collection :allCollections)
	{
		if (collection == nullptr)
		{
			continue;
		}
		int32 index = 0;
		for (const TObjectPtr<UTHsPresetNode>& node : *collection)
		{
			if (node->IsOptEditingMode())
			{
				FTHsPresetMetaData_Dep metaData = {};
				metaData.PresetType = node->NodeType;
				metaData.PresetIndex = index;
				metaData.bIsDefaultAsset = isDefaultAsset;
				result.Add(node->PresetName,metaData);
			}
			++index;
		}
	}
	return result;
}

void ATHsPresetManager::BeginPlay()
{
	Super::BeginPlay();
	//	default
	if (gPresetManagerInstance != nullptr)
	{
		checkNoEntry();
	}
	gPresetManagerInstance = this;

	
	UE_LOG(LogTemp, Warning, TEXT("ATHsPresetManager::BeginPlay called"));
	
	if (LoadDefaultPresets( FSoftObjectPath(TEXT("/Script/TIHHousingCore.THsPresetDataAsset'/TIHHousingSystem/resources/db/Bp_StuffPreset1.Bp_StuffPreset1'") )))
	{
		ApplyPresetManagerSettingsFromDataAsset();
		if (VerifyPresetMgrStoreReady())
		{
			StoreDefaultPresets();
			StoreRuntimePresets();
		}
		mFrameworkSubsystem->OnAfterStorePreset.Broadcast(this);
	}
	
	
}