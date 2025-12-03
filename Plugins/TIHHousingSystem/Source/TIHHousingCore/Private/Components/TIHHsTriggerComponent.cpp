// Fill out your copyright notice in the Description page of Project Settings.


#include "TIHHousingCore/Public/TIHComponents/TIHHsTriggerComponent.h"

#include "InteractiveGizmoManager.h"
#include "PatternTool.h"


#include "ActionSystem/TIHHsActionSystem.h"
#include "Cores/TIHHsCoreClasses.h"
#include "Surface/TIHHsSurfaceCore.h"

//FTimerManager* UTIHHsProbeTraceTarget::gTimerManager = nullptr;
//UTIHHsActionSystem * UTIHHsCommandTraceMouse::gActionSystem = nullptr;



int32 UTIHHsConnectionNode::ProcessGeneration_Implementation(TArray<UActorComponent*>& others)
{
	return 0;
}



void UTIHHsProbeTraceMouse::TraceBegin(TScriptInterface<ITIHHsInteraction> target)
{
	mTarget = target;
	if (UWorld* world = mTarget.GetObject()->GetWorld())
	{
		mActionSystem = world->GetSubsystem<UTIHHsActionSystem>();
		mWorldContext = world;
	}
	
	//	TODO: 검증 코드를 넣어야함. 예를 들어 지금 물체인 target이 자유롭게 움직이는게 가능하니? 같은거.
	mOnMouseMoveHandle = mActionSystem->OnActionEventMouseMoveDelegate.AddUObject(this,&UTIHHsProbeTraceMouse::TraceUpdate);

	if (mMovementSurface == nullptr && mTarget.GetObject() != nullptr)
	{
		if (AActor* targetActor = Cast<AActor>(mTarget.GetObject()))
		{
			const FVector& camLocation = mActionSystem->GetCameraLocation();
			// 기본 평면을 생성
			mMovementSurface = UTIHHsSurfaceUtility::CreateCameraPlan(
			camLocation,
			mActionSystem->GetCameraForwardVector(),
			targetActor->GetActorLocation(),
			targetActor->GetWorld(),
				this
			);
			mMovementSurface->AddProcessor(TEXT("grid-check"));
		}
	}
	if (AActor* targetActor = Cast<AActor>(mTarget.GetObject()))
	{
		mMovementSurface->UpdatePlane(targetActor->GetActorLocation(),mActionSystem->GetCameraForwardVector());
	}
	
}
void UTIHHsProbeTraceMouse::TraceUpdate(const FTIHHsActionParamIn& paramIn)
{
	if (mMovementSurface.IsResolved())
	{
		if (mMovementSurface->Intersect(
		mActionSystem->GetCurrentMouseRay(),
		mHitResult
		))
		{
			UE_LOG(LogTemp,Warning, TEXT("Mouse Trace Hit at: %s"), *mHitResult.Location.ToString());
			//mHitResult.Location
			if (AActor* targetActor = Cast<AActor>(mTarget.GetObject()))
			{
				FTIHHsSurfaceRuleProcResult input;
				
				input.Transform.SetLocation(mHitResult.Location);
				input.Transform.SetRotation(targetActor->GetActorQuat());
				input.Transform.SetScale3D(targetActor->GetActorScale3D());

				//	GridCheck -> ConcaveCheck -> convexCheck 
				input = mMovementSurface->ExecutePipeline(input);
				targetActor->SetActorTransform(input.Transform);
				/*
				 *	TODO:
				 *	transform이 같을수도 다를수도 있음
				 *	만약 원한다면 나중에 그걸 구분하는 것을 여기에 넣어서 호출을 다르게
				 *	해줄수도 있음
				 *	근데 지금은 그냥 변화가 있든 없든 무조건 호출함
				 *	왜냐하면 일단 마우스가 움직인거긴 하니깐.
				 */
				mTarget->ActionEventMoveAny_Deprecated(paramIn);
				mMovementSurface->DrawDebug(mWorldContext,0.0333f);
				//targetActor->SetActorLocation(mHitResult.Location);
			}
		}
	}
}

void UTIHHsProbeTraceMouse::TraceEnd()
{
	if (mActionSystem)
	{
		UE_LOG(LogTemp, Display, TEXT("%s"), *mActionSystem->GetName());
		//mActionSystem->OnActionEventMouseMoveRegistry->UnRegisterInterafce(this);
		mActionSystem->OnActionEventMouseMoveDelegate.Remove(mOnMouseMoveHandle);
	}
}


void UTIHHsProbeTraceTarget::TraceEnd()
{
	//if (mTimerMgr && mTimerMgr->IsTimerActive(mLoopTimerHandle))
	//{
	//	mTimerMgr->ClearTimer(mLoopTimerHandle);
	//}
	mIsTracing           = false;
	mLastElapsedTime     = 0.0f;
	mSubjectComponent    = nullptr;
	mObjectiveComponent  = nullptr;
	mCurveFloat          = nullptr;
}

void UTIHHsProbeTraceTarget::TraceUpdate()
{
	if (not mIsTracing || mSubjectComponent == nullptr || mObjectiveComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *mSubjectComponent->GetName());
		return;
	}
	// if (not mTimerMgr->IsTimerActive(mLoopTimerHandle))
	// {
	// 	TraceEnd();
	// 	return;
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Display, TEXT("%s"), *mSubjectComponent->GetName());
	// }
	
	if (mSubjectComponent->Mobility != EComponentMobility::Movable ||
		mObjectiveComponent->Mobility != EComponentMobility::Movable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTIHHsProbeTraceTarget::TraceUpdate: Subject or Objective component is not movable."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("UTIHHsProbeTraceTarget::TraceUpdate called."));
	const FTransform currObjW = mObjectiveComponent->GetComponentTransform();
	const FTransform newW = mRelativeTransform * currObjW;

	//mSubjectComponent->SetRelativeLocation(FVector(0,0,-1000));
	mSubjectComponent->SetRelativeTransform(newW);
	//mSubjectComponent->SetWorldTransform(currObjW);
	
	if (EndConditionFunction && EndConditionFunction( mSubjectComponent, mObjectiveComponent))
	{
		TraceEnd();
	}
}

void ATIHHsTriggerInteractableActor::ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	if (mLinkedTriggerComponent)
	{
		mLinkedTriggerComponent->OffVisibleTriggerGizmo();
	}
	
}

ATIHHsTriggerInteractableActor::ATIHHsTriggerInteractableActor()
{
	//	/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'
	mStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(mStaticMeshComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshObj(
	   TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'")
   );
	if (CubeMeshObj.Succeeded())
	{
		mStaticMeshComponent->SetStaticMesh(CubeMeshObj.Object);
	}
}

void ATIHHsTriggerInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	if (mStaticMeshComponent && mStaticMeshComponent->GetStaticMesh() == nullptr)
	{
		//	G:/unreal/5_4/UnrealEngine/Engine/Content/BasicShapes/Cube.uasset
		UStaticMesh* Mesh = Cast<UStaticMesh>(
		   StaticLoadObject(
			   UStaticMesh::StaticClass(),
			   nullptr,
			   TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"))
			   );
		mStaticMeshComponent->SetStaticMesh(Mesh);
	}
}

void ATIHHsTriggerInteractableActor::ActionProcessHoldStart_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	if (mTraceMouse == nullptr)
	{
		mTraceMouse = NewObject<UTIHHsProbeTraceMouse>();
	}
	mTraceMouse->TraceBegin(this);
	if (mTraceTarget == nullptr)
	{
		mTraceTarget = NewObject<UTIHHsProbeTraceTarget>();
	}
	mTraceTarget->TraceBegin(mLinkedTriggerComponent->GetOwner()->GetRootComponent(),GetRootComponent());
}

void ATIHHsTriggerInteractableActor::ActionProcessHoldOngoing_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	mTraceMouse->TraceUpdate(actionParamIn);
	mTraceTarget->TraceUpdate();
}

void ATIHHsTriggerInteractableActor::ActionProcessHoldEnd_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	mTraceMouse->TraceEnd();
	mTraceTarget->TraceEnd();
}


// Sets default values for this component's properties
UTIHHsTriggerComponent::UTIHHsTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}




// Called when the game starts
void UTIHHsTriggerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTIHHsTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTIHHsTriggerComponent::ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	UE_LOG(LogTemp, Log, TEXT("UTIHHsTriggerComponent::ActionEventTap called with actionParamIn: %s"), *actionParamIn.ToString());
	if (not mVisibleTriggerGizmo)
	{
		OnVisibleTriggerGizmo();
	}
	else
	{
		OffVisibleTriggerGizmo();
	}
}

void UTIHHsTriggerComponent::VisitCollectingTrigger(FTIHHsTriggerVisitor& triggerVisitor)
{
	
}

void UTIHHsTriggerComponent::RegisterTrigger(const FTIHHsTriggerRegister& triggerRegister)
{
}

int32 UTIHHsTriggerComponent::ProcessGeneration_Implementation(TArray<UActorComponent*>& others)
{
	
	return 0;
}
void UTIHHsTriggerComponent::VisitCollctingInputAction(FTIHHsInputActionVisitor& actionVisitor)
{
	UE_LOG(LogTemp, Log, TEXT("UTIHHsTriggerComponent::VisitCollctingInputAction called"));
	actionVisitor(UTIHHsInteractionUtils::Name_ActionEventTap, this);
	
}


void UTIHHsProbeAttachTest::AttachTest(TScriptInterface<ITIHHsBaseObject> parent,
                                       TScriptInterface<ITIHHsBaseObject> child)
{
	if (parent.GetInterface() == nullptr || child.GetInterface() == nullptr)
	{
		return;
	}
	FTIHHsAttachmentRuleWrapper attachmentRuleWrapper = parent->GetAttachmentRuleWrapper();
	if (parent->IsDeckAcceptable(child))
	{
		child->Attach(parent);
		parent->AttachNotify(child);
	}
}

void UTIHHsProbeAttachTest::Init(FTIHHsProbeInitData initData)
{
	initData.Execute(this);
}

void UTIHHsProbeAttachTest::StartProbe()
{
	UPrimitiveComponent * parentComp = nullptr;
	parentComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool UTIHHsProbeAttachTest::IsProbeRunning()
{
	return Super::IsProbeRunning();
}

void UTIHHsProbeAttachTest::UpdateProbe(float deltaTime)
{
	Super::UpdateProbe(deltaTime);
}

void UTIHHsProbeAttachTest::StopProbe()
{
	Super::StopProbe();
}
