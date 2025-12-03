// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Deque.h"
#include "UObject/Object.h"
#include "Cores/TIHHsCoreStructures.h"
#include "DSP/AudioDebuggingUtilities.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/SaveGame.h"
#include "THsNodeTreeCore.generated.h"

// enum class ETHsNodeType;
// enum class ETHsNodeLifecycle;
// enum class ETHsNodeFlags;
// enum class ETHsNodeDesignByContract;

class UTHsNodeFactory;
class UTHsNodeForest;
class UTHsNodeLibrary;
class UTHsNodeTree;
class UTHsNodeProbe;
class UTHsNodeVersionControl;
class UTHsNodeBody;
class UTHsNode;
class UTHsNodeCatalogBody;
class UTHsNodeRuntimeBody;
class UTHsNodeCatalogDataAsset;
class ATHsNodeManager;
class FTHsCoroutineWorker;

/**
 * 
 */
UENUM(BlueprintType)
enum class ETHsNodeType : uint8
{
	EUndefined		UMETA(DisplayName="Undefined"),
	EActor 		UMETA(DisplayName="Actor"),
	EComponent		UMETA(DisplayName="Component"),
	EGroup			UMETA(DisplayName="Group"),
};
UENUM(BlueprintType)
enum class ETHsNodeLifecycle : uint8
{
	CatalogNode      UMETA(ToolTip="Imbedded: 절대 변하지 않음 (카탈로그 원본)"),
	RuntimeNode     UMETA(ToolTip="Runtime: 생성/삭제 가능 (유저 배치)"),
	TemporaryNode   UMETA(ToolTip="Temporary: 저장되지 않음 (미리보기 고스트 등)")
};
/**
 * @enum ETHsNodePolicyFlags
 * @brief 노드의 정책 플래그를 나타내는 열거형입니다. 노드의 동작 및 제약 조건을 정의합니다.
 */
UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ETHsNodePolicyFlags : uint8
{
	ENone        = 0,
	ENoCloning      = 1 << 0, 
	ENoSpawning     = 1 << 1,
	ENoTemporary   = 1 << 2,
	ENoMoving      = 1 << 3,
	ENoSaving      = 1 << 4,
	
};ENUM_CLASS_FLAGS(ETHsNodePolicyFlags);
/**
 * @enum ETHsNodeDesignByContract
 * @brief 노드의 상태를 계약 기반으로 나타내는 열거형입니다. 검증의 결과이고, 보장을 위한 플래그입니다.
 */
UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ETHsNodeDesignByContract : uint8
{
	InValid         = 0,
	IsInitialized   = 1 << 0, // 초기화 완료
	
	
};ENUM_CLASS_FLAGS(ETHsNodeDesignByContract);

UENUM(BlueprintType)
enum class ETHsNodeClassContainerPolicy : uint8
{
	None            UMETA(DisplayName="None"),          // 컨테이너 아님 (Terminal)
	SingleWrapper   UMETA(DisplayName="Single Wrapper"),// 단일 자식/루트 (예: Tree -> RootNode)
	Collection      UMETA(DisplayName="Collection"),    // 다중 자식 (예: Forest -> Trees, Node -> Children)
	Selector        UMETA(DisplayName="Selector"),      // 선택적 로드 (예: Library)
};
ENUM_CLASS_FLAGS(ETHsNodeClassContainerPolicy)

UENUM(Blueprintable)
enum ETHsNodeClassHierarchyType : uint8
{
	EUndefined			UMETA(DisplayName="Undefined"),
	ELibrary			UMETA(DisplayName="Library"),
	EForest				UMETA(DisplayName="Forest"),
	ETree				UMETA(DisplayName="Tree"),
	ENode				UMETA(DisplayName="Node"),
};

UINTERFACE()
class UTHsNodeTrait : public UInterface{	GENERATED_BODY()};
class ITHsNodeTrait
{
	GENERATED_BODY()
public:
	virtual ETHsNodeClassContainerPolicy GetContainerPolicy() const { return ETHsNodeClassContainerPolicy::None; };
	virtual ETHsNodeClassHierarchyType GetHierarchyType() const { return ETHsNodeClassHierarchyType::EUndefined; };

	virtual ATHsNodeManager* TryGetOwnerNodeManager() const { return nullptr; };
	virtual UTHsNodeLibrary* TryGetOwnerNodeLibrary() const { return nullptr; };
	virtual UTHsNodeForest* TryGetOwnerNodeForest() const { return nullptr; };
	virtual UTHsNodeTree* TryGetOwnerNodeTree() const { return nullptr; };
	virtual UTHsNode* TryGetOwnerNode() const { return nullptr; };
};

USTRUCT()
struct FTHsNodeRecordData
{
	GENERATED_BODY()

	
};

USTRUCT(BlueprintType)
struct FTHsNodeKey
{
	GENERATED_BODY()
	
	UPROPERTY(Category="Header",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	TArray<FString> NodeKeys;
	
	FTHsNodeKey()
	{
		NodeKeys.Add("NotDefined");
	}
	FTHsNodeKey(const FString& InSignature)
	{
		SetFromSignatureByString(InSignature);
	}
	FTHsNodeKey(const FName& InSignature)
	{
		SetFromSignatureByName(InSignature);
	}
	FTHsNodeKey(const TCHAR* InSignature)
	{
		SetFromSignatureByString(FString(InSignature));
	}
	
	operator FName() const
	{
		return GetSignatureKey();
	}
	operator FString() const
	{
		return GetSignatureKey().ToString();
	}
	operator const TCHAR*() const
	{
		return *GetSignatureKey().ToString();
	}
	operator int64() const 
	{
		return GetNodeKeyHash();
	}
	FTHsNodeKey& operator=(const FName& InSignature)
	{
		SetFromSignatureByName(InSignature);
		return *this;
	}
	FTHsNodeKey& operator=(const FString& InSignature)
	{
		SetFromSignatureByString(InSignature);
		return *this;
	}
	FTHsNodeKey& operator=(const TCHAR* InSignature)
	{
		SetFromSignatureByName(FName(InSignature));
		return *this;
	}
	
	FName GetSignatureKey() const
	{
		if (NodeKeys.Num() > 0)
		{
			return FName(*FString::Join(NodeKeys, TEXT(".")));
		}
		return NAME_None;
	}
	int32 GetKeyDepth() const
	{
		return NodeKeys.Num();
	}
	
	void SetFromSignatureByName(const FName& InSignature)
	{
		NodeKeys.Empty();
		FString SignatureString = InSignature.ToString();
		if (!SignatureString.IsEmpty())
		{
			// 점(.)을 구분자로 분리 (예: "Furniture.Desk.Leg" -> ["Furniture", "Desk", "Leg"])
			SignatureString.ParseIntoArray(NodeKeys, TEXT("."), true);
		}
	}
	void SetFromSignatureByString(const FString& InSignature)
	{
		NodeKeys.Empty();
		if (!InSignature.IsEmpty())
		{
			// 점(.)을 구분자로 분리 (예: "Furniture.Desk.Leg" -> ["Furniture", "Desk", "Leg"])
			InSignature.ParseIntoArray(NodeKeys, TEXT("."), true);
		}
	}
	
	bool IsValidNodeKey()const
	{
		if (NodeKeys.IsEmpty())
		{
			return false;
		}
		if (NodeKeys.Contains("NotDefined"))
		{
			return false;
		}
		return true;
	}
	int64 GetNodeKeyHash() const
	{
		FName SignatureKey = GetSignatureKey();
		return GetTypeHash(SignatureKey);
	}
	
	static FName NotDefined() 
	{
		return FName("NotDefined");
	}
	
};

class FTHsCoroutineWorker : public FTickableGameObject
{
public:
	using FTHsCoroutineFunc = TFunction<bool(int32 perFrameCount,double TimeLimit)>;
	using FTHsCoroutineEndFunc = TFunction<void(const FTHsCoroutineWorker* worker)>;
	
	FTHsCoroutineWorker(FTHsCoroutineFunc inTimeLimitedFunc,double inTimeBudgetPerFrame = 0.005,const FString& InOwnerDebugName = TEXT("UnknownWorker"), FTHsCoroutineEndFunc inEndFunc = nullptr)
		: WorkFunc(MoveTemp(inTimeLimitedFunc))
		, TimeBudget(inTimeBudgetPerFrame)
		, DebugName(InOwnerDebugName)
		,TickCount(0)
	, EndFunc(MoveTemp(inEndFunc))
	{}
	
	virtual ~FTHsCoroutineWorker() override
	{
		if (EndFunc != nullptr)
		{
			EndFunc(this);
		}
		UE_LOG(LogTemp, Log, TEXT("[HsWorker] %s Destroyed."), *DebugName);
	}
	
	virtual void Tick(float DeltaTime) override
	{
		// 1. 로직이 없으면 바로 자폭
		if (!WorkFunc)
		{
			delete this;
			return;
		}
		
		// 2. 작업 수행 (TimeLimit을 넘겨줌)
		// 리턴값이 true면 "작업 끝났다"는 뜻
		bool bIsFinished = WorkFunc(TickCount,TimeBudget);
		++TickCount;
		// 3. 작업이 끝났으면 자폭 (Suicide)
		if (bIsFinished)
		{
			delete this;
		}
		// false면 살아서 다음 프레임에 다시 Tick이 불림
	}
    
	virtual TStatId GetStatId() const override 
	{ 
		RETURN_QUICK_DECLARE_CYCLE_STAT(FTHsCoroutineWorker, STATGROUP_Tickables); 
	}
	virtual ETickableTickType GetTickableTickType() const override 
	{ 
		return ETickableTickType::Always; 
	}
    
	virtual bool IsTickable() const override 
	{ 
		return true; 
	}
private:
	FTHsCoroutineFunc WorkFunc;
	FTHsCoroutineEndFunc EndFunc;
	double TimeBudget;
	FString DebugName;
	int32 TickCount = 0;
};

USTRUCT(Blueprintable)
struct FTHsNodeVersionControlPoint
{
	GENERATED_BODY()

	UPROPERTY(Category="VersionControl|Point",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	bool IsCloning = false;
	
	UPROPERTY(Category="VersionControl|Point",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<UTHsNode> VersionNode;

};

UCLASS(BlueprintType) 
class UTHsNodeVersionControl : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(Category="VersionControl",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	int32 Version = 1;

	UPROPERTY(Category="VersionControl",Transient,
		EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<UTHsNode> OwnerNode;
	
	UPROPERTY(Category="VersionControl",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	FTHsNodeVersionControlPoint PrevVersionPoint;
	
	UPROPERTY(Category="VersionControl",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	FTHsNodeVersionControlPoint NextVersionPoint;
	
	UFUNCTION()
	int64 GetVersionHash() const
	{
		return Version;
	}

	virtual void CloneFrom(const UTHsNodeVersionControl* other, UTHsNode* ownerNode)
	{
		if (other == nullptr)
		{
			return;
		}
		Version = other->Version;
		PrevVersionPoint.IsCloning = true;
		PrevVersionPoint.VersionNode = other->OwnerNode;
		OwnerNode = ownerNode;
	}

	
};

UCLASS(Abstract, BlueprintType,EditInlineNew,DefaultToInstanced)
class UTHsNodeBody : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(Category="Body",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	FGameplayTagContainer NodeTags;
	
	UFUNCTION(BlueprintCallable)
	virtual UObject* GetBodyAsset() const PURE_VIRTUAL(UTHsNodeBody::GetBodyAsset, return nullptr;);

	int64 GetBodyHash() const
	{
		TArray<FGameplayTag> gamePlayTagArray = NodeTags.GetGameplayTagArray();
		gamePlayTagArray.Sort([](const FGameplayTag& A, const FGameplayTag& B)
		{
			return A < B;
		});
		return GetTypeHash(gamePlayTagArray);
	}
	
	virtual void CloneFrom(const UTHsNodeBody* other)
	{
		if (other == nullptr)
		{
			return;
		}
		NodeTags = other->NodeTags;
	}
	
};

class FTHsNodeSolver
{
public:
	FTHsNodeSolver() = default;
	virtual ~FTHsNodeSolver() = default;

	
};


// UENUM(Blueprintable)
// enum ETHsNodeInspectionFlag : uint8
// {
// 	
// };
/**
 * @class UTHsNodeState
 * @brief 노드의 상태를 나타내는 클래스입니다. 노드의 검사 유효성, 변경상태, 링크 같이 현재 상태에 대한 것을 표시합니다.
 * @details
 * - InspectionValid: 노드가 현재 유효한지 여부를 나타냅니다. 검사 결과를 내부에 저장합니다.
 * - IsInitialized: 노드가 초기화되었는지 여부를 나타냅니다. DbC(Design by Contract) 용도 옮길예정
 * - IsDirty: 노드가 변경되었는지 여부를 나타냅니다. 변경된 경우 true로 설정됩니다. update를 하면 false로 바뀜
 * - IsLinkedInstance: 월드의 객체에 링크되었는지 여부를 나타냅니다. 노드가 실제 월드 객체와 연결된 경우 true로 설정됩니다.
 */
UCLASS(BlueprintType,EditInlineNew,DefaultToInstanced)
class UTHsNodeState : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(Category="Header|State",Transient,
				EditAnywhere, BlueprintReadWrite)
	bool InspectionValid = false;
	UPROPERTY(Category="Header|State",Transient,
				EditAnywhere, BlueprintReadWrite)
	bool IsInitialized = false;	//	이거 DbC임. 
	UPROPERTY(Category="Header|State",Transient,
				EditAnywhere, BlueprintReadWrite)
	bool IsDirty = false;
	UPROPERTY(Category="Header|State",Transient,
				EditAnywhere, BlueprintReadWrite)
	bool IsLinkedInstance = false;

	void OnSetInspectionValid(bool bValid)
	{
		InspectionValid = bValid;
	}
	void OnSetInitialized(bool bInitialized)
	{
		IsInitialized = bInitialized;
	}
	void OnSetDirty(bool bDirty)
	{
		IsDirty = bDirty;
	}
	void OnSetLinkedInstance(bool bLinked)
	{
		IsLinkedInstance = bLinked;
	}
	void UpdateNodeState(TWeakObjectPtr<UTHsNode> ownerNode);
};
UCLASS(BlueprintType,EditInlineNew,DefaultToInstanced)
class UTHsNodeHeader : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(Category="Header",SaveGame,
	EditAnywhere,BlueprintReadWrite)
	FTHsNodeKey NodeKey;
	UPROPERTY(Category="Header",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	ETHsNodeType NodeType = ETHsNodeType::EUndefined;
	UPROPERTY(Category="Header",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	ETHsNodeLifecycle NodeLifecycle = ETHsNodeLifecycle::CatalogNode;
	UPROPERTY(Category="Header",SaveGame,
		EditAnywhere, BlueprintReadWrite,
		meta=(Bitmask, BitmaskEnum="ETHsNodeFlags"))
	uint8 NodePolicyFlags = 0;	//	TODO: 지금당장은 사용할 여력이 없음
	UPROPERTY(Category="Header",SaveGame,
			EditAnywhere, BlueprintReadWrite,
			meta =(Bitmask, BitmaskEnum="ETHsNodeDesignByContract"))
	uint8 NodeDbC = 0;
	
	UPROPERTY(Category="Header",SaveGame,
		EditAnywhere, BlueprintReadWrite)
	int32 NodeLayer = 0;

	UPROPERTY(Category="Header",SaveGame,
		EditAnywhere,BlueprintReadWrite,Instanced,
		meta=(AllowPrivateAccess="true",ShowOnlyInnerProperties))
	TObjectPtr<UTHsNodeVersionControl> NodeVersionControl;

	UPROPERTY(Category="Header",Transient,
		EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UTHsNode> OwnerNode;
	
	UTHsNodeHeader* OnNodePolicyFlag(ETHsNodePolicyFlags flag)
	{
		NodePolicyFlags |= static_cast<uint8>(flag);
		return this;
	}
	UTHsNodeHeader* OffNodePolicyFlag(ETHsNodePolicyFlags flag)
	{
		NodePolicyFlags &= ~static_cast<uint8>(flag);
		return this;
	}
	bool HasNodePolicyFlag(ETHsNodePolicyFlags flag) const
	{
		return (NodePolicyFlags & static_cast<uint8>(flag)) != 0;
	}
	bool HasNotNodePolicyFlag(ETHsNodePolicyFlags flag) const
	{
		return not HasNodePolicyFlag(flag);
	}
	bool NoCloneNodeFlag() const
	{
		return HasNodePolicyFlag(ETHsNodePolicyFlags::ENoCloning);
	}
	bool CloneableNodeFlag() const
	{
		return !NoCloneNodeFlag();
	}
	virtual void CloneFrom(const UTHsNodeHeader* other, UTHsNode* ownerNode)
	{
		if (other == nullptr || ownerNode == nullptr)
		{
			return;
		}
		if (other->HasNotNodePolicyFlag(ETHsNodePolicyFlags::ENoCloning))
		{
			NodeKey = other->NodeKey;
			NodeType = other->NodeType;
			NodeLifecycle = other->NodeLifecycle;
			NodePolicyFlags = other->NodePolicyFlags;
			NodeDbC = other->NodeDbC;
			NodeLayer = other->NodeLayer;
			if (other->NodeVersionControl)
			{
				if (NodeVersionControl == nullptr)
				{
					NodeVersionControl = NewObject<UTHsNodeVersionControl>(this);
					NodeVersionControl->OwnerNode = ownerNode;
				}
				NodeVersionControl->CloneFrom(other->NodeVersionControl,ownerNode);
			}
		}
	}
};

UCLASS(BlueprintType,EditInlineNew)
class UTHsNodeProbe : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * 
	 * @param inNodeFactory 노드 처리를 위한 팩토리 인스턴스
	 * @param inNodeManager 사용될 매니저
	 * @param inNodeTree 연결될 노드트리
	 * @param inNodeForest 연결될 노드포레스트
	 * @param inNodeLibrary 연결될 노드라이브러리
	 */
	UFUNCTION(Category="NodeProbe", BlueprintCallable)
	void SetupProbeContext(
		UTHsNodeFactory* inNodeFactory,
		ATHsNodeManager* inNodeManager,
		UTHsNodeTree* inNodeTree,
		UTHsNodeForest* inNodeForest,
		UTHsNodeLibrary* inNodeLibrary)
	{
		NodeFactory = inNodeFactory;
		NodeManager = inNodeManager;
		NodeTree = inNodeTree;
		NodeForest = inNodeForest;
		NodeLibrary = inNodeLibrary;
	}

	UFUNCTION(Category="NodeProbe|Process", BlueprintCallable)
	virtual TScriptInterface<ITHsNodeTrait> ProcessProbe(TScriptInterface<ITHsNodeTrait> nodeTrait ){return nullptr;	}
	UFUNCTION(Category="NodeProbe|Process", BlueprintCallable)
	virtual TScriptInterface<ITHsNodeTrait> ProcessCloneProbe(
		TScriptInterface<ITHsNodeTrait> srcNodeTrait,
		TScriptInterface<ITHsNodeTrait> dstNodeTrait){return nullptr;}
	
protected:
	UPROPERTY()
	TWeakObjectPtr<UTHsNodeFactory> NodeFactory;
	UPROPERTY()
	TWeakObjectPtr<ATHsNodeManager> NodeManager;
	UPROPERTY()
	TWeakObjectPtr<UTHsNodeTree> NodeTree;
	UPROPERTY()
	TWeakObjectPtr<UTHsNodeForest> NodeForest;
	UPROPERTY()
	TWeakObjectPtr<UTHsNodeLibrary> NodeLibrary;
};


/**
 * @class UTHsNode
 * @brief Represents a hierarchical structure of nodes used in a node tree.
 *
 * UTHsNode is designed to model a node with a specific type and its child nodes.
 * - ETHsNodeType: Defines the type of the node (e.g., Actor, Component, Group).
 * - ETHsNodeLifecycle: Indicates the lifecycle of the node
 * - NodeFlags: Bitmask flags representing various properties of the node.
 * - NodeBody: An instance of UTHsNodeBody that contains the data specific to
 * - 노드의 규칙
 *	- 카탈로그 노드: 절대 변하지 않음 (원본)
 *	- 런타임 노드: 생성/삭제 가능 (유저 배치)
 *	- 임시 노드: 저장되지 않음 (미리보기 고스트 등)
 *	- 노드의 자식 규칙
 *		- Component 은 무조건 Component만을 자식으로 가진다.
 *		- Actor는 Component 또는 Group을 자식으로 가진다.
 *		- Group는 Actor 또는 Group을 자식으로 가진다.
 *		- Component 의 부모는 Component || Actor 여야 한다. group이면 false
 *		- Actor 의 부모는 Group || 없음(루트) 여야 한다. component 면 false
 *		- Group 의 부모는 Actor || Group 여야 한다. component 면 false
 *		- 만약 Actor 의 자식으로 Actor를 넣으려 한다면 parent위에 Group 노드를 만들고 해야한다.
 *	- 액터노드: 액터노드는 씬에 배치될 수 있다. 내부에 컴포넌트를 가진다면 상위 액터에 붙게 된다.
 *	- 컴포넌트노드: 컴포넌트노드는 씬에 직접 배치될 수 없으며, 반드시 액터노드의 자식으로 존재해야 한다.
 *	- 그룹노드: 씬에 직접배치 될 수 있으며 배치된다면 빈 액터에 그냥 계층적으로 붙게 된다. 그룹노드는 논리적인 그룹화를 위해 사용된다. 
 */
UCLASS(BlueprintType,EditInlineNew,DefaultToInstanced)
class UTHsNode : public UObject , public ITHsNodeTrait
{
	GENERATED_BODY()

public:
	virtual ETHsNodeClassHierarchyType GetHierarchyType() const override final { return ETHsNodeClassHierarchyType::ENode; };
	UTHsNode();
	
	UPROPERTY(Category="Header",SaveGame,
		EditAnywhere,BlueprintReadWrite,Instanced)
	TObjectPtr<UTHsNodeHeader> NodeHeader;

	UPROPERTY(Category="Header",Transient,
		EditAnywhere,BlueprintReadWrite,Instanced)
	TObjectPtr<UTHsNodeState> NodeState;

	UPROPERTY(Category="Body|Runtime",Transient,
		EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<UTHsNode> ParentNode;
	UPROPERTY(Category="Body|Runtime",Transient,
	EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<UTHsNodeTree> OwningNodeTree;
	
	UPROPERTY(Category="Body",SaveGame,
		EditAnywhere,BlueprintReadWrite,Instanced,
		meta=(AllowPrivateAccess="true",ShowOnlyInnerProperties))
	TObjectPtr<UTHsNodeBody> NodeBody;
	
	UPROPERTY(Category="Tail",SaveGame,
		EditAnywhere,BlueprintReadWrite,Instanced)
	TArray<TObjectPtr<UTHsNode>> ChildNodes;
	
	bool IsRuntimeNode() const;

	UTHsNode* SetNodeLifecycle(ETHsNodeLifecycle lifecycle);
	UFUNCTION(BlueprintCallable)
	UTHsNode* SetCatalogNodeCycle()
	{
		return SetNodeLifecycle(ETHsNodeLifecycle::CatalogNode);
	}
	UFUNCTION(BlueprintCallable)
	UTHsNode* SetRuntimeNodeCycle(){
		return SetNodeLifecycle(ETHsNodeLifecycle::RuntimeNode);
	}
	UFUNCTION(BlueprintCallable)
	UTHsNode* SetTemporaryNodeCycle()
	{
		return SetNodeLifecycle(ETHsNodeLifecycle::TemporaryNode);
	}

	UFUNCTION(BlueprintCallable)
	int32 AddNodeToChildNodes(UTHsNode* childNode)
	{
		int32 index = -1;
		if (childNode != nullptr)
		{
			if(childNode->ParentNode == nullptr)
			{
				childNode->ParentNode = this;
			}
			index = ChildNodes.Add(childNode);
		}
		return index;
	}

	ETHsNodeType GetNodeType() const
	{
		return NodeHeader->NodeType;
	}
	
	bool IsNotUndefinedNode()
	{
		return GetNodeType() != ETHsNodeType::EUndefined && CheckInspectionNodeType();
	}
	
	bool IsAllowedAsChildOf(UTHsNode* parent) const
	{
		bool result = false;
		if (parent)
		{
			ETHsNodeType parentType = parent->GetNodeType();
			switch (GetNodeType())
			{
			case ETHsNodeType::EUndefined:	break;
			case ETHsNodeType::EActor:
				if (parentType == ETHsNodeType::EActor || parentType == ETHsNodeType::EGroup)
				{
					result = true;
				}
				break;
			case ETHsNodeType::EComponent:
				if (parentType == ETHsNodeType::EActor || parentType == ETHsNodeType::EComponent)
				{
					result = true;
				}
				break;
			case ETHsNodeType::EGroup:
				if (parentType == ETHsNodeType::EActor || parentType == ETHsNodeType::EGroup)
				{
					result = true;
				}
				break;
			}
		}
		return result;
	}
	
	
	bool IsAllowedAsParentOf(UTHsNode* child) const
	{
		bool result = false;
		if (child)
		{
			ETHsNodeType childType = child->GetNodeType();
			switch (GetNodeType())
			{
			case ETHsNodeType::EUndefined:	break;
			case ETHsNodeType::EActor:
				if (childType != ETHsNodeType::EUndefined)
				{
					result = true;
				}
				break;
			case ETHsNodeType::EComponent:
				if (childType == ETHsNodeType::EComponent)
				{
					result = true;
				}
				break;
			case ETHsNodeType::EGroup:
				if (childType == ETHsNodeType::EActor || childType == ETHsNodeType::EGroup)
				{
					result = true;
				}
				break;
			}
			//	빈 child 일때도 해줘야하나?
		}
		return result;
	}

	/**
	 * @brief 보모 자식이 허용되는지를 검사하는 정적 함수
	 * @param parent 부모 노드, null이면 루트 노드 검사
	 * @param child 자식노드 겸 새로운 노드, 없으면 false
	 * @return boolean
	 */
	static bool IsAllowedBetweenParentAndChild(UTHsNode* parent, UTHsNode* child)
	{
		if (parent == nullptr && child == nullptr)
		{
			return false;
		}
		
		if (parent != nullptr)
		{
			if (child != nullptr)
			{
				return parent->IsAllowedAsParentOf(child);
			}
			else
			{
				//	부모만 남는건 틀림.
				//	빈걸 만든다는 소리인데...
				return false;
			}
		}
		else
		{
			//	root
			return child->IsNotUndefinedNode();
		}

	}
	
	bool UpdateNodeState();

	bool InspectionNodeTypeRecursive();
	
	/*
	 * 컨셉을 잘짜야하는게, 부모에서 자식으로 검사를 하면서 자신의 state만 바꾸는가? 자식도 바꾸는가?
	 * 간단하게 갈거고, 복잡하지 않게 가려면 자신의 state만 바꾸는걸로 하자.
	 */
	bool CheckInspectionNodeType() 
	{
		TravelChildNodes([weakThis =  TWeakObjectPtr<UTHsNode>(this)](UTHsNode* childNodePtr)
		{
			if (weakThis.IsValid() && childNodePtr != nullptr)
			{
				weakThis->NodeState->InspectionValid = childNodePtr->IsAllowedAsChildOf(weakThis.Get());
				if (weakThis->NodeState->InspectionValid)
				{
					childNodePtr->CheckInspectionNodeType();
				}
			}
		});
		return NodeState->InspectionValid;
	}
	void TravelChildNodes(TFunctionRef<void(UTHsNode* childNodePtr)> InFunc)
	{
		for (TObjectPtr<UTHsNode>& childNodePtr : ChildNodes)
		{
			InFunc(childNodePtr.Get());
		}
	}

	virtual ETHsNodeClassContainerPolicy GetContainerPolicy() const override
	{
		return ETHsNodeClassContainerPolicy::Collection;
	}

	/*
	 * TODO: PostInterpChange 같이 편의 기능, 결국 DataAsset에서도 가능해야하니깐 만들어줘야하는데, 중요한건 런타임에서는 작동하지 않아야 하는게 많다는거임.
	 */
	virtual ATHsNodeManager* TryGetOwnerNodeManager() const override;
	virtual UTHsNodeLibrary* TryGetOwnerNodeLibrary() const override;
	virtual UTHsNodeForest* TryGetOwnerNodeForest() const override;
	virtual UTHsNodeTree* TryGetOwnerNodeTree() const override;
	virtual UTHsNode* TryGetOwnerNode() const override;
};
UCLASS()
class UTHsNodeProxy : public UObject
{
	GENERATED_BODY()
public:
	//UFUNCTION()
	//virtual UTHsNode* MakeNodeFromCatalogNode(UTHsNode* InCatalogNode);
	
};
UCLASS()
class UTHsNodeProxy_MakeRuntimeNode : public UTHsNodeProxy
{
	GENERATED_BODY()

public:
	virtual UTHsNode* MakeNodeFromCatalogNode(UTHsNode* InCatalogNode);
};
UCLASS()
class UTHsNodeProxy_MakeTemporaryNode : public UTHsNodeProxy
{
	GENERATED_BODY()
public:
	
};


UCLASS(BlueprintType,EditInlineNew,DefaultToInstanced)
class UTHsNodeCatalogBody : public UTHsNodeBody
{
	GENERATED_BODY()

public:
	virtual UObject* GetBodyAsset() const override
	{
		return CatalogAssetData;
	}

	UPROPERTY(Category="CatalogBody",SaveGame,
		EditAnywhere,BlueprintReadWrite,Instanced)
	TObjectPtr<UObject> CatalogAssetData;
	
};
UCLASS(BlueprintType,EditInlineNew,DefaultToInstanced)
class UTHsNodeRuntimeBody : public UTHsNodeBody
{
	GENERATED_BODY()

public:
	virtual UObject* GetBodyAsset() const override
	{	
		return LinkedRuntimeObject.Get();
	}

	UPROPERTY(Category="CatalogBody",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<UObject> LinkedRuntimeObject;
};


/**
 * @class UTHsNodeTree
 * @brief 노드의 계층구조를 포함한다.
 * @details 생각해보면 사실 구조는 노드가 다 가진다. 자식도 있고, 스스로 타입도 가지고 있다. 그렇다면 노드 트리는 무엇을 하는걸까? 정확히 말하자면 루트노드를 알려주고, 추가적인 메타데이터를 담는 역할을 한다고 볼 수 있다. 
 * - 종류:
 *	- 단일 노드 트리: 딱 하나의 노드만 가진다. 이 경우 루트 노드가 곧 유일한 노드가 된다.
 *	- 다중 노드 트리: 여러 개의 노드를 포함하며, 노드가 그룹일지, 액터일지 컴포넌트일지 모르지만 어쨌든 단일 루트에서 시작해서 진행하기에 루트의 규칙대로 존재하는 것이다.
 *	
 */
UCLASS(Blueprintable,BlueprintType,EditInlineNew,DefaultToInstanced)
class UTHsNodeTree: public UObject , public ITHsNodeTrait
{
	GENERATED_BODY()
public:
	virtual ETHsNodeClassHierarchyType GetHierarchyType() const override final { return ETHsNodeClassHierarchyType::EForest; };
	UFUNCTION(BlueprintCallable, Category="Trait")
	virtual ETHsNodeClassContainerPolicy GetContainerPolicy() const override
	{
		return ETHsNodeClassContainerPolicy::SingleWrapper;
	}
	UPROPERTY(Category= "NodeTree",SaveGame,
			EditAnywhere,BlueprintReadWrite)
	FName TreeName;	//	if name == None -> RootNode[0] is default root, 중복 체크를 해줌.
	UPROPERTY(Category= "NodeTree|Runtime",Transient,
				EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<UTHsNodeForest> OwningNodeForest;
	
	UPROPERTY(Category= "NodeTree",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UTexture2D> ThumbnailImage;
	
	UPROPERTY(Category= "NodeTree",SaveGame,
		EditAnywhere,BlueprintReadWrite,Instanced)
	TObjectPtr<UTHsNode> RootNode;

	
	UFUNCTION(BlueprintCallable)
	bool InspectNodeTree()
	{
		if (RootNode)
		{
			return RootNode->CheckInspectionNodeType();
		}
		return false;
	}
	
	void TravelTreeNodes(TFunctionRef<void(UTHsNode* node)> InFunc)
	{
		if (IsValid(RootNode))
		{
			RootNode->TravelChildNodes(InFunc);
		}
	}
	virtual void UpdateTree();
	
	virtual ATHsNodeManager* TryGetOwnerNodeManager() const override;
	virtual UTHsNodeLibrary* TryGetOwnerNodeLibrary() const override;
	virtual UTHsNodeForest* TryGetOwnerNodeForest() const override;
	
};

USTRUCT(Blueprintable)
struct FTHsNodeEnvironment
{
	GENERATED_BODY()

	UPROPERTY(Category= "NodeEvrionment",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	FName EnvironmentName;
};



UCLASS(Blueprintable,EditInlineNew)
class UTHsNodeForest : public UObject , public ITHsNodeTrait
{
	GENERATED_BODY()
public:
	virtual ETHsNodeClassHierarchyType GetHierarchyType() const override final { return ETHsNodeClassHierarchyType::EForest; };

	UFUNCTION(BlueprintCallable, Category="Trait")
	virtual ETHsNodeClassContainerPolicy GetContainerPolicy() const override final
	{
		return ETHsNodeClassContainerPolicy::Collection;
	}
	UPROPERTY(Category= "NodeForest",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	FName ForestName;
	/*
	 *	환경, 노드 트리들을 포함한다. 
	 */
	UPROPERTY(Category= "NodeForest",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	FTHsNodeEnvironment Environment;

	UPROPERTY(Category= "NodeForest",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UTHsNodeProbe> CustomForestProbeClass;
	
	UPROPERTY(Category= "NodeForest|Runtime",Transient,
		EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<UTHsNodeLibrary> OwningLibrary;
	
	UPROPERTY(Category= "NodeForest",SaveGame,
		EditAnywhere,BlueprintReadWrite,Instanced)
	TArray<TObjectPtr<UTHsNodeTree>> NodeTrees;

	

	void TravelNodeTrees(TFunctionRef<void(UTHsNodeTree* srcNodeTreePtr)> InFunc)
	{
		for (TObjectPtr<UTHsNodeTree>& nodeTreePtr : NodeTrees)
		{
			InFunc(nodeTreePtr.Get());
		}
	}
	
	int32 BatchingForeachNodeTreeByRange(int32 startIndex,int32 loopCount,TFunctionRef<void(UTHsNodeTree* srcNodeTreePtr)> InFunc)
	{
		int32 endCount = startIndex + loopCount;
		if (endCount > NodeTrees.Num())
		{
			endCount = NodeTrees.Num();
		}
		for (int32 i = startIndex; i < endCount; ++i)
		{
			InFunc(NodeTrees[i]);
		}
		return endCount - startIndex;
	}

	int32 AddTreeToForest(UTHsNodeTree* newNodeTree)
	{
		return NodeTrees.Add(newNodeTree);
	}

	virtual ATHsNodeManager* TryGetOwnerNodeManager() const override;
	virtual UTHsNodeLibrary* TryGetOwnerNodeLibrary() const override;
};



UCLASS(Blueprintable,EditInlineNew)
class UTHsNodeLibrary : public USaveGame , public ITHsNodeTrait
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Trait")
	virtual ETHsNodeClassHierarchyType GetHierarchyType() const override final { return ETHsNodeClassHierarchyType::ELibrary; }

	void SetForestToLibrary(UTHsNodeForest* NewForest)
	{
		NodeForest = NewForest;
	}

	UFUNCTION(BlueprintCallable, Category="Trait")
	virtual ETHsNodeClassContainerPolicy GetContainerPolicy() const override
	{
		return ETHsNodeClassContainerPolicy::Selector;
	}
	
	UPROPERTY(Category="NodeLibrary",SaveGame,
		EditAnywhere,BlueprintReadWrite)
	bool bIsUserLibrary = false;
	
	UPROPERTY(Category="NodeLibrary|Runtime",Transient,
		EditAnywhere,BlueprintReadWrite)
	TWeakObjectPtr<ATHsNodeManager> OwningNodeManager;
	
	UPROPERTY(Category="NodeLibrary",SaveGame,
		EditAnywhere,BlueprintReadWrite,Instanced)
	TObjectPtr<UTHsNodeForest> NodeForest;

	bool DbcHasNodeForest() const
	{
		return NodeForest != nullptr;
	}
	bool DbcHasOwnerNodeManager() const
	{
		return OwningNodeManager.IsValid();
	}
	bool DbcCopyableNodeLibrary() const
	{
		return DbcHasOwnerNodeManager() && bIsUserLibrary;
	}
	
	bool DbcValidNodeLibrary() const
	{
		return DbcHasOwnerNodeManager() && DbcHasNodeForest();
	}
	bool DbcValidUserNodeLibrary() const
	{
		return bIsUserLibrary && DbcValidNodeLibrary();
	}
	bool DbcValidCatalogNodeLibrary() const
	{
		return !bIsUserLibrary && DbcHasNodeForest();
	}
	
	virtual ATHsNodeManager* TryGetOwnerNodeManager() const override final;

};

UCLASS(Blueprintable)
class UTHsNodeWorldLibrary : public UTHsNodeLibrary
{
	GENERATED_BODY()
public:
	UPROPERTY(Category="NodeLibrary",Transient,
		EditAnywhere,BlueprintReadWrite)
	TArray<TObjectPtr<AActor>> SpawnedActors;
};

UCLASS(BlueprintType,EditInlineNew)
class UTHsNodeForestProbe : public UTHsNodeProbe
{
	GENERATED_BODY()
public:
	virtual TScriptInterface<ITHsNodeTrait> ProcessProbe(TScriptInterface<ITHsNodeTrait> nodeTrait) override;
	virtual TScriptInterface<ITHsNodeTrait> ProcessCloneProbe(TScriptInterface<ITHsNodeTrait> srcNodeTrait,TScriptInterface<ITHsNodeTrait> dstNodeTrait) override;
};
USTRUCT(Blueprintable)
struct FTHsNodeSavedLibraryMetaData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,SaveGame)
	TObjectPtr<UTexture2D> ThumbnailImage;

	UPROPERTY(EditAnywhere,SaveGame)
	FString SaveDescription;
	
	UPROPERTY(EditAnywhere,SaveGame)
	FDateTime CreationTime;

	UPROPERTY(EditAnywhere,SaveGame)
	FString SlotName = "DefaultSlot";
	
	UPROPERTY(EditAnywhere,SaveGame)
	int32 UserIndex = -1;
};


UCLASS(Blueprintable)
class UTHsNodeSavedTOC : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,SaveGame)
	int32 CurrentLibraryIndex = -1;

	UPROPERTY(EditAnywhere,SaveGame)
	TArray<FTHsNodeSavedLibraryMetaData> LibraryMetaDatas;
};

UCLASS()
class UTHsNodeCatalogDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UTHsNodeCatalogDataAsset();
	/*
	 * TOC정보가 필요한데...
	 * UTHsNodeCatalogDataAsset 이건 무조건 로드<- 단 런타임에서는 고정이란걸 알아야함.
	 * TOC위치를 로드. 그 TOC는 이제 로드할 데이터를 선택해야함
	 *	선택: 여기에서는 유저의 입력이 들어와야함. 어떤 것을 로드할지
	 *	최초: 처음에는 아무것도 없을거임. 유저가 뭐를 선택하지도 않았을 거고, 그럼 기본값을 로드해야함. 그 기본값은 UTHsNodeCatalogDataAsset에 있어야함.
	 * UTHsNodeSavedTOC : 이걸 계속 덮어 쓰기 할거임. 즉 고정된 위치이고 최초에도 추후에도 존재해야한다라는 말임.
	 */
	UPROPERTY(Category="Catalog|TOC",SaveGame,
				EditAnywhere,BlueprintReadWrite)
	FString TOCSlotName;
	UPROPERTY(Category="Catalog|TOC",SaveGame,
			EditAnywhere,BlueprintReadWrite)
	int32 TOCUserIndex;

	UPROPERTY(Category="Catalog|Default|Class",SaveGame,
			EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UTHsNodeWorldLibrary> DefaultWorldNodeLibraryClass;
	UPROPERTY(Category="Catalog|Default|Class",SaveGame,
			EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UTHsNodeLibrary> DefaultUserNodeLibraryClass;
	
	UPROPERTY(Category="Catalog|Default|Class",SaveGame,
			EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UTHsNodeFactory> DefaultNodeFactoryClass;
	
	UPROPERTY(Category="Catalog|Data",SaveGame,
			EditAnywhere,BlueprintReadWrite,Instanced)
	TObjectPtr<UTHsNodeLibrary> DefaultWorldNodeLibrary;
	
	UPROPERTY(Category="Catalog|Data",SaveGame,
    		EditAnywhere,BlueprintReadWrite,Instanced)
	TObjectPtr<UTHsNodeForest> CatalogNodeForest;
	
	UPROPERTY(Category="Catalog|Default|TOC",SaveGame,
			EditAnywhere,BlueprintReadWrite)
	bool UseIfFirstTOC;
	UPROPERTY(Category="Catalog|Default|TOC",SaveGame,
			EditAnywhere,BlueprintReadWrite,Instanced)
	TObjectPtr<UTHsNodeSavedTOC> FirstTOC;
	
	UPROPERTY(Category="Catalog|Setup",SaveGame,
				EditAnywhere,BlueprintReadWrite)
	bool UseAutoStartUpLoadUserTOCSavedData= true;
	
};

// USTRUCT()
// struct FTHsNodeLoadingContext
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY()
// 	
// };
USTRUCT()
struct FTHsNodeLinkWrapper
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<UTHsNode> ControlNode;

	UPROPERTY()
	TWeakObjectPtr<UObject> LinkedObject;

	bool IsLinked() const
	{
		return ControlNode.IsValid() && LinkedObject.IsValid();
	}
	template<typename IsA_T>
	bool CheckWrapperType(ETHsNodeType expectType) const
	{
		if (IsLinked())
		{
			bool nodeIsActor = ControlNode->NodeHeader->NodeType == expectType;
			bool objectIsActor = LinkedObject->IsA<IsA_T>();
			return nodeIsActor && objectIsActor;
		}
		return false;
	}
	
	bool IsActor() const
	{
		return CheckWrapperType<AActor>(ETHsNodeType::EActor);
	}
	bool IsComponent() const
	{
		return CheckWrapperType<UActorComponent>(ETHsNodeType::EComponent);
	}
	bool IsGroup() const
	{
		return CheckWrapperType<UObject>(ETHsNodeType::EGroup);
	}
	
};

UCLASS()
class UTHsNodeFactory : public UObject
{
	GENERATED_BODY()

public:
	//virtual AActor* CreateActorFromNode(UWorld* worldContext,UTHsNode* sourceNode,UTHsNode* parent = nullptr);
	//virtual FTHsNodeLinkWrapper CreateComponentFromNode(UWorld* worldContext,UTHsNode* sourceNode,AActor* ownerActor,UTHsNode* parent = nullptr);


	virtual void CloneForestFromLibrary(UTHsNodeLibrary* srcLibrary, UTHsNodeLibrary* dstLibrary);
	
	/**
	 * @brief 런타임용으로 노드 포레스트를 복제합니다.
	 * @details
	 * - UTHsNodeProbe가 생성되고 적용됩니다.
	 * - sourceNodeForest의 모든 노드들은 CloneableNodeFlag()가 true인 경우에만 복제됩니다.
	 * - 복제된 노드들은 ownerLibrary에 속하게 됩니다.
	 * - 보통은 UTHsNodeProbe의 내부에서 tree들을 복제하고 그 tree의 요소들을 복제합니다.
	 *	- 복제후에 children 들도 복제합니다. 이때 parentNode를 지정해줄 수 있습니다.
	 * @param sourceNodeForest 
	 * @param ownerLibrary 
	 * @return 
	 */
	virtual UTHsNodeForest* CloneForestForRuntime(UTHsNodeForest* sourceNodeForest,UTHsNodeLibrary* ownerLibrary);
	virtual UTHsNodeTree* CloneTreeForRuntime(UTHsNodeTree* sourceNodeTree,UTHsNodeForest* ownerForest);
	virtual UTHsNode* CloneNodeForRuntime(UTHsNode* sourceNode,UTHsNodeTree* ownerTree,UTHsNode* parentNode = nullptr);
	
	virtual void CloneHeaderForRuntime(const UTHsNodeHeader* sourceHeader,UTHsNode* ownerNode);
	virtual void CloneBodyForRuntime(const UTHsNodeBody* sourceBody,UTHsNode* ownerNode);
	virtual void CloneChildrenForRuntime(TArray<TObjectPtr<UTHsNode>>& sourceChildren,UTHsNode* ownerNode);
	
	/*
	*	component
	*	group
	*	actor
	*	forest
	*	Tree
	*	node
	* */
	//virtual FTHsNodeLinkWrapper ParsingFromNode(UWorld* worldContext,UTHsNode* sourceNode);
	//virtual UTHsNodeTree* ParsingFromTree(UWorld* worldContext,UTHsNodeTree* sourceNodeTree,UTHsNodeLibrary* ownerLibrary);
	
	//virtual UTHsNodeForest* ParsingFromForest(UWorld* worldContext,const UTHsNodeForest* sourceNodeForest,UTHsNodeForest* targetNodeForest,UTHsNodeLibrary* ownerLibrary);
	//virtual UTHsNodeLibrary* ParsingFromLibrary(UWorld* worldContext,const UTHsNodeLibrary* sourceNodeLibrary);
	//virtual UTHsNode* MakeEmptyNode(UObject* outer);

	TSet<TWeakObjectPtr<UTHsNode>> CreatedNodes;

	TSubclassOf<UTHsNodeProbe> DefaultForestProbeClass;
	
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTHsNodeManagerOnInputModal,TWeakObjectPtr<ATHsNodeManager> ,NodeManager);

/**
 * @class ATHsNodeManager
 * @brief 노드 시스템의 중앙 관리 액터입니다.
 * @details
 * - 검색이 가능합니다. 
 */
UCLASS(Blueprintable)
class ATHsNodeManager : public AActor
{
	GENERATED_BODY()
///Script/TIHHousingCore.THsNodeCatalogDataAsset'/Game/NodeTest/NodeCatalogDataAsset0.NodeCatalogDataAsset0'
protected:
	virtual void BeginPlay() override;

	void CheckUserFirstTOC();
public:
	ATHsNodeManager();

	void LoadNodeCatalogDataAsset(const FString& assetPath =
		TEXT("/Script/TIHHousingCore.THsNodeCatalogDataAsset'/Game/NodeTest/NodeCatalogDataAsset0.NodeCatalogDataAsset0'"));
	void LoadNodeLibraries();
	void LoadUserTOCSavedData();

	UTHsNodeCatalogDataAsset* GetNodeCatalogDataAsset() const
	{
		return NodeCatalogDataAsset;
	}
	UTHsNodeLibrary* GetWorldNodeLibrary() const
	{
		return WorldNodeLibrary;
	}
	UTHsNodeLibrary* GetUserNodeLibrary() const
	{
		return UserNodeLibrary;
	}
	
	UPROPERTY(Category="NodeManager|Events",
		BlueprintAssignable,BlueprintReadWrite)
	FTHsNodeManagerOnInputModal OnInputModal;
	UFUNCTION(Category="NodeManager|Events|InputModal",
			BlueprintCallable)
	void OnLoadTOCSelectionModal()
	{
		if (DbcReadyToClone())
		{
			OnInputModal.Broadcast(this);
		}
	}

	
	UFUNCTION(Category="NodeManager|Events|InputModal",
		BlueprintCallable)
	void OnLoadAcceptInputModal(int32 savedMataDataIndex);
	UFUNCTION(Category="NodeManager|Events|InputModal",
		BlueprintCallable)
	void OnLoadCancelInputModal();
	
	UFUNCTION(Category="NodeManager|Events|InputModal",	BlueprintCallable)
	void OnNewNodeSavedLibraryMetaData(UTexture2D* thumbnailImage,const FString& saveDescription);
	UFUNCTION(Category="NodeManager|Events|InputModal",	BlueprintCallable)
	void SetLoadUserTOC(bool bLoadUserTOC)
	{
		bLoadedUserTOC = bLoadUserTOC;
	}
	
	UFUNCTION(Category="NodeManager|DbC",BlueprintCallable)
	bool DbcIsLoadingTOC() const
	{
		return IsValid(NodeCatalogDataAsset) && IsValid(SavedTOC);
	}
	UFUNCTION(Category="NodeManager|DbC",BlueprintCallable)
	bool DbcIsValidLibraries() const
	{
		return WorldNodeLibrary != nullptr && UserNodeLibrary != nullptr;
	}
	UFUNCTION(Category="NodeManager|DbC",BlueprintCallable)
	bool DbcReadyToTocSetUp() const
	{
		return IsValid(NodeCatalogDataAsset) && DbcIsValidLibraries();
	}
	UFUNCTION(Category="NodeManager|DbC",BlueprintCallable)
	bool DbcReadyToClone()
	{
		return IsValid(SavedTOC) && DbcReadyToTocSetUp();
	}
	
	UFUNCTION(Category="NodeManager|DbC",BlueprintCallable)
	bool DbcIsLoadComplete() const
	{
		return NodeCatalogDataAsset != nullptr &&
			WorldNodeLibrary != nullptr &&
			UserNodeLibrary != nullptr;
	}
	UFUNCTION()
	UTHsNode* FindNodeByActor(AActor* InActor) const{return nullptr;};
	UFUNCTION()
	AActor* FindActorByNode(UTHsNode* InNode) const{return nullptr;}

	UPROPERTY()
	TObjectPtr<UTHsNodeLibrary> ReservedLibraryAsset = nullptr;
	
	UTHsNodeFactory* GetNodeFactory();
	
	UFUNCTION(BlueprintCallable)
	void CloneLibraryFromSaveGame(USaveGame* saveGameData);
	
	UObject* FindLinkedObjectByNode(UTHsNode* InNode) const
	{
		//	TODO
		return nullptr;
	};
	
private:
	/*
	 *	DbC 검사
	 *	- 완료시: NodeManagerDbC 가 보장됨
	 *		- 보장됨은 새로운 액터를 생성하는게 가능하다.
	 *	
	 *	기초 로드검사:
	 *	- NodeCatalogDataAsset 가 로드되었는가?
	 *	- WorldNodeLibrary 가 nullptr 이면 안됨
	 *	- UserNodeLibrary 가 nullptr 이면 안됨
	 *	- 완료시: LoadComplete 가 보장됨
	 *	월드 노드 라이브러리 검사:
	 *	- NodeForest 가 nullptr 이면 안됨
	 *	- RootNode 가 nullptr 이면 안됨
	 *	- 완료시: WorldNodeLibraryValid 가 보장됨
	 *	유저 노드 라이브러리 검사:
	 *	- NodeForest 가 nullptr 이면 안됨
	 *	- RootNode 가 nullptr 이면 안됨,
	 *	- 완료시: UserNodeLibraryValid 가 보장됨
	 *	ui 위젯 검사
	 *	- UI 위젯이 nullptr 이면 안됨
	 * 
	 */
	// bool DbcIsStoredComplete() const
	// {
	// 	return DbcIsLoadComplete() && ;
	// }
	
	FTHsNodeSavedLibraryMetaData CurrentWorldLibraryMetaData;

	UPROPERTY()
	TObjectPtr<UTHsNodeSavedTOC> SavedTOC;
	
	UPROPERTY()
	TObjectPtr<UTHsNodeCatalogDataAsset> NodeCatalogDataAsset;
	UPROPERTY()
	TObjectPtr<UTHsNodeWorldLibrary> WorldNodeLibrary;	//	게임을 껐다 켰을때 복원되어야함.
	UPROPERTY()
	TObjectPtr<UTHsNodeLibrary> UserNodeLibrary;	//	유저가 저장하고 불러올 수 있어야함.

	TSet<FTHsCoroutineWorker*> CoroutineWorkers;

	UPROPERTY()
	TObjectPtr<UTHsNodeFactory> NodeFactory;
private:
	bool bTOCSelectionModalAccept = false;
	bool bLoadedUserTOC = false;
};

UCLASS()
class ATHsNodeGameMode : public AGameMode
{
    GENERATED_BODY()
public:
	
};


UCLASS()
class TIHHOUSINGCORE_API UTHsNodeTreeCore : public UObject
{
	GENERATED_BODY()
};
