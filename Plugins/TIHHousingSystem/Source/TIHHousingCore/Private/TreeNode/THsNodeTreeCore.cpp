// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeNode/THsNodeTreeCore.h"

#include "Kismet/GameplayStatics.h"


void UTHsNodeState::UpdateNodeState(TWeakObjectPtr<UTHsNode> ownerNode)
{
	if (ownerNode.IsValid())
	{
		
	}
}

UTHsNode::UTHsNode()
{
	// NodeKey = FTHsNodeKey::NotDefined();
	// NodeType = ETHsNodeType::EUndefined;
	// NodeLifecycle = ETHsNodeLifecycle::CatalogNode;
	// NodeFlags = ToBits(ETHsNodeFlags::IsCatalogAsset);
	// NodeDbC = ToBits(ETHsNodeDesignByContract::InValid);
	// NodeVersionControl = NewObject<UTHsNodeVersionControl> (this);
	//NodeHeader = FTHsNodeHeader::CreateDefaultHeader(this);
	NodeHeader = CreateDefaultSubobject<UTHsNodeHeader>(TEXT("THsNodeHeader"));
	NodeBody = CreateDefaultSubobject<UTHsNodeCatalogBody> (TEXT("THsNodeCatalogBody"));
}

bool UTHsNode::IsRuntimeNode() const
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return false;
	}
	UObject* outer = GetOuter();
	if (outer && outer->IsA(UTHsNodeCatalogDataAsset::StaticClass()))
	{
		return false;
	}
	
	if (GetWorld()) 
	{
		return true;
	}

	return false;
}



UTHsNode* UTHsNode::SetNodeLifecycle(ETHsNodeLifecycle lifecycle)
{
	if (IsRuntimeNode())
	{
		const ETHsNodeLifecycle currCycle = NodeHeader->NodeLifecycle;
		const ETHsNodeLifecycle targetCycle = lifecycle;
		switch (currCycle)
		{
		case ETHsNodeLifecycle::CatalogNode:
			if (targetCycle == ETHsNodeLifecycle::RuntimeNode)
			{
				//	복사 바로 셋하는거임. 이럴땐 로드할때 말고는 없음.
			}
			else if (targetCycle == ETHsNodeLifecycle::TemporaryNode)
			{
				//	복사, 임시 배치를 위한거임. 보통 CatalogNode -> TemporaryNode -> RuntimeNode 순서로 감.
			}
			break;
		case ETHsNodeLifecycle::RuntimeNode:
			if (targetCycle == ETHsNodeLifecycle::CatalogNode)
			{
				//	아니 불가능, 유저 라이브러리로 가는걸 왜 사이클을 변경해. 그냥 불가
				//	어떤 경우에서든 xxx -> CatalogNode 로는 못감.
			}
			else if (targetCycle == ETHsNodeLifecycle::TemporaryNode)
			{
				//	복사 및 홀로그램? 복사를 위해서 임시 노드로 변경할때
			}
			break;
		case ETHsNodeLifecycle::TemporaryNode:
			if (targetCycle == ETHsNodeLifecycle::CatalogNode)
			{
				//	불가능, 하지만 에디터에서는 가능하게 하고 싶은데...
			}
			else if (targetCycle == ETHsNodeLifecycle::RuntimeNode)
			{
				//	복사 후 그 자리에 놓기, 데이터를 저장하기 위한거라고 봐야함.
			}
			break;
		}
		NodeHeader->NodeLifecycle = lifecycle;
	}
	return this;
}

bool UTHsNode::UpdateNodeState()
{
	if (NodeHeader->HasNodePolicyFlag(ETHsNodePolicyFlags::ENoCloning))
	{
		/*
		 * 복사된건지 아닌지는
		 */
	}
	if (IsAllowedBetweenParentAndChild(ParentNode.Get(),this))
	{
		NodeState->IsDirty = false;
		if (ATHsNodeManager* nodeMgr = TryGetOwnerNodeManager())
		{
			 NodeState->IsLinkedInstance = nodeMgr->FindLinkedObjectByNode(this) != nullptr ?	 	true :	false;
		}
	}
    TravelChildNodes([weakThis = TWeakObjectPtr<UTHsNode>(this)](UTHsNode* childNodePtr)
	{
	    if (childNodePtr)
	    {
		    childNodePtr->UpdateNodeState();
	    }
	    return true;
	});
	
	return true;
}

ATHsNodeManager* UTHsNodeLibrary::TryGetOwnerNodeManager() const
{
	return OwningNodeManager.Get();
}

ATHsNodeManager* UTHsNodeTree::TryGetOwnerNodeManager() const
{
	if (UTHsNodeLibrary* ownerLibrary= TryGetOwnerNodeLibrary())
	{
		return ownerLibrary->TryGetOwnerNodeManager();
	}
	return nullptr;
}

UTHsNodeLibrary* UTHsNodeTree::TryGetOwnerNodeLibrary() const
{
	if (UTHsNodeForest* ownerForest = TryGetOwnerNodeForest())
	{
		return ownerForest->TryGetOwnerNodeLibrary();
	}
	return nullptr;
}

UTHsNodeForest* UTHsNodeTree::TryGetOwnerNodeForest() const
{
	return OwningNodeForest.Get();
}

ATHsNodeManager* UTHsNodeForest::TryGetOwnerNodeManager() const
{
	if (UTHsNodeLibrary* ownerLibrary = TryGetOwnerNodeLibrary())
	{
		return ownerLibrary->TryGetOwnerNodeManager();
	}
	return nullptr;
}

UTHsNodeLibrary* UTHsNodeForest::TryGetOwnerNodeLibrary() const
{
	return OwningLibrary.Get();
}

ATHsNodeManager* UTHsNode::TryGetOwnerNodeManager() const
{
	if (UTHsNodeLibrary* ownerLibrary= TryGetOwnerNodeLibrary())
	{
		return ownerLibrary->TryGetOwnerNodeManager();
	}
	return nullptr;
}

UTHsNodeLibrary* UTHsNode::TryGetOwnerNodeLibrary() const
{
	if (UTHsNodeForest* ownerForest = TryGetOwnerNodeForest())
	{
		return ownerForest->TryGetOwnerNodeLibrary();
	}
	return nullptr;
}

UTHsNodeForest* UTHsNode::TryGetOwnerNodeForest() const
{
	if (OwningNodeTree.IsValid())
	{
		return OwningNodeTree->TryGetOwnerNodeForest();
	}
	return nullptr;
}

UTHsNodeTree* UTHsNode::TryGetOwnerNodeTree() const
{
	return OwningNodeTree.Get();
}

UTHsNode* UTHsNode::TryGetOwnerNode() const
{
	return ParentNode.Get();
}



UTHsNode* UTHsNodeProxy_MakeRuntimeNode::MakeNodeFromCatalogNode(UTHsNode* InCatalogNode)
{
	if (InCatalogNode == nullptr)
	{
		return nullptr;
	}
	
	UTHsNode* NewNode = nullptr;

	
	return nullptr;
}

void UTHsNodeTree::UpdateTree()
{
	/*
	 * TODO: NodeTree 의 상태를 갱신하는 작업
	 * - 루트 노드부터 시작해서 전체 노드를 순회하면서 상태를 갱신
	 * - 노드의 상태에 따라 트리의 메타데이터(예: 노드 수, 깊이 등)를 업데이트
	 * - 무조건 유효성 검사
	 * - 트리 상태 설정
	 */
}

TScriptInterface<ITHsNodeTrait> UTHsNodeForestProbe::ProcessProbe(TScriptInterface<ITHsNodeTrait> nodeTrait)
{
	/*
	* record : try check nodeTrait is UTHsNodeForest
	* NodeLibrary, nodeTrait
	*/
	if (NodeLibrary.IsValid() &&
		nodeTrait!= nullptr &&
		nodeTrait.GetObject()->IsA(UTHsNodeForest::StaticClass()))
	{
		/*
		* record : try cast nodeTrait to UTHsNodeForest and create new UTHsNodeForest
		* srcNodeForest, dstNodeForest
		*/
		UTHsNodeForest* srcNodeForest = Cast<UTHsNodeForest>(nodeTrait.GetObject());
		/* New Node Forest  */
		UTHsNodeForest* dstNodeForest = NewObject<UTHsNodeForest>(NodeLibrary.Get(),nodeTrait.GetObject()->GetClass());

		if (dstNodeForest)
		{
			/*
			 * record : try clone forest properties from src to dst
			 * OwningLibrary, Environment, CustomForestProbeClass
			 */
			dstNodeForest->OwningLibrary = NodeLibrary.Get();
			dstNodeForest->Environment = srcNodeForest->Environment;
			dstNodeForest->CustomForestProbeClass = srcNodeForest->CustomForestProbeClass;
			return ProcessCloneProbe(srcNodeForest,dstNodeForest);
		}
	}
	return nullptr;
}

TScriptInterface<ITHsNodeTrait> UTHsNodeForestProbe::ProcessCloneProbe(
	TScriptInterface<ITHsNodeTrait> srcNodeTrait,TScriptInterface<ITHsNodeTrait> dstNodeTrait)
{
	Super::ProcessCloneProbe(srcNodeTrait,dstNodeTrait);
	
	UTHsNodeForest* srcForest = Cast<UTHsNodeForest>(srcNodeTrait.GetObject());
	UTHsNodeForest* dstForest = Cast<UTHsNodeForest>(dstNodeTrait.GetObject());
	
	if (srcForest != nullptr &&
		dstForest != nullptr &&
		NodeFactory.IsValid())
	{
		srcForest->TravelNodeTrees(
			[
				dstForest,
				nodeFactory = NodeFactory
			](UTHsNodeTree* srcNodeTreePtr)
			{
				if (nodeFactory.IsValid())
				{
					if (UTHsNodeTree* newTree = nodeFactory->CloneTreeForRuntime(srcNodeTreePtr,dstForest))
					{
						dstForest->AddTreeToForest(newTree);
					}
				}
			});
		return dstForest;
	}
	return nullptr;
}

UTHsNodeCatalogDataAsset::UTHsNodeCatalogDataAsset()
{
	TOCSlotName = TEXT("CatalogTOC");
	TOCUserIndex = 0;
	DefaultWorldNodeLibraryClass = UTHsNodeWorldLibrary::StaticClass();
	DefaultNodeFactoryClass = UTHsNodeFactory::StaticClass();
}


void UTHsNodeFactory::CloneForestFromLibrary(UTHsNodeLibrary* srcLibrary, UTHsNodeLibrary* dstLibrary)
{
	/*
	* record: try clone forest from source library to destination library
	* srcLibrary, dstLibrary
	*/
	if (srcLibrary &&
	srcLibrary->DbcValidCatalogNodeLibrary() &&
	dstLibrary->DbcCopyableNodeLibrary())
	{
		if (UTHsNodeWorldLibrary* srcWorldLibrary = Cast<UTHsNodeWorldLibrary>(srcLibrary))
		{
			/*
			* record : success clone forest from source library to destination library
			* srcLibrary, dstLibrary
			*/
			UTHsNodeForest* srcForestPtr = srcWorldLibrary->NodeForest;
			UTHsNodeFactory* nodeFactory = this;
			if (srcForestPtr != nullptr && nodeFactory != nullptr)
			{
				/*
				 *	record: try clone forest from source forest to destination library
				 *	srcForestPtr, dstLibrary
				 */
				if (UTHsNodeForest* newForest = nodeFactory->CloneForestForRuntime(srcForestPtr,dstLibrary))
				{
					dstLibrary->SetForestToLibrary(newForest);
				}
			}
		}
	}
}

UTHsNodeForest* UTHsNodeFactory::CloneForestForRuntime(
	UTHsNodeForest* sourceNodeForest,UTHsNodeLibrary* ownerLibrary)
{
	UTHsNodeForest* result = nullptr;
	if (sourceNodeForest != nullptr && ownerLibrary != nullptr)
	{
		/*
		* record : must get custom probe class from source
		* probeCls, 
		*/
		UClass* probeCls = sourceNodeForest->CustomForestProbeClass;
		
		if (probeCls == nullptr)
		{
			//	default forest probe
			probeCls = UTHsNodeForestProbe::StaticClass();
		}
		/*
		* record : must process probe with custom probe class from source
		* probeCls, ownerLibrary
		*/
		if (UTHsNodeProbe* customProbe = NewObject<UTHsNodeProbe>(ownerLibrary,probeCls))
		{
			customProbe->SetupProbeContext(	
			this,
			ownerLibrary->OwningNodeManager.Get(),
			nullptr,
			nullptr,
			ownerLibrary
			);
			
			TScriptInterface<ITHsNodeTrait> probeResult = customProbe->ProcessProbe(sourceNodeForest);
		
			if (probeResult.GetObject() != nullptr)
			{
				result = Cast<UTHsNodeForest>(probeResult.GetObject());
			}
		}
	}
	return result;
}

UTHsNodeTree* UTHsNodeFactory::CloneTreeForRuntime(UTHsNodeTree* sourceNodeTree, UTHsNodeForest* ownerForest)
{
	UTHsNodeTree* result = nullptr;
	if (sourceNodeTree != nullptr)
	{
		result = NewObject<UTHsNodeTree>(ownerForest);
		result->TreeName = sourceNodeTree->TreeName;
		result->ThumbnailImage = sourceNodeTree->ThumbnailImage;
		result->OwningNodeForest = ownerForest;
		
		if (UTHsNode* newRootNode = CloneNodeForRuntime(sourceNodeTree->RootNode,result,nullptr))
		{
			result->RootNode = newRootNode;
			result->UpdateTree();
			if (not newRootNode->CheckInspectionNodeType())
			{
				result = nullptr;
			}
		}
	}
	return result;
}

UTHsNode* UTHsNodeFactory::CloneNodeForRuntime(UTHsNode* sourceNode, UTHsNodeTree* ownerTree, UTHsNode* parentNode)
{
	UTHsNode* result = nullptr;
	
	if (UTHsNode::IsAllowedBetweenParentAndChild(parentNode,sourceNode))
	{
		if (sourceNode->NodeHeader->CloneableNodeFlag())
		{
			result = NewObject<UTHsNode>(ownerTree);
			result->ParentNode = parentNode;
			result->OwningNodeTree = ownerTree;
			CloneHeaderForRuntime(sourceNode->NodeHeader,result);
			CloneBodyForRuntime(sourceNode->NodeBody,result);
			CloneChildrenForRuntime(sourceNode->ChildNodes,result);
			result->UpdateNodeState();
		}
	}
	
	return result;
}

void UTHsNodeFactory::CloneHeaderForRuntime(const UTHsNodeHeader* sourceHeader, UTHsNode* ownerNode)
{
	if (sourceHeader)
	{
		ownerNode->NodeHeader->CloneFrom(sourceHeader,ownerNode);
		ownerNode->SetRuntimeNodeCycle();
	}
	else
	{
		
	}
}

void UTHsNodeFactory::CloneBodyForRuntime(const UTHsNodeBody* sourceBody, UTHsNode* ownerNode)
{
	if (sourceBody)
	{
		ownerNode->NodeBody->CloneFrom(sourceBody);
	}
}

void UTHsNodeFactory::CloneChildrenForRuntime(TArray<TObjectPtr<UTHsNode>>& sourceChildren, UTHsNode* ownerNode)
{
	if (not sourceChildren.IsEmpty())
	{
		for (auto sourceChildNode :sourceChildren)
		{
			if (UTHsNode* newNode = CloneNodeForRuntime(sourceChildNode,ownerNode->OwningNodeTree.Get(),ownerNode))
			{
				ownerNode->AddNodeToChildNodes(newNode);
			}
		}
	}
}

void ATHsNodeManager::CheckUserFirstTOC()
{
	if (not UGameplayStatics::DoesSaveGameExist(NodeCatalogDataAsset->TOCSlotName, NodeCatalogDataAsset->TOCUserIndex))
	{
		UTHsNodeSavedTOC* firstTOC = nullptr;
		if (NodeCatalogDataAsset->UseIfFirstTOC && NodeCatalogDataAsset->FirstTOC != nullptr)
		{
			firstTOC = NodeCatalogDataAsset->FirstTOC;
		}
		else
		{
			firstTOC = NewObject<UTHsNodeSavedTOC>(this, UTHsNodeSavedTOC::StaticClass());
		}
		UGameplayStatics::SaveGameToSlot(firstTOC, NodeCatalogDataAsset->TOCSlotName, NodeCatalogDataAsset->TOCUserIndex);
	}
}

void ATHsNodeManager::LoadUserTOCSavedData()
{
	if (DbcReadyToTocSetUp())
	{
		UE_LOG(LogTemp, Warning, TEXT("ATHsNodeManager::LoadUserTOCSavedData"));
		CheckUserFirstTOC();
		
		if (USaveGame* toc= UGameplayStatics::LoadGameFromSlot(NodeCatalogDataAsset->TOCSlotName, NodeCatalogDataAsset->TOCUserIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("ATHsNodeManager::LoadUserTOCSavedData : Load Success"));
			if (UTHsNodeSavedTOC* userSavedList = Cast<UTHsNodeSavedTOC>(toc))
			{
				UE_LOG(LogTemp, Warning, TEXT("ATHsNodeManager::LoadUserTOCSavedData : Cast Success"));
				SavedTOC = userSavedList;
				/*
				 * Trigger Input Modal for TOC Selection
				 */
				OnInputModal.Broadcast(this);
			}
		}
	}
}

void ATHsNodeManager::LoadNodeCatalogDataAsset(const FString& assetPath)
{
	if (NodeCatalogDataAsset == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATHsNodeManager::LoadNodeCatalogDataAsset : %s"), *assetPath);
		NodeCatalogDataAsset = LoadObject<UTHsNodeCatalogDataAsset>(nullptr,*assetPath );
		//	GC대상인지 확인해보기
	}
}

void ATHsNodeManager::LoadNodeLibraries()
{
	if (NodeCatalogDataAsset )
	{
		UE_LOG(LogTemp, Warning, TEXT("ATHsNodeManager::LoadNodeLibraries"));
		UClass* nodeLibraryCls = NodeCatalogDataAsset->DefaultWorldNodeLibraryClass;
		if (nodeLibraryCls == nullptr)
		{
			nodeLibraryCls = UTHsNodeWorldLibrary::StaticClass();
		}
		
		if (WorldNodeLibrary == nullptr)
		{
			WorldNodeLibrary = NewObject<UTHsNodeWorldLibrary>(this,nodeLibraryCls);
			WorldNodeLibrary->OwningNodeManager = this;
			WorldNodeLibrary->bIsUserLibrary = true;
		}
		
		nodeLibraryCls = nullptr;
		
		nodeLibraryCls = NodeCatalogDataAsset->DefaultUserNodeLibraryClass;
		if (nodeLibraryCls == nullptr)
		{
			nodeLibraryCls = UTHsNodeLibrary::StaticClass();
		}

		if (UserNodeLibrary == nullptr)
		{
			UserNodeLibrary = NewObject<UTHsNodeLibrary>(this,nodeLibraryCls);
			UserNodeLibrary->OwningNodeManager = this;
			UserNodeLibrary->bIsUserLibrary = true;
		}
	}
}

void ATHsNodeManager::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("ATHsNodeManager::BeginPlay"));

	LoadNodeCatalogDataAsset(TEXT("/Script/TIHHousingCore.THsNodeCatalogDataAsset'/Game/NodeTest/NodeCatalogDataAsset0.NodeCatalogDataAsset0'"));

	LoadNodeLibraries();
	
	if (DbcReadyToTocSetUp() && NodeCatalogDataAsset->UseAutoStartUpLoadUserTOCSavedData)
	{
		LoadUserTOCSavedData();
	}
}

ATHsNodeManager::ATHsNodeManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATHsNodeManager::OnLoadAcceptInputModal(int32 savedMataDataIndex)
{
	if (DbcIsLoadingTOC())
	{
		bTOCSelectionModalAccept = true;

		if (SavedTOC->LibraryMetaDatas.IsValidIndex(savedMataDataIndex))
		{
			SavedTOC->CurrentLibraryIndex = savedMataDataIndex;
			CurrentWorldLibraryMetaData = SavedTOC->LibraryMetaDatas[savedMataDataIndex];
		
			const FString slotName = CurrentWorldLibraryMetaData.SlotName;
			const int32 userIndex = CurrentWorldLibraryMetaData.UserIndex;
			/*
			 * record: try async load user library from slot
			 * slotName, userIndex
			 */
			UGameplayStatics::AsyncLoadGameFromSlot(slotName, userIndex,
				FAsyncLoadGameFromSlotDelegate::CreateWeakLambda(
					this,
					[weakMgrThis = TWeakObjectPtr<ATHsNodeManager>(this),slotName,userIndex]
					(const FString& InSlotName, const int32 InUserIndex, USaveGame* LoadedGame)
				{
					if (IsInGameThread() && weakMgrThis.IsValid())
					{
						weakMgrThis->CloneLibraryFromSaveGame(LoadedGame);
					}
					else
					{
						checkf(false,TEXT("AsyncLoadGameFromSlot Delegate is not in Game Thread"));
					}	
				}));
			
		}
		
	}
}

void ATHsNodeManager::OnLoadCancelInputModal()
{
	if (DbcIsLoadingTOC())
	{
		
	}
}

void ATHsNodeManager::OnNewNodeSavedLibraryMetaData(UTexture2D* thumbnailImage,const FString& saveDescription)
{
}


UTHsNodeFactory* ATHsNodeManager::GetNodeFactory()
{
	/*
	* record : must get node factory
	* NodeFactory
	*/
	if (NodeFactory == nullptr)
	{
		NodeFactory = NewObject<UTHsNodeFactory>(this,
			GetNodeCatalogDataAsset()->DefaultNodeFactoryClass);
		/*
		 * TODO: DbC 팩토리 검사.
		 */
	}
	return NodeFactory;
}

void ATHsNodeManager::CloneLibraryFromSaveGame(USaveGame* saveGameData)
{
	if (saveGameData && WorldNodeLibrary->DbcCopyableNodeLibrary())
	{
		/*
		 * record: success set load user toc true
		 * slotName, userIndex
		 */
		SetLoadUserTOC(true);
		/*
		 * record: try clone forest from loaded user library to world library
		 * slotName, userIndex
		 */
		if (UTHsNodeWorldLibrary* srcWorldLibrary = Cast<UTHsNodeWorldLibrary> (saveGameData))
		{
			/*
			 * record: success load user library from slot
			 * slotName, userIndex
			 */
			ReservedLibraryAsset = WorldNodeLibrary;
			GetNodeFactory()->CloneForestFromLibrary(srcWorldLibrary, WorldNodeLibrary);
		}
	}
}
