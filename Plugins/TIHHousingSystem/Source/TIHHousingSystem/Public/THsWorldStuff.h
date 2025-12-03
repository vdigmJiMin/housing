// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveGizmoManager.h"
#include "BaseGizmos/TransformProxy.h"
#include "Cores/TIHHsCoreInterface.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "THsWorldStuff.generated.h"

UCLASS()
class ATHsDefaultGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	/*
	 *	TODO: 날씨나 라이트 같은 설정 값들. 월드를 컨트롤 하기 위한것
	 *
	 * 
	 */
};

UCLASS()
class ATHsDefaultPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	/*
 *	TODO: 유저 개인 정보, 인벤토리, 돈, 경험치, 상태등등
 *
 * 
 */
};


UCLASS()
class ATHsDefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	ATHsDefaultPlayerController();
	/*
	 *	TODO: 유저 입력, 카메라 컨트롤, UI 컨트롤, 세이브/로드 같은것들
	 *	근데 이건 이미 모든 키입력을 EnhancedInput로 받고있으니 이놈이 할건 그냥 그 인풋을 위한 컨트롤을 만들어주는거임.
	 *	근데 그것도 사실 커스텀 인풋 시스템에서 다음 틱에 이놈이 생성되고 하라고 해놔서 이놈은 사실 그냥 할게 딱히 없음.
	 *	그래도 이놈이 있어야 플레이어가 생기니까. 나중에 해주자.
	 * 
	 */

};

UCLASS()
class ATHsDefaultPawn : public APawn , public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	/*
	 *	이놈도 사실 딱히 할게 없음. 인풋 시스템에 다음 이벤트시에 호출하라고 만드는거 말고는
	 *	근데 장점이 poccess라는 것이랑 계층을 가질 수 있어서 그걸 이용해서 시점으로 써먹는건 괜찮음
	 *	해당 놈에게 들어오면 버튼이 이거이거 있고, 그걸 호출한다. 이렇게 해줘도 되는거지. 그게 가능하게 만들었으니깐.
	 *	즉 이놈에게 들어오면 키를 바인드 해줄 뿐임. 그건 언리얼의 철학과 같은데, 나는 더 편하게 키를 모두 쓰게 만들어 줬으니
	 *	이놈이 할건 그것에 바인드 걸었다가 풀었다 하는거고 다행히 나는 그걸 중앙에서 통제 가능함. 즉 모든 키를 통제 가능하게 만든거임.
	 *	그니깐 인풋에 대한게 인풋시스템에 들어가면 이놈이 필요로 하는것에 바인딩 걸어줌. 그리고 호출해줄뿐임.
	 * 
	 */

	
	
private:
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* mCamera;
	
};

UCLASS()
class ATHsDefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/*
	TSubclassOf<AGameSession> GameSessionClass;
	TSubclassOf<AGameStateBase> GameStateClass;
	TSubclassOf<APlayerController> PlayerControllerClass;
	TSubclassOf<APlayerState> PlayerStateClass;
	TSubclassOf<AHUD> HUDClass;
	TSubclassOf<APawn> DefaultPawnClass;
	TSubclassOf<ASpectatorPawn> SpectatorClass;
	TSubclassOf<APlayerController> ReplaySpectatorPlayerControllerClass;
	TSubclassOf<AServerStatReplicator> ServerStatReplicatorClass;
	EReplicationSystem GetGameNetDriverReplicationSystem() const { return GameNetDriverReplicationSystem; }
	TObjectPtr<AGameSession> GameSession;
	TObjectPtr<AGameStateBase> GameState;
	 */
	ATHsDefaultGameMode();
};


UCLASS()
class TIHHOUSINGSYSTEM_API ATHsWorldStuff : public AActor,public ITIHHsInteraction
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATHsWorldStuff();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn) override;

private:
	UPROPERTY()
	USceneComponent* mRootComponent;
	UPROPERTY()
	UStaticMeshComponent* mStaticMeshComponent;

	UPROPERTY()
	UInteractiveGizmoManager* mInteractiveGizmoManager;

	UPROPERTY()
	UCombinedTransformGizmo* mCombinedTransformGizmo;
	UPROPERTY()
	UTransformProxy* mTransformProxy;
};
