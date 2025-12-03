// Fill out your copyright notice in the Description page of Project Settings.


#include "THsWorldStuff.h"

#include "ActionSystem/THsEnhancedActionSystem.h"
#include "Camera/CameraComponent.h"


void ATHsDefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
}

ATHsDefaultPlayerController::ATHsDefaultPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ATHsDefaultPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//GetWorld()->SpawnActor(ATHsWorldStuff::StaticClass(), &FTransform::Identity);
	RootComponent = NewObject<USceneComponent>(this);
	RootComponent->RegisterComponent();
	RootComponent->SetWorldRotation(FRotator(-45, 0, 0));
	RootComponent->SetWorldLocation(FVector(-450,0,330));
	
	mCamera = NewObject<UCameraComponent>(this);
	mCamera->RegisterComponent();
	mCamera->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{
		if (UTHsEnhancedActionSystem* actionSystem = GetWorld()->GetSubsystem<UTHsEnhancedActionSystem>())
		{
			FDelegateHandle result = actionSystem->BindInputKeyActionPointByName
			<ETHsActionFunctionCategory::EProcess>(ETHsActionFunctionType::EProcess_Pressing,TEXT("W"),[
				weakThis = TWeakObjectPtr<ATHsDefaultPawn>(this)
			](UTHsActionPoint* actionPoint,float deltaTime)
			{
				if (weakThis.IsValid())
				{
					USceneComponent* moveComponent = weakThis->GetRootComponent();
						
							moveComponent->AddWorldOffset(	moveComponent->GetForwardVector() * 2.0f);
				}
			});

			actionSystem->BindInputKeyProcessByName(ETHsActionFunctionType::EProcess_Pressing,TEXT("A"),
				[weakThis = TWeakObjectPtr<ATHsDefaultPawn>(this)]
				(UTHsActionPoint* actionPoint,float deltaTime)
			{
				if (weakThis.IsValid())
				{
					USceneComponent* moveComponent = weakThis->GetRootComponent();
					moveComponent->AddWorldOffset(	moveComponent->GetRightVector() * -2.0f);
				}
			});

			actionSystem->BindInputKeyProcessByName(ETHsActionFunctionType::EProcess_Pressing,TEXT("S"),
				[weakThis = TWeakObjectPtr<ATHsDefaultPawn>(this)]
				(UTHsActionPoint* actionPoint,float deltaTime)
			{
				if (weakThis.IsValid())
				{
					USceneComponent* moveComponent = weakThis->GetRootComponent();
					moveComponent->AddWorldOffset(	moveComponent->GetForwardVector() * -2.0f);
				}
			});

			actionSystem->BindInputKeyProcessByName(ETHsActionFunctionType::EProcess_Pressing,TEXT("D"),
				[weakThis = TWeakObjectPtr<ATHsDefaultPawn>(this)]
				(UTHsActionPoint* actionPoint,float deltaTime)
			{
				if (weakThis.IsValid())
				{
					USceneComponent* moveComponent = weakThis->GetRootComponent();
					moveComponent->AddWorldOffset(	moveComponent->GetRightVector() * 2.0f);
				}
			});
			actionSystem->BindInputKeyProcessByName(ETHsActionFunctionType::EProcess_Pressing,TEXT("Q"),
				[weakThis = TWeakObjectPtr<ATHsDefaultPawn>(this)]
				(UTHsActionPoint* actionPoint,float deltaTime)
			{
				if (weakThis.IsValid())
				{
					USceneComponent* moveComponent = weakThis->GetRootComponent();
					moveComponent->AddWorldOffset(	moveComponent->GetUpVector() * -2.0f);
				}
			});
			
			actionSystem->BindInputKeyProcessByName(ETHsActionFunctionType::EProcess_Pressing,TEXT("E"),
				[weakThis = TWeakObjectPtr<ATHsDefaultPawn>(this)]
				(UTHsActionPoint* actionPoint,float deltaTime)
			{
				if (weakThis.IsValid())
				{
					USceneComponent* moveComponent = weakThis->GetRootComponent();
					moveComponent->AddWorldOffset(	moveComponent->GetUpVector() * 2.0f);
				}
			});
			UE_LOG(LogTemp, Warning, TEXT("ATHsDefaultPawn::BeginPlay()"));
		}
	},1.5f,false);	//	이거 나중에 프레임워크단에서 다른것들이 셋팅되고 나서 해줘야함.
	
}

ATHsDefaultGameMode::ATHsDefaultGameMode()
{
	PlayerControllerClass = ATHsDefaultPlayerController::StaticClass();
	GameStateClass = ATHsDefaultGameState::StaticClass();
	PlayerStateClass = ATHsDefaultPlayerState::StaticClass();
	DefaultPawnClass = ATHsDefaultPawn::StaticClass();
}

// Sets default values
ATHsWorldStuff::ATHsWorldStuff()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATHsWorldStuff::BeginPlay()
{
	Super::BeginPlay();

	FInputDeviceState inputState;


	//	여기 begin play 라 CreateSubObject 로 하면 안됨
	mRootComponent = NewObject<USceneComponent>(this);
	RootComponent = mRootComponent;
	mRootComponent->RegisterComponent();
	
	mStaticMeshComponent = NewObject<UStaticMeshComponent>(this);
	mStaticMeshComponent->RegisterComponent();
	mStaticMeshComponent->AttachToComponent(mRootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	mStaticMeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'")));
	mStaticMeshComponent->SetRelativeScale3D(FVector(10.f,10.f,1.f));
}

// Called every frame
void ATHsWorldStuff::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATHsWorldStuff::ActionEventTap_Deprecated(const FTIHHsActionParamIn& actionParamIn)
{
	if (UTHsActionMouseClick* mouseClick = Cast<UTHsActionMouseClick>(actionParamIn.CallerObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("mouseClick Gimochi"));
	}
}

