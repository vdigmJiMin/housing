// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cores/TIHHsCoreInterface.h"
#include "UObject/Object.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "TIHHsSurfaceCore.generated.h"

/**
 * @class UTIHHsSurfaceCore
 * @brief Represents the core implementation for surface-related functionality in the TI Housing module.
 *
 * This class provides the base structure for handling surface-related logic or features
 * within the TI Housing Core API. It extends from the UObject class, making it compatible
 * with Unreal Engine's object system.
 */
UCLASS()
class TIHHOUSINGCORE_API UTIHHsSurfaceCore : public UObject
{
	GENERATED_BODY()
};

/*
서페이스룰을 새로 만들고 싶을땐
	 1. UTIHHsSurfaceRuleProcBase 를 상속받는다.
	 2. GetProcessorName,GetProcessorFlag,를 반드시 구현해준다.
		1. GetProcessorName 는 프로세서의 이름을 반환한다.
		2. UTIHHsSurfaceProcessorRegistry::mProcessorTable 에서 
		   GetProcessorName()과 일치하는 프로세서를 찾는다.
		3. 일치하는 프로세서가 없으면, 프로세서를 새로 등록한다.
 */



USTRUCT()
struct FTIHHsSurfaceRuleProcResult
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform Transform;
	UPROPERTY()
	int32 Result;
	UPROPERTY()
	FTIHHsAttachmentRuleWrapper AttachmentRuleWrapper;	//	어태치먼트 룰

	FTIHHsSurfaceRuleProcResult()
		: Transform(FTransform::Identity), Result(0) , AttachmentRuleWrapper(FTIHHsAttachmentRuleWrapper()) {}
	FTIHHsSurfaceRuleProcResult(const FTransform& InTransform, int32 InResult = 0, const FTIHHsAttachmentRuleWrapper& InAttachmentRuleWrapper = FTIHHsAttachmentRuleWrapper())
		: Transform(InTransform), Result(InResult) , AttachmentRuleWrapper(InAttachmentRuleWrapper) {}
	FTIHHsSurfaceRuleProcResult(const FTIHHsSurfaceRuleProcResult& Other)
		: Transform(Other.Transform), Result(Other.Result), AttachmentRuleWrapper(Other.AttachmentRuleWrapper){}
	FTIHHsSurfaceRuleProcResult(FTIHHsSurfaceRuleProcResult&& Other) noexcept
		: Transform(MoveTemp(Other.Transform)), Result(Other.Result), AttachmentRuleWrapper(MoveTemp(Other.AttachmentRuleWrapper))
	{
		Other.Result = 0; // Reset the moved-from object's result
	}
	FTIHHsSurfaceRuleProcResult& operator=(const FTIHHsSurfaceRuleProcResult& Other)
	{
		if (this != &Other)
		{
			Transform = Other.Transform;
			Result = Other.Result;
			AttachmentRuleWrapper = Other.AttachmentRuleWrapper; // Copy the attachment rule wrapper
		}
		return *this;
	}
	FTIHHsSurfaceRuleProcResult& operator=(FTIHHsSurfaceRuleProcResult&& Other) noexcept
	{
		if (this != &Other)
		{
			Transform = MoveTemp(Other.Transform);
			Result = Other.Result;
			Other.Result = 0; // Reset the moved-from object's result
			AttachmentRuleWrapper = MoveTemp(Other.AttachmentRuleWrapper); // Move the attachment rule wrapper
		}
		return *this;
	}
	FString ToString() const
	{
		FString jsonString;
		jsonString.Append(TEXT("{\n"));
		jsonString.Append(TEXT("  \"transform\": {\n"));
		jsonString.Append(FString::Printf(TEXT("    \"location\": [%.2f, %.2f, %.2f],\n"), Transform.GetLocation().X, Transform.GetLocation().Y, Transform.GetLocation().Z));
		jsonString.Append(FString::Printf(TEXT("    \"rotation\": [%.2f, %.2f, %.2f, %.2f],\n"), Transform.GetRotation().X, Transform.GetRotation().Y, Transform.GetRotation().Z, Transform.GetRotation().W));
		jsonString.Append(FString::Printf(TEXT("    \"scale\": [%.2f, %.2f, %.2f]\n"), Transform.GetScale3D().X, Transform.GetScale3D().Y, Transform.GetScale3D().Z));
		jsonString.Append(TEXT("  },\n"));
		jsonString.Append(FString::Printf(TEXT("  \"result\": %d,\n"), Result));
		jsonString.Append(TEXT("  \"attachment_rule\": {\n"));
		jsonString.Append(AttachmentRuleWrapper.ToString());
		jsonString.Append(TEXT("  }\n"));
		jsonString.Append(TEXT("}"));
		return jsonString;
	}
	FTIHHsSurfaceRuleProcResult Clone() const
	{
		return FTIHHsSurfaceRuleProcResult(Transform, Result);
	}
	void Clear()
	{
		Transform = FTransform::Identity;
		Result = 0;
	}
	FTIHHsSurfaceRuleProcResult& operator=(const FTransform& InTransform)
	{
		Transform = InTransform;
		Result = 0; // Reset result when assigning a new transform
		return *this;
	}
	FTransform GetTransform() const
	{
		return Transform;
	}
	int32 GetResult() const
	{
		return Result;
	}
	
};

UENUM(BlueprintType, Meta = (Bitflags))
enum class ETIHHsSurfaceProcFlag : uint8
{
	ENone = 0x00 UMETA(Hidden),

	//	not ECtxRequired == optional or use const
	ECtxRequired = 0x01 UMETA(DisplayName = "Context Required"),	//	프로세스가 context를 요청하는지 여부

	//	not EStoreState == not store and not per instance
	EStoreState = 0x02 UMETA(DisplayName = "Store Context"),	//	프로세스가 context를 저장하는지 여부

	//	not ECalculate == not calculate and just condition
	ECalculate = 0x04 UMETA(DisplayName = "Calculate"),	//	프로세스가 계산을 수행하는지 여부
	//EVisualize = 0x08 UMETA(DisplayName = "Visualize"),	//	프로세스가 시각화를 수행하는지 여부
	//ECollision = 0x10 UMETA(DisplayName = "Collision"),	//	프로세스가 충돌 처리를 수행하는지 여부
};
ENUM_CLASS_FLAGS(ETIHHsSurfaceProcFlag);
/**
 * @class UTIHHsSurfaceRuleProcBase
 * @brief Abstract base class for all surface processors.
 *
 * This class defines the common interface for all surface processors in the system.
 * 
 * @warning Any non-abstract subclass **must** implement the following pure virtual functions:
 *          - FName GetProcessorName() const  
 *          - ETIHHsSurfaceProcType GetProcessorType() const
 *
 * These functions are critical for identifying and categorizing processors at runtime.
 * Failure to implement them will result in a compile-time error or a crash at runtime
 * if the default PURE_VIRTUAL fallback is executed.
 */
UCLASS(Blueprintable,Abstract)
class TIHHOUSINGCORE_API UTIHHsSurfaceRuleProcBase : public UObject
{
	GENERATED_BODY()
public:
	
	virtual FName GetProcessorName() const PURE_VIRTUAL
	(UTIHHsSurfaceProcBase::GetProcessorName,return TEXT("proc-base"); );
	
	virtual ETIHHsSurfaceProcFlag GetProcessorFlag() const PURE_VIRTUAL(UTIHHsSurfaceBase::GetProcessorType,return ETIHHsSurfaceProcFlag::ENone ;);	
	
	virtual FTIHHsSurfaceRuleProcResult Process(const FTIHHsSurfaceRuleProcResult& inTransform,class UTIHHsSurfaceBase* surface) const{
		return inTransform;
	};

	//	해당 프로세스가 context를 요청하는지, 그리고 target이 해당 context를 가지고 있는지 검증.
	virtual bool VerifyRequestedContext(const TMap<FName,FInstancedStruct>& surfaceContextTable ) const
	{
		bool result = true;
		ETIHHsSurfaceProcFlag procFlag = GetProcessorFlag();
		
		if (EnumHasAnyFlags(procFlag, ETIHHsSurfaceProcFlag::ECtxRequired))
		{
			result &= surfaceContextTable.Contains(GetProcessorName());
		}
		
		return result;
	}
	
	virtual void UnRegisterProcessorCallBack()
	{
		
	}
};



UCLASS()
class TIHHOUSINGCORE_API UTIHHsSurfaceProcRegistrySubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FName, TObjectPtr<UTIHHsSurfaceRuleProcBase>> mProcessorTable;	//	프로세서 이름과 프로세서 객체를 매핑하는 테이블
public:
	UFUNCTION()
	void RegisterProcessor(FName name, UTIHHsSurfaceRuleProcBase* processor);
	
	UFUNCTION()
	void UnregisterProcessor(FName name);
	
	UFUNCTION()
	UTIHHsSurfaceRuleProcBase* GetProcessor(FName name) const
	{
		if (mProcessorTable.Contains(name))
		{
			return mProcessorTable[name];
		}
		return nullptr;
	}

	// UFUNCTION()
	// static UTIHHsSurfaceProcessorRegistry* GetInstance(UTIHHsSurfaceProcessorRegistry* selfInit = nullptr)
	// {
	// 	static UTIHHsSurfaceProcessorRegistry* instance = nullptr;
	// 	if (instance == nullptr)
	// 	{
	// 		if (selfInit)
	// 		{
	// 			instance = selfInit;
	// 		}
	// 		else
	// 		{
	// 			instance = NewObject<UTIHHsSurfaceProcessorRegistry>();
	// 			instance->AddToRoot();
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if (selfInit)
	// 		{
	// 			UTIHHsSurfaceProcessorRegistry* prevInstance = instance;
	// 			selfInit->mProcessorTable.Append(instance->mProcessorTable);
	// 			selfInit->AddToRoot();
	// 			instance = selfInit;	//	새로운 인스턴스로 교체
	// 			prevInstance->RemoveFromRoot();
	// 		}
	// 	}
	// 	return instance;
	// }
	


	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
};
/*
UTIHHsSurfaceBase 는 표면에 대한 정보를 담고있다.
UTIHHsSurfaceBase::mProcessorNames 에 현재 표면에서 적용될 룰프로세서들의 이름을 담고있음.
	참고로 해당 이름은 UTIHHsSurfaceProcessorRegistry::mProcessorTable 에서 룰 프로세서를 찾을 때 사용됨.
	
 */
UCLASS()
class TIHHOUSINGCORE_API UTIHHsSurfaceBase : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetWorldContext(UWorld* world)
	{
		mWorld = world;
	}
	
	virtual void DrawDebug(UWorld* world,float duration = 0.0f)const{};
	
	virtual bool Intersect(const FRay& ray,FHitResult& outHitResult)const {return false;}
	virtual FVector GetClosestPoint(const FVector& point) const { return FVector::ZeroVector; }
	virtual FVector GetNormalAtPoint(const FVector& point) const { return FVector::ZeroVector; }
	
	virtual bool HasInitialized() const { return false; }
	virtual bool SupportsSnapping() const { return false; }
	virtual float GetSnapSize() const { return 0.0f; }

	FTIHHsSurfaceRuleProcResult ExecutePipeline(const FTIHHsSurfaceRuleProcResult& input);

	void AddProcessor(FName processorName)
	{
		if (not mProcessorNames.Contains(processorName))
		{
			mProcessorNames.Add(processorName);
		}
	}
	UFUNCTION(BlueprintCallable)
	void RemoveProcessor(FName processorName)
	{
		mProcessorNames.RemoveSwap(processorName);
	}
	UFUNCTION(BlueprintCallable)
	TArray<FName> GetProcessorNames() const
	{
		return mProcessorNames;
	}
	UFUNCTION(BlueprintCallable)
	TScriptInterface<ITIHHsInteraction> GetSurfaceOwner() const
	{
		return mSurfaceOwner;
	}
	UFUNCTION(BlueprintCallable)
	void SetSurfaceOwner(TScriptInterface<ITIHHsInteraction> owner)
	{
		mSurfaceOwner = owner;
	}
	UFUNCTION(BlueprintCallable)
	const TMap<FName,FInstancedStruct>& GetSurfaceContextTable()
	{
		return mSurfaceContextTable;
	}
	UFUNCTION(BlueprintCallable)
	const FInstancedStruct& GetSurfaceContext(FName contextName)
	{
		static FInstancedStruct emptyStruct;
		if (mSurfaceContextTable.Contains(contextName))
		{
			return mSurfaceContextTable[contextName];
		}
		return emptyStruct;
	}
	UFUNCTION(BlueprintCallable)
	FInstancedStruct& GetSurfaceContextMutable(FName contextName)
	{
		static FInstancedStruct emptyStruct;
		if (mSurfaceContextTable.Contains(contextName))
		{
			return mSurfaceContextTable[contextName];
		}
		return emptyStruct;
	}
	UFUNCTION(BlueprintCallable)
	TMap<FName,FInstancedStruct>& GetSurfaceContextTableMutable()
	{
		return mSurfaceContextTable;
	}
	UFUNCTION(BlueprintCallable)
	bool HasSurfaceContext(FName contextName) const
	{
		return mSurfaceContextTable.Contains(contextName);
	}

	virtual UWorld* GetWorld() const override
	{
		return mWorld;
	}

private:
	UPROPERTY()
	UWorld* mWorld;
	UPROPERTY()
	TArray<FName> mProcessorNames;	//	프로세서 이름들
	UPROPERTY()
	TScriptInterface<ITIHHsInteraction> mSurfaceOwner;	//	이 서피스를 소유하는 인터페이스, 예를 들어 AActor나 UTIHHsBaseObject 등
	
	/*
		UPROPERTY(EditAnywhere, Category = Foo, meta = (BaseStruct = "/Script/ModuleName.TestStructBase")) FInstancedStruct Test;
		UPROPERTY(EditAnywhere, Category = Foo, meta = (BaseStruct = "/Script/ModuleName.TestStructBase")) TArray<FInstancedStruct> TestArray;
		
		UFUNCTION(BlueprintCallable, Category="Ctx")
		void ModifyContext(UPARAM(ref) FInstancedStruct& InOutValue);
	*/
	
	UPROPERTY()
	TMap<FName,FInstancedStruct> mSurfaceContextTable;	//	서피스 프로퍼티, 서피스에 대한 추가적인 정보를 담는 맵, 이름과 인스턴스화된 구조체로 구성됨
};

USTRUCT()
struct FTIHHsSurfaceProcGridSnapContext
{
	GENERATED_BODY()
	UPROPERTY()
	float GridSize = 100.0f;	
	UPROPERTY()
	float RotationSnapAngle = 15.0f;	
};

UCLASS()
class TIHHOUSINGCORE_API UTIHHsSurfaceGridSnapProc : public UTIHHsSurfaceRuleProcBase
{
	GENERATED_BODY()
	static constexpr ETIHHsSurfaceProcFlag ProcessorTypeFlags = ETIHHsSurfaceProcFlag::ECalculate;
private:
	UPROPERTY()
	float mRefGridSize = 250.0f;	
	UPROPERTY()
	float mRefRotationSnapAngle = 15.0f;	//	회전 스냅 각도, 기본값은 15도
public:
	UFUNCTION()
	void SetRefGridSize(float gridSize)
	{
		mRefGridSize = gridSize;
	}
	UFUNCTION()
	void SetRefRotationSnapAngle(float angle)
	{
		mRefRotationSnapAngle = angle;
	}

	UFUNCTION(BlueprintCallable)
	virtual FName GetProcessorName() const override
	{
		return TEXT("grid-check");
	}
	UFUNCTION(BlueprintCallable)
	virtual ETIHHsSurfaceProcFlag GetProcessorFlag() const override
	{
		return ProcessorTypeFlags;	
	}
	virtual FTIHHsSurfaceRuleProcResult Process(const FTIHHsSurfaceRuleProcResult& inResult, UTIHHsSurfaceBase* surface) const override;
};
// UCLASS()
// class TIHHOUSINGCORE_API UTIHHsSurfaceAttachTestProc : public UTIHHsSurfaceProcBase
// {
// 	GENERATED_BODY()
// 	static constexpr ETIHHsSurfaceProcFlag ProcessorTypeFlags = ETIHHsSurfaceProcFlag::ECalculate;	//	상태를 유지하지 않는 프로세서로
// public:
// 	UFUNCTION(BlueprintCallable)
// 	virtual ETIHHsSurfaceProcFlag GetProcessorFlag() const override
// 	{
// 		return ProcessorTypeFlags;	//	상태를 유지하지 않는 프로세서로 설정
// 	}
// 	UFUNCTION(BlueprintCallable)
// 	virtual FName GetProcessorName() const override
// 	{
// 		return TEXT("attach-test");
// 	}
//
// 	virtual FTIHHsSurfaceProcResult Process(const FTIHHsSurfaceProcResult& inResult, UTIHHsSurfaceBase* surface) const override;
// private:
// 	mutable FHitResult mHitResult;
// };

/*
 *	plane을 체크하는것.
 *	내가 원하는거: 해당 물체의 규칙을 들고오는것
 *	정확히는 해당 물체의 서페이스를 가져온다. 그것과 내가 겹칠수 있는가 물어본다.
 *		물체의 경우에는 바닥에서 ray를 쏘고, 하는게 맞나? 표면을 
 */



UCLASS()
class UTIHHsPlaneSurface : public UTIHHsSurfaceBase
{
	GENERATED_BODY()
private:
	FPlane mPlane;
public:
	virtual void DrawDebug(UWorld* world,float duration = 0.0f)const override;
	UFUNCTION(BlueprintCallable)
	void Init(FPlane plane)
	{
		mPlane = plane;
	}
	UFUNCTION(BlueprintCallable)
	void SetPlane(const FPlane& plane)
	{
		
		mPlane = plane;
	}
	UFUNCTION(BlueprintCallable)
	void SetPlaneFromPointNormal(const FVector& point, const FVector& normal)
	{
		mPlane = FPlane(point, normal);
	}
	UFUNCTION(BlueprintCallable)
	void UpdatePlane(const FVector& Point, const FVector& Normal)
	{
		SetPlaneFromPointNormal(Point, Normal);
	}
	UFUNCTION(BlueprintCallable)
	void SetDependencyInjection(const TScriptInterface<ITIHHsInteraction>& owner)
	{
		SetSurfaceOwner(owner);
	}
	
	virtual bool Intersect(const FRay& ray, FHitResult& outHitResult) const override;
	virtual FVector GetClosestPoint(const FVector& point) const override;
	virtual bool SupportsSnapping() const override;
	virtual float GetSnapSize() const override;
	virtual bool HasInitialized() const override{return true;}
	virtual FVector GetNormalAtPoint(const FVector& point) const override;


	
};
UCLASS()
class UTIHHsCustomSurface : public UTIHHsSurfaceBase
{
	GENERATED_BODY()
public:
};

UCLASS()
class UTIHHsSurfaceUtility : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	static UTIHHsPlaneSurface* CreateAxisPlan(
		EAxis::Type ueAxis,
		const FVector& origin,
		UWorld* worldContext,
		UObject* outer = nullptr
	);
	UFUNCTION(BlueprintCallable)
	static UTIHHsPlaneSurface* CreateCameraPlan(
		const FVector& cameraLocation,
		const FVector& cameraForward,
		const FVector& targetPoint,
		UWorld* worldContext,
		UObject* outer = nullptr
	);
	UFUNCTION(BlueprintCallable)
	static UTIHHsPlaneSurface* CreatePlaneFrom3Points(
		const FVector& pointA,
		const FVector& pointB,
		const FVector& pointC,
		UWorld* worldContext,
		UObject* outer = nullptr
	);
	UFUNCTION(BlueprintCallable)
	static UTIHHsPlaneSurface* CreatePlaneFromHit(
		const FHitResult& hit,
		UWorld* worldContext,
		UObject* outer = nullptr
	);
	UFUNCTION(BlueprintCallable)
	static UTIHHsPlaneSurface* CreateLocalAxisPlane(
		const FTransform& transform,
   		EAxis::Type localAxis,
   		UWorld* worldContext,
   		UObject* outer = nullptr
   );
	
};


