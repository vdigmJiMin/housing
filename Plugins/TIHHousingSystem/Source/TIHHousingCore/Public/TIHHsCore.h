#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Cores/TIHHsCoreEnums.h"
#include "Cores/TIHHsCoreStructures.h"
#include "Cores/TIHHsCoreInterface.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameModeBase.h"
#include "TIHHsCore.generated.h"

/*
 *
 *	Interface들을 위해 CoreInterface를 만들면
 *		CoreEnum <- CoreInterface <- CoreSystem <- Core 
 * 
 */

class UTIHHsTriggerComponent;

UCLASS()
class TIHHOUSINGCORE_API UTIHHsGlobalConfigure : public UObject
{
	GENERATED_BODY()
public:
	
	UFUNCTION()
	static bool IsTickEnable(UWorld* world) 
	{
		bool reValue = false;
		if (not gForceDisable)
		{
			if (world == nullptr)
			{
				reValue = false;
			}
			else
			{
				if (world->IsPaused())
				{
					reValue = gTickPauseEnable;
				}
				else
				{
					reValue = gTickRuntimeActive;
				}
			}
		}
		return reValue;
	}
	
	UFUNCTION()
	static void SetTickForceDisable(bool inForceDisable){gForceDisable = inForceDisable;}
	UFUNCTION()
	static bool IsTickForceDisable() {return gForceDisable;}

	UFUNCTION()
	static void SetTickPauseEnable(bool inTickPauseEnable){gTickPauseEnable = inTickPauseEnable;}
	UFUNCTION()
	static bool IsTickPauseEnable() {return gTickPauseEnable;}

	UFUNCTION()
	static void SetTickRuntimeActiveEnable(bool inTickPauseEnable){gTickPauseEnable = inTickPauseEnable;}
	UFUNCTION()
	static bool IsTickRuntimeActive() {return gTickPauseEnable;}

	
	//	IsTickableInEditor 이거만 다르게
private:
	static bool gForceDisable;
	static bool gTickPauseEnable;
	static bool gTickRuntimeActive;	//	이거는 개별로 해도 되는거임
};

//class UTIHHsTapActionSubSystem;

/*
 *	commandSubsystem 에 들어가야 하는 기능들
 *	명령함수 등록
 *		참고로 이건 어디서든 호출가능한 글로벌한 것이다.
*	명령함수 호출
*	명령함수 등록 해제
*	명령함수가 valid 한지 확인
 *	액터에게 직접호출
 *	명령 호출
 *	게임플레이 커맨드 라인 실행임
 *	나중에 언리얼의 커맨드 라인 실행과 통합을 시켜야함
 * 
 */
/*
	생각을 해보자. 힙타이머
	레지스트를 하는 시점에서의 delayTime과 타이머시간을 넣는다.
	그럼 현재 시점에서 시간의 작동이 시작된다.
	자...이거 생각보다 복잡할거니깐 이걸 만들기 전에 이걸 만들었을때 뭐를 할건지 생각해보자.
	모든 틱타임을 대체할 수 있나?
	상태들이 전이 될때 자동화가 가능해진다. 그럼 state가 변화시 호출될 함수도 정할 수 있다. 
	상태들을 생각해보자. 상태의 FSM을 만들것도 생각해야한다.
	노드 + 전이컨디션 + 결과 이렇게 있어야한다.
	전이 컨디션 노드만 있어도 되지 않을까?
	인풋으로 지금의 상태가 들어오고 다음으로 바뀔 상태를 선택하게 한다.
	즉 함수를 만들고 그걸 노드라고 부른다. 그 노드를 등록하면 된다.
	그 노드 트리는 객체로 존재하고 그건 공유가 가능하다.
	그 공유된 트리에 Evaluate(currState) 를 넣어 호출한다. 그러면 결과를 이곳의 내부에서 받고 변화가 있으면 change해주고 아니면 냅둔다.
	이건 전이가 될때만 호출해준다. 그럼 정확히는 stateFSM이 아니라 StateTransitionFSM이 된다.
	이러면 장점이 무엇일까? 그리고 이게 언리얼 BT보다 나은점이 뭘까?
 */
/*
 *	필요한 요소
 *	TArray 타임 매스 노드가 들어갈 힙.
 *		현재 묶여서 같이 실행되는 객체들의 lightweight 힙 <- 지금 시점에서 무조건 실행되는것임 tick에 넣을거고 빼고 넣고는 외부에서 해줄거임.
 *	타임매스노드의 정의
 *		delayTime, runningTime,타임 매스 콜백함수 <- 이거 어떤 액션을 등록했다고 한다면
 *	타임매스노드힙을 담을 객체 <- 트리로 고정
 *	그러한 트리들의 인터페이스 <- 서브시스템
 *		틱커블이여야함.
 *	상태 FSM 노드 <- c++전용으로 설계
 *	이걸 담을 트리
 *	이걸 사용하게 해줄 인터페이스 객체
 *	registTimer(delaytime,runningTime,Callback)
 *		일단 그냥 node째로 넣어놓고 있다가 tick을 실행해서 delayTime들을 제거해줌.
 *			FTIHHsTimeMassNode를 넣어놓는 배열이 있어야함.
 *		그리고 Tick쯤에 저것들중
 *			delayTime <= 0 인애들을 찾아서 internal 노드로 변환한다.
 *			인터널 노드는 들어간 시간을 그냥 기록해놓는다.
 *	---- 보류: 이벤트 기반이라 타이머 기반이 지금은 필요없음.
 *
 *	stateTransitionFSM 노드
		전이전 상태-> 컨디션 -> 전이후 상태
		컨디션(전이전 상태객체)
 */



UCLASS()
class TIHHOUSINGCORE_API UTIHHsObjcetSubSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:

	
	//UFUNCTION(BlueprintCallable, Category = "TIHHsObjectSubSystem|Object")
	ITIHHsBaseObject* CreateTIHHsObject(FName objectName);
	//UFUNCTION(BlueprintCallable, Category = "TIHHsObjectSubSystem|Object")
	//ITIHHsBaseObject* CreateTIHHsComponent(FName componentName){return nullptr;}

	void RegisterObjectClass(FName objectName, UClass* objectClass)
	{
		if (objectClass && not mObjectTable.Contains(objectName))
		{
			mObjectTable.Add(objectName, objectClass);
		}
	}
	void RegisterComponentClass(FName componentName, UClass* componentClass)
	{
		if (componentClass && not mComponentTable.Contains(componentName))
		{
			mComponentTable.Add(componentName, componentClass);
		}
	}
	
	UClass* GetObjectClass(FName objectName)
	{
		if (mObjectTable.Contains(objectName))
		{
			return mObjectTable[objectName];
		}
		return nullptr;
	}
	UClass* GetComponentClass(FName componentName)
	{
		if (mComponentTable.Contains(componentName))
		{
			return mComponentTable[componentName];
		}
		return nullptr;
	}
	
	void BeginTranjection(ITIHHsBaseObject* target)
	{
		//	트랜잭션 시작
	}

	// void OnWorldBeginPlay(UWorld& InWorld) override;
	// void PostInitialize() override;

	//virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return true; }
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UTIHHsObjcetSubSystem, STATGROUP_Tickables);
	}
	//	bluePrint 버전은 좀 나중에
	//UFUNCTION(BlueprintCallable)
	//FTIHHsCommandNode* MakeCommandNode(FName commandName)
	//{
	//	
	//	return nullptr;
	//}
	
	void DefualtCommands();
	
	UFUNCTION(BlueprintCallable)
	TScriptInterface<ITIHHsBaseObject> CreateStuff(const FTransform& transform)
	{
		TScriptInterface<ITIHHsBaseObject> result;
		FActorSpawnParameters param;
		
		AActor* actor= mWorld->SpawnActor(mDefaultStuffClass, &transform, FActorSpawnParameters());
		if (actor)
		{
			int32 uniqueID = actor->GetUniqueID();
			mObjectInterfaceTable.Add(uniqueID, TScriptInterface<ITIHHsBaseObject>(actor));
			result = mObjectInterfaceTable[uniqueID];
			//result->ChangeTIHState(ETIHHsStateDetail::ETIHHsStates_Default_CreateCandidate);
		}
		return result;
	}

private:
	TArray<ITIHHsBaseObject*> mObjectList;
	TMap<FName,UClass*> mObjectTable;
	TMap<FName,UClass*> mComponentTable;
	
	UWorld* mWorld;
	
	TMap<FName,FTIHHsCommandNode> mCommandNodeTable;	//	명령 노드 테이블, 명령 이름과 명령 노드의 맵
	class UTIHHsRelationshipSubsystem* mRelationshipSubsystem;	//	관계 서브시스템, 관계를 관리하는 서브시스템

	TMap<int32,TScriptInterface<ITIHHsBaseObject>> mObjectInterfaceTable;	//	오브젝트 인터페이스 테이블, 오브젝트 아이디와 오브젝트 인터페이스의 맵
	FSoftClassPath mDefaultStuffClassPath;
	UClass* mDefaultStuffClass = nullptr;	//	기본 스터프 클래스, 기본 스터프 클래스의 경로
};

UCLASS()
class UTIHHsRelationshipSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	void Attach(TScriptInterface<ITIHHsBaseObject> parent, TScriptInterface<ITIHHsBaseObject> child);

	UFUNCTION(BlueprintCallable)
	const FTIHHsRelationshipEdge& FindEdges(TScriptInterface<ITIHHsBaseObject> any)
	{
		static FTIHHsRelationshipEdge edge;

		
		
		
		return edge;
	}
	
	UFUNCTION(BlueprintCallable)
	void DetachFrom(TScriptInterface<ITIHHsBaseObject> child);

	UFUNCTION(BlueprintCallable)
	void DetachAllEdgesFromParent(TScriptInterface<ITIHHsBaseObject> parent);

	UFUNCTION(BlueprintCallable)
	TArray<TScriptInterface<ITIHHsBaseObject>> GetChildren(TScriptInterface<ITIHHsBaseObject> parent);

	UFUNCTION(BlueprintCallable)
	void DetachIsolatedEdge(TScriptInterface<ITIHHsBaseObject> target);

private:
	TMap<int64,FTIHHsRelationshipEdge> mRelationshipEdges;

	TMap<int32, FTIHHsRelationshipEdgeRefData> mRelativeParents;
	
	//TSet<FTIHHsRelationshipEdge> mRelationshipEdges;
	/*
	 * 
	 */
	//TMap<uint32,int64> mRelationshipEdgeIndex;	//	엣지 인덱스, 엣지의 해시값과 인덱스의 맵, 엣지의 해시값은 FTIHHsRelationshipEdge::GetHash()로 구할 수 있다.
};



UCLASS()
class TIHHOUSINGCORE_API ATIHHsDefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
};

UCLASS()
class TIHHOUSINGCORE_API ATIHHsStartPawn : public APawn
{
	GENERATED_BODY()
public:
};
