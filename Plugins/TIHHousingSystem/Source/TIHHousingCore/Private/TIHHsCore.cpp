// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHsCore.h"

bool UTIHHsGlobalConfigure::gForceDisable = false;
bool UTIHHsGlobalConfigure::gTickPauseEnable = false;
bool UTIHHsGlobalConfigure::gTickRuntimeActive = true;


ITIHHsBaseObject* UTIHHsObjcetSubSystem::CreateTIHHsObject(FName objectName)
{
	ITIHHsBaseObject* newObject = nullptr;

	if (mObjectTable.Contains(objectName))
	{
		UClass* objectClass = mObjectTable[objectName];
		if (objectClass)
		{

		}
	}
	return newObject;
}

void UTIHHsObjcetSubSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	mWorld = &InWorld;
	UE_LOG(LogTemp, Warning, TEXT("[[ UTIHHsObjcetSubSystem::OnWorldBeginPlay ]] "));
}

void UTIHHsObjcetSubSystem::DefualtCommands()
{
	mCommandNodeTable.Add(TEXT("attach")).FunctionDelegate.BindLambda([this](FTIHHsCommandArgument& args)
	{
		bool success = true;
		success &= args.JsonValue.HasField(TEXT("parent"));
		success &= args.JsonValue.HasField(TEXT("child"));
		
		if (success)
		{
			int32 parentId = args.JsonValue.GetIntegerField(TEXT("parentId"));
			int32 childId = args.JsonValue.GetIntegerField(TEXT("child"));
			bool valid = true;
			valid &= mObjectInterfaceTable.Contains(parentId);
			valid &=mObjectInterfaceTable.Contains(childId);

			if (valid)
			{
				mRelationshipSubsystem->Attach(mObjectInterfaceTable[parentId],mObjectInterfaceTable[childId]);
			}
			
		}
	});
	mCommandNodeTable.Add(TEXT("detach")).FunctionDelegate.BindLambda([this](FTIHHsCommandArgument& args)
	{
		bool success = true;
		success &= args.JsonValue.HasField(TEXT("child"));
		
		if (success)
		{
			int32 childId = args.JsonValue.GetIntegerField(TEXT("child"));
			if (mObjectInterfaceTable.Contains(childId))
			{
				mRelationshipSubsystem->DetachFrom(mObjectInterfaceTable[childId]);
			}
		}
	});
	
	mCommandNodeTable.Add(TEXT("create-stuff")).FunctionDelegate.BindLambda([this](FTIHHsCommandArgument& args)
	{
		bool success = true;
		
		success &= args.VariantValue.Contains(TEXT("transform"));
		
		if (success)
		{
			CreateStuff(args.VariantValue[TEXT("transform")].GetValue<FTransform>());
		}
	});
	// mCommandNodeTable.Add(TEXT("create-prefab")).FunctionDelegate.BindLambda([this](FTIHHsCommandArgument& args)
	// {
	// 	bool success = true;
	// 	
	// 	success &= args.JsonValue.HasField(TEXT("class"));
	// 	success &= args.JsonValue.HasField(TEXT("transform"));
	// 	
	// 	if (success)
	// 	{
	// 	
	// 		FSoftClassPath classPath(args.JsonValue.GetStringField(TEXT("class")));
	// 		if (classPath.IsValid())
	// 		{
	// 			classPath.TryLoadClass<UClass>();	
	// 		}
	// 	}
	// 	
	// });
}

void UTIHHsRelationshipSubsystem::Attach(TScriptInterface<ITIHHsBaseObject> parent,
	TScriptInterface<ITIHHsBaseObject> child)
{
	// if (parent.GetInterface() != nullptr && child.GetInterface() != nullptr)
	// {
	// 	if (parent.GetInterface() == child.GetInterface())
	// 	{
	// 		return;
	// 	}
	// 	
	// 	TSet<FString> requirementList;
	// 	parent->AttachingRequestList(child, requirementList);
	// 	if (child->AttachingRequestProcessing(parent, requirementList))
	// 	{
	// 		int64 edgeUniqueId = FTIHHsRelationshipEdge::MakeUniqueEdgeId(parent,child);
	// 		if (not mRelationshipEdges.Contains(edgeUniqueId))
	// 		{
	// 			mRelationshipEdges.Add(edgeUniqueId,
	// 				FTIHHsRelationshipEdge(parent,child,parent->GetAttachmentTags()));
	// 		}
	// 		int32 parentId = parent.GetObject()->GetUniqueID();
	// 		int32 childId = child.GetObject()->GetUniqueID();
	// 		
	// 		FTIHHsRelationshipEdgeRefData& refData = mRelativeParents.FindOrAdd(parentId);
	// 		refData.Target = parent;
	// 		
	// 		if (not refData.ChildIds.Contains(childId))
	// 		{
	// 			refData.ChildIds.Add(childId);
	// 			child->SetAttachmentParentId(parentId);
	// 			child->Execute_AttachTo
	// 			(
	// 				child.GetObject(),
	// 				parent,
	// 				parent->GetAttachmentRuleWrapper()
	// 			);
	// 			//child->AttachTo(parent,parent->GetAttachmentRuleWrapper());
	// 			parent->AttachNotify(child);
	// 		}
	// 	}
	// }
}

void UTIHHsRelationshipSubsystem::DetachFrom(TScriptInterface<ITIHHsBaseObject> child)
{
	// int32 parentId = child->GetAttachmentParentId();
	// if (mRelativeParents.Contains(parentId))
	// {
	// 	TScriptInterface<ITIHHsBaseObject> parentInterface = mRelativeParents[parentId].Target;
	// 	int64 edgeId = FTIHHsRelationshipEdge::MakeUniqueEdgeId(parentId,child);
	// 	if (mRelationshipEdges.Contains(edgeId))
	// 	{
	// 		mRelationshipEdges[edgeId].Child->DetachFrom(parentInterface);
	// 		mRelationshipEdges[edgeId].Parent->DetachFrom(child);
	// 		
	// 		mRelationshipEdges[edgeId].Clear();
	// 		mRelationshipEdges.Remove(edgeId);
	// 			
	// 		mRelativeParents[parentId].ChildIds.Remove(edgeId);
	// 			
	// 		if (mRelativeParents[parentId].ChildIds.IsEmpty())
	// 		{
	// 			mRelativeParents.Remove(parentId);
	// 		}
	// 	}
	// }
}

void UTIHHsRelationshipSubsystem::DetachAllEdgesFromParent(TScriptInterface<ITIHHsBaseObject> parent)
{
	// if (parent.GetObject() != nullptr)
	// {
	// 	int32 parentId = parent.GetObject()->GetUniqueID();
	// 	if (mRelativeParents.Contains(parentId))
	// 	{
	// 		const TSet<int32>& childIds = mRelativeParents[parentId].ChildIds;
	// 		for (const int32& childId :childIds)
	// 		{
	// 			int64 edgeId = FTIHHsRelationshipEdge::MakeUniqueEdgeId(parentId, childId);
	// 			if (mRelationshipEdges.Contains(edgeId))
	// 			{
	// 				TScriptInterface<ITIHHsBaseObject> childInterface = mRelationshipEdges[edgeId].Child;
	// 				
	// 				childInterface->DetachFrom(parent);
	// 				parent->DetachFrom(childInterface);
	// 				
	// 				mRelationshipEdges[edgeId].Clear();
	// 				mRelationshipEdges.Remove(edgeId);
	// 			}
	// 		}
	// 		mRelativeParents.Remove(parentId);
	// 	}
	// }
}

TArray<TScriptInterface<ITIHHsBaseObject>> UTIHHsRelationshipSubsystem::GetChildren(
	TScriptInterface<ITIHHsBaseObject> parent)
{
	TArray<TScriptInterface<ITIHHsBaseObject>> reValue;
	if(parent.GetObject())
	{
		int32 parentId = parent.GetObject()->GetUniqueID();
		if (mRelativeParents.Contains(parentId))
		{
			const TSet<int32>& childIds = mRelativeParents[parentId].ChildIds;
			reValue.Reserve(childIds.Num());
			for (const int32& childId :childIds)
			{
				int64 edgeId = FTIHHsRelationshipEdge::MakeUniqueEdgeId(parentId, childId);
				if (mRelationshipEdges.Contains(edgeId))
				{
					if (mRelationshipEdges[edgeId].Child.GetInterface() != nullptr)
					{
						reValue.Add(mRelationshipEdges[edgeId].Child);
					}
				}
			}
		}
	}
	return reValue;
}

void UTIHHsRelationshipSubsystem::DetachIsolatedEdge(TScriptInterface<ITIHHsBaseObject> target)
{
	if (target.GetObject() != nullptr)
	{
		//int32 targetId = target.GetObject()->GetUniqueID();
		//	그냥 부모id까지 가져와서 해도되긴하는데 그냥 함. 귀찮음.
		DetachAllEdgesFromParent(target);
		DetachFrom(target);
	}
}
