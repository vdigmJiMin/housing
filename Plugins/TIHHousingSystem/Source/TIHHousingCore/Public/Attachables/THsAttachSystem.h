// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGizmos/CombinedTransformGizmo.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Cores/TIHHsCoreInterface.h"
#include "UObject/Object.h"
#include "THsAttachSystem.generated.h"

/**
 * 
 */
UCLASS()
class TIHHOUSINGCORE_API UTHsAttachSystem : public UObject
{
	GENERATED_BODY()
};
/*
 *	룰베이스를 버리고 전용을 선택했다!
 *	Attachable Surface 와 Attachment Point 를 명확히 구분하고 각각 역할대로만 만든다!
 *		전용 기즈모 유오브젝트를 만든다잇!
 *			그리고 그걸 담을 컴포넌트를 만든다면 그게 트리거컴포넌트임!
 *		전용 어테치 서페이스를 만든다잇! attachSurface
 *			그게 바로 표면임! 그게 바로 데크컴포넌트인데, 어차피 스터프 버렸으니 지금은 생각을 좀 해봐야함!
*		전용 어테치먼트 포인트를 만든다잇! attachPoint
*			이게 바로 붙을 표면인데, 음...서페이스와 포인트 하나로 합칠까? 아니! 분리하고 처리해주는 컴포넌트를 하나로 합치는게 맞음. 즉 데이터는 나누고 처리는 하나로!
 */
namespace TIHUtils
{
	namespace Attach
	{
		static const FName THsDefaultGizmoName = FName(TEXT("THsDefaultGizmo"));
	}
}

DECLARE_MULTICAST_DELEGATE_TwoParams(FTHsFloorMeshChangedDelegate, UStaticMesh*, TWeakObjectPtr<UTHsFloorComponent>);

UCLASS()
class UTHsFloorComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:

	FTHsFloorMeshChangedDelegate DelegateFloorMeshChanged;
	
	UFUNCTION(BlueprintCallable)
	void AsyncSetFloorMeshByPath( const FString& inPath)
	{
		if (inPath.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("AsyncSetFloorMeshByPath: inPath is empty"));
			return;
		}
		TWeakObjectPtr<UTHsFloorComponent> weakThis(this);
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [inPath, weakThis]()
		{
			UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *inPath));
			if (mesh == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("AsyncSetFloorMeshByPath: Failed to load mesh from path: %s"), *inPath);
				return;
			}
			AsyncTask(ENamedThreads::GameThread, [mesh, weakThis]()
			{
				if (weakThis.IsValid())
				{
					weakThis->SetFloorMesh(mesh);
				}
			});
		});
	}
	
	UFUNCTION(BlueprintCallable)
	void SetFloorMeshByPath( const FString& inPath)
	{
		if (inPath.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("SetFloorMeshByPath: inPath is empty"));
			return;
		}
		UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *inPath));
		if (mesh == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SetFloorMeshByPath: Failed to load mesh from path: %s"), *inPath);
			return;
		}
		SetFloorMesh(mesh);
	}
	
	UFUNCTION(BlueprintCallable)
	void SetFloorMesh(UStaticMesh* inMesh)
	{
		if (inMesh == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SetFloorMesh: inMesh is nullptr"));
			return;
		}
		mFloorMesh = inMesh;
		SetStaticMesh(mFloorMesh);
		DelegateFloorMeshChanged.Broadcast(mFloorMesh,this);
	}
	UFUNCTION()
	UStaticMesh* GetFloorMesh() const
	{
		return mFloorMesh;
	}
	
	
private:
	UPROPERTY()
	UStaticMesh* mFloorMesh = nullptr;
};

USTRUCT()
struct FTHsFloorLayout
{
	GENERATED_BODY()

	UPROPERTY()
	TScriptInterface<ITIHHsInteraction> TargetInterface = nullptr;

	
};


UCLASS(Blueprintable)
class ATHsAttachGizmoActor : public AGizmoActor
{
public:
	ATHsAttachGizmoActor();

private:
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> mGizmoRoot; //= nullptr;

	UPROPERTY()
	UGizmoRectangleComponent* mGizmoCenterRectPlane = nullptr;
	UPROPERTY()
	UStaticMeshComponent* mGizmoCenterMeshComponent = nullptr;
	
	TScriptInterface<ITIHHsInteraction> mTargetObject = nullptr;
};
UCLASS(Blueprintable)
class ATHsAttachTestingActor : public AActor
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr< ATHsAttachGizmoActor> mGizmoActor = nullptr;
private:
};
/*

    ○
  ┌ │ ┐
○ ─ ┼ ─ ○ 
  └ │ ┘
	○
 */