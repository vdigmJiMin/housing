// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Cores/TIHHsCoreInterface.h"
#include "UObject/Object.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/SaveGame.h"
#include "UObject/PropertyBag.h"
#include "TIHCollisionCore.generated.h"

class UTHsStuffConnectPoint_Dep;
class UTHsStuffConnectSurface_Dep;
class UTHsStuffSearchSphere_Dep;
class ATHsStuff_Dep;
class UTHsHousingFamily_Dep;
class ITHsHousingFamily_Dep;
class UTHsSurfaceRule_Dep;
class UTHsRootSceneComponent_Dep;
class ITHsConnectInterface_Dep;
class ATHsStuffManager_desperate;
class UTHsPresetStuffHierarchyNode_Dep;
class UTHsPresetStuffComponent_Dep;
class UTHsPresetStuffActor;
class ATHsPresetManager;
class UTHsPresetRuntimeSaveAgent_Dep;


class UTHsPresetNode;

class UTHsProxyDirectionTransform;

struct FTHsSurfaceTraceResult;


/*
 *	TODO: 큰 방향의 목적으로는 
 *	
 * 
 */
UENUM(Blueprintable)
enum class ETHsPresetNodeType_Dep : uint8
{
	EJustNode	UMETA(DisplayName="JustNode",ToolTip="Just Node without Actor or Component"),
	EGroup		UMETA(DisplayName="Group",ToolTip="Group Node"),
	EActor		UMETA(DisplayName="Actor",ToolTip="Actor Node"),
	EComponent	UMETA(DisplayName="Component",ToolTip="Component Node"),
	EMax		UMETA(Hidden)
};
ENUM_CLASS_FLAGS(ETHsPresetNodeType_Dep);

UENUM(Blueprintable,Meta=(Bitflags))
enum class ETHsStuffConnectDirection : uint8
{
	ENone	UMETA(DisplayName="None",ToolTip="No Direction"),
	EUp		UMETA(DisplayName="Up",ToolTip="Up Direction"),
	EDown	UMETA(DisplayName="Down",ToolTip="Down Direction"),
	EForward UMETA(DisplayName="Forward",ToolTip="Forward Direction"),
	EBackward UMETA(DisplayName="Backward",ToolTip="Backward Direction"),
	ELeft	UMETA(DisplayName="Left",ToolTip="Left Direction"),
	ERight	UMETA(DisplayName="Right",ToolTip="Right Direction"),
	
};
UENUM()
enum class ETHsConnectType
{
	EUnknown		UMETA(DisplayName="Unknown",ToolTip="Unknown Connect Type"),
	ESurface		UMETA(DisplayName="Surface",ToolTip="Surface Connect Type"),
	EPoint			UMETA(DisplayName="Point",ToolTip="Point Connect Type"),
	ERule			UMETA(DisplayName="Rule",ToolTip="Rule Connect Type, ex: Surface Rule"),
	EHelper			UMETA(DisplayName="Helper",ToolTip="Helper Connect Type,ex : Search Sphere"),
	EUser			UMETA(DisplayName="User",ToolTip="User Connect Type, ex: ATHsStuff"),
};


UENUM(Blueprintable)
enum class ETHsPresetCompHierarchyProcType: uint8
{
	EProgressive	UMETA(DisplayName="Progressive",ToolTip="Progressive Component Hierarchy"),
	EConditional		UMETA(DisplayName="Conditional",ToolTip="Conditional Component Hierarchy"),
	EIgnore			UMETA(DisplayName="Ignore",ToolTip="Ignore Component Hierarchy"),
};
UENUM(Blueprintable)
enum class ETHsPresetCompHierarchyUseType : uint8
{
	EUsePreset	UMETA(DisplayName="Use Preset",ToolTip="Use Preset Component Properties"),
	EUseCustomComponent	UMETA(DisplayName="Use Override Custom Component",ToolTip="Use Override Custom Component Properties"),
	EForRegisterPreset	UMETA(DisplayName="Use Default",ToolTip="Use Default Component Properties"),
};
UENUM()
enum class ETHsPresetStuffOverrideType
{
	EUseDefault	UMETA(DisplayName="Use Default",ToolTip="Use Default Preset Properties"),
	EUseDefaultOverride	UMETA(DisplayName="Use Default Override",ToolTip="Use Default Preset Properties with Override, default + override"),
	EUseCustomOverride	UMETA(DisplayName="Use Custom Override",ToolTip="Use Custom Override Properties, defulat + custom override"),
	EUseInheritOverride	UMETA(DisplayName="Use Inherit Override",ToolTip="Use Inherit Override Properties,Order and priority Default < Parent < Child < Custom override"),
};
UENUM(Blueprintable, Meta=(Bitflags))
enum class ETHsPresetNodeOptions : uint8
{
	ENone					= 0			UMETA(DisplayName="None",ToolTip="No Options"),
	EUsePresetName			= 1<<0		UMETA(DisplayName="UsePresetName",ToolTip="Use Preset Name when applying preset"),
	EUseOverrideNode		= 1<<1		UMETA(DisplayName="UseOverrideNode",ToolTip="Use Override Node when applying preset"),
	EUseDefaultRootScene	= 1<<2		UMETA(DisplayName="UseDefaultRootScene",ToolTip="Use Default Root Scene Component when applying preset"),
	EDefaultPreset			= 1<<3		UMETA(DisplayName="DefaultPreset",ToolTip="This is a Default Preset"),
	
	EEditingMode			= 1<<4		UMETA(DisplayName="EditingMode",ToolTip="This is in Editing Mode"),
	ETestingMode			= 1<<5		UMETA(DisplayName="TestingMode",ToolTip="This is in Testing Mode"),
	
	ESetBasicPresetOption		= EUsePresetName | EUseDefaultRootScene UMETA(DisplayName="SetBasicPresetOption",ToolTip="Default Option Set"),
	ESetDefaultPresetOption = ESetBasicPresetOption | EDefaultPreset UMETA(DisplayName="SetDefaultPresetOption",ToolTip="Default Preset Option Set"),
	ESetRuntimePresetOption	= ESetBasicPresetOption UMETA(DisplayName="SetRuntimePresetOption",ToolTip="Runtime Preset Option Set"),
	
};

constexpr int64 THsHashNone = 0;

ENUM_CLASS_FLAGS(ETHsPresetNodeOptions);
USTRUCT()
struct FTHsStuffConnectingHandle
{
	GENERATED_BODY()

	ITHsConnectInterface_Dep* ConnectedInterface = nullptr;
	UPrimitiveComponent* ConnectedComponent = nullptr;
};

UCLASS()
class UTHsPresetCompHierarchyProcCondition : public UObject
{
	GENERATED_BODY()

public:
	virtual bool CheckCondition(ATHsStuffManager_desperate* stuffMgr, USceneComponent* parentComp, USceneComponent* targetComp,UTHsPresetStuffHierarchyNode_Dep* currentEvaluatingNode)
	{
		return true;
	}
};


UINTERFACE()
class TIHHOUSINGCORE_API UTHsConnectInterface_Dep : public UInterface{	GENERATED_BODY()};
class TIHHOUSINGCORE_API ITHsConnectInterface_Dep
{
	GENERATED_BODY()

public:
	virtual ETHsConnectType GetConnectType() const
	{
		return ETHsConnectType::EUnknown;
	}
	virtual TArray<FTHsStuffConnectingHandle> GetConnectedInterfaces() const
	{
		return TArray<FTHsStuffConnectingHandle>();
	}
	virtual int32 GetConnectDirectionMask() const
	{
		return ToBits(ETHsStuffConnectDirection::ENone);
	}
	virtual bool AttachToSurface(const struct FTHsSurfaceTraceResult& traceResult,const FTransform& targetTransform)
	{
		return false;
	}
	
	virtual void DetachFromSurface(ATHsStuff_Dep* detachingChild)
	{
	}
	
	virtual ATHsStuff_Dep* GetOwningStuff() const
	{
		return nullptr;
	}
	virtual bool CheckAttachable(ATHsStuff_Dep* reserveParentStuff)
	{
		return false;
	}
	
	virtual void OverridePostAttachProcess()
	{
	}
	virtual void UpdateHierarchy(TWeakObjectPtr<ATHsStuff_Dep> newParentStuff,const FTransform& newTransform)
	{
	}
	/*
	 *	callbychild
	 * 
	 */
};


UCLASS()
class UTHsSurfaceRule_Dep : public UObject
{
	GENERATED_BODY()

public:
	virtual FTransform VerifySurfaceRule(const TScriptInterface<ITHsHousingFamily_Dep>& targetStuff,const FTransform& inTransform,UTHsStuffConnectSurface_Dep* requester ) const
	{
		return inTransform;
	}
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FName RuleName = NAME_None;
private:
	
};


/**
 * 나눈이유
 * 인터페이스만 필요해서.
 * 계층관계때문에.
 */
UINTERFACE()class TIHHOUSINGCORE_API UTHsHousingFamily_Dep : public UInterface{GENERATED_BODY()};
class TIHHOUSINGCORE_API ITHsHousingFamily_Dep
{
	GENERATED_BODY()

public:
	virtual const FTransform PlaceOnSurface(const FTransform& targetTransform)
	{
		return targetTransform;
	}
	/*
	*	rootScene 에서 모두 저장하고 있자. 귀찮다.	
	*
	* 
	 */
	virtual TArray<TWeakObjectPtr<UTHsStuffConnectSurface_Dep>> GetAllSurfaces()
	{
		return TArray<TWeakObjectPtr<UTHsStuffConnectSurface_Dep>>();
	};
	virtual TArray<TWeakObjectPtr<UTHsStuffConnectPoint_Dep>> GetAllConnectPoints()
	{
		return TArray<TWeakObjectPtr<UTHsStuffConnectPoint_Dep>>();
	};
	
	virtual ETHsStuffConnectDirection GetStuffDirectionMask() const
	{
		return ETHsStuffConnectDirection::ENone;
	}
	
	virtual void AddGameplayTag(const FGameplayTag& tag)
	{
	}
	virtual void AddGameplayTagContainer(const FGameplayTagContainer& tags)
	{
	}
	virtual const FGameplayTagContainer GetStuffTags() const
	{
		return FGameplayTagContainer();
	}
	virtual FTransform GetStuffWorldTransform() const
	{
		return FTransform::Identity;
	}

	virtual const FName GetStuffPresetName() const
	{
		return NAME_None;
	}
	virtual ATHsStuff_Dep* LoadStuff(const FName& presetName)
	{
		return nullptr;
	}
	virtual void OverridePostLoadProcess(ATHsStuff_Dep* loadedStuff)
	{
	}
	
	virtual void ActivateStuff(bool bPropagateToChildren){}
	virtual void DeactivateStuff(bool bPropagateToChildren){}

	virtual void PostRegisterStuffManager(ATHsStuffManager_desperate* stuffManager)
	{
	}
	virtual void PreUnregisterStuffManager(ATHsStuffManager_desperate* stuffManager)
	{
	}
	
	virtual UTHsPresetStuffActor* ToPresetStuffActor()
	{
		return nullptr;
	}
	virtual UTHsPresetStuffComponent_Dep* ToPresetStuffComponent()
	{
		return nullptr;
	}
};

UCLASS()
class TIHHOUSINGCORE_API UTHsStuffSearchSphere_Dep : public UObject,public ITHsConnectInterface_Dep
{
	GENERATED_BODY()
public:
	void InitStuffSearchSphere(UWorld* worldContext,USceneComponent* rootComponent)
	{
		mWorldContext = worldContext;
		mLinkedStuffRootComponent = rootComponent;
		RefreshSearchSphere(rootComponent->GetComponentTransform());
	}
	
	float GetCurrentRadius() const
	{
		return mCurrentMaxRadius;
	}
	
	FORCEINLINE void SetLinkedStuffRootComponent(USceneComponent* rootComponent)
	{
		mLinkedStuffRootComponent = rootComponent;
	}
	FORCEINLINE USceneComponent* GetLinkedStuffRootComponent() const
	{
		return mLinkedStuffRootComponent.Get();
	}
	FORCEINLINE const FVector& GetRecentWorldCenterLocation() const
	{
		return mRecentWorldCenterLocation;
	}
	FORCEINLINE void SetTraceChannel(ECollisionChannel channel)
	{
		mTraceChannel = channel;
	}
	FORCEINLINE ECollisionChannel GetTraceChannel() const
	{
		return mTraceChannel;
	}
		
	void AddLinkedConnectPoint(UTHsStuffConnectPoint_Dep* connectPoint);
	void TryUpdateRecentSurfaces(const FTransform& reserveWorldTransform);
	
	FTHsSurfaceTraceResult TryFindBestSurface(const TScriptInterface<ITHsHousingFamily_Dep>& stuff, const FTransform& reserveWorldTransform);
	
	void RefreshSearchSphere(const FTransform& reserveWorldTransform) ;
	bool CheckRefreshNeed(const FVector& reserveWorldLocation) const;
	const TArray<TWeakObjectPtr<UTHsStuffConnectSurface_Dep>>& SearchSurfaces();
	const TArray<FOverlapResult>& SearchOverlapResults()
	{
		return mRecentOverlapResults;
	}
	//void RemoveLinkedConnectPoint(UTHsStuffConnectPoint* connectPoint);
private:
	//	FTransform 을 적용해야함.
	void ReCalculateSphere(const FVector& reserveWorldLocation);
	bool CalculateCenterFromValidConnectPoints(FVector& refCenter);
	//void CalculateCenterFromValidConnectPoints(int32 linkedConnectPointCount, FVector& center, int32& realValidCount);
	
	void CalculateRadiusFromLinkedPoints(const FVector& reserveWorldLocation, const FVector& center,float& currentMaxRadius);


public:
	virtual ETHsConnectType GetConnectType() const override final
	{
		return ETHsConnectType::EHelper;
	}

private:
	float mExtendRadiusScale = 3.f;
	
	float mCurrentMinRadius = 50.f;
	float mCurrentMaxRadius = 100.f;

	float mRefreshDistanceThresholdScale = 0.5f;
	
	FCollisionShape mCollisionShape;
	ECollisionChannel mTraceChannel = ECC_Visibility;
	
	TWeakObjectPtr<USceneComponent> mLinkedStuffRootComponent; 
	TArray<TWeakObjectPtr<UTHsStuffConnectPoint_Dep>> mLinkedConnectPoints;

	//	추가
	
	FVector mRecentWorldCenterLocation; // 최근 위치이며 이것의 world 위치
	FVector mRecentLocalCenterLocation; // 최근 위치이며 이것의 mLinkedStuffRootComponent 과의 차이 즉 offset 위치
	TArray<TWeakObjectPtr<UTHsStuffConnectSurface_Dep>> mRecentSurfaces; // 최근에 감지된 서피스들
	TArray<FOverlapResult> mRecentOverlapResults; // 최근에 감지된 오버랩 결과들
	UWorld* mWorldContext = nullptr;

	bool mValidSphere = false;
};

USTRUCT(BlueprintType)
struct FTHsPresetMetaData_Dep
{
	GENERATED_BODY()
	
	UPROPERTY()
	ETHsPresetNodeType_Dep PresetType = ETHsPresetNodeType_Dep::EJustNode;
	UPROPERTY()
	bool bIsDefaultAsset = false;
	UPROPERTY()
	int32 PresetIndex = -1;

	FTHsPresetMetaData_Dep() : PresetType(ETHsPresetNodeType_Dep::EJustNode), bIsDefaultAsset(false), PresetIndex(-1)
	{}
	FTHsPresetMetaData_Dep(ETHsPresetNodeType_Dep InType, bool bInIsDefaultAsset, int32 InIndex)
		: PresetType(InType), bIsDefaultAsset(bInIsDefaultAsset), PresetIndex(InIndex)
	{}
	FTHsPresetMetaData_Dep(const FTHsPresetMetaData_Dep& other)
		: PresetType(other.PresetType), bIsDefaultAsset(other.bIsDefaultAsset), PresetIndex(other.PresetIndex)
	{}
	FTHsPresetMetaData_Dep(FTHsPresetMetaData_Dep&& other) noexcept
		: PresetType(other.PresetType), bIsDefaultAsset(other.bIsDefaultAsset), PresetIndex(other.PresetIndex)
	{}
	FTHsPresetMetaData_Dep& operator=(const FTHsPresetMetaData_Dep& other) = default;
	FTHsPresetMetaData_Dep&  operator=(FTHsPresetMetaData_Dep&& other) = default;

	bool IsValidIndex() const
	{
		return INDEX_NONE < PresetIndex;
	}
	bool IsValidPreset() const
	{
		return IsValidIndex() && PresetType != ETHsPresetNodeType_Dep::EJustNode;
	}
	
};

DECLARE_MULTICAST_DELEGATE_OneParam(FTHsPresetDelegateAfterStorePreset,TWeakObjectPtr<ATHsPresetManager>);
UCLASS()
class UTHsStuffSurfaceFinder : public UObject
{
	GENERATED_BODY()

public:

	void InitFinder(UWorld* worldContext,const FTransform& worldTransform,float nearMinRadius,float extendRadiusScale = 3.f,ECollisionChannel traceChannel = ECC_Visibility)
	{
		mWorldContext = worldContext;
		mWorldTransform = worldTransform;
		mTraceChannel = traceChannel;
		mExtendRadiusScale = FMath::Max(1.0f,extendRadiusScale);
		mCurrentNearRadius = FMath::Max(nearMinRadius,gMinRadius);
		mCurrentFarRadius = mCurrentNearRadius * mExtendRadiusScale;
	}
	
	
	const TArray<FOverlapResult>& CaptureOverlapResults(const FTransform& reserveWorldTransform,const TArray<FVector>& pointsSnapshot = TArray<FVector>());
	bool IsValidFinder() const
	{
		return mWorldContext != nullptr && mCurrentNearRadius <  gMinRadius - KINDA_SMALL_NUMBER;
	}
	bool CheckRefreshNeed(const FVector& reserveWorldLocation) const
	{
		const float dist = (mRecentCaptureLocation - reserveWorldLocation).Size() + mCurrentNearRadius;
		if (dist < mCurrentFarRadius)
		{
			return false;
		}
		return true;
	}

	void DefaultCollisionShape()
	{
		mExtendRadiusScale = 3.f;
		mCurrentNearRadius = gMinRadius;
		mCurrentFarRadius = mCurrentNearRadius * mExtendRadiusScale;
		mTraceChannel = ECC_Visibility;
		mCollisionShape = FCollisionShape::MakeSphere(mCurrentFarRadius);
	}

	void RefreshSearchRange(const TArray<FVector>& pointsSnapshot)
	{
		if (not pointsSnapshot.IsEmpty())
		{
			FVector center = FVector::ZeroVector;
			for (const FVector& point : pointsSnapshot)
			{
				center += point;
			}
			center /= static_cast<float>(pointsSnapshot.Num());
			float maxRadius = 0.f;
			for (const FVector& point : pointsSnapshot)
			{
				maxRadius = FMath::Max(maxRadius,(point - center).SizeSquared());
			}
			if (constexpr float sqRadius = gMinRadius * gMinRadius; sqRadius < maxRadius )
			{
				mCurrentNearRadius = FMath::Sqrt(maxRadius);
                mCurrentFarRadius = (mCurrentNearRadius * mExtendRadiusScale) - mCurrentNearRadius;
				mRecentCaptureLocation = center;
				mCollisionShape = FCollisionShape::MakeSphere(mCurrentFarRadius);
				return;
			}
		}
		DefaultCollisionShape();
	}
	
private:
	static constexpr float gMinRadius = 50.f;
	float mExtendRadiusScale = 3.f;

	ECollisionChannel mTraceChannel = ECC_Visibility;
	UWorld* mWorldContext = nullptr;
	
	float mCurrentNearRadius = 50.f;
	float mCurrentFarRadius = 100.f;
	FCollisionShape mCollisionShape;	//	sphere
	FVector mRecentCaptureLocation;
	FTransform mWorldTransform;
	TArray<FOverlapResult> mRecentOverlapResults; //	캐싱을 할까말까...
};



UCLASS()
class TIHHOUSINGCORE_API UTHsStuffConnectPoint_Dep : public UObject , public ITHsConnectInterface_Dep
{
	GENERATED_BODY()

public:
	/*
	 *	**ByLinkedComponent 를 통하면 현재 위치를 자동으로 가져온다.
	 *	bestSurface 를 가져오는 방법
	 *	1. 이동할 위치의 transform 을 가져온다.
	 *	2. search sphere 는 best surface 를 가져온다.
	 *		1.		search sphere는 이동할 위치가 새로 surface들을 갱신해야하는지 검증한다.
	 *		1.1.	만약 새롭게 검증해야한다면 자신을 이동시키고, 지금까지 있던 surface 들을 모두지우고 새롭게 검증한다.
	 *		1.2.	검증이 필요없다면 기존에 가지고 있던 surface 들을 사용한다.
	 *		2.		모든 points 들과 가장 가까운 surface 들을 가져온다. <- 여기에 최적화 기법들이 많이 들어감
	 *				- 모든 points 들을 돌면서 가장 가까운 surface 를 찾는다.
	 *				- 간단하게 주변에 있는 surface 들과의 거리를 모두 가져오고 모든 surface 들중에서 가장 가까운 surface 를 찾는다.
	 *				- octree 같은걸로 공간을 나누고, 각 point 들의 방향대로만 각각 검사해준다.
	 *	3. best surface 를 
	 * 
	 */
	
	FORCEINLINE FVector GetWorldLocation(const FTransform& parentTransform) const
	{
		return parentTransform.TransformPosition(mStartOffset);
	}
	FORCEINLINE FVector GetWorldEndLocation(const FTransform& parentTransform) const
	{
		return parentTransform.TransformPosition(GetLocalEndOffset());
	}
	FORCEINLINE FVector GetWorldDirection(const FTransform& parentTransform) const
	{
		return parentTransform.TransformVectorNoScale(mDirection).GetSafeNormal();
	}

	FORCEINLINE FVector GetWorldStartLocationByLinkedComponent() const
	{
		if (mLinkedComponent.IsValid())
		{
			return mLinkedComponent->GetComponentTransform().TransformPosition(mStartOffset);
		}
		return FVector::ZeroVector;
	}
	
	FORCEINLINE FVector GetWorldLocationByLinkedComponent() const
	{
		return mLinkedComponent.IsValid() ? GetWorldLocation(mLinkedComponent->GetComponentTransform()) : FVector::ZeroVector;
	}
	FORCEINLINE FVector GetWorldEndLocationByLinkedComponent() const
	{
		return mLinkedComponent.IsValid() ? GetWorldEndLocation(mLinkedComponent->GetComponentTransform()) : FVector::ZeroVector;
	}
	FORCEINLINE FVector GetWorldDirectionByLinkedComponent() const
	{
		return mLinkedComponent.IsValid() ? GetWorldDirection(mLinkedComponent->GetComponentTransform()) : FVector::ZeroVector;
	}

	FORCEINLINE FTransform GetWorldTransformByLinkedComponent() const
	{
		if (mLinkedComponent.IsValid())
		{
			FVector location = GetWorldLocation(mLinkedComponent->GetComponentTransform());
			FVector direction = GetWorldDirection(mLinkedComponent->GetComponentTransform());
			FRotator rotator = direction.Rotation();
			return FTransform(rotator, location);
		}
		return FTransform::Identity;
	}
	
	FORCEINLINE FVector GetLocalStartOffset() const
	{
		return mStartOffset;
	}
	FORCEINLINE FVector GetLocalEndOffset() const
	{
		return mStartOffset + (mDirection * mLength);
	}
	FORCEINLINE FVector GetLocalDirection() const
	{
		return mDirection;
	}
	FORCEINLINE float GetLength() const
	{
		return mLength;
	}

	FORCEINLINE void SetStartOffset(const FVector& offset)
	{
		mStartOffset = offset;
	}
	FORCEINLINE void SetDirection(const FVector& direction)
	{
		mDirection = direction.GetSafeNormal();
	}
	FORCEINLINE void SetLength(const float length)
	{
		mLength = length;
	}

	FORCEINLINE bool IsValidConnectPoint() const
	{
		return mLinkedComponent.IsValid() && mLinkedSearchSphere.IsValid();
	}

	virtual ETHsConnectType GetConnectType() const override final
	{
		return ETHsConnectType::EPoint;
	}

	virtual ATHsStuff_Dep* GetOwningStuff() const override final;
	
	UTHsStuffConnectSurface_Dep* CheckSurface();
private:
	FVector mStartOffset = FVector::ZeroVector;
	FVector mDirection = FVector::ForwardVector;
	float mLength = 100.f;

	TWeakObjectPtr<UPrimitiveComponent> mLinkedComponent = nullptr;
	TWeakObjectPtr<UTHsStuffSearchSphere_Dep> mLinkedSearchSphere = nullptr;	//	dependency 가 하나만있어서 분리해도됨.

	ETHsStuffConnectDirection mAllowedConnectDirection = ETHsStuffConnectDirection::ENone;
};

USTRUCT()
struct FTHsSurfaceTraceResult
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<UTHsStuffConnectPoint_Dep> BestConnectPoint = nullptr;
	UPROPERTY()
	TWeakObjectPtr<UTHsStuffConnectSurface_Dep> BestConnectSurface = nullptr;
	UPROPERTY()
	int32 BestConnectDirection = (int32)ETHsStuffConnectDirection::ENone;
	UPROPERTY()
	FVector RecentImpactPoint = FVector::ZeroVector;
	UPROPERTY()
	FVector RecentImpactNormal = FVector::ZeroVector;

	bool IsValid() const
	{
		return BestConnectPoint.IsValid() && BestConnectSurface.IsValid();
	}
	bool IsSameOwnerStuff() const;
	/*
	 * check BestConnectPoint.IsValid() && BestConnectSurface.IsValid() && BestConnectPoint.Get()->GetOwningStuff() != BestConnectSurface.Get()->GetOwningStuff()
	 */
	bool IsAttachPossible() const;
};


UCLASS()
class TIHHOUSINGCORE_API UTHsStuffConnectSurface_Dep : public UObject, public ITHsConnectInterface_Dep
{
	GENERATED_BODY()
public:
	virtual bool LineTraceSingle(FHitResult& OutHit,const FVector& Start,const FVector& End,ECollisionChannel TraceChannel,const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam)
	{
		return false;
	}
	virtual bool LineTraceSingleByLocal(FHitResult& OutHit,const FVector& localStart,const FVector& localEnd,const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam)
	{
		/*
		*	선을 surface 의 local 로 옮긴다. 
		* 
		* */
		return false;
	}
	virtual FTransform PlaceOnSurface(const FTransform& targetTransform)
	{
		return targetTransform;
	}

	void LinkToComponent(UPrimitiveComponent* component)
	{
		mLinkedComponent = component;
	}
	FORCEINLINE UPrimitiveComponent* GetLinkedComponent() const
	{
		return mLinkedComponent.Get();
	}
	void UnlinkComponent()
	{
		if (mLinkedComponent.IsValid())
		{
			
		}
		mLinkedComponent = nullptr;
	}

	virtual ETHsConnectType GetConnectType() const override final
	{
		return ETHsConnectType::ESurface;
	}
	void SetBestConnectDirection(int32 direction)
	{
		mBestConnectDirection = (int32)direction;
	}
	int32 GetBestConnectDirection() const
	{
		return mBestConnectDirection;
	}


	void SetBestHitInfo(TWeakObjectPtr<UTHsStuffConnectPoint_Dep> bestPoint,int32 INT32, const FVector& Vector, const FVector& BestImpactNormal)
	{
		mBestConnectPoint = bestPoint;
		mBestConnectDirection = INT32;
		mRecentImpactPoint = Vector;
		mRecentImpactNormal = BestImpactNormal;
	}

	const FTransform ExecuteRule(const FTransform& inTransform)
	{
		FTransform outTransform = inTransform;
		for (const TWeakObjectPtr<UTHsSurfaceRule_Dep>& rule :mSurfaceRules)
		{
			outTransform = rule->VerifySurfaceRule(TScriptInterface<ITHsHousingFamily_Dep>(nullptr), outTransform,this);
		}
		return outTransform;
	}

	virtual ATHsStuff_Dep* GetOwningStuff() const override final;
	TWeakObjectPtr<UPrimitiveComponent> GetLinkedComponentWeakPtr() const
	{
		return mLinkedComponent;
	}
	
protected:
	TArray<TWeakObjectPtr<UTHsSurfaceRule_Dep> > mSurfaceRules;

	TWeakObjectPtr<UTHsStuffConnectPoint_Dep> mBestConnectPoint;
	int32 mBestConnectDirection = (int32)ETHsStuffConnectDirection::ENone;
	FVector mRecentImpactPoint = FVector::ZeroVector;
	FVector mRecentImpactNormal = FVector::ZeroVector;
	
	TWeakObjectPtr<UPrimitiveComponent> mLinkedComponent = nullptr;
	int32 mAllowedConnectDirection = ToBits(ETHsStuffConnectDirection::ENone);
};
UCLASS()
class TIHHOUSINGCORE_API UTHsStuffConnectPlaneSurface : public UTHsStuffConnectSurface_Dep
{
	GENERATED_BODY()

public:
	virtual bool LineTraceSingle(FHitResult& OutHit, const FVector& Start, const FVector& End,
		ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
		const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam) override;
	virtual FTransform PlaceOnSurface(const FTransform& targetTransform) override;
	virtual bool AttachToSurface(const FTHsSurfaceTraceResult& traceResult, const FTransform& targetTransform) override;

private:
	
	FPlane mPlane;	//	origin + normal
	FVector2d mSize;// X:width , Y:height == bounds
	
};

UCLASS(BlueprintType,Blueprintable)
class TIHHOUSINGCORE_API ATHsStuff_Dep : public AActor , public ITHsHousingFamily_Dep, public ITHsConnectInterface_Dep,public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	ATHsStuff_Dep();
	//void MakeStuff(UObject* seed,TFunctionRef<void(ATHsStuff*)> postProcess);
	virtual ETHsConnectType GetConnectType() const override final
	{
		return ETHsConnectType::EUser;
	}
	void TryDetach();

	UTHsRootSceneComponent_Dep* RenewRootSceneComponent(TSubclassOf<UTHsRootSceneComponent_Dep> rootCompClass);
	
protected:
	virtual void BeginPlay() override;

public:
	//	TODO
	//virtual bool LoadPresetStuff(UTHsPresetNode* presetNode);
	//virtual UTHsPresetNode* SaveAsPresetNode() const;
	
	UTHsStuffSearchSphere_Dep* GetSearchSphere() const;//	dependency 가 하나라서 분리가능
	virtual const FTransform PlaceOnSurface(const FTransform& targetTransform) override;
	virtual void OverrideActionPointClick(UTHsActionPoint* actionPoint) override;
	virtual void OverrideSelection(bool bSelect) override;
	virtual bool AttachToSurface(const FTHsSurfaceTraceResult& traceResult, const FTransform& targetTransform) override;
	bool CheckContainsChildStuff(ATHsStuff_Dep* childStuff) const
	{
		return mChildStuffs.Contains(childStuff);
	}
	virtual bool CheckAttachable(ATHsStuff_Dep* reserveParentStuff) override;
	virtual void OverridePostAttachProcess() override;
	virtual const FGameplayTagContainer GetStuffTags() const override
	{
		return mStuffTags;
	}
	void AddChildStuff(ATHsStuff_Dep* addingChild)
	{
		if (!mChildStuffs.Contains(addingChild))
		{
			mChildStuffs.Add(addingChild);
		}
	}
	virtual void DetachFromSurface(ATHsStuff_Dep* detachingChild) override
	{
		if (mChildStuffs.Contains(detachingChild))
		{
			mChildStuffs.Remove(detachingChild);
		}
	}
	
	virtual void BeginDestroy() override;

private:
	bool mStuffSelectState = false;
	
	UPROPERTY()
	TObjectPtr<UTHsRootSceneComponent_Dep> mRootComponent = nullptr;
	
	UPROPERTY()
	TWeakObjectPtr<ATHsStuff_Dep> mParentStuff = nullptr;
	UPROPERTY()
	TSet<TWeakObjectPtr<ATHsStuff_Dep>> mChildStuffs;
	UPROPERTY()
	FGameplayTagContainer mStuffTags;
};


UCLASS()
class TIHHOUSINGCORE_API UTHsRootSceneComponent_Dep : public USceneComponent
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	
	UTHsStuffSearchSphere_Dep* GetSearchSphere() const//	dependency 가 하나라서 분리가능
	{
		return mSearchSphere;
	}
	
	
private:
	UPROPERTY()
	TObjectPtr<UTHsStuffSearchSphere_Dep> mSearchSphere = nullptr;	//	없애기 가능 <- 이놈을 없애기 보다는 차라리 프록시로 만들자.
};


UCLASS()
class TIHHOUSINGCORE_API ATHsTestExecuteActor : public AActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
};

// EUseOverrideComponent	UMETA(DisplayName="Use Override Component",ToolTip="Use Override Component Properties"),


UCLASS(Blueprintable,EditInlineNew)
class TIHHOUSINGCORE_API UTHsPresetStuffHierarchyNode_Dep : public UObject
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ETHsPresetCompHierarchyUseType HierarchyUseType = ETHsPresetCompHierarchyUseType::EUsePreset;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(EditConditionHides="HierarchyUseType != ETHsPresetCompHierarchyUseType::EUseCustomComponent "))
	FName PresetComponentName = NAME_None;/* 프리셋 이름이 있는 것은 무조건 검증을 거칠거임. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced,meta=(EditConditionHides="HierarchyUseType != ETHsPresetCompHierarchyUseType::EUsePreset"))
	TObjectPtr<USceneComponent> OverrideCustomComponent = nullptr;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bUseOverrideTransform = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(EditCondition="bUseOverrideTransform == true"))
	FTransform OverrideTransform = FTransform::Identity;

	//	계층구조를 어떻게 처리할지에 대한 정보.
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ETHsPresetCompHierarchyProcType HierarchyProcType =ETHsPresetCompHierarchyProcType::EProgressive;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced,meta=(EditCondition="HierarchyProcType == ETHsPresetCompHierarchyProcType::EConditional"))
	TObjectPtr<UTHsPresetCompHierarchyProcCondition> ConditionFunctor = nullptr;
	/*	무조건 loaded memory at runtime 시에만 있어야함. 그외에는 저장할 필요도 없음
	 *	필요한 경우: 런타임에 복사되어서 로드되었을때. 사실상 런타임 에딧에서만 필요함.
	 *	필요없는 경우: dataAsset 에서, json 으로 저장할때, 
	 *	필요없어서 제거함.
	 */
	//UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(HideInDetailPanel))
	//TWeakObjectPtr<UTHsPresetStuffHierarchyNode> ParentComponent = nullptr;//	없을수도 있음.
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced,meta=(EditCondition="HierarchyProcType != ETHsPresetCompHierarchyProcType::EIgnore"))
	TArray<TObjectPtr<UTHsPresetStuffHierarchyNode_Dep>> ChildComponents;
	
	static const FName& GetTHsRootSceneComponentName()
	{
		static FName THsRootSceneComponentName = FName(TEXT("THsRootSceneComponent"));
		return THsRootSceneComponentName;
	}
	//	노노 거의 무조건 true 가 나도록 만들었음.
	static UTHsPresetStuffHierarchyNode_Dep* DefaultRootSceneCompHierarchyData(UObject* outer = nullptr);

	bool VerifySettingNotPreset(bool bPropagate = true) const 
	{
		bool result = true;
		switch (HierarchyUseType)
		{
		case ETHsPresetCompHierarchyUseType::EUsePreset:
			break;
		case ETHsPresetCompHierarchyUseType::EUseCustomComponent:
			if (OverrideCustomComponent == nullptr)
			{
				result = false;
			}
			break;
		case ETHsPresetCompHierarchyUseType::EForRegisterPreset:
			if (OverrideCustomComponent == nullptr)
			{
				result = false;
			}
			break;
		}
		switch (HierarchyProcType)
		{
		case ETHsPresetCompHierarchyProcType::EProgressive:
			break;
		case ETHsPresetCompHierarchyProcType::EConditional:
			if (ConditionFunctor == nullptr)
			{
				result = false;
			}
			break;
		case ETHsPresetCompHierarchyProcType::EIgnore:
			break;
		}
		if (bPropagate)
		{
			for (const TObjectPtr<UTHsPresetStuffHierarchyNode_Dep>& childNode : ChildComponents)
			{
				if (childNode != nullptr&& not childNode->VerifySettingNotPreset(true))
				{
					result = false;
					break;
				}
			}
		}
		return result;
	}
	
	void SetDefault()
	{
		HierarchyUseType = ETHsPresetCompHierarchyUseType::EUsePreset;
		PresetComponentName = NAME_None;
		OverrideCustomComponent = nullptr;
		bUseOverrideTransform = false;
		OverrideTransform = FTransform::Identity;
		HierarchyProcType =ETHsPresetCompHierarchyProcType::EProgressive;
		ConditionFunctor = nullptr;
		ChildComponents.Empty();
	}
	UFUNCTION()
	void Clear()
	{
		SetDefault();
	}
	UFUNCTION()
	void Clone(UTHsPresetStuffHierarchyNode_Dep* sourceNode)
	{
		if (sourceNode != nullptr)
		{
			HierarchyUseType = sourceNode->HierarchyUseType;
			PresetComponentName = sourceNode->PresetComponentName;
			OverrideCustomComponent = sourceNode->OverrideCustomComponent;
			bUseOverrideTransform = sourceNode->bUseOverrideTransform;
			OverrideTransform = sourceNode->OverrideTransform;
			HierarchyProcType = sourceNode->HierarchyProcType;
			ConditionFunctor = sourceNode->ConditionFunctor;
			//ParentComponent = sourceNode->ParentComponent;
			ChildComponents = sourceNode->ChildComponents;
		}
	}
	
	void TraversalNodes(ATHsStuffManager_desperate* stuffMgr,
		TFunctionRef<void(
			ATHsStuffManager_desperate* /*stuffMgr*/,TWeakObjectPtr<UTHsPresetStuffHierarchyNode_Dep>/*target*/)> procFunc)
	{
		procFunc(stuffMgr,this);
		for (TWeakObjectPtr<UTHsPresetStuffHierarchyNode_Dep> childNode : ChildComponents)
		{
			if (childNode.IsValid())
			{
				childNode->TraversalNodes(stuffMgr,procFunc);
			}
		}
	}
	UFUNCTION()
	TWeakObjectPtr<UTHsPresetStuffHierarchyNode_Dep> FindChildHierarchyNodeByPresetName(const FName& presetName)
	{
		for (TWeakObjectPtr<UTHsPresetStuffHierarchyNode_Dep> childNode : ChildComponents)
		{
			if (childNode.IsValid())
			{
				if (childNode->PresetComponentName == presetName)
				{
					return childNode;
				}
				TWeakObjectPtr<UTHsPresetStuffHierarchyNode_Dep> findInChild = childNode->FindChildHierarchyNodeByPresetName(presetName);
				if (findInChild.IsValid())
				{
					return findInChild;
				}
			}
		}
		return nullptr;
	}
	
	bool CheckRegisterValid(TWeakObjectPtr<ATHsStuffManager_desperate> stuffMgr);
	
	UFUNCTION()
	TWeakObjectPtr<UTHsPresetStuffHierarchyNode_Dep> AddChildHierarchyNode(const FName& presetName);
	virtual void BeginDestroy() override;
	
};

UCLASS(Blueprintable,BlueprintType,EditInlineNew)
class TIHHOUSINGCORE_API UTHsPresetStuffComponent_Dep : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName PresetName = NAME_None;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FTransform RelativeTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced)
	TObjectPtr<USceneComponent> ComponentData = nullptr;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGameplayTagContainer ComponentTags = FGameplayTagContainer();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced)
	TArray<TObjectPtr<UTHsPresetStuffHierarchyNode_Dep>> ChildHierarchies;

	bool CheckRegisterValid();

	UClass* GetComponentClass() const
	{
		if (ComponentData != nullptr)
		{
			return ComponentData->GetClass();
		}
		return nullptr;
	}
	
	bool HasChildHierarchy() const
	{
		return ChildHierarchies.Num() > 0;
	}
	
};



// UCLASS(Blueprintable,EditInlineNew)
// class TIHHOUSINGCORE_API UTHsPresetStuffComponentHierarchy : public UObject
// {
// 	GENERATED_BODY()
// public:
// 	UPROPERTY(EditAnywhere,BlueprintReadWrite)
// 	bool bPresetNameBase = true;
// 	UPROPERTY(EditAnywhere,BlueprintReadWrite)
// 	FName PresetName;
// 	UPROPERTY(EditAnywhere,BlueprintReadWrite)
// 	TSubclassOf<USceneComponent> ComponentClassPath;
// 	
// 	UPROPERTY(EditAnywhere,BlueprintReadWrite)
// 	ETHsPresetStuffOverrideType bUseOverride = ETHsPresetStuffOverrideType::EUseDefault;
// 	
// 	UPROPERTY(EditAnywhere,BlueprintReadWrite)
// 	TMap<FName,FString> OverridePropertyBags;
//
// 	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(ShowOnlyInnerProperties ))
// 	TMap<FName,TObjectPtr<USceneComponent>> DataAssetPresetComponents;
// 	
// 	UPROPERTY(EditAnywhere,BlueprintReadWrite)
// 	TArray<TObjectPtr<UTHsPresetStuffComponentHierarchy>> ChildComponents;
// };

//	무조건 root는 UTHsRootSceneComponent 여야함.
UCLASS(Blueprintable,BlueprintType,EditInlineNew)
class TIHHOUSINGCORE_API UTHsPresetStuffActor : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName PresetName = NAME_None;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<ATHsStuff_Dep> StuffClass = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGameplayTagContainer StuffTags = FGameplayTagContainer();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool IsRuntimePreset = false;

	//	자동으로 RootSceneComponent 를 생성할지 여부.
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bAutoDefaultRootSceneComponent = true;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced)
	TObjectPtr<UTHsPresetStuffHierarchyNode_Dep> RootComponentPreset;
	
	void SetDefault();
	void Clear()
	{
		SetDefault();
	}
	void TraversalHierarchyNodes(ATHsStuffManager_desperate* stuffMgr,TFunctionRef<void(ATHsStuffManager_desperate* /*stuffMgr*/,TWeakObjectPtr<UTHsPresetStuffHierarchyNode_Dep>/*target*/)> procFunc);
	/*
	 *	TODO: preset 이름을 제외한 그냥 전체 순회하면서 검증.
	 */
	bool VerifySettingNotPreset(bool bPropagate = true) const;

	bool CheckRegisterValid();
};




USTRUCT(Blueprintable)
struct FTHsPresetExtraData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced)
	UObject* ExtraDataObject = nullptr;
};

USTRUCT(Blueprintable)
struct FTHsStuffPropertyBagOverride
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(ShowOnlyInnerProperties))
	FInstancedStruct  PropertyBag;
	
	
};


UCLASS()
class TIHHOUSINGCORE_API UTHsStuffPresetDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName ThemaName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<ATHsStuff_Dep> DefaultStuffClass;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UTHsRootSceneComponent_Dep> DefaultRootSceneComponentClass;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 RuntimePresetMaxCount = 64;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class UTHsPresetSpawnAgent> PresetSpawnAgentClass; 
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<FName,FTHsPresetExtraData> ExtraDataMap;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<FName,FTHsPresetExtraData> ExtraComponentMap;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced)
	TArray<TObjectPtr<UTHsPresetStuffComponent_Dep>> ComponentPresets;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced)
	TArray<TObjectPtr<UTHsPresetStuffActor>> StuffPresets;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(ShowOnlyInnerProperties))
	TMap<FName,FInstancedStruct> PresetOverrideProperty;
};

UCLASS()
class TIHHOUSINGCORE_API UTHsPresetSpawnAgent : public UObject
{
	GENERATED_BODY()

public:
	void InitPresetSpawnAgent(ATHsStuffManager_desperate* manager);
	virtual void OverrideInitPresetDataAsset(ATHsStuffManager_desperate* manager,UTHsStuffPresetDataAsset* presetDataAsset){}

	/*
	 * @param presetName		:	프리셋 네임. ATHsStuffManager 에서 관리하는 프리셋 네임.
	 * @param spawnTransform	:	생성하고 싶은 위치
	 * @param overridePresetName	:	(opt)만약 오버라이드 프리셋 네임이 있다면 그걸로 생성함. 하지만 검색했을때 없으면 기본 프리셋 네임으로 생성함.
	 * @return true: presetName or overridePresetName 으로 생성된 스터프 포인터, false: 생성 실패
	 */
	virtual ATHsStuff_Dep* CreatePresetStuff(const FName& presetName,const FTransform& spawnTransform);
	
	virtual USceneComponent* RecursiveHierarchyComponent(ATHsStuffManager_desperate* stuffMgr,USceneComponent* parentComponent,UTHsPresetStuffHierarchyNode_Dep* hierarchyData);
	
	/*
	 	@brief : 프리셋을 통해서 컴포넌트를 생성함.
	 	@param stuffMgr: 매니저 포인터
	 	@param parentComponent: 생성된 컴포넌트의 부모 컴포 
	 	@param componentPreset: 생성할 컴포넌트 프리셋
	 	@return 생성된 컴포넌트 포인터
	 	@note : AddOwnedComponent,RegisterComponent,AttachToComponent 를 모두 호출함.
	 	@note : parentComponent에 올 수 있는 가능성 제대로된 parentComponent,stuffMgr->GetCurrentSelectedComponent(),check(parent != nullptr)
	 */
	virtual USceneComponent* CreatePresetComponent(ATHsStuffManager_desperate* stuffMgr,USceneComponent* parentComponent,const UTHsPresetStuffComponent_Dep* componentPreset);
private:
	void RecursiveChildHierarchy(ATHsStuffManager_desperate* stuffMgr, const TArray<TObjectPtr<UTHsPresetStuffHierarchyNode_Dep>>& nodes,USceneComponent* parentComponent);
	
	/*
	 *	선택을 해야함. component가 자식을 가진다면 어떻게 할건가?
	 *	그걸 같은 세트로 봐야하나는가? 아니면 하이어라키어에 맡길건가?그럴거면 컴포넌트가 가질 이유가 있는가? 없다. 그렇다면 그건 세트로 봐야한다.
	 * 
	 */
	
	UWorld* mWorldContext = nullptr;
	TWeakObjectPtr<UTHsStuffPresetDataAsset> mPresetDataAsset;
	TWeakObjectPtr<ATHsStuffManager_desperate> mOwnerManager = nullptr;
	bool mIsValidAgent = false;
};

UCLASS()
class  UTHsPresetEditorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TWeakObjectPtr<ATHsStuffManager_desperate> OwnerManager;
	
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* PresetName;
	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* CurrentInfo;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* ApplyButton;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Button0;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button1;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button2;
	UPROPERTY(meta=(BindWidget))
	class UButton* Button3;

	virtual UButton* TryGetOverrideButton(int32 index)
	{
		return nullptr;
	}
	
	UButton* TryGetButtonByIndex(int32 index)
	{
		switch (index)
		{
		case 0:	return Button0;
		case 1:	return Button1;
		case 2:	return Button2;
		case 3:	return Button3;
		default:return TryGetOverrideButton(index);
		}
	}

protected:
	
};


UCLASS()
class TIHHOUSINGCORE_API ATHsStuffManager_desperate : public AActor
{
	GENERATED_BODY()
public:
	ATHsStuffManager_desperate();
	virtual void BeginPlay() override;

	UFUNCTION(Category="System")
	UTHsPresetSpawnAgent* TryGetSpawnAgent()
	{
		if (mPresetSpawnAgentInstance == nullptr)
		{
			mPresetSpawnAgentInstance = NewObject<UTHsPresetSpawnAgent>(this, mPresetSpawnAgentClass);
			mPresetSpawnAgentInstance->InitPresetSpawnAgent(this);
		}
		return mPresetSpawnAgentInstance;
	}
	UTHsStuffPresetDataAsset* GetLoadedPresetDataAsset() const
	{
		return mLoadedPresetDataAsset;
	}
	
	void StoreClonedManagedStuffs(UTHsStuffPresetDataAsset* stuffPresets);
	void StoreManagerInfoFromDataAsset(UTHsStuffPresetDataAsset* presetDataAsset);


	UFUNCTION(Category="Utility")
	UTHsPresetStuffComponent_Dep* TryGetPresetStuffComponentByName(const FName& Name)
	{
		if (int32* index = mPresetStuffComponentIndexTable.Find(Name))
		{
			if (mAllPresetDatas.IsValidIndex(*index))
			{
				if (UTHsPresetStuffComponent_Dep* stuffPreset = Cast<UTHsPresetStuffComponent_Dep>(mAllPresetDatas[*index].Get()))
				{
					return stuffPreset;
				}
			}
		}
		return nullptr;
	}
	UTHsPresetStuffComponent_Dep* FindPresetStuffComponent(const FName& Name)
	{
		return TryGetPresetStuffComponentByName(Name);
	}
	
	UFUNCTION(Category="Utility")
	UTHsPresetStuffActor* TryGetPresetStuffActorByName(const FName& Name)
	{
		if (int32* index = mPresetStuffActorIndexTable.Find(Name))
		{
			if (mAllPresetDatas.IsValidIndex(*index))
			{
				if (UTHsPresetStuffActor* stuffPreset = Cast<UTHsPresetStuffActor>(mAllPresetDatas[*index].Get()))
				{
					return stuffPreset;
				}
			}
		}
		return nullptr;
	}
	UTHsPresetStuffActor* FindPresetStuffActor(const FName& Name)
	{
		return TryGetPresetStuffActorByName(Name);
	}
	/*
	 * TODO: 이름 규칙도 넣어야함.
	 */
	bool ValidatePresetStuffActorName(const FName& Name)const
	{
		return mPresetStuffActorIndexTable.Contains(Name);
	}
	bool ValidatePresetStuffComponentName(const FName& Name)const
	{
		return mPresetStuffComponentIndexTable.Contains(Name);
	}
	bool ValidatePresetStuffName(const FName& Name)const
	{
		return ValidatePresetStuffActorName(Name) || ValidatePresetStuffComponentName(Name);
	}

	//	이거는 삭제될수도? 왜? 안씀
	static bool ApplyOverridePropertiesForComponent(USceneComponent* targetComponent,const TMap<FName,FString>& propertyOverrides);
	
	UFUNCTION(Category="RuntimeEditor")
	void RegisterSpawnedManagedStuff(ATHsStuff_Dep* spawnedStuff)
	{
		if (spawnedStuff)
		{
			mSpawnedManagedStuffs.Add(spawnedStuff);
			
		}
	}
	
	UFUNCTION(Category="Utility")
	UObject* GetTemporaryOuter()
	{
		return this;
	}
	
	
	/*
	 *	----------------------------------------------------------------
	 *					Global Static Functions
	 */
	static void CheckGlobalStuffManager();
	
	static ATHsStuffManager_desperate* GetStuffManagerInstance()
	{
		return gStuffManagerInstance.Get();
	}
	static TWeakObjectPtr<ATHsStuffManager_desperate> GetWeakStuffManagerInstance()
	{
		return gStuffManagerInstance;
	}

	static TWeakObjectPtr<ATHsStuffManager_desperate> gStuffManagerInstance;
	
	static UClass* LoadClassFromPath(const TSubclassOf<UObject>& classPath)
	{
		if (classPath)
		{
			return classPath.Get();
		}
		return nullptr;
	}
	static UClass* ExtractClassFromComponentPreset(const UTHsPresetStuffComponent_Dep* componentPreset)
	{
		if (componentPreset)
		{
			return componentPreset->GetComponentClass();
		}
		return nullptr;
	}
	

	/*
	 *	TODO: 아직 레지스트 한 기능도 없고, 프리셋을 추가하는 기능도 없음.
	 * 
	 */
	UFUNCTION()
	void AddStuffPresetData(){} // RegisterPresetStuffActor 로 변경될건데, 저거 런타임 데이터임.
	UFUNCTION()
	void AddComponentPresetData(){}

	static bool IsPresetAsset(const UObject* Obj)
	{
		if (!Obj) return false;
		
		for (const UObject* Current = Obj; Current != nullptr; Current = Current->GetOuter())
		{
			if (Current->IsAsset())
			{
				return true;
			}
		}
    
		return false;
	}
	UFUNCTION()
	UTHsPresetStuffActor* ToStuffActorPreset(const FName& newPresetName,ATHsStuff_Dep* sourceStuff);
	UFUNCTION()
	UTHsPresetStuffComponent_Dep* ToStuffComponentPreset(const FName& newPresetName,USceneComponent* sourceComponent);
	
	UFUNCTION()
	UTHsPresetStuffHierarchyNode_Dep* ToStuffHierarchyNodePreset(const FName& newPresetName,USceneComponent* sourceComponent);
	
	
	UFUNCTION()
	void RegisterPresetStuffActor(UTHsPresetStuffActor* newPreset);
	UFUNCTION()
	void RegisterPresetStuffHierarchyNode(UTHsPresetStuffHierarchyNode_Dep* newHierarchyNode);
	UFUNCTION()
	void RegisterPresetStuffComponent(UTHsPresetStuffComponent_Dep* newPreset);
	
private:
	UPROPERTY()
	TObjectPtr<UTHsRootSceneComponent_Dep> mManagerRootComponent = nullptr;
	
	FName mCurrentThemaName = NAME_None;
	int32 mRuntimePresetMaxCount = 64;
	TSubclassOf<ATHsStuff_Dep> mDefaultStuffClass;
	UPROPERTY()
	UClass* mPresetSpawnAgentClass = nullptr;
	

	UPROPERTY()
	TObjectPtr<UTHsPresetSpawnAgent> mPresetSpawnAgentInstance = nullptr;
	
	//	/Script/UMGEditor.WidgetBlueprint'/TIHHousingSystem/Ui/ForPreset/Bp_PresetEditorWidget.Bp_PresetEditorWidget'

	
	UPROPERTY()
	TMap<FName,AActor*> mRecentlyTemplateActorTable;

	UPROPERTY()
	TObjectPtr<UTHsStuffPresetDataAsset> mLoadedPresetDataAsset = nullptr;
	/*
	 *	----------------------------------------------------------------
	 *							All Datas
	 */
	UPROPERTY()
	TMap<FName,int32> mPresetStuffComponentIndexTable;	//	All Component Presets with runtime Data and dataAsset
	UPROPERTY()
	TMap<FName,int32> mPresetStuffActorIndexTable;		//	all Stuff Presets with runtime data and dataAsset
	UPROPERTY()
	TArray<TObjectPtr<UObject>> mAllPresetDatas;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<ATHsStuff_Dep>> mSpawnedManagedStuffs;

public:
	UFUNCTION(Category="Utility")
	USceneComponent* GetTemporaryRootComponent() const
	{
		if (CurrentSpawnedStuff.IsValid())
		{
			if (CurrentSpawnedStuff->GetRootComponent() == nullptr)
			{
				return CurrentSpawnedStuff->RenewRootSceneComponent(CurrentRootComponentClass);
			}
			return CurrentSpawnedStuff->GetRootComponent();
		}
		return GetRootComponent();	//	TODO: 뭐가 위험해. 이거 일단 sceneRoot 기본형으로 만들어주자.
	}	//	CurrentSpawnedSelectedComponent
	
	UFUNCTION(Category="Utility")
	USceneComponent* GetCurrentSelectedComponent()
	{
		if (CurrentSpawnedSelectedComponent.IsValid())
		{
			return CurrentSpawnedSelectedComponent.Get();
		}
		return GetTemporaryRootComponent();
	}
	
	UFUNCTION(Category="Utility")
	AActor* GetTemporaryStuffActor()
	{
		if (CurrentSpawnedStuff.IsValid())
		{
			return CurrentSpawnedStuff.Get();
		}
		return this;
	}
	/*
	 *	----------------------------------------------------------------
	 *					Runtime Editor Variables and Functions
	 */
	UPROPERTY()
	TObjectPtr<UTHsPresetEditorWidget> mPresetEditorWidget = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta=(MakeEditWidget))
	FTransform SpawnPointerTransform = FTransform::Identity;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName CurrentPresetName = NAME_None;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bBeginMakePresetStuff = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UTHsRootSceneComponent_Dep> CurrentRootComponentClass = UTHsRootSceneComponent_Dep::StaticClass();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<ATHsStuff_Dep> CurrentSpawnedStuff = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<USceneComponent> CurrentSpawnedSelectedComponent = nullptr;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr< UTHsPresetStuffActor> CurrentEditingPreset = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<ATHsStuff_Dep> CurrentSelectedStuffClass = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<ATHsStuff_Dep> CurrentTemplateStuff;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UTHsStuffSurfaceFinder> GlobalSurfaceFinder = nullptr;
	
	UPROPERTY()
	TObjectPtr<UTHsPresetStuffActor> DefaultPresetStuffActor;
	void MakeDefaultPresetStuffActor();

	
	TMap<FObjectKey,TWeakObjectPtr<UTHsPresetStuffActor>> mOnWorldSpawnedPresetCache;
	UFUNCTION()
	TWeakObjectPtr<UTHsPresetStuffActor> FindOnWorldSpawnedPresetCache(ATHsStuff_Dep* spawnedStuff){return nullptr;}
	
	UFUNCTION()
	ATHsStuff_Dep* SpawnStuffActor(const FName& presetName,const FTransform& spawnTransform){return nullptr;}
	UFUNCTION()
	USceneComponent* SpawnStuffComponent(const FName& presetName,USceneComponent* parentComp){return nullptr;}
	UFUNCTION()
	UTHsPresetStuffActor* ToPresetFromStuffActor(const FName& newPresetName,ATHsStuff_Dep* sourceStuff){return nullptr;}
	UFUNCTION()
	bool SaveStuffActor(){return false;}
	
	UFUNCTION()
	void MakeStuffPreset();
	UFUNCTION()
	void SpawnStuffPresetEditorWidget();

	
	UFUNCTION()
	void SaveCurrentPresetToJson(){}
	
	UFUNCTION()
	void MakeStuffComponent(){}

	virtual void BeginDestroy() override;
};



UCLASS()
class UTHsProxyDirectionTransform : public UObject
{
	GENERATED_BODY()

public:
	FTransform TransformDirectionProxy(const FTransform& inTransform,const FVector& directionVector) const
	{
		FVector transformedDirection = inTransform.TransformVectorNoScale(directionVector).GetSafeNormal();
		FRotator rotator = transformedDirection.Rotation();
		return FTransform(rotator, inTransform.GetLocation());
	}
	
};
/*
 *	plane 을 타고 이동하는것
 *	
 *
 * 
 */
UCLASS()
class UTHsStuffMeshComponent : public UStaticMeshComponent,public ITHsConnectInterface_Dep
{
	GENERATED_BODY()

public:
	TArray<TObjectPtr<UTHsStuffConnectSurface_Dep>> GetAllConnectSurfaces() const
	{
		return mAllConnectSurfaces;
	}
	
	void AddConnectSurface(UTHsStuffConnectSurface_Dep* newSurface)
	{
		if (!mAllConnectSurfaces.Contains(newSurface))
		{
			mAllConnectSurfaces.Add(newSurface);
		}
	}
	
	virtual TArray<FTHsStuffConnectingHandle> GetConnectedInterfaces() const override
	{
		return TArray<FTHsStuffConnectingHandle>();
	}
private:
	UPROPERTY()
	TArray<TObjectPtr<UTHsStuffConnectSurface_Dep>> mAllConnectSurfaces;
	UPROPERTY()
	TArray<TObjectPtr<UTHsStuffConnectPoint_Dep>> mAllConnectPoints;
};




USTRUCT(Blueprintable)
struct FTHsPresetNodeHash
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 MainHash = 0;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 ChildrenHash = 0;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Version = 1;

	bool operator==(const FTHsPresetNodeHash& Other) const
	{
		return MainHash == Other.MainHash && 
			   ChildrenHash == Other.ChildrenHash &&
			   Version == Other.Version;
	}

	static uint32 GetTypeHash(const FTHsPresetNodeHash& PresetNodeHash)
	{
		uint32 Hash = 0;
		Hash = HashCombine(Hash, ::GetTypeHash(PresetNodeHash.MainHash));
		Hash = HashCombine(Hash, ::GetTypeHash(PresetNodeHash.ChildrenHash));
		Hash = HashCombine(Hash, ::GetTypeHash(PresetNodeHash.Version));
		return Hash;
	}
};

/*
 *	preset 을 만든다
 *	appearance: 썸네일, 설명, 이름 등의 정보가 있는걸 만든다.
 *	위젯을 오픈한다
 *		그때 위젯이 없다면 만들어준다.
 *		해당 위젯이 새롭게 오픈되면 현재 정보를 토대로 물체들을 가지고 온다.
 *			이때 매니저의 프리셋들을 돌면서 그냥 이름이든 썸네일이든 가지고옴.
 *			만약 그 정보가 없다면 디폴트로 바꾼다.
 *		그러면 결국 위젯의 아이템이 로드 될때 매니저에서 끌고 가지고 와야할텐데, 클릭하거나 뭐 그런거 했을때는?
 * 순수하게 스폰하고, 그걸 등록한다음 언제든 편입이 가능하도록 만들어야함.
 *	preset -> spawn -> edit -> to preset -> save to json
 *	이런식인데, empty preset 도 만들어야함.
 *	그리고 그걸 에딧가능하게 해야하고, 그냥 노말 스폰도 있어야함.
 * 그걸 에딧가능해야하고.
 */



UCLASS(Blueprintable,EditInlineNew)
class UTHsPresetNodeAppearance : public UObject
{
	GENERATED_BODY()

public:
	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//class UPaperSprite* Thumbnail = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString Description = TEXT("");
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString Name = TEXT("");
	
	//UFUNCTION(BlueprintCallable)
	//UPaperSprite* GetThumbnail() const
	//{
	//	return Thumbnail;
	//}
	UFUNCTION(BlueprintCallable)
	FString GetDescription() const
	{
		return Description;
	}
	UFUNCTION(BlueprintCallable)
	FString GetPresetAppearanceName() const
	{
		return Name;
	}
	
	static UTHsPresetNodeAppearance* GetDefaultAppearance(UObject* outer = nullptr);

private:
	UPROPERTY()
	TWeakObjectPtr<UTHsPresetNode> OwnerPresetNode = nullptr;
	
};




/*
 *	preset: 반드시 이름이 있어야함.
 *	override: UObject 로 아무거나 넣을수 있음. <- 프리셋시에 여기에 있는 값을 복사해서 붙여넣기함. 
 *	
 * 
 */
UCLASS(Blueprintable,BlueprintType,EditInlineNew,DefaultToInstanced)
class TIHHOUSINGCORE_API UTHsPresetNode : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="PresetNode")
	ETHsPresetNodeType_Dep NodeType = ETHsPresetNodeType_Dep::EGroup;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="PresetNode|Setting",Meta=(Bitmask,BitmaskEnum="ETHsPresetNodeOptions"))
	int32 NodeOptions = static_cast<int32>(ETHsPresetNodeOptions::ENone);
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="PresetNode")
	FName PresetName = NAME_None;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="PresetNode")
	FTransform RelativeTransform = FTransform::Identity;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="PresetNode|Value",meta=(EditConditionHides="NodeType == ETHsPresetNodeType::EActor"))
	TSubclassOf<ATHsStuff_Dep> ActorClass = nullptr;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="PresetNode|Value",meta=(EditConditionHides="NodeType == ETHsPresetNodeType::EComponent"))
	TSubclassOf<USceneComponent> SceneClass = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="PresetNode|Value")
	FGameplayTagContainer NodeTags = FGameplayTagContainer();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced,Category="PresetNode|Value",meta=(EditConditionHides="bUseOverrideNode == true"))
	TObjectPtr<UObject> OverrideNode = nullptr;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced,Category="PresetNode|Hierarchy")
	TArray<TObjectPtr<UTHsPresetNode>> ChildNodes;	//	만약 bUseDefaultRootScene == true 며면 0번 인덱스가 루트 씬 컴포넌트로 사용함. empty 면 자동생성 하고 ChildNodes를 그아래에 붙임.

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced,Category="PresetNode|Appearance")
	TObjectPtr<UTHsPresetNodeAppearance> PresetAppearance;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="PresetNode")
	int64 NodeHash = THsHashNone;

	FORCEINLINE int64 TryGetNodeHash()
	{
		if (NodeHash == THsHashNone)
		{
			UpdateNodeID();
		}
		return NodeHash;
	}

	FORCEINLINE bool IsOptUsingPresetName() const
	{
		return (NodeOptions & static_cast<int32>(ETHsPresetNodeOptions::EUsePresetName)) != 0;
	}
	FORCEINLINE bool IsOptUsingOverrideNode() const
	{
		return (NodeOptions & static_cast<int32>(ETHsPresetNodeOptions::EUseOverrideNode)) != 0;
	}
	FORCEINLINE bool IsOptUsingDefaultRootScene() const
	{
		return (NodeOptions & static_cast<int32>(ETHsPresetNodeOptions::EUseDefaultRootScene)) != 0;
	}
	FORCEINLINE bool IsOptDefaultPreset() const
	{
		return (NodeOptions & static_cast<int32>(ETHsPresetNodeOptions::EDefaultPreset)) != 0;
	}
	FORCEINLINE bool IsOptEditingMode() const
	{
		return (NodeOptions & static_cast<int32>(ETHsPresetNodeOptions::EEditingMode)) != 0;
	}
	FORCEINLINE void SetOptSetName(bool bUse)
	{
		if (bUse)
		{
			NodeOptions |= static_cast<int32>(ETHsPresetNodeOptions::EUsePresetName);
		}
		else
		{
			NodeOptions &= ~static_cast<int32>(ETHsPresetNodeOptions::EUsePresetName);
		}
	}
	FORCEINLINE void OnOptUsePresetName() { SetOptSetName(true); }
	FORCEINLINE void OffOptUsePresetName() { SetOptSetName(false); }
	
	FORCEINLINE void SetOptUseOverrideNode(bool bUse)
	{
		if (bUse)
		{
			NodeOptions |= static_cast<int32>(ETHsPresetNodeOptions::EUseOverrideNode);
		}
		else
		{
			NodeOptions &= ~static_cast<int32>(ETHsPresetNodeOptions::EUseOverrideNode);
		}
	}
	FORCEINLINE void OnOptUseOverrideNode() { SetOptUseOverrideNode(true); }
	FORCEINLINE void OffOptUseOverrideNode() { SetOptUseOverrideNode(false); }
	
	FORCEINLINE void SetOptUseDefaultRootScene(bool bUse)
	{
		if (bUse)
		{
			NodeOptions |= static_cast<int32>(ETHsPresetNodeOptions::EUseDefaultRootScene);
		}
		else
		{
			NodeOptions &= ~static_cast<int32>(ETHsPresetNodeOptions::EUseDefaultRootScene);
		}
	}
	FORCEINLINE void OnOptUseDefaultRootScene() { SetOptUseDefaultRootScene(true); }
	FORCEINLINE void OffOptUseDefaultRootScene() { SetOptUseDefaultRootScene(false);}
	
	FORCEINLINE void SetOptDefaultPreset(bool bUse)
	{
		if (bUse)
		{
			NodeOptions |= static_cast<int32>(ETHsPresetNodeOptions::EDefaultPreset);
		}
		else
		{
			NodeOptions &= ~static_cast<int32>(ETHsPresetNodeOptions::EDefaultPreset);
		}
	}
	FORCEINLINE void OnOptDefaultPreset() { SetOptDefaultPreset(true); }
	FORCEINLINE void OffOptDefaultPreset() { SetOptDefaultPreset(false); }

	FORCEINLINE void SetOptEditingMode(bool bUse)
	{
		if (bUse)
		{
			NodeOptions |= static_cast<int32>(ETHsPresetNodeOptions::EEditingMode);
		}
		else
		{
			NodeOptions &= ~static_cast<int32>(ETHsPresetNodeOptions::EEditingMode);
		}
	}
	FORCEINLINE void OnOptEditingMode() { SetOptEditingMode(true); }
	FORCEINLINE void OffOptEditingMode() { SetOptEditingMode(false); }
	
	FORCEINLINE bool ValidOverrideActor() const
	{
		return  IsOptUsingOverrideNode() && OverrideNode != nullptr && OverrideNode->IsA<ATHsStuff_Dep>();
	}
	FORCEINLINE bool ValidOverrideComponent() const
	{
		return IsOptUsingOverrideNode() && OverrideNode != nullptr && OverrideNode->IsA<USceneComponent>();
	}
	
	FORCEINLINE bool IsActorTypeValid() const
	{
		return NodeType == ETHsPresetNodeType_Dep::EActor &&
			(ActorClass != nullptr || ValidOverrideActor());
	}
	
	FORCEINLINE bool IsComponentTypeValid() const
	{
		return NodeType == ETHsPresetNodeType_Dep::EComponent &&
			(SceneClass != nullptr || ValidOverrideComponent());
	}
	
	FORCEINLINE bool IsGroupTypeValid() const
	{
		return NodeType == ETHsPresetNodeType_Dep::EGroup && ChildNodes.Num() > 0;
	}

	FORCEINLINE bool IsJustNodeTypeValid() const
	{
		return NodeType == ETHsPresetNodeType_Dep::EJustNode && (NodeOptions & static_cast<int32>(ETHsPresetNodeOptions::EEditingMode));
	}
	
	FORCEINLINE bool IsEnsuredTypeValid() const
	{
		switch (NodeType)
		{
		case ETHsPresetNodeType_Dep::EActor:
			return IsActorTypeValid();
		case ETHsPresetNodeType_Dep::EComponent:
			return IsComponentTypeValid();
		case ETHsPresetNodeType_Dep::EGroup:
			return IsGroupTypeValid();
		case ETHsPresetNodeType_Dep::EJustNode:
			return IsJustNodeTypeValid();
		}
		return false;
	}
	FORCEINLINE bool IsEnsuredSpawnableType() const
	{
		switch (NodeType)
		{
		case ETHsPresetNodeType_Dep::EActor:
			return IsActorTypeValid();
		case ETHsPresetNodeType_Dep::EComponent:
			return IsComponentTypeValid();
		case ETHsPresetNodeType_Dep::EGroup:
			return false;
		case ETHsPresetNodeType_Dep::EJustNode:
			return false;
		}
		return false;
	}
	
	
	UClass* GetAvailableClass() const
	{
		if (IsOptUsingOverrideNode() && OverrideNode)
		{
			//	이게 우선순임.
			return OverrideNode->GetClass();
		}
		
		switch (NodeType)
		{
		case ETHsPresetNodeType_Dep::EActor:
			if (ActorClass)
			{
				return ActorClass.Get();
			}
			break;
		case ETHsPresetNodeType_Dep::EComponent:
			if (SceneClass)
			{
				return SceneClass.Get();
			}
			break;
		case ETHsPresetNodeType_Dep::EJustNode:break;
		case ETHsPresetNodeType_Dep::EGroup:break;
		}
		return nullptr;
	}
	AActor* TryGetOverrideActor() const
	{
		if (ValidOverrideActor())
		{
			return Cast<ATHsStuff_Dep>(OverrideNode);
		}
		return nullptr;
	}
	USceneComponent* TryGetOverrideComponent() const
	{
		if (ValidOverrideComponent())
		{
			return Cast<USceneComponent>(OverrideNode);
		}
		return nullptr;
	}
	
	
	void SetDefaultActorType()
	{
		SetDefault();
		NodeType = ETHsPresetNodeType_Dep::EActor;
	}
	void SetDefaultComponentType()
	{
		SetDefault();
		NodeType = ETHsPresetNodeType_Dep::EComponent;
	}
	void SetDefaultGroupType()
	{
		SetDefault();
		NodeType = ETHsPresetNodeType_Dep::EGroup;
	}
	
	
	FORCEINLINE void UpdateNodeID()
	{
		NodeHash = CalculateFullHash();
	}
	
	FORCEINLINE void SetDefault()
	{
		NodeType = ETHsPresetNodeType_Dep::EJustNode;
		RelativeTransform = FTransform::Identity;
		ActorClass = nullptr;
		SceneClass = nullptr;
		NodeTags = FGameplayTagContainer();
		NodeOptions = static_cast<int32>(ETHsPresetNodeOptions::ESetRuntimePresetOption);
		PresetName = NAME_None;
		OverrideNode = nullptr;
		ChildNodes.Empty();
		NodeHash = 0;
	}
private:
	
	FORCEINLINE int64 GetNodeTypeHash() const
	{
		return static_cast<int64>(NodeType);
	}
	FORCEINLINE int64 GetClassHash()const
	{
		int64 result = 0;
		switch (NodeType) {
		case ETHsPresetNodeType_Dep::EJustNode:
			break;
		case ETHsPresetNodeType_Dep::EGroup:
			break;
		case ETHsPresetNodeType_Dep::EActor:
			if (ActorClass)
			{
				result = FCrc::StrCrc32(*ActorClass->GetPathName());
			}
			break;
		case ETHsPresetNodeType_Dep::EComponent:
			if (SceneClass)
			{
				result = FCrc::StrCrc32(*SceneClass->GetPathName());
			}
			break;
		}
		return result;
	}
	FORCEINLINE int64 GetTagsHash() const
	{
		int64 result = 0;
		TArray<FGameplayTag> Tags;
		NodeTags.GetGameplayTagArray(Tags);
		Tags.Sort([](const FGameplayTag& A, const FGameplayTag& B)
		{
			return A.GetTagName().LexicalLess(B.GetTagName());
			//return GetTypeHash(A) < GetTypeHash(B);
		});
		for (const FGameplayTag& tag :Tags)
		{
			result = HashCombine(result, GetTypeHash(Tags));
		}
		return result;
	}
	
	FORCEINLINE int64 GetPresetNameHash() const
	{
		if (not PresetName.IsNone() && PresetName != NAME_None)
		{
			return FCrc::StrCrc32(*PresetName.ToString());
		}
		return 0;
	}
	
	FORCEINLINE int64 GetTransformHash() const
	{
		// 위치 (cm 단위로 반올림)
		FVector Loc = RelativeTransform.GetLocation();
		int32 LocX = FMath::RoundToInt(Loc.X);
		int32 LocY = FMath::RoundToInt(Loc.Y);
		int32 LocZ = FMath::RoundToInt(Loc.Z);
        
		// 회전 (0.1도 단위로 반올림)
		FRotator Rot = RelativeTransform.Rotator();
		int32 RotP = FMath::RoundToInt(Rot.Pitch * 10);
		int32 RotY = FMath::RoundToInt(Rot.Yaw * 10);
		int32 RotR = FMath::RoundToInt(Rot.Roll * 10);
        
		// 스케일 (0.01 단위로 반올림)
		FVector Scale = RelativeTransform.GetScale3D();
		int32 ScaleX = FMath::RoundToInt(Scale.X * 100);
		int32 ScaleY = FMath::RoundToInt(Scale.Y * 100);
		int32 ScaleZ = FMath::RoundToInt(Scale.Z * 100);
        
		// 조합
		int64 Hash = 0;
		Hash = HashCombine(Hash, LocX);
		Hash = HashCombine(Hash, LocY);
		Hash = HashCombine(Hash, LocZ);
		Hash = HashCombine(Hash, RotP);
		Hash = HashCombine(Hash, RotY);
		Hash = HashCombine(Hash, RotR);
		Hash = HashCombine(Hash, ScaleX);
		Hash = HashCombine(Hash, ScaleY);
		Hash = HashCombine(Hash, ScaleZ);
        
		return Hash;
	}
	FORCEINLINE bool IsRuntime() const
	{
		// DataAsset인지 런타임 인스턴스인지 확인
		return !HasAnyFlags(RF_ClassDefaultObject) && 
			   GetWorld() != nullptr;
	}
	FORCEINLINE int64 GetOverrideNodeHash() const
	{
		if (not IsOptUsingOverrideNode() || not OverrideNode)
			return 0;
            
		// DataAsset에서는 Asset Path 사용
		if (not IsRuntime())
		{
			// Asset인 경우 경로 사용
			if (OverrideNode->IsAsset())
			{
				FString Path = OverrideNode->GetPathName();
				return FCrc::StrCrc32(*Path);
			}
		}
        
		// 런타임에서는 인스턴스 ID 사용
		// 주의: 이건 세션마다 달라질 수 있음
		return OverrideNode->GetUniqueID();
        
		// 또는 OverrideNode의 클래스 타입만 체크
		// return GetTypeHash(OverrideNode->GetClass()->GetPathName());
	}
	FORCEINLINE int64 GetFlagsHash() const
	{
		int64 Flags = 0;
		Flags = NodeOptions;
		//Flags |= bUsePresetName ? (1 << 0) : 0;
		//Flags |= bUseOverrideNode ? (1 << 1) : 0;
		//Flags |= bUseDefaultRootScene ? (1 << 2) : 0;
		//Flags |= IsPreset ? (1 << 3) : 0;
        
		return Flags;
	}
	FORCEINLINE int64 CalculateFullHash() const
	{
		int64 Hash = 0;
        
		// 1. 기본 속성들
		Hash = HashCombine(Hash, GetNodeTypeHash());
		Hash = HashCombine(Hash, GetClassHash());
		Hash = HashCombine(Hash, GetTagsHash());
		Hash = HashCombine(Hash, GetPresetNameHash());
        
		// 2. Transform
		Hash = HashCombine(Hash, GetTransformHash());
        
		// 3. 플래그들
		Hash = HashCombine(Hash, GetFlagsHash());
        
		// 4. OverrideNode
		if (IsOptUsingOverrideNode())
		{
			Hash = HashCombine(Hash, GetOverrideNodeHash());
		}
        
		// 5. 자식들 (재귀적으로)
		Hash = HashCombine(Hash, GetChildrenHash());
        
		return Hash;
	}
	FORCEINLINE int64 GetChildrenHash() const
	{
		int64 Hash = 0;
        
		for (int32 i = 0; i < ChildNodes.Num(); i++)
		{
			if (ChildNodes[i])
			{
				// 인덱스도 포함 (순서가 중요)
				Hash = HashCombine(Hash, i);
                
				// 자식의 전체 해시
				Hash = HashCombine(Hash, ChildNodes[i]->CalculateFullHash());
			}
			else
			{
				// null 자식도 고려
				Hash = HashCombine(Hash, -1);
			}
		}
        
		return Hash;
	}
	// 해시 조합 유틸리티
	static int64 HashCombine(int64 A, int64 B)
	{
		// boost::hash_combine 알고리즘
		uint64 UA = static_cast<uint64>(A);
		uint64 UB = static_cast<uint64>(B);
		UA ^= UB + 0x9e3779b9 + (UA << 6) + (UA >> 2);
		return static_cast<int64>(UA);
	}
};
// uint32 GetTypeHash(UTHsPresetNode* node)
// {
// 	
// }



USTRUCT(Blueprintable,BlueprintType)
struct FTHsPresetNodeCollection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Instanced,meta=(ForceInlineRow ))
	TArray<TObjectPtr<UTHsPresetNode>> PresetNodes;

	TObjectPtr<UTHsPresetNode>* FindPresetNodeByName(const FName& presetName)
	{
		for (TObjectPtr<UTHsPresetNode>& node : PresetNodes)
		{
			if (node && node->PresetName == presetName)
			{
				return &node;
			}
		}
		return nullptr;
	}
	UTHsPresetNode* FindPresetNodeByIndex(int32 index)
	{
		if (PresetNodes.IsValidIndex(index))
		{
			return PresetNodes[index].Get();
		}
		return nullptr;
	}

	TObjectPtr<UTHsPresetNode>& operator[](int32 index)
	{
		return PresetNodes[index];
	}
	TArray<int64> GetAllPresetNodeHashes() const
	{
		TArray<int64> Hashes;
		Hashes.Reserve(PresetNodes.Num());
		for (const TObjectPtr<UTHsPresetNode>& node : PresetNodes)
		{
			if (node)
			{
				Hashes.Add(node->TryGetNodeHash());
			}
			else
			{
				Hashes.Add(THsHashNone);
			}
		}
		return Hashes;
	}
	
	// const가 아닌 객체에 대한 반복자
	auto begin() { return PresetNodes.begin(); }
	auto end() { return PresetNodes.end(); }
	// const 객체에 대한 반복자
	auto begin() const { return PresetNodes.begin(); }
	auto end() const { return PresetNodes.end(); }
};

UENUM(Blueprintable)
enum class ETHsPresetSaveAgentState : uint8
{
	EInvalid,
	ELoading,
	ESaving,
	EWaiting,
};


USTRUCT(Blueprintable)
struct FTHsPresetSaveSlotData_Dep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite,SaveGame)
	FString SaveSlotName = TEXT("");
	UPROPERTY(EditAnywhere, BlueprintReadWrite,SaveGame)
	FDateTime SaveDateTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,SaveGame)
	int32 SaveUserID = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,SaveGame)
	int32 SaveSlotState = 0; // 0: 정상, 1: 저장중, 2: 로딩중, 3: 손상됨
	UPROPERTY(EditAnywhere, BlueprintReadWrite,SaveGame)
	int32 SavedPresetCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,SaveGame)
	int32 FileVersion = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,SaveGame)
	FString SaveUserName = TEXT("");
	UPROPERTY(EditAnywhere, BlueprintReadWrite,SaveGame)
	int64 SaveFileHash = 0;
	
	FString GetDisplaySaveDateTimeString() const
	{
		return SaveDateTime.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
	}
	FString GetDisplayFullInfoString() const
	{
		return FString::Printf(TEXT("Slot: %s | User: %s (%d) | Presets: %d | Saved At: %s"),
			*SaveSlotName,
			*SaveUserName,
			SaveUserID,
			SavedPresetCount,
			*GetDisplaySaveDateTimeString());
	}

	FTHsPresetSaveSlotData_Dep& operator=(const FTHsPresetSaveSlotData_Dep& other) 
	{
		SaveSlotName = other.SaveSlotName;
		SaveDateTime = other.SaveDateTime;
		SavedPresetCount = other.SavedPresetCount;
		FileVersion = other.FileVersion;
		SaveFileHash = other.SaveFileHash;
		SaveUserName = other.SaveUserName;
		SaveUserID = other.SaveUserID;
		return *this;
	}
	FTHsPresetSaveSlotData_Dep& operator=(const struct FTHsPresetRuntimeSavingSnapshot& other);
	
	int64 CalculateSlotDataHash() const
	{
		int64 Hash = 0;
		Hash = HashCombine(Hash, FCrc::StrCrc32(*SaveSlotName));
		Hash = HashCombine(Hash, SaveDateTime.GetTicks());
		Hash = HashCombine(Hash, SavedPresetCount);
		Hash = HashCombine(Hash, FileVersion);
		//Hash = HashCombine(Hash, SaveFileHash);
		Hash = HashCombine(Hash, FCrc::StrCrc32(*SaveUserName));
		Hash = HashCombine(Hash, SaveUserID);
		return Hash;
	}
	void CalculateInternalFileHash()
	{
		SaveFileHash = CalculateSlotDataHash();
	}
	
	/**
	 * @brief 슬롯 데이터가 같은지 검사합니다. 슬롯이름,유저이름,유저ID 가 같으면 같은 슬롯으로 간주합니다.
	 * @invariant 완전히 같은 대상임을 보증합니다. 즉 이것에 예외가 있다면 그건 어쩔 수 없습니다.
	 * @param newData 비교 대상
	 * @return 같으면 true, 다르면 false
	 */
	bool IsSameSlotData(const FTHsPresetSaveSlotData_Dep& newData) const 
	{
		return ((SaveSlotName == newData.SaveSlotName) &&SaveUserName == newData.SaveUserName && SaveUserID == newData.SaveUserID);
	}

	/**
	 * @brief 이름을 검사하고, 덮어쓸지 여부를 판단하여 덮어씁니다.
	 * @param newData 덮어쓸 대상
	 */
	void OverrideSlotData(const FTHsPresetSaveSlotData_Dep& newData)
	{
		SaveDateTime = FDateTime::Now();
		if (IsSameSlotData(newData))
		{
			//	update override
			SavedPresetCount = newData.SavedPresetCount;
			FileVersion += 1;
		}
		else
		{
			//	cover override
			FileVersion = 0;
			SaveSlotName = newData.SaveSlotName;
			SaveDateTime = newData.SaveDateTime;
			SavedPresetCount = newData.SavedPresetCount;
			SaveUserName = newData.SaveUserName;
			SaveUserID = newData.SaveUserID;
		}
		CalculateInternalFileHash();
	}
	
	void Clear()
	{
		SaveSlotName = TEXT("");
		SaveDateTime = FDateTime::Now();
		SavedPresetCount = -1;
		FileVersion = -1;
		SaveFileHash = 0;
		SaveUserName = TEXT("");
		SaveUserID = -1;
		
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTHsOnPresetRuntimeSaveTOCDelegate,TWeakObjectPtr<UTHsPresetRuntimeSaveTOC>,updatedRuntimeSaveTOC);

UCLASS(BlueprintType)
class UTHsPresetRuntimeSaveTOC  : public USaveGame
{
	GENERATED_BODY()

	/**
	 * @brief 이미 존재하는 세이브 슬롯 이름들
	 * @pre 이름을 저장할지 결정한다.
	 * @post 사용자가 요청한 이름이 있는지 내부에 저장하고 있어야함.
	 * @post 덮어쓰기 여부를 판단해야해서 내부에 요청이 들어온 세이브를 머금고 있어야함.
	 */
public:
	UPROPERTY(EditAnywhere,SaveGame)
	TArray<FTHsPresetSaveSlotData_Dep> SavedPresetSlots;
	UPROPERTY(EditAnywhere,SaveGame)
	FDateTime TOCRecentCreatedDateTime;
	UPROPERTY(EditAnywhere,SaveGame)
	int32 CurrentSelectedPresetSlotIndex = -1;
	
	UPROPERTY(Transient)
	FTHsPresetSaveSlotData_Dep PendingSaveSlotData;
	UPROPERTY(Transient)
	TMap<FString,int32> ExistingSaveSlotNameMap;
	UPROPERTY(Transient)
	bool IsPending = false;
	UPROPERTY(Transient)
	bool IsActivated = false;

#pragma region SaveTOC
	UPROPERTY(Category="Save|Delegate",
		EditAnywhere,BlueprintAssignable)
	FTHsOnPresetRuntimeSaveTOCDelegate OnPresetRuntimeSaveTOCOverridePending;
	UPROPERTY(Category="Save|Delegate",
		EditAnywhere,BlueprintAssignable)
	FTHsOnPresetRuntimeSaveTOCDelegate OnPresetRuntimeSaveTOCUpdated;
	UPROPERTY(Category="Save|Delegate",
		EditAnywhere,BlueprintAssignable)
	FTHsOnPresetRuntimeSaveTOCDelegate OnPresetRuntimeSaveTOCComplete;
	UPROPERTY(Category="Save|Delegate",
		EditAnywhere,BlueprintAssignable)
	FTHsOnPresetRuntimeSaveTOCDelegate OnPresetRuntimeSaveTOCCanceled;

	/**
	 * 
	 * @brief Entry Point for Saving Preset Slot Data.비동기 적으로 새로운 슬롯 데이터를 저장합니다.
	 * @details 내부적으로 ExistingSaveSlotNameMap 를 통해서 충돌이 나는지 확인하고, 충돌이 나면 pendingSaveSlotData 에 캐싱을 하고, 2를 반환합니다. 충돌이 나지 않으면 바로 SavedPresetSlots 에 추가하고 1을 반환합니다. wait상태가 아니면 무조건 0을 반환합니다.
	 * @return 충돌이 나지 않으면 true 를 반환하고, 충돌이 나면 false 를 반환합니다.
	 * @param newSlotData 충돌이 나는지 확인하고 추가하거나 pending을 위해 캐싱함
	 */
	UFUNCTION(Category="Save",
		BlueprintCallable)
	int32 TryAsyncSavePresetSlotData(const FTHsPresetSaveSlotData_Dep& newSlotData);
	
	/**
	 * @brief 무조건 pendingSaveSlotData를 현재 TOC에 저장함.
	 * @details 덮어쓰기 여부에 대한 절대적 결과이므로 검사를 원한다면 SavePresetSlotData 를 통해야함.
	 */
	UFUNCTION(Category="Save|Reaction",
		BlueprintCallable)
	void OnSavePresetRuntimeSaveTOCAccept();
	UFUNCTION(Category="Save|Reaction",
		BlueprintCallable)
	void OnSavePresetRuntimeSaveTOCCancel();
#pragma endregion

#pragma region Dbt States
	UFUNCTION(BlueprintCallable)
	bool IsTOCReady() const
	{
		return IsActivated;
	}
	UFUNCTION(BlueprintCallable)
	bool IsTOCUseSaveLoad() const
	{
		return IsActivated && !IsPending;
	}
	
	void ClearRuntimeSaveTOC()
	{
		SavedPresetSlots.Empty();
		CurrentSelectedPresetSlotIndex = -1;
		ExistingSaveSlotNameMap.Empty();
		TOCRecentCreatedDateTime = FDateTime::Now();
	}

	void PopulateExistingSaveSlotNames()
	{
		for (int32 i = 0; i < SavedPresetSlots.Num(); ++i)
		{
			ExistingSaveSlotNameMap.Add(SavedPresetSlots[i].SaveSlotName, i);
		}
	}
	/**
	 * @brief 런타임 세이브 TOC 초기화
	 * @details 로드된 SavedPresetSlots가 있어야 작동을 함. 이거 자체로는 아무것도 아님.
	 */
	void InitRuntimeSaveTOC()
	{
		if (IsExistSaveSlotData())
		{
			PopulateExistingSaveSlotNames();
			TOCRecentCreatedDateTime = FDateTime::Now();
		}
		IsActivated = true;
	}
	bool IsExistSaveSlotData() const
	{
		return SavedPresetSlots.Num() > 0;
	}
	
	int32 GetSavedPresetSlotCount() const
	{
		return SavedPresetSlots.Num();
	}
	const FTHsPresetSaveSlotData_Dep* GetSavedPresetSlotDataAtConst(int32 index) const
	{
		if (SavedPresetSlots.IsValidIndex(index))
		{
			return &SavedPresetSlots[index];
		}
		return nullptr;
	}
	FTHsPresetSaveSlotData_Dep* GetSavedPresetSlotDataAt(int32 index)
	{
		if (SavedPresetSlots.IsValidIndex(index))
		{
			return &SavedPresetSlots[index];
		}
		return nullptr;
	}
	
	// const가 아닌 객체에 대한 반복자
	auto begin() { return SavedPresetSlots.begin(); }
	auto end() { return SavedPresetSlots.end(); }
	// const 객체에 대한 반복자
	auto begin() const { return SavedPresetSlots.begin(); }
	auto end() const { return SavedPresetSlots.end(); }
#pragma endregion
	
private:
	void OverridePresetSlotDataAt(int32 index,const FTHsPresetSaveSlotData_Dep& newSlotData)
	{
		if (SavedPresetSlots.IsValidIndex(index))
		{
			SavedPresetSlots[index] = newSlotData;
		}
	}
	
	const FTHsPresetSaveSlotData_Dep* FindPresetSaveSlotDataByName(const FString& slotName) const
	{
		for (const FTHsPresetSaveSlotData_Dep& slotData : SavedPresetSlots)
		{
			if (slotData.SaveSlotName == slotName)
			{
				return &slotData;
			}
		}
		return nullptr;
	}
	const FTHsPresetSaveSlotData_Dep* TryGetCurrentSelectedPresetSlotData() const
	{
		if (SavedPresetSlots.IsValidIndex(CurrentSelectedPresetSlotIndex))
		{
			return &SavedPresetSlots[CurrentSelectedPresetSlotIndex];
		}
		return nullptr;
	}
	void AddNewSlotData(FTHsPresetSaveSlotData_Dep newSlotData);
public:
	virtual void BeginDestroy() override;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTHsOnPresetRuntimeLoadedSignature,TWeakObjectPtr<class UTHsPresetRuntimeSaveAgent_Dep>,loadedPresetRuntimeSaveAgent);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTHsOnPresetRuntimeLoadedInnerSignature,TWeakObjectPtr<class UTHsPresetRuntimeSaveAgent_Dep>,loadedPresetRuntimeSaveAgent);


USTRUCT(BlueprintType)
struct FTHsPresetRuntimeLoadingSnapshot
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SaveSlotIndex;

	UPROPERTY()
	int32 UserIndex;
};
USTRUCT(Blueprintable)
struct FTHsPresetRuntimeSavingSnapshot
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame)
	FTHsPresetSaveSlotData_Dep SaveSlotData;
};

// 그냥 PresetNode 쓸까?
UCLASS(BlueprintType,Blueprintable, EditInlineNew)
class TIHHOUSINGCORE_API UTHsPresetRuntimeNode : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(Category="PresetNode|Hierarchy",
			SaveGame,EditAnywhere,BlueprintReadWrite)
	FName PresetNodeName = NAME_None;
	
	UPROPERTY(Category="PresetNode|Hierarchy",
			SaveGame,EditAnywhere,BlueprintReadWrite)
	FTransform RelativeTransform = FTransform::Identity;

	UPROPERTY(Category="PresetNode|Hierarchy",
			Transient,EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<UTHsPresetRuntimeNode> ParentNode;
	
	UPROPERTY(Category="PresetNode|Hierarchy",
		SaveGame,EditAnywhere,BlueprintReadWrite,Instanced)
	TArray<TObjectPtr<UTHsPresetRuntimeNode>> HierarchyChildNodes;
	
};

/*
 * 최초로 하나가 생기고 나서 그다음부터는 계속 이걸로 불러오고 저장함.
 */
UCLASS()
class UTHsPresetRuntimeSaveAgent_Dep : public USaveGame
{
	GENERATED_BODY()
public:
	
	UPROPERTY(
		SaveGame,
		EditAnywhere,BlueprintReadWrite,
		meta=(ShowInnerProperties,FullyExpand=true,ShowOnlyInnerProperties))
	TMap<ETHsPresetNodeType_Dep,FTHsPresetNodeCollection> RuntimeNodes;
	
	UPROPERTY(Transient)
	ETHsPresetSaveAgentState CurrentState = ETHsPresetSaveAgentState::EInvalid;
	UPROPERTY(SaveGame)
	int32 CurrentSelectedSlotIndex = -1;
	UPROPERTY(Transient)
	int32 CurrentSelectedUserIndex = 0;
	UPROPERTY(Transient)
	int64 RecentLoadedFileHash = 0;
	
	UPROPERTY(Transient)
	FTHsPresetRuntimeLoadingSnapshot CurrentLoadingSnapshot;

	/**
	 * @brief preset관련된 것들을 세이브/로드 할때 Blocking 을 발생시키는 델리게이트
	 */
	UPROPERTY(EditAnywhere,BlueprintAssignable)
	FTHsOnPresetRuntimeLoadedSignature OnPresetRuntimeBlocking;
	FTHsOnPresetRuntimeLoadedInnerSignature OnPresetRuntimeBlockingInner;
	UFUNCTION(BlueprintCallable)
	void BroadcastPresetRuntimeBlocking()
	{
		OnPresetRuntimeBlockingInner.Execute(this);
		OnPresetRuntimeBlocking.Broadcast(this);
	}
	/**
	 * @brief 로드시 완료시에 발생하는 델리게이트
	 */
	UPROPERTY(EditAnywhere,BlueprintAssignable)
	FTHsOnPresetRuntimeLoadedSignature OnPresetRuntimeLoadedCompleteCallBack;
	FTHsOnPresetRuntimeLoadedInnerSignature OnPresetRuntimeLoadedCompleteInnerCallBack;
	UFUNCTION(BlueprintCallable)
	void BroadcastPresetRuntimeLoadedComplete()
	{
		OnPresetRuntimeLoadedCompleteInnerCallBack.Execute(this);
		OnPresetRuntimeLoadedCompleteCallBack.Broadcast(this);
	}
	/**
	 * @brief 로드 실패시에 발생하는 델리게이트
	 */
	UPROPERTY(EditAnywhere,BlueprintAssignable)
	FTHsOnPresetRuntimeLoadedSignature OnPresetRuntimeLoadedFailedCallBack;
	FTHsOnPresetRuntimeLoadedInnerSignature OnPresetRuntimeLoadedFailedInnerCallBack;
	UFUNCTION(BlueprintCallable)
	void BroadcastPresetRuntimeLoadedFailed()
	{
		OnPresetRuntimeLoadedFailedInnerCallBack.Execute(this);
		OnPresetRuntimeLoadedFailedCallBack.Broadcast(this);
	}
	/**
	 * @brief 로드가 성공이든 실패든 끝나면 발생시키는 델리게이트
	 */
	UPROPERTY(EditAnywhere,BlueprintAssignable)
	FTHsOnPresetRuntimeLoadedSignature OnPresetRuntimeLoadedEndCallBack;
	FTHsOnPresetRuntimeLoadedInnerSignature OnPresetRuntimeLoadedEndInnerCallBack;
	UFUNCTION(BlueprintCallable)
	void BroadcastPresetRuntimeLoadedEnd()
	{
		OnPresetRuntimeLoadedEndInnerCallBack.Execute(this);
		OnPresetRuntimeLoadedEndCallBack.Broadcast(this);
	}
	/**
	 * @brief 디스플레이용 메시지
	 */
	UPROPERTY(Transient,VisibleAnywhere)
	FString DisplayMessage;

	void ClearTransientData()
	{
		CurrentState = ETHsPresetSaveAgentState::EInvalid;
		CurrentLoadingSnapshot = FTHsPresetRuntimeLoadingSnapshot();
		DisplayMessage = TEXT("");
	}

	/**
	 * @brief 런타임 프리셋 세이브 agent 검증을 하고 상태를 변화시키는 함수
	 */
	void VerifyPresetRuntimeSaveAgent()
	{
		
	}
	/**
	 * @brief 프리셋 매니저를 설정하고 초기화합니다.
	 * @param presetManager owner preset manager, presetManager는 유효해야합니다. 특히 presetManager는 IsPresetMgrReadyToStore 가 true 여야합니다. 검증버전인 VerifyPresetMgrStoreReady를 사용합니다.
	 * @details
	 * **Dbt**
	 * - pre: presetManager 가 유효해야합니다.
	 * - mPresetManager를 통해 TocName 과 MaxCount 를 숨겨진 인자로 사용합니다.
	 * - mLoadedSaveTocName 은 비어있지 않음이 보장됩니다. (Default: "DefaultRuntimePresetTOC")
	 * - mRuntimePresetCount > 0 (Default: 128) 가 보장됩니다.
	 * - mLoadedTOC != nullptr임이 보장됩니다. 
	 *		- DoesSaveGameExist 를통해 기존 세이브가 있는지 확인합니다.
	 *		- 불러와진 mLoadedTOC를 통해서 LoadGameFromSlot 를 하여 TOC를 마운트 합니다.(주의: 이때 TOC는 owner가 UTHsPresetRuntimeSaveAgent 가 아닙니다.todo: 이부분은 나중에 duplicate로 해결할수도 있습니다. )
	 *		- InitRuntimeSaveTOC 를 통해서 무조건 불러옵니다.
	 *		- mLoadedTOC가 비어있다면 새로운 TOC를 만듭니다.
	 *		- 최초의 TOC는 자동으로 만들어집니다.
	 *	- OnSetSaveAgentState_Waiting 상태가 보장됩니다.
	 */
	void InitPresetRuntimeAgent(ATHsPresetManager* presetManager);
	
	const TArray<FTHsPresetSaveSlotData_Dep>* GetAllSavedPresetSlotData() 
	{
		if (mLoadedTOC)
		{
			if (mLoadedTOC && mLoadedTOC->IsExistSaveSlotData())
			{
				return &mLoadedTOC->SavedPresetSlots;
			}
		}
		return nullptr;
	}
	
	/**
	* @brief 로드용,비동기적으로 프리셋 런타임 세이브 TOC를 로드합니다.
	* @param loadingSnapshot 로드 스냅샷 정보
	* @details 내부에서 일단 현재 스냅샷으로 저장하고, IsReadyToSaveLoad() 를 호출한다. 그리고 
	*/
	UFUNCTION(Blueprintable)
	void AsyncLoadPresetRuntimeSaveTOCBegin(const FTHsPresetRuntimeLoadingSnapshot& loadingSnapshot);
	UFUNCTION(Blueprintable)
	void OnLoadPresetRuntimeSaveTOCAccept();
	UFUNCTION(Blueprintable)
	void OnLoadPresetRuntimeSaveTOCCancel();
	
	bool LoadSaveGameSlot(int32 saveSlotIndex,int32 userIndex);
	
	bool LoadSaveGameSlotBySnapshot(const FTHsPresetRuntimeLoadingSnapshot& loadingSnapshot)
	{
		return LoadSaveGameSlot(loadingSnapshot.SaveSlotIndex,loadingSnapshot.UserIndex);
	}
	
	UTHsPresetNode* FindRuntimeNodeByName(const ETHsPresetNodeType_Dep& nodeType,int32 index)
	{
		if (FTHsPresetNodeCollection* nodeCollection = RuntimeNodes.Find(nodeType))
		{
			return nodeCollection->FindPresetNodeByIndex(index);
		}
		return nullptr;
	}
	UTHsPresetNode* FindRuntimePresetNodeByMetaData(const FTHsPresetMetaData_Dep& metaData )
	{
		if (metaData.IsValidIndex())
		{
			return FindRuntimeNodeByName(metaData.PresetType,metaData.PresetIndex);	
		}
		return nullptr;
	}
	bool DeepCopyRuntimePresets(const UTHsPresetRuntimeSaveAgent_Dep* sourceAgent)
	{
		if (sourceAgent == nullptr)
		{
			return false;
		}
		RuntimeNodes = sourceAgent->RuntimeNodes;
		return true;
	}

	/**
	* @brief 저장이 가능한지 여부를 반환합니다.
	* 
	* **보장 내용 (Guarantees):**
	* - **Invariant**: `IsInitStateValid() == true`일 경우 다음을 보장합니다:
	*   - `mLoadedSaveTocName`이 비어있지 않습니다. (Default: "DefaultRuntimePresetTOC")
	*   - `mRuntimePresetCount > 0` (Default: 128)
	* @retval true 초기화가 완료되었으며(`mIsInitialized`), `mPresetManager`가 유효함이 보장됩니다.
	* @retval false 초기화되지 않았거나 매니저가 유효하지 않습니다.
	*/
	bool IsInitStateValid() const
	{
		return mIsInitialized && mPresetManager.IsValid();
	}
	/**
	 * @brief TOC가 마운트 되었는지 여부를 반환합니다.
	 * 
	 * **선행 조건 및 보장 (Contract):**
	 * - 1. #IsInitStateValid() 가 **true**임을 전제로 합니다.
	 * - 2. `mLoadedTOC`가 `nullptr`이 아님을 보장합니다.
	 * 
	 * @retval true IsInitStateValid()를 보장, TOC가 마운트를 보장
	 * @retval false 초기화되지 않았거나 TOC가 마운트되지 않았습니다.
	 * @see IsInitStateValid()
	 */
	bool IsTOCMounted() const
	{
		return IsInitStateValid() && mLoadedTOC != nullptr;
	}
	/**
	 * @brief TOC가 준비되었는지 여부를 반환합니다.
	 * 
	 * **요구 사항 (Requirements):**
	 * - #IsTOCMounted() 상태여야 합니다.
	 * - `mLoadedTOC->IsTOCReady()`가 **true**여야 합니다.
	 * 
	 * @retval true TOC가 완전히 준비되어 사용 가능함을 보장합니다.
	 * @retval false TOC가 준비되지 않았거나 마운트되지 않았습니다.
	 * @see IsTOCMounted
	 */
	bool IsTOCReady() const
	{
		return IsTOCMounted() && mLoadedTOC->IsTOCReady();
	}

	/**
	 * @brief 저장 및 로드가 가능한지 여부를 반환합니다.
	 * 
	 * **상태 보장 (State Guarantees):**
	 * - `IsTOCReady()` == true
	 * - `IsSaveAgentState_Waiting()` == true (대기 상태임)
	 * 
	 * @return 저장 및 로드가 안전하게 가능한지 여부.
	 * @see IsTOCReady, IsSaveAgentState_Waiting
	 */
	bool IsReadyToSaveLoad() const
	{
		return IsTOCReady() && IsSaveAgentState_Waiting();
	}
	

	void OnSetSaveAgentState(const ETHsPresetSaveAgentState& newState)
	{
		CurrentState = newState;
	}
	void OnSetSaveAgentState_Loading()	{OnSetSaveAgentState(ETHsPresetSaveAgentState::ELoading);}
	void OnSetSaveAgentState_Saving()	{OnSetSaveAgentState(ETHsPresetSaveAgentState::ESaving);}
	void OnSetSaveAgentState_Waiting()	{OnSetSaveAgentState(ETHsPresetSaveAgentState::EWaiting);}
	void OnSetSaveAgentState_Invalid()	{OnSetSaveAgentState(ETHsPresetSaveAgentState::EInvalid);}
	void OffResetSaveAgentState()
	{
		CurrentState = ETHsPresetSaveAgentState::EInvalid;
	}

	bool IsSaveAgentState(const ETHsPresetSaveAgentState& newState) const
	{
		return CurrentState == newState;
	}
	bool IsSaveAgentState_Loading() const	{return IsSaveAgentState(ETHsPresetSaveAgentState::ELoading);}
	bool IsSaveAgentState_Saving() const	{return IsSaveAgentState(ETHsPresetSaveAgentState::ESaving);}
	bool IsSaveAgentState_Waiting() const	{return IsSaveAgentState(ETHsPresetSaveAgentState::EWaiting);}
	bool IsSaveAgentState_Invalid() const	{return IsSaveAgentState(ETHsPresetSaveAgentState::EInvalid);}

	void UpdateSaveSnapshotData()
	{
		//	임시임. 어차피 위젯에서 가져올거임. 혹은 어디 저장된곳에서 들고오게 할거임. 그리고 SaveSnapshot 여기에 기입.
	}
	

	
	FTHsPresetRuntimeSavingSnapshot SaveSnapshot;
	
	UFUNCTION(Category ="Save|TOC", Blueprintable)
	void SavePreset();
	
	UFUNCTION(Category ="Save|TOC", Blueprintable)
	void OnSavePresetRuntimeSaveTOCAccept() const
	{
		if (IsReadyToSaveLoad())
		{
			mLoadedTOC->OnSavePresetRuntimeSaveTOCAccept();
		}
	}
	UFUNCTION(Category ="Save|TOC", Blueprintable)
	void OnSavePresetRuntimeSaveTOCCancel() const
	{
		if (IsReadyToSaveLoad())
		{
			mLoadedTOC->OnSavePresetRuntimeSaveTOCCancel();
		}
	}
	
private:
	void NewTOC()
	{
		if (mLoadedTOC == nullptr)
		{
			mLoadedTOC = NewObject<UTHsPresetRuntimeSaveTOC>();
		}
		mLoadedTOC->ClearRuntimeSaveTOC();
	}
	
	UPROPERTY(Transient)
	TObjectPtr<UTHsPresetRuntimeSaveTOC> mLoadedTOC = nullptr;
	UPROPERTY(Transient)
	TWeakObjectPtr<ATHsPresetManager> mPresetManager;
	UPROPERTY(Transient)
	FString mLoadedSaveTocName = TEXT("");
	UPROPERTY(Transient)
	int32 mLoadedFileVersion = 0;
	UPROPERTY(Transient)
	int32 mRuntimePresetCount = 0;


	/**
	 * @brief 프리셋 런타임 세이브 에이전트가 초기화되었는지 여부를 나타냅니다.
	 * @invariant if(mIsInitialized == true) mLoadedSaveTocName != "" or mLoadedSaveTocName == "DefaultRuntimePresetTOC"
	 * @invariant if(mIsInitialized == true) mRuntimePresetCount > 0 or mRuntimePresetCount == 128
	 * @invariant if(mLoadedTOC != nullptr) load exist mLoadedSaveTocName + slotIndex or NewObject
	 */
	UPROPERTY(Transient)
	bool mIsInitialized = false;
	UPROPERTY(Transient)
	bool mIsLoadedRuntimePresets = false;
};


UCLASS(Blueprintable)//	304
class UTHsPresetDataAsset_Dep : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Asset Properties",
		EditAnywhere,BlueprintReadWrite/*,meta=(DisplayPriority=0)*/)	
	FName ThemaName = NAME_None;
	
	UPROPERTY(Category = "Asset Properties",
		EditAnywhere,BlueprintReadWrite/*,
		meta=(DisplayPriority=1)*/)
	bool bIsReleaseVersion = false;

	UPROPERTY(Category = "Asset Properties",
		EditAnywhere,BlueprintReadWrite/*,
		meta=(DisplayPriority=2)*/)
	bool bUseSaveGameObjectForRuntimePresets = true;
	UPROPERTY(Category = "Asset Properties",
		EditAnywhere,BlueprintReadWrite)
	bool bUseAsyncLoadForRuntimePresets = true;
	
	
	UPROPERTY(Category = "Asset Properties | Runtime Preset Settings",
		EditAnywhere,BlueprintReadWrite/*,meta=(DisplayPriority=3)*/)
	int32 RuntimePresetCount = 128;
	
	UPROPERTY(Category = "Asset Properties",
		EditAnywhere,BlueprintReadWrite/*,
		meta=(DisplayPriority=4)*/)
	FTransform DefaultSpawnTransform = FTransform::Identity;
	
	UPROPERTY(Category = "Asset Properties",
		EditAnywhere,BlueprintReadWrite/*,
		meta=(DisplayPriority=5)*/)
	TSubclassOf<ATHsStuff_Dep> DefaultStuffClass = nullptr;
	
	UPROPERTY(Category = "Asset Properties",
		EditAnywhere,BlueprintReadWrite/*,meta=(DisplayPriority=6)*/)
	TSubclassOf<UTHsRootSceneComponent_Dep> DefaultRootSceneComponentClass = UTHsRootSceneComponent_Dep::StaticClass();
	
	UPROPERTY(Category = "Asset Properties | Runtime Preset Settings",
		EditAnywhere,BlueprintReadWrite,
		meta=(EditConditionHides="bUseSaveGameObjectForRuntimePresets==true"/*,DisplayPriority=7*/))
	TSubclassOf<UTHsPresetRuntimeSaveAgent_Dep> RuntimePresetSaverClass = UTHsPresetRuntimeSaveAgent_Dep::StaticClass();
	
	UPROPERTY(Category = "Asset Properties | Runtime Preset Settings",
		EditAnywhere,BlueprintReadWrite,
		meta=(EditConditionHides="bUseSaveGameObjectForRuntimePresets==false"/*,DisplayAfter="bUseSaveGameObjectForRuntimePresets"*/))
	FString RuntimePresetSaveTocName = TEXT("");

	UPROPERTY(Category = "Preset Nodes|Actor Presets",
		EditAnywhere,BlueprintReadWrite,meta=(ShowInnerProperties,FullyExpand=true,ShowOnlyInnerProperties))
	TMap<ETHsPresetNodeType_Dep,FTHsPresetNodeCollection> AllPresetNodesByType;

	UTHsPresetNode* FindDefaultPresetNodeByIndex(const ETHsPresetNodeType_Dep& nodeType,int32 index)
	{
		if (FTHsPresetNodeCollection* nodeCollection = AllPresetNodesByType.Find(nodeType))
		{
			return nodeCollection->FindPresetNodeByIndex(index);
		}
		return nullptr;
	}
	UTHsPresetNode* FindDefaultPresetNodeByMetaData(const FTHsPresetMetaData_Dep& metaData )
	{
		if (metaData.IsValidIndex())
		{
			return FindDefaultPresetNodeByIndex(metaData.PresetType,metaData.PresetIndex);	
		}
		return nullptr;
	}
	//static TMap<FName,FTHsPresetMetaData>&& 
	TMap<FName,FTHsPresetMetaData_Dep>&& GetAllPresetMetaDataMap() const
	{
		const bool isDefaultAsset = true;
		TMap<FName,FTHsPresetMetaData_Dep> result;
		constexpr int32 start =  static_cast<int32>(ETHsPresetNodeType_Dep::EJustNode);
		constexpr int32 max = static_cast<int32>(ETHsPresetNodeType_Dep::EMax);
		int32 reserveCount = 0;
		TArray<const FTHsPresetNodeCollection*> allCollections;
		allCollections.SetNum(max);
		
		for (int32 i = start; i < max; ++i)
		{
			allCollections[i] = nullptr;
			if (const FTHsPresetNodeCollection* found = AllPresetNodesByType.Find(ToFlags<ETHsPresetNodeType_Dep>(i)))
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
		return MoveTemp(result);
	}
};

UCLASS()
class UTHsPresetNodeBuilder: public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TWeakObjectPtr<UTHsPresetNode> TargetNode;
	UPROPERTY()
	TWeakObjectPtr<ATHsPresetManager> PresetManager = nullptr;
	UPROPERTY()
	UWorld* WorldContext = nullptr;

	bool IsValidBuilder() const;

	UTHsPresetNodeBuilder& BeginBuild(ATHsPresetManager* InPresetManager,UTHsPresetNode* targetNode);
	
	UTHsPresetNodeBuilder& SetNodeType(const ETHsPresetNodeType_Dep& InType)
	{
		if (IsValidBuilder())
		{
			TargetNode->NodeType = InType;
		}
		return *this;
	}
	UTHsPresetNodeBuilder& OnActorType(){return SetNodeType(ETHsPresetNodeType_Dep::EActor);}
	UTHsPresetNodeBuilder& OnComponentType(){return SetNodeType(ETHsPresetNodeType_Dep::EComponent);}
	UTHsPresetNodeBuilder& OnGroupType(){return SetNodeType(ETHsPresetNodeType_Dep::EGroup);}
	
	UTHsPresetNodeBuilder& SetNodeOptions(const int32& InOptions)
	{
		if (IsValidBuilder())
		{
			TargetNode->NodeOptions = InOptions;
		}
		return *this;
	}
	UTHsPresetNodeBuilder& OnNodeOptionsOr(const int32& InOptions)
	{
		return SetNodeOptions(TargetNode->NodeOptions | InOptions);
	}
	UTHsPresetNodeBuilder& OnNodeOptionsNot(const int32& InOptions)
	{
		return SetNodeOptions(TargetNode->NodeOptions & (~InOptions));
	}
	UTHsPresetNodeBuilder& OnOptUsePresetName(){ return OnNodeOptionsOr(static_cast<int32>(ETHsPresetNodeOptions::EUsePresetName)); }
	UTHsPresetNodeBuilder& OffOptUsePresetName(){ return OnNodeOptionsNot(static_cast<int32>(ETHsPresetNodeOptions::EUsePresetName)); }
	
	UTHsPresetNodeBuilder& OnOptUseOverrideNode(){ return OnNodeOptionsOr(static_cast<int32>(ETHsPresetNodeOptions::EUseOverrideNode)); }
	UTHsPresetNodeBuilder& OffOptUseOverrideNode(){ return OnNodeOptionsNot(static_cast<int32>(ETHsPresetNodeOptions::EUseOverrideNode)); }
	
	UTHsPresetNodeBuilder& OnOptUseDefaultRootScene(){ return OnNodeOptionsOr(static_cast<int32>(ETHsPresetNodeOptions::EUseDefaultRootScene)); }
	UTHsPresetNodeBuilder& OffOptUseDefaultRootScene(){ return OnNodeOptionsNot(static_cast<int32>(ETHsPresetNodeOptions::EUseDefaultRootScene)); }
	
	UTHsPresetNodeBuilder& OnOptDefaultPreset(){ return OnNodeOptionsOr(static_cast<int32>(ETHsPresetNodeOptions::EDefaultPreset)); }
	UTHsPresetNodeBuilder& OffOptDefaultPreset(){ return OnNodeOptionsNot(static_cast<int32>(ETHsPresetNodeOptions::EDefaultPreset)); }

	UTHsPresetNodeBuilder& OnOptSetDefaultPreset()
	{
		return SetNodeOptions(static_cast<int32>(ETHsPresetNodeOptions::ESetDefaultPresetOption));
	}
	UTHsPresetNodeBuilder& OnOptSetRuntimePreset()
	{
		return SetNodeOptions(static_cast<int32>(ETHsPresetNodeOptions::ESetRuntimePresetOption));
	}

	UTHsPresetNodeBuilder& OnOptEditingMode(){ return OnNodeOptionsOr(static_cast<int32>(ETHsPresetNodeOptions::EEditingMode)); }
	UTHsPresetNodeBuilder& OffOptEditingMode(){ return OnNodeOptionsNot(static_cast<int32>(ETHsPresetNodeOptions::EEditingMode)); }
	
	UTHsPresetNodeBuilder& SetNodeName(const FName& InName)
	{
		if (IsValidBuilder())
		{
			TargetNode->PresetName = InName;
		}
		return *this;
	}
	UTHsPresetNodeBuilder& OnNodeNameTypeNumbering(const FString& inPrefix = TEXT("PresetNode"));
	UTHsPresetNodeBuilder& OnNodeNameCheckOverlapNumbering(const FString& presetName);

		
	UTHsPresetNodeBuilder& SetNodeTransform(const FTransform& InTransform)
	{
		if (IsValidBuilder())
		{
			TargetNode->RelativeTransform = InTransform;
		}
		return *this;
	}
	UTHsPresetNodeBuilder& SetActorClass(const TSubclassOf<ATHsStuff_Dep>& InActorClass)
	{
		if (IsValidBuilder() && TargetNode->NodeType == ETHsPresetNodeType_Dep::EActor)
		{
			TargetNode->ActorClass = InActorClass;
		}
		return *this;
	}
	
	UTHsPresetNodeBuilder& SetActorClass(const TSubclassOf<USceneComponent>& InSceneClass)
	{
		if (IsValidBuilder() && TargetNode->NodeType == ETHsPresetNodeType_Dep::EComponent)
		{
			TargetNode->ActorClass = InSceneClass;
		}
		return *this;
	}
	UTHsPresetNodeBuilder& SetNodeTags(const FGameplayTagContainer& InTags)
	{
		if (IsValidBuilder())
		{
			TargetNode->NodeTags = InTags;
		}
		return *this;
	}
	UTHsPresetNodeBuilder& OnNodeTagsAdd(const FGameplayTag& InTag)
	{
		if (IsValidBuilder())
		{
			TargetNode->NodeTags.AddTag(InTag);
		}
		return *this;
	}
	UTHsPresetNodeBuilder& OnNodeTagsRemove(const FGameplayTag& InTag)
	{
		if (IsValidBuilder())
		{
			TargetNode->NodeTags.RemoveTag(InTag);
		}
		return *this;
	}
	UTHsPresetNodeBuilder& OnNodeTag_ActionTraitAttachable();
	UTHsPresetNodeBuilder& OffNodeTag_ActionTraitAttachable();
	UTHsPresetNodeBuilder& OnNodeTag_ActionTraitHoverable();
	UTHsPresetNodeBuilder& OffNodeTag_ActionTraitHoverable();
	UTHsPresetNodeBuilder& OnNodeTag_ActionTraitSelectable();
	UTHsPresetNodeBuilder& OffNodeTag_ActionTraitSelectable();
	UTHsPresetNodeBuilder& OnNodeTag_ActionTraitMovable();
	UTHsPresetNodeBuilder& OffNodeTag_ActionTraitMovable();
	
	UTHsPresetNodeBuilder& OnNodeTagAddAllActionTraits()
	{
		return OnNodeTag_ActionTraitAttachable()
			.OnNodeTag_ActionTraitHoverable()
			.OnNodeTag_ActionTraitSelectable()
			.OnNodeTag_ActionTraitMovable();;
	}
	UTHsPresetNodeBuilder& OnNodeTagRemoveAllActionTraits()
	{
		return OffNodeTag_ActionTraitAttachable()
			.OffNodeTag_ActionTraitHoverable()
			.OffNodeTag_ActionTraitSelectable()
			.OffNodeTag_ActionTraitMovable();
	}
	
	UTHsPresetNodeBuilder& SetOverrideNode(UObject* InOverrideNode)
	{
		if (IsValidBuilder())
		{
			TargetNode->OverrideNode = InOverrideNode;
		}
		return *this;
	}
	UTHsPresetNodeBuilder& AddChildNode(UTHsPresetNode* InChildNode)
	{
		if (IsValidBuilder() && InChildNode)
		{
			TargetNode->ChildNodes.Add(InChildNode);
		}
		return *this;
	}
	UTHsPresetNodeBuilder& SetPresetAppearance(UTHsPresetNodeAppearance* InAppearance)
	{
		if (IsValidBuilder())
		{
			TargetNode->PresetAppearance = InAppearance;
		}
		return *this;
	}
	bool FinalizeBuild()
	{
	   
		bool result = true;
		result &= IsValidBuilder();
		result &= TargetNode->IsEnsuredTypeValid();
		return result;
	}
	
	static TObjectPtr<UTHsPresetNodeBuilder> CreatePresetNodeBuilder(UObject* outer = nullptr)
	{
		return NewObject<UTHsPresetNodeBuilder>(outer);
	}
	static UTHsPresetNode* DefaultPresetNodeActorType(UObject* outer = nullptr);
private:
	UPROPERTY()
	bool mIsBuildPossible = false;
};


UCLASS()
class ATHsPresetManager : public AActor
{
	GENERATED_BODY()

public:
	ATHsPresetManager();
	//TEXT("/Script/TIHHousingCore.THsStuffPresetDataAsset'/TIHHousingSystem/resources/db/Bp_StuffPreset1.Bp_StuffPreset1'")
	
	void StartPresetManager();

	UFUNCTION()
	FString GetRuntimePresetTocName() const 
	{
		return mLoadedPresetDataAsset ?
			mLoadedPresetDataAsset->RuntimePresetSaveTocName :
			TEXT("");
	}
	UFUNCTION()
	int32 GetRuntimePresetMaxCount() const
	{
		return mLoadedPresetDataAsset ?
			mLoadedPresetDataAsset->RuntimePresetCount :
			ReserveRuntimePresetCount;
	}

	static TWeakObjectPtr<ATHsPresetManager> gPresetManagerInstance;
	static TWeakObjectPtr<ATHsPresetManager> GetPresetManagerInstance()
	{
		return gPresetManagerInstance;
	}

	
	/**
	 * @retval true mLoadedPresetDataAsset 과 mIsAppliedPresetManagerSettings 이 모두 완료됨을 보장함.
	 * @retval false 둘중 하나가 터졌다는 증거
	 */
	bool IsPresetManagerReady() const
	{
		return mIsLoadedDataAsset && mIsAppliedPresetManagerSettings;
	}

	/**
	 * @brief mIsLoadedDataAsset 를 재 검증합니다.
	 * @retval true 프리셋 데이터 에셋이 로드되었음을 보장합니다.
	 */
	bool VerifyLoadedDataAsset()
	{
		mIsLoadedDataAsset = (mLoadedPresetDataAsset != nullptr && IsValid(mLoadedPresetDataAsset));
		return mIsLoadedDataAsset;
	}

	/**
	 * @brief 매니저 셋팅이 올바르게 적용되었는지 재검증합니다.
	 * @reval true 프리셋 매니저 설정이 올바르게 적용되었음을 보장합니다.
	 */
	bool VerifyAppliedPresetManagerSettings()
	{
		bool result = true;
		result &= CurrentDataAssetThemaName.IsValid();
		result &= DefaultStuffClass != nullptr;
		result &= DefaultRootComponentClass != nullptr;
		result &= RuntimePresetSaveTocName.IsEmpty() == false;
		mIsAppliedPresetManagerSettings = result;
		return result;	
	}

	/**
	 * @def defaultPreset == 디폴트프리셋 == 에디터프리셋 은 동일한 개념, 사전에 에디터에서 구성된 프리셋임.
	 * @brief 에셋경로에 있는 것을 가지고 디폴트 프리셋들을 불러옵니다. 
	 * @param assetPath 디폴트 에셋의 경로대로 불러옵니다.
	 * @details 보통 제일 먼저 호출됩니다. beginPlay 에서 호출됨.
	 * @retval VerifyLoadedDataAsset() 데이터 에셋을 검증한 결과를 리턴합니다.
	 */
	bool LoadDefaultPresets(const FSoftObjectPath& assetPath);
	/**
	 * @brief 로드된 데이터 에셋에서 프리셋 매니저 설정을 적용합니다. 특수한 설정정보를 불러오는 역할을 합니다.
	 * @details 보통 로드된 mLoadedPresetDataAsset 를 숨겨진 인자로 받아서 처리합니다.
	 */
	void ApplyPresetManagerSettingsFromDataAsset();
	/**
	 * @brief 디폴트 프리셋들을 저장합니다.
	 * @details 보통 매니저가 시작될때 한번 호출됩니다.
	 */
	void StoreDefaultPresets();

	bool IsPresetMgrInitValid() const
	{
		return IsPresetManagerReady();
	}

	/**
	 * @brief 프리셋 매니저가 디폴트 값을 저장 준비가 되었는지 여부를 반환합니다. 검증버전입니다.
	 * @return true 프리셋 매니저가 프리셋 값(default,runtime)들을 저장 준비가 되었음을 보장합니다.
	 */
	bool VerifyPresetMgrStoreReady()
	{
		return VerifyLoadedDataAsset() && VerifyAppliedPresetManagerSettings() && mLoadedPresetDataAsset->bIsReleaseVersion;
	}
	/**
	 * @brief 프리셋 매니저가 디폴트 값을 저장 준비가 되었는지 여부를 반환합니다. 간소화 버전입니다.
	 * @return true 프리셋 매니저가 프리셋 값들을 저장 준비가 되었음을 '약하게' 보장합니다.
	 */
	bool IsPresetMgrReadyToStore() const
	{
		return mIsLoadedDataAsset && mIsAppliedPresetManagerSettings && mLoadedPresetDataAsset->bIsReleaseVersion;
	}
	
	bool IsPresetMgrStoredDefaultPresets() const
	{
		return mIsStoredDefaultPresets && not mPresetNameIndexTable.IsEmpty();
	}
	
	void SnapshotWidgetData(){}
	
	int32* CurrentSelectedSaveSlotIndexPtr = nullptr;
	int32* CurrentSelectedUserIndexPtr = nullptr;
	
	
	bool mIsLoadedRuntimePresets = false;


	/**
	 * 
	 * @return 
	 */
	void StoreRuntimePresets();

	/*
	 *	여기에서 작업하자
	 *	emptyPreset -> select CurrentSelectedPresetNode
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSoftObjectPath DefaultPresetDataAssetPath = FSoftObjectPath(TEXT("/Script/TIHHousingCore.THsPresetDataAsset'/TIHHousingSystem/resources/db/Bp_StuffPreset1.Bp_StuffPreset1'"));
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString RuntimePresetSaveTocName = TEXT("DefaultTOC");
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<UTHsPresetNode> CurrentSelectedPresetNode;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<ATHsStuff_Dep> CurrentFocusPresetStuff;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	ETHsPresetNodeType_Dep CurrentEditingPresetNodeType = ETHsPresetNodeType_Dep::EJustNode;

	FTransform CurrentSpawnTransform = FTransform::Identity;
	FName CurrentDataAssetThemaName = NAME_None;
	int32 ReserveRuntimePresetCount = 256;
	
	UClass* DefaultStuffClass;
	UClass* DefaultRootComponentClass;
	
	
	UPROPERTY()
	FString DisplayMessage;

	ATHsStuff_Dep* SpawnPresetStuffActorByPresetName(const FName& presetName,const FTransform& spawnTransform);
	UTHsPresetNode* CreatePresetNodeFromActor(ATHsStuff_Dep* sourceStuff){return nullptr;}

	
	UFUNCTION(BlueprintCallable)
	void BeginEditPresetNodeByName(const FName& presetName)
	{
		CreatePresetNode(presetName,CurrentEditingPresetNodeType);
	}
	
	UFUNCTION(BlueprintCallable)
	UTHsPresetNode* CreatePresetNode(const FName& presetName,ETHsPresetNodeType_Dep nodeType = ETHsPresetNodeType_Dep::EJustNode);
	
	
	int32 GetPresetNodeCountByType(ETHsPresetNodeType_Dep presetType) const
	{
		if (const int32* presetCount = mPresetCountTable.Find(presetType))
		{
			return *presetCount;
		}
		return INDEX_NONE;
	}
	int32 GetActorPresetNodeCount() const	{	return GetPresetNodeCountByType(ETHsPresetNodeType_Dep::EActor); }
	int32 GetComponentPresetNodeCount() const	{	return GetPresetNodeCountByType(ETHsPresetNodeType_Dep::EComponent); }
	int32 GetGroupPresetNodeCount() const	{	return GetPresetNodeCountByType(ETHsPresetNodeType_Dep::EGroup);	}

	UTHsPresetNode* GetCurrentEditingPresetNode() const
	{
		if (not CurrentSelectedPresetNode->IsOptDefaultPreset() && not CurrentSelectedPresetNode->IsOptDefaultPreset())
		{
			return CurrentSelectedPresetNode.Get();
		}
		return nullptr;
	}
	

	UTHsPresetNode* GetCurrentSelectPresetNode() const
	{
		return CurrentSelectedPresetNode.Get();
	}

	void SetFocusPresetStuff(ATHsStuff_Dep* focusStuff)
	{
		CurrentFocusPresetStuff = focusStuff;
		if (CurrentFocusPresetStuff.IsValid())
		{
			CurrentFocusPresetStuff->OverrideSelection(true);
		}
	}
	//	최초에는 어지간해서는 이름으로 찾는거임.
	UTHsPresetNode* GetPresetNodeByName(const FName& presetName) const
	{
		const FTHsPresetMetaData_Dep& presetMetaData= GetPresetMetaDataByName(presetName);
		if (presetMetaData.IsValidIndex())
		{
			return GetPresetNodeByMetaData(presetMetaData);
		}
		return nullptr;
	}
	//	weakobjectPtr 버전
	TWeakObjectPtr<UTHsPresetNode> GetPresetNodeWeakByName(const FName& presetName) const
	{
		return GetPresetNodeByName(presetName);
	}
	int32 GetPresetNodeIndexByName(const FName& presetName) const
	{
		const FTHsPresetMetaData_Dep& presetMetaData= GetPresetMetaDataByName(presetName);
		if (presetMetaData.IsValidIndex())
		{
			return presetMetaData.PresetIndex;
		}
		return INDEX_NONE;
	}

#pragma region HelperFunctionsForPresetNodeByName
	
	FTHsPresetMetaData_Dep GetPresetMetaDataByName(const FName& presetName) const
	{
		if (const FTHsPresetMetaData_Dep* presetMeta = mPresetNameIndexTable.Find(presetName))
		{
			return *presetMeta;
		}
		return FTHsPresetMetaData_Dep();
	}
	
	bool IsExistPresetNodeByName(const FName& presetName) const
	{
		const FTHsPresetMetaData_Dep& presetMetaData= GetPresetMetaDataByName(presetName);
		return presetMetaData.IsValidIndex();
	}
	bool IsDefaultAssetPresetByName(const FName& presetName) const
	{
		const FTHsPresetMetaData_Dep& presetMetaData= GetPresetMetaDataByName(presetName);
		return presetMetaData.IsValidIndex() && presetMetaData.bIsDefaultAsset;
	}
	ETHsPresetNodeType_Dep GetPresetNodeTypeByName(const FName& presetName) const
	{
		const FTHsPresetMetaData_Dep& presetMetaData= GetPresetMetaDataByName(presetName);
		return presetMetaData.IsValidIndex() ? presetMetaData.PresetType : ETHsPresetNodeType_Dep::EJustNode;
	}
	bool IsPresetNodeEditingModeByName(const FName& presetName) const
	{
		if (UTHsPresetNode* presetNode = GetPresetNodeByName(presetName))
		{
			return presetNode->IsOptEditingMode();
		}
		return false;
	}
	FTransform IsPresetNodeTransformByName(const FName& presetName) const
	{
		if (UTHsPresetNode* presetNode = GetPresetNodeByName(presetName))
		{
			return presetNode->RelativeTransform;
		}
		return FTransform::Identity;
	}

	UTHsPresetNode* GetPresetNodeByMetaData(const FTHsPresetMetaData_Dep& presetMetaData) const
	{
		if (presetMetaData.IsValidIndex())
		{
			if (presetMetaData.bIsDefaultAsset)
			{
				if (mLoadedPresetDataAsset != nullptr)
				{
					return  mLoadedPresetDataAsset->FindDefaultPresetNodeByMetaData(presetMetaData);
				}
			}
			else
			{
				if (mRuntimePresetAgent)
				{
					return mRuntimePresetAgent->FindRuntimePresetNodeByMetaData(presetMetaData);
				}
			}
		}
		return nullptr;
	}
#pragma endregion 
	
	const FName& GetThemaName() const
	{
		return CurrentDataAssetThemaName;
	}
	void SetFrameworkSubsystem(UTHsFrameworkSubsystem_Dep* subsystem)
	{
		mFrameworkSubsystem = subsystem;
	}

	static TMap<FName, FTHsPresetMetaData_Dep> GetAllPresetMetaDataMap(
		const TMap<ETHsPresetNodeType_Dep, FTHsPresetNodeCollection>& allPresetNodesByType,
		const bool isDefaultAsset);


	virtual void BeginDestroy() override;

protected:
	//	DEFAULT
	virtual void BeginPlay() override;

private:
	
	UPROPERTY()
	TObjectPtr<UTHsRootSceneComponent_Dep> mDefaultRootScene = nullptr;
	
	UPROPERTY()
	class UTHsFrameworkSubsystem_Dep* mFrameworkSubsystem = nullptr;
	//	PresetManager Instance Variables

	//	Data Storage
	UPROPERTY()
	TObjectPtr<UTHsPresetDataAsset_Dep> mLoadedPresetDataAsset = nullptr;	//	여기에 default 데이터
	UPROPERTY()
	TObjectPtr<UTHsPresetRuntimeSaveAgent_Dep> mRuntimePresetAgent = nullptr; //	여기에 런타임 preset 들 저장
	
	//	Runtime Preset Data
	UPROPERTY()
	TArray<TObjectPtr<ATHsStuff_Dep>> mSpawnedStuffs;

	//	for Quick Access or Search
	UPROPERTY()
	TMap<FName,FTHsPresetMetaData_Dep> mPresetNameIndexTable;	//	name -> (type, isDefaultAsset, index in typeArray)
	UPROPERTY()
	TMap<ETHsPresetNodeType_Dep,int32> mPresetCountTable;
	

	UPROPERTY()
	int32 mCurrentEditingPresetIndex = INDEX_NONE;
	
	UPROPERTY()
	TObjectPtr<UTHsPresetEditorWidget> mPresetEditorWidget = nullptr;

	/**
	 * @brief mLoadedPresetDataAsset != nullptr : 프리셋 매니저가 준비된 경우, mLoadedPresetDataAsset 가 nullptr 이 아님을 보장합니다.
	 */
	bool mIsLoadedDataAsset = false;
	/**
	 * @brief mIsAppliedPresetManagerSettings == true : 프리셋 매니저가 준비된 경우, mIsAppliedPresetManagerSettings 가 true 임을 보장합니다.
	 */
	bool mIsAppliedPresetManagerSettings = false;
	/**
	 * @brief 로드된 프리셋 데이터 에셋
	 */
	bool mIsStoredDefaultPresets = false;
};


UCLASS()
class UTHsFrameworkSubsystem_Dep : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	FTHsPresetDelegateAfterStorePreset OnAfterStorePreset;
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		return false;
	}

private:
	UWorld* mWorldContext = nullptr;
};

UCLASS()
class TIHHOUSINGCORE_API UTIHCollisionCore : public UObject
{
	GENERATED_BODY()
};