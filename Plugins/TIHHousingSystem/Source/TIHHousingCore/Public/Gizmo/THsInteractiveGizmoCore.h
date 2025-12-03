// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionSystem/THsEnhancedActionSystem.h"
#include "BaseGizmos/GizmoArrowComponent.h"
#include "BaseGizmos/GizmoLineHandleComponent.h"
#include "BaseGizmos/GizmoRectangleComponent.h"
#include "Cores/TIHHsCoreInterface.h"
#include "UObject/Object.h"
#include "THsInteractiveGizmoCore.generated.h"

/**
 * 
 */


class UGizmoCircleComponent;
class UGizmoRectangleComponent;
class UGizmoLineHandleComponent;
class UGizmoArrowComponent;



UCLASS()
class TIHHOUSINGCORE_API UTHsInteractiveGizmoCore : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class ATHsInteractiveGizmoActor : public AActor,public ITHsInteractiveGizmoAPI
{
	GENERATED_BODY()

public:
	
};

USTRUCT()
struct FTHsInteractiveGizmoBucketDescriptor
{
	GENERATED_BODY()

	UPROPERTY()
	int32 CurrentInstanceCount = 0;
	
	TFunction<ITHsInteractiveGizmoAPI*(class ATHsInteractiveGizmoManager*)> OnInteractiveGizmoActorCreated;
	//	
};

UCLASS(Blueprintable,EditInlineNew, DefaultToInstanced)
class UTHsInteractiveGizmoCreateDescriptor : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bUsePooling = true;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bUseOverrideCreatedMethod = false;	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName GizmoName = NAME_None;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MaxInstanceCount = 1;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AActor> GizmoActorClass = nullptr;

	TFunction<ITHsInteractiveGizmoAPI*(class ATHsInteractiveGizmoManager*)> OnInteractiveGizmoActorCreated;
	
	virtual ITHsInteractiveGizmoAPI* OnInteractiveGizmoActorCreatedMethod(class ATHsInteractiveGizmoManager* gizmoManager)
	{
		return nullptr;
	}
	
};


USTRUCT(Blueprintable)
struct FInteractiveGizmoDefaultActorData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName GizmoName = NAME_None;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UTHsInteractiveGizmoAPI> GizmoActorClass = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced)
	UTHsInteractiveGizmoCreateDescriptor* Descriptor = nullptr;
};

UCLASS()
class UTHsInteractiveGizmoCreatorSetting : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced)
	TArray<UTHsInteractiveGizmoCreateDescriptor*> GizmoCreators;
};





UCLASS()
class UTHsInteractiveGizmoBridge : public UObject
{
	GENERATED_BODY()

public:
	bool IsConnected() const
	{
		if (mGizmoObject != nullptr && mTargetObject != nullptr)
		{
			return true;
		}
		return false;
	}
	bool ConnectToTarget(const TScriptInterface<ITIHHsInteraction>& Target)
	{
		if (Target == nullptr)
		{
			if (Target.GetObject() == nullptr )
			{
				UE_LOG(LogTemp, Warning, TEXT("ITHsInteractiveGizmoBridge::ConnectToTarget - Target is null."));
				return false;
			}
		}
		mTargetObject = Target;
		mTargetObject.SetInterface(Cast<ITIHHsInteraction>(Target.GetObject()));
		if (mTargetObject == nullptr)
		{
			mTargetObject = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("ITHsInteractiveGizmoBridge::ConnectToTarget - Target does not implement ITIHHsInteraction interface."));
			return false;
		}
		mGizmoObject->ActiveGizmo(Target);
		mTargetObject->OverrideConnectGizmo(mGizmoObject);
		UE_LOG(LogTemp, Log, TEXT("ITHsInteractiveGizmoBridge::ConnectToTarget - Successfully connected gizmo to target."));
		return true;
	}
	void DisconnectFromTarget()
	{
		if (mTargetObject != nullptr)
		{
			mTargetObject->OverrideConnectGizmo(nullptr);
			mTargetObject = nullptr;
		}
		if (mGizmoObject != nullptr)
		{
			mGizmoObject->DeActiveGizmo();
		}
		mGizmoObject = nullptr;
	}
	TScriptInterface<ITHsInteractiveGizmoAPI>& GetGizmoObject()
	{
		return mGizmoObject;
	}
	TScriptInterface<ITIHHsInteraction>& GetTargetObject()
	{
		return mTargetObject;
	}
	void InitBridge(ITHsInteractiveGizmoAPI* gizmo)
	{
		mGizmoObject.SetInterface(gizmo);
		mGizmoObject.SetObject(Cast<UObject>(gizmo));
		mGizmoObject->DeActiveGizmo();
	}
	void SetCurrentActivateIdx(int32 idx)
	{
		mCurrentActivateIdx = idx;
	}
	int32 GetCurrentActivateIdx() const
	{
		return mCurrentActivateIdx;
	}

	virtual void BeginDestroy() override
	{
		Super::BeginDestroy();
		UE_LOG(LogTemp, Log, TEXT("UTHsInteractiveGizmoBridge::BeginDestroy - Cleaning up gizmo bridge."));
		DisconnectFromTarget();
	}

private:
	//	instance를 가지지는 않음.
	UPROPERTY()
	TScriptInterface<ITHsInteractiveGizmoAPI> mGizmoObject;
	UPROPERTY()
	TScriptInterface<ITIHHsInteraction> mTargetObject;
	UPROPERTY()
	int32 mCurrentActivateIdx = -1;	
};

UCLASS()
class UTHsInteractiveGizmoBridgeBucket : public UObject
{
	GENERATED_BODY()
	friend class ATHsInteractiveGizmoManager;
public:
	void InitBucket(UClass* gizmoCls,ATHsInteractiveGizmoManager* gizmoManager)
	{
		mGizmoClass = gizmoCls;
		mGizmoManager = gizmoManager;
	}
	
	
	void DeActivateInteractiveGizmo(TScriptInterface<ITHsInteractiveGizmoAPI> taget);
	
	UClass* GetGizmoClass() const
	{
		return mGizmoClass;
	}

	void ForEachActiveBridge(TFunctionRef<void(UTHsInteractiveGizmoBridge*)> func)
	{
		for (const TWeakObjectPtr<UTHsInteractiveGizmoBridge>& activatedBridge : mActiveBridges)
		{
			if (UTHsInteractiveGizmoBridge* activatedBridgePtr = activatedBridge.Get())
			{
				if (IsValid(activatedBridgePtr) && activatedBridgePtr->IsConnected())
				{
					func(activatedBridgePtr);
				}
			}
		}
	}

	int32 GetBucketIndex() const
	{
		return mBucketIndex;
	}

	UTHsInteractiveGizmoBridge* CreateEmptyBridge();

	
private:
	void SetBucketIndex(int32 idx)
	{
		mBucketIndex = idx;
	}

	TScriptInterface<ITHsInteractiveGizmoAPI>  ActivateInteractiveGizmoBucket(TScriptInterface<ITIHHsInteraction> target,int32& outActiveBridgeIdx);
	
	void SortDisconnectedGizmo();

public:
	virtual void BeginDestroy() override
	{
		Super::BeginDestroy();
		UE_LOG(LogTemp, Log, TEXT("UTHsInteractiveGizmoBridgeBucket::BeginDestroy - Cleaning up gizmo bridges."));
	}
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UTHsInteractiveGizmoBridge>> mBridgePool;
	
	UClass* mGizmoClass = nullptr;

	TArray<TWeakObjectPtr<UTHsInteractiveGizmoBridge>> mActiveBridges;
	
	TWeakObjectPtr<class ATHsInteractiveGizmoManager> mGizmoManager = nullptr;

	UPROPERTY()
	int32 mDisConnectedGizmoSortCount = 0;
	UPROPERTY()
	int32 mDisConnectedGizmoSortMax = 8;
	UPROPERTY()
	int32 mBucketIndex = -1;
	
};

USTRUCT()
struct FTHsInteractiveGizmoHandle
{
	GENERATED_BODY()
	
	ATHsInteractiveGizmoManager* InteractiveGizmoManager = nullptr;
	int32 BridgeBucketIndex = -1;
	int32 AciveBridgeIndex = -1;
	
	bool IsValidHandle()const
	{
		bool result = true;
		result &= InteractiveGizmoManager != nullptr;
		result &= BridgeBucketIndex > -1;
		result &= AciveBridgeIndex > -1;
		return result;
	}
	TScriptInterface<ITHsInteractiveGizmoAPI> TryGetGizmo() const;
	TScriptInterface<ITIHHsInteraction> TryGetTarget() const;
	TWeakObjectPtr<UTHsInteractiveGizmoBridge> TryGetBridge() const;

	static FTHsInteractiveGizmoHandle InvalidHandle()
	{
		return FTHsInteractiveGizmoHandle{nullptr,-1,-1};
	}
};

namespace TIHUtils
{
	namespace Gizmo
	{
		inline bool RayPlaneLocal(
			const FVector& rayOriginLocal,
			const FVector& rayDirectionLocalNormalized,
			const FVector& planeCenterLocal,
			const FVector& planeNormalLocalNormalized,
			float& outRayTLocal,
			FVector& outHitPointLocal)
		{
			// 평면 방정식: (P - P0) · N = 0
			const float denom = FVector::DotProduct(planeNormalLocalNormalized, rayDirectionLocalNormalized);
			if (FMath::Abs(denom) < KINDA_SMALL_NUMBER)
			{
				return false; // 평면과 광선이 평행함
			}
			// t = (P0 - O) · N / (D · N)
			// P0: 평면 위의 한 점 (planeCenterLocal) : 보통은 평면의 중심점, 사실 아무점이나 상관없음 대신 평면 위에 있어야 함
			// O: 광선의 시작점 (rayOriginLocal) : 보통은 카메라 위치
			// D: 광선의 방향 (rayDirectionLocalNormalized) : 보통은 카메라의 시선 방향
			// P: 광선과 평면의 교차점
			// t: 광선의 시작점에서 교차점까지의 거리
			// D · N: 광선 방향과 평면 법선 벡터의 내적
			// denom : denom 의 원래 의미는 denominator (분모) D · N
			
			const float t = FVector::DotProduct(planeNormalLocalNormalized, planeCenterLocal - rayOriginLocal) / denom;
			if (t < 0)
			{
				return false;
			}
			outRayTLocal = t;
			outHitPointLocal = rayOriginLocal + t * rayDirectionLocalNormalized;
			return true;
		}

		//	oriented bounding box
		inline bool RayOBBLocal(
			const FVector& rayOriginLocal	/* ray 의 시작점인데, 보통은 카메라 위치 */,
			const FVector& rayDirectionLocalNormalized/*  */,
			const FVector& obbHalfExtentsLocal/**/,
			float& outRayTLocal/**/)
		{
			float tMin = 0.0f;
			float tMax = TNumericLimits<float>::Max();

			for (int axis = 0; axis < 3; ++axis)
			{
				const float origin = rayOriginLocal[axis];
				const float direction = rayDirectionLocalNormalized[axis];
				const float extent = obbHalfExtentsLocal[axis];

				if (FMath::Abs(direction) < 1e-6f)
				{
					if (origin < -extent || origin > extent)
						return false;
				}
				else
				{
					float t1 = (-extent - origin) / direction;
					float t2 = ( extent - origin) / direction;
					if (t1 > t2)
					{
						//	swap
						const float tmp = t1;
						t1 = t2;
						t2 = tmp;
					}

					tMin = FMath::Max(tMin, t1);
					tMax = FMath::Min(tMax, t2);
					if (tMin > tMax) return false;
				}
			}

			outRayTLocal = tMin;
			return tMax >= FMath::Max(tMin, 0.0f);
		}

		// Ray-Capsule using segment [segmentStartLocal, segmentEndLocal] and radiusLocal
		inline bool RayCapsuleLocal(
			const FVector& rayOriginLocal,
			const FVector& rayDirectionLocalNormalized,
			const FVector& segmentStartLocal,
			const FVector& segmentEndLocal,
			float capsuleRadiusLocal,
			float& outRayTLocal)
		{
			const FVector segmentVector = segmentEndLocal - segmentStartLocal;
			const FVector originToStart = rayOriginLocal - segmentStartLocal;

			const float dirDotDir = 1.0f; // rayDirectionLocalNormalized · itself
			const float dirDotSeg = FVector::DotProduct(rayDirectionLocalNormalized, segmentVector);
			const float segDotSeg = FVector::DotProduct(segmentVector, segmentVector);
			const float dirDotW0  = FVector::DotProduct(rayDirectionLocalNormalized, originToStart);
			const float segDotW0  = FVector::DotProduct(segmentVector, originToStart);

			const float denom = dirDotDir * segDotSeg - dirDotSeg * dirDotSeg;

			float rayParamS, segParamT;
			if (denom > 1e-6f)
			{
				rayParamS = (dirDotSeg * segDotW0 - segDotSeg * dirDotW0) / denom;
				segParamT = (dirDotDir * segDotW0 - dirDotSeg * dirDotW0) / denom;
			}
			else
			{
				// 거의 평행
				segParamT = (segDotSeg > 1e-6f) ? (segDotW0 / segDotSeg) : 0.0f;
				rayParamS = dirDotW0 + dirDotSeg * segParamT;
			}

			// 세그먼트 클램프 후 다시 레이 파라미터 보정
			segParamT = FMath::Clamp(segParamT, 0.0f, 1.0f);
			rayParamS = FVector::DotProduct(rayDirectionLocalNormalized,
				(segmentStartLocal + segParamT * segmentVector) - rayOriginLocal);
			rayParamS = FMath::Max(rayParamS, 0.0f);

			const FVector closestOnRay = rayOriginLocal + rayParamS * rayDirectionLocalNormalized;
			const FVector closestOnSeg = segmentStartLocal + segParamT * segmentVector;

			const float distanceSquared = FVector::DistSquared(closestOnRay, closestOnSeg);
			if (distanceSquared > capsuleRadiusLocal * capsuleRadiusLocal)
				return false;

			outRayTLocal = rayParamS;
			return true;
		}

		// Ray-Annulus (ring on a plane): radii in local space
		inline bool RayAnnulusLocal(
			const FVector& rayOriginLocal,
			const FVector& rayDirectionLocalNormalized,
			const FVector& ringCenterLocal,
			const FVector& ringPlaneNormalLocalNormalized,
			float ringInnerRadiusLocal,
			float ringOuterRadiusLocal,
			float& outRayTLocal)
		{
			FVector hitOnPlaneLocal;
			float tLocal;
			if (!RayPlaneLocal(rayOriginLocal, rayDirectionLocalNormalized,
				ringCenterLocal, ringPlaneNormalLocalNormalized,
				tLocal, hitOnPlaneLocal))
			{
				return false;
			}

			// 반지름: 평면 성분으로 계산(수치적 안정성)
			const FVector toPoint = hitOnPlaneLocal - ringCenterLocal;
			const FVector planarVector =
				toPoint - FVector::DotProduct(toPoint, ringPlaneNormalLocalNormalized) * ringPlaneNormalLocalNormalized;

			const float radiusAtHit = planarVector.Size();
			if (radiusAtHit < ringInnerRadiusLocal || radiusAtHit > ringOuterRadiusLocal)
				return false;

			outRayTLocal = tLocal;
			return true;
		}

		//	#### Gizmo Primitives Raycast Methods ####
		// Arrow = capsule shaft (+X) + tip as small OBB near the end
		inline bool RaycastArrow(
			const FTransform& gizmoTransform,
			const FVector& rayOriginWorld,
			const FVector& rayDirectionWorldNormalized,
			float shaftLengthLocal,
			float shaftRadiusLocal,
			float tipLengthLocal,
			const FVector& tipBoxHalfExtentsLocal,
			float& outRayTWorld,
			FVector& outHitPointWorld)
		{
			const FTransform inverseTransform = gizmoTransform.Inverse();
			const FVector rayOriginLocal    = inverseTransform.TransformPosition(rayOriginWorld);
			// ↓ 스케일까지 포함해 역변환 (정규화는 유지)
			const FVector rayDirectionLocal = gizmoTransform.InverseTransformVector(rayDirectionWorldNormalized).GetSafeNormal();

			bool bHit = false;
			float bestRayTLocal = TNumericLimits<float>::Max();
			FVector bestHitPointLocal(ForceInitToZero);

			// 1) 샤프트: +X 축으로 [0, shaftLengthLocal]
			{
				float rayTLocal;
				if (RayCapsuleLocal(rayOriginLocal, rayDirectionLocal,
					FVector(0,0,0), FVector(shaftLengthLocal,0,0),
					shaftRadiusLocal, rayTLocal))
				{
					if (rayTLocal < bestRayTLocal)
					{
						bestRayTLocal = rayTLocal;
						bestHitPointLocal = rayOriginLocal + rayTLocal * rayDirectionLocal;
						bHit = true;
					}
				}
			}

			// 2) 화살촉: 샤프트 끝 근처의 작은 OBB로 근사
			{
				const FVector tipCenterLocal(shaftLengthLocal + 0.5f * tipLengthLocal, 0, 0);
				const FVector rayOriginInTipLocal = rayOriginLocal - tipCenterLocal;

				float rayTLocal;
				if (RayOBBLocal(rayOriginInTipLocal, rayDirectionLocal, tipBoxHalfExtentsLocal, rayTLocal))
				{
					if (rayTLocal < bestRayTLocal)
					{
						bestRayTLocal = rayTLocal;
						bestHitPointLocal = rayOriginLocal + rayTLocal * rayDirectionLocal;
						bHit = true;
					}
				}
			}

			if (!bHit) return false;

			const FVector hitPointWorld = gizmoTransform.TransformPosition(bestHitPointLocal);
			outRayTWorld    = FVector::DotProduct(hitPointWorld - rayOriginWorld, rayDirectionWorldNormalized);
			outHitPointWorld = hitPointWorld;
			return outRayTWorld >= 0.0f;
		}

		// Line handle = capsule along arbitrary segment in local
		inline bool RaycastLineHandle(
			const FTransform& gizmoTransform,
			const FVector& rayOriginWorld,
			const FVector& rayDirectionWorldNormalized,
			const FVector& segmentStartLocal,
			const FVector& segmentEndLocal,
			float capsuleRadiusLocal,
			float& outRayTWorld,
			FVector& outHitPointWorld)
		{
			const FTransform inverseTransform = gizmoTransform.Inverse();
			const FVector rayOriginLocal    = inverseTransform.TransformPosition(rayOriginWorld);
			// ↓ 스케일까지 포함해 역변환 (정규화는 유지)
			const FVector rayDirectionLocal = gizmoTransform.InverseTransformVector(rayDirectionWorldNormalized).GetSafeNormal();

			float rayTLocal;
			if (!RayCapsuleLocal(rayOriginLocal, rayDirectionLocal, segmentStartLocal, segmentEndLocal, capsuleRadiusLocal, rayTLocal))
				return false;

			const FVector hitPointWorld = gizmoTransform.TransformPosition(rayOriginLocal + rayTLocal * rayDirectionLocal);
			outRayTWorld    = FVector::DotProduct(hitPointWorld - rayOriginWorld, rayDirectionWorldNormalized);
			outHitPointWorld = hitPointWorld;
			return outRayTWorld >= 0.0f;
		}

		// Rectangle = finite plane patch defined by center/axes/extents in local
		inline bool RaycastRectangle(
			const FTransform& gizmoTransform,
			const FVector& rayOriginWorld,
			const FVector& rayDirectionWorldNormalized,
			const FVector& rectCenterLocal,
			const FVector& rectAxisULocalNormalized,
			const FVector& rectAxisVLocalNormalized,
			float rectHalfExtentULocal,
			float rectHalfExtentVLocal,
			float& outRayTWorld,
			FVector& outHitPointWorld,
			FVector2D* outUV01Optional = nullptr)
		{
			const FTransform inverseTransform = gizmoTransform.Inverse();
			const FVector rayOriginLocal    = inverseTransform.TransformPosition(rayOriginWorld);
			// ↓ 스케일까지 포함해 역변환 (정규화는 유지)
			const FVector rayDirectionLocal = gizmoTransform.InverseTransformVector(rayDirectionWorldNormalized).GetSafeNormal();

			const FVector planeNormalLocal = FVector::CrossProduct(rectAxisULocalNormalized, rectAxisVLocalNormalized).GetSafeNormal();

			float rayTLocal; FVector hitPointLocal;
			if (!RayPlaneLocal(rayOriginLocal, rayDirectionLocal, rectCenterLocal, planeNormalLocal, rayTLocal, hitPointLocal))
				return false;

			const float uCoord = FVector::DotProduct(hitPointLocal - rectCenterLocal, rectAxisULocalNormalized);
			const float vCoord = FVector::DotProduct(hitPointLocal - rectCenterLocal, rectAxisVLocalNormalized);

			if (FMath::Abs(uCoord) > rectHalfExtentULocal || FMath::Abs(vCoord) > rectHalfExtentVLocal)
				return false;

			const FVector hitPointWorld = gizmoTransform.TransformPosition(hitPointLocal);
			outRayTWorld    = FVector::DotProduct(hitPointWorld - rayOriginWorld, rayDirectionWorldNormalized);
			outHitPointWorld = hitPointWorld;

			if (outUV01Optional)
			{
				*outUV01Optional = FVector2D(
					0.5f + 0.5f * (uCoord / rectHalfExtentULocal),
					0.5f + 0.5f * (vCoord / rectHalfExtentVLocal));
			}
			return outRayTWorld >= 0.0f;
		}

		// Circle ring (annulus) for rotation handles
		inline bool RaycastCircle(
			const FTransform& gizmoTransform,
			const FVector& rayOriginWorld,
			const FVector& rayDirectionWorldNormalized,
			const FVector& ringCenterLocal,
			const FVector& ringPlaneNormalLocalNormalized,
			float ringRadiusLocal,
			float ringThicknessWorld,  // pixel 유지 시 외부에서 환산하여 전달
			float& outRayTWorld,
			FVector& outHitPointWorld,
			float* outAngleRadianOptional = nullptr,
			const FVector& ringAxisULocalNormalized = FVector(1,0,0),
			const FVector& ringAxisVLocalNormalized = FVector(0,1,0))
		{
			const FTransform inverseTransform = gizmoTransform.Inverse();
			const FVector rayOriginLocal    = inverseTransform.TransformPosition(rayOriginWorld);
			// ↓ 스케일까지 포함해 역변환 (정규화는 유지)
			const FVector rayDirectionLocal = gizmoTransform.InverseTransformVector(rayDirectionWorldNormalized).GetSafeNormal();

			// 비균일 스케일 보정(대충): 두께를 로컬로 환산할 때 최소 스케일 사용
			const FVector gizmoScale = gizmoTransform.GetScale3D();
			const float localScaleApprox = FMath::Max(FMath::Min3(FMath::Abs(gizmoScale.X), FMath::Abs(gizmoScale.Y), FMath::Abs(gizmoScale.Z)), 1e-3f);
			const float halfThicknessLocal = (ringThicknessWorld * 0.5f) / localScaleApprox;

			float rayTLocal;
			if (!RayAnnulusLocal(rayOriginLocal, rayDirectionLocal,
				ringCenterLocal, ringPlaneNormalLocalNormalized,
				ringRadiusLocal - halfThicknessLocal, ringRadiusLocal + halfThicknessLocal,
				rayTLocal))
			{
				return false;
			}

			const FVector hitPointLocal = rayOriginLocal + rayTLocal * rayDirectionLocal;
			const FVector hitPointWorld = gizmoTransform.TransformPosition(hitPointLocal);
			outRayTWorld    = FVector::DotProduct(hitPointWorld - rayOriginWorld, rayDirectionWorldNormalized);
			outHitPointWorld = hitPointWorld;

			if (outAngleRadianOptional)
			{
				const FVector vecLocal = hitPointLocal - ringCenterLocal;
				const float x = FVector::DotProduct(vecLocal, ringAxisULocalNormalized);
				const float y = FVector::DotProduct(vecLocal, ringAxisVLocalNormalized);
				*outAngleRadianOptional = FMath::Atan2(y, x);
			}
			return outRayTWorld >= 0.0f;
		}

	}
}



UCLASS()
class ATHsInteractiveGizmoManager : public AActor
{
	GENERATED_BODY()
	friend struct FTHsInteractiveGizmoHandle;
public:
	ATHsInteractiveGizmoManager();
	void UpdateInteractiveGizmo(const FTHsActionCursorData& cursorData);

	FTHsActionCursorData mTempCursorData;
	const FTHsActionCursorData& GetCurrentCursorDataCapture() ;

	bool VerifyInteractiveGizmo();
	UTHsInteractiveGizmoBridgeBucket* FindInteractiveGizmoBridgeBucket(UClass* gizmoCls);
	/*
	 * hit 가져오고, 파라미터 변경하고, cursorData 는 GetCurrentCursorDataCapture() 로 가져다 쓰셈.
	 */
	bool LineTraceHitGizmo(const FTHsActionCursorData& cursorData) ;
	UPROPERTY() bool mIsEnableWorldOccllusion = true;
	ECollisionChannel OcclusionChannel;//	temp
	
	
	bool mAnyHitGizmo = false;
	FHitResult mBestHit;
	int32 mBestPriority = TNumericLimits<int32>::Min();
	UTHsInteractiveGizmoBridge* mBestGizmoBridge = nullptr;
	void SetAnyHitGizmo(bool bHit)
	{
		mAnyHitGizmo = bHit;
	}
	bool IsAnyHitGizmo() const
	{
		return mAnyHitGizmo;
	}
	UTHsInteractiveGizmoBridge* GetBestGizmoBridge() const
	{
		return mBestGizmoBridge;
	}
	void SetBestGizmoBridge(UTHsInteractiveGizmoBridge* bridge)
	{
		mBestGizmoBridge = bridge;
	}
	void SetBestPriority(int32 priority)
	{
		mBestPriority = priority;
	}
	int32 GetBestPriority() const
	{
		return mBestPriority;
	}
	void SetBestHitResult(const FHitResult& hit)
	{
		mBestHit = hit;
	}
	const FHitResult& GetBestHitResult() const
	{
		return mBestHit;
	}
	bool Occluded(const FVector& origin,const FVector& end)const
	{
		/*
		 *	여기에 충돌에 관한걸 만들어줘야함.
		 *	정확히는 저 충돌을 뺄거임. 액터의 충돌로.
		 *	이것 테스트 후에 힛의 결과를 저기 액터로 넘기던가 해서 linetrace의 횟수를 줄이면 됨.
		 * 
		 */
		bool result = false;
		if (mIsEnableWorldOccllusion)
		{
			if (const UWorld* world = GetWorld())
			{
				FHitResult hitResult = {};
				FCollisionQueryParams queryPrams(SCENE_QUERY_STAT(THsGizmoOcc), true);
				if (world->LineTraceSingleByChannel(hitResult,origin,end,OcclusionChannel,queryPrams))
				{
					const float deltaHit = (hitResult.ImpactPoint - origin).Size();
					const float deltaEnd = (end - origin).Size();
					result = (deltaHit+ KINDA_SMALL_NUMBER < deltaEnd);
				}
			}
		}
		return result;
	}

	//	loading 하는거임. 
	void LoadGizmoCreators();

	//	오로지 기본형만 쓸수 있음
	void RegisterGizmoCreator(UTHsInteractiveGizmoCreateDescriptor* descriptor);
	//	오로지 기본형만 쓸수 있음
	void RegisterGizmoCreateDescriptor(
		FName gizmoName,
		int32 maxInstanceCount,
		UClass* gizmoCls,
		 TFunction<ITHsInteractiveGizmoAPI*(class ATHsInteractiveGizmoManager*)> onCreatedFunc)
	{
		UTHsInteractiveGizmoCreateDescriptor* descriptor = NewObject<UTHsInteractiveGizmoCreateDescriptor>(this);
		descriptor->bUsePooling = (maxInstanceCount > 0);
		descriptor->bUseOverrideCreatedMethod = false;
		descriptor->GizmoName = gizmoName;
		descriptor->MaxInstanceCount = maxInstanceCount;
		descriptor->GizmoActorClass = gizmoCls;
		descriptor->OnInteractiveGizmoActorCreated = MoveTemp(onCreatedFunc);
		RegisterGizmoCreator(descriptor);
	}
	/*

	void RegisterGizmoCreateDescriptorEx(
		UClass* extendGizmoCls,
		bool bUsePooling,
		bool bUseOverrideCreatedMethod,
		FName gizmoName,
		int32 maxInstanceCount,
		UClass* gizmoCls,
		TFunction<ITHsInteractiveGizmoAPI*(class ATHsInteractiveGizmoManager*)> onCreatedFunc = nullptr);
	 */
	
	FTHsInteractiveGizmoHandle  ActivateInteractiveGizmoHandle(UClass* gizmoCls,TScriptInterface<ITIHHsInteraction> target);
	
	void DeActivateInteractiveGizmo(TScriptInterface<ITHsInteractiveGizmoAPI> taget);

	ITHsInteractiveGizmoAPI* CreateGizmoActor(UClass* gizmoCls);

	TWeakObjectPtr<UTHsInteractiveGizmoBridge> TryGetBridge(int32 bucketIdx,int32 activeBridgeIndex)
	{
		TWeakObjectPtr<UTHsInteractiveGizmoBridge> result = nullptr;
		if (mGizmoBuckets.IsValidIndex(bucketIdx))
		{
			if (mGizmoBuckets[bucketIdx]->mActiveBridges.IsValidIndex(activeBridgeIndex))
			{
				result = mGizmoBuckets[bucketIdx]->mActiveBridges[activeBridgeIndex];
			}
		}
		
		return result;
	}

	UTHsInteractiveGizmoBridgeBucket* CreateInteractiveGizmoBridgeBucket(UClass* cls);
	void TransferToGizmoManager(ITHsInteractiveGizmoAPI* gizmo);
	
	//UGizmoArrowComponent*;
	//UGizmoLineHandleComponent*;
	//UGizmoRectangleComponent*;
	//UGizmoCircleComponent*;
	
private:
	
	//	/Script/TIHHousingCore.THsInteractiveGizmoCreatorSetting'/TIHHousingSystem/resources/db/Bp_GizmoCreator.Bp_GizmoCreator'
	UPROPERTY()
	FSoftObjectPath mDataSettingPath = FSoftClassPath("/Script/TIHHousingCore.THsInteractiveGizmoCreatorSetting'/TIHHousingSystem/resources/db/Bp_GizmoCreator.Bp_GizmoCreator'");
	
	TScriptInterface<ITHsInteractiveGizmoAPI>  ActivateInteractiveGizmo(UClass* gizmoCls,TScriptInterface<ITIHHsInteraction> target,int32& outBucketIdx,int32& outActiveBridgeIdx);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<TObjectPtr<UTHsInteractiveGizmoBridgeBucket>> mGizmoBuckets;
	
	//TMap<UClass*,FTHsInteractiveGizmoBucketDescriptor> mGizmoBucketDescriptors;
	
	TMap<UClass*,TObjectPtr<UTHsInteractiveGizmoCreateDescriptor>> mGizmoCreatorDescriptors;

	const FTHsActionCursorData* mCurrentCursorDataCapture;

	//FTHsInteractiveGizmoHitResult mCurrentHitResult;
	FHitResult mCurrentResult;
	TObjectPtr<UTHsInteractiveGizmoCreatorSetting> mCreatorSettingObject = nullptr;
};

inline ITHsInteractiveGizmoAPI* ATHsInteractiveGizmoManager::CreateGizmoActor(UClass* gizmoCls)
{
	ITHsInteractiveGizmoAPI* result = nullptr;
	
	if (TObjectPtr<UTHsInteractiveGizmoCreateDescriptor>* gizmoCreatorPtr = mGizmoCreatorDescriptors.Find(gizmoCls))
	{
		if (UTHsInteractiveGizmoCreateDescriptor* gizmoCreator = gizmoCreatorPtr->Get())
		{
			if (gizmoCreator->bUseOverrideCreatedMethod)
			{
				result = gizmoCreator->OnInteractiveGizmoActorCreated(this);
			}
			else
			{
				result = gizmoCreator->OnInteractiveGizmoActorCreatedMethod(this);
			}
		}
	}
	if (result == nullptr)
	{
		if (UObject* tryObject = NewObject<UObject>(this,gizmoCls))
		{
			result =  Cast<ITHsInteractiveGizmoAPI>(tryObject);
			if (result == nullptr)
			{
				tryObject = nullptr;	
			}
		}
	}
	return result;
}

UCLASS()
class UTHsGizmoArrowComponent : public UGizmoArrowComponent, public ITIHHsInteraction
{
	GENERATED_BODY()

public:
};
UCLASS()
class ATHsInteractiveGizmoTest : public AActor, public ITHsInteractiveGizmoAPI , public ITIHHsInteraction
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:
	ATHsInteractiveGizmoTest();

	virtual bool RaycastGizmoHitResult(const FTHsActionCursorData& cursor,
	FHitResult& outHitResult) override;
	
	virtual void ActiveGizmo(TScriptInterface<ITIHHsInteraction> target ) override;
	virtual void DeActiveGizmo() override;
	
	virtual void OverrideActionPointDown(UTHsActionPoint* actionPoint) override;
	virtual void OverrideActionPointUp(UTHsActionPoint* actionPoint) override;
	virtual void OverrideActionPointClick(UTHsActionPoint* actionPoint) override;
	virtual void OverrideActionPointDoubleClick(UTHsActionPoint* actionPoint) override;
	virtual void OverrideActionPointHoldOngoingStart(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual void OverrideActionPointHoldOngoing(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual void OverrideActionPointHoldOngoingEnd(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual void OverrideActionPointHoverStart(ATHsActionCursor* cursor, float deltaTime) override;
	virtual void OverrideActionPointHoverOngoing(ATHsActionCursor* cursor, float deltaTime) override;
	virtual void OverrideActionPointHoverEnd(ATHsActionCursor* cursor, float deltaTime) override;
	virtual void OverrideActionPointClickDragOngoing(UTHsActionPoint* actionPoint, float deltaTime) override;
	virtual void Interaction(FTHsActionPointAnyParam& interactionAnyParam) override;

	bool CastedPrimitiveRaycast(UPrimitiveComponent* primitive, FHitResult& OutHit, const FVector Start, const FVector End, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam) const
	{
		return primitive->LineTraceComponent(OutHit, Start, End, Params);
	}

	virtual bool IsActiveGizmo() const override
	{
		bool result = true;
		result &= mLine->GetVisibleFlag();
		result &= mArrow->GetVisibleFlag();
		result &= mRect->GetVisibleFlag();
		result &= mGizmoManager != nullptr;
		return result;
	}

	virtual FGameplayTagContainer GetInteractionTags() override
	{
		return FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{THS_Action_Trait_Selectable, THS_Action_Trait_Hoverable});
	}

	virtual ETHsInteractionAgentType GetInteractionAgentType() const override
	{
		return ETHsInteractionAgentType::EComponent;
	}
	virtual ETHsInteractionAgentType GetInteractionAgentTypeGizmo() const override
	{
		return GetInteractionAgentType();
	}

	UPROPERTY()
	TObjectPtr<USceneComponent> mRootComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<UGizmoLineHandleComponent> mLine;
	UPROPERTY()
	TObjectPtr<UTHsGizmoArrowComponent> mArrow;
	UPROPERTY()
	TObjectPtr<UGizmoRectangleComponent> mRect;

	ATHsInteractiveGizmoManager* mGizmoManager = nullptr;

	FTHsActionPointAnyParam mActionAnyParam;
	TScriptInterface<ITIHHsInteraction> mTarget;
	bool mIsValidTarget = false;
};


UCLASS()
class ATHsInteractiveGizmoTestActor : public AActor,public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	ATHsInteractiveGizmoTestActor();	

	virtual void BeginPlay() override;

	ATHsInteractiveGizmoManager* mGizmoManager = nullptr;

	FTHsInteractiveGizmoHandle mGizmoHandle;
	
	UPROPERTY()
	TObjectPtr< UStaticMeshComponent> mMeshComponent = nullptr;
	TScriptInterface<class ITHsInteractiveGizmoAPI> mGizmo;
	virtual FGameplayTagContainer GetInteractionTags() override
	{
		return  FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{THS_Action_Trait_Selectable, THS_Action_Trait_Hoverable});
	}

	virtual void OverrideActionPointClick(UTHsActionPoint* actionPoint) override;
	virtual void OverrideConnectGizmo(TScriptInterface<class ITHsInteractiveGizmoAPI> gizmo) override;
	virtual int32
	OverrideCommunicateInteractionString(const FString& stringMsgm, const FTHsCommunicationMessage& message) override;
};

UCLASS()
class UTHsInteractiveAxisMoveProxy : public UObject,public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	void Initialize(USceneComponent* targetComponent,TScriptInterface<ITHsInteractiveGizmoAPI> gizmoComponent)
	{
		mTargetComponent = targetComponent;
		mGizmoComponent = gizmoComponent;
	}
	void ApplyMoveDelta(const FVector& worldDelta,const FVector& axisDirWorld,EAxis::Type axis)
	{
		if (mTargetComponent.IsValid())
		{
			const FTransform& worldTransform = mTargetComponent->GetComponentTransform();
			const FVector axisDirectionWorld = worldTransform.GetUnitAxis(axis);
			
			const FVector MoveDelta = worldDelta.ProjectOnTo(axisDirectionWorld);
			mTargetComponent->AddWorldOffset(MoveDelta);
			/*
			 *	여기에 rule 정보로 인한 transform 보정이 이루어짐.
			 *	
			 * 
			 */
			if (mGizmoComponent && mGizmoComponent->IsActiveGizmo())
			{
				mGizmoComponent->MoveGizmo(worldTransform);
			}
		}
	}
	
	
private:
	UPROPERTY()
	TWeakObjectPtr<USceneComponent> mTargetComponent = nullptr;
	UPROPERTY()
	TScriptInterface<ITHsInteractiveGizmoAPI> mGizmoComponent = nullptr;	

};
UCLASS()
class UTHsInteractivePlaneMoveProxy : public UObject,public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	void Initialize(USceneComponent* targetComponent,TScriptInterface<ITHsInteractiveGizmoAPI> gizmoComponent)
	{
		mTargetComponent = targetComponent;
		mGizmoComponent = gizmoComponent;
	}
	void ApplyMoveDelta(const FVector& worldDelta,const FVector& planeNormalWorld);

private:
	UPROPERTY()
	TWeakObjectPtr<USceneComponent> mTargetComponent = nullptr;
	UPROPERTY()
	TScriptInterface<ITHsInteractiveGizmoAPI> mGizmoComponent = nullptr;	
	
};

USTRUCT()
struct FTHsInteractiveTransformGizmoBridge
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<USceneComponent> TargetComponent = nullptr;
	
	UPROPERTY()
	TScriptInterface<ITHsInteractiveGizmoAPI> GizmoAPI = nullptr;

	UPROPERTY()
	TWeakObjectPtr<UTHsInteractiveAxisMoveProxy> mAxisProxy = nullptr;

	UPROPERTY()
	TWeakObjectPtr<UTHsInteractivePlaneMoveProxy> mPlaneProxy = nullptr;
	
};
UCLASS()
class UTHsInteractiveGizmoLineComponent : public UGizmoLineHandleComponent, public ITIHHsInteraction
{
	GENERATED_BODY()
public:
	UTHsInteractiveGizmoLineComponent();
	
	virtual void OverrideActionPointClick(UTHsActionPoint* actionPoint) override;
	virtual void OverrideActionPointClickDragOngoing(UTHsActionPoint* actionPoint, float deltaTime) override;

	EAxis::Type MoveAxis;
	
	virtual void BeginPlay() override;
	
	void SetGizmoBridge(FTHsInteractiveTransformGizmoBridge* bridge)
	{
		mGizmoBridge = bridge;
	}
private:
	FTHsInteractiveTransformGizmoBridge* mGizmoBridge = nullptr;
	
};



UCLASS()
class ATHsInteractiveTransformGizmo : public AActor,public ITHsInteractiveGizmoAPI
{
	GENERATED_BODY()

public:
	ATHsInteractiveTransformGizmo();
	
	virtual void ActiveGizmo(TScriptInterface<ITIHHsInteraction> target) override;
	virtual void DeActiveGizmo() override;
	virtual bool IsActiveGizmo() const override;

	virtual bool RaycastGizmoHitResult(const FTHsActionCursorData& cursor, FHitResult& outHitResult) override;
	virtual int32 GetGizmoPriority() const override;
	
	virtual ETHsInteractionAgentType GetInteractionAgentTypeGizmo() const override
	{
		return ETHsInteractionAgentType::EComponent;
	}

private:
	UPROPERTY()
	TObjectPtr<USceneComponent> mRootComponent;
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> mUpComponent;
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> mForwardComponent;
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> mRightComponent;
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> mXYPlaneComponent;
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> mXZPlaneComponent;
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> mYZPlaneComponent;

	// UPROPERTY()
	// TObjectPtr<UTHsInteractiveAxisMoveProxy> mMoveProxy = nullptr;
	// UPROPERTY()
	// TObjectPtr<UTHsInteractiveAxisMoveProxy> mPlaneProxy = nullptr;
	// UPROPERTY()
	TScriptInterface<ITIHHsInteraction> mTarget = nullptr;
	bool mIsValidTarget = false;
	ATHsInteractiveGizmoManager* mGizmoManager = nullptr;

	UPROPERTY()
	FTHsInteractiveTransformGizmoBridge mGizmoBridge;
	
	UPROPERTY()
	TObjectPtr<UTHsInteractiveAxisMoveProxy> mAxisMoveProxies;
	UPROPERTY()
	TObjectPtr<UTHsInteractivePlaneMoveProxy> mPlaneMoveProxies;
};

