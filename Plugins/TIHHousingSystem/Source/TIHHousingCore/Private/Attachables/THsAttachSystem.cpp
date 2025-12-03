// Fill out your copyright notice in the Description page of Project Settings.


#include "Attachables/THsAttachSystem.h"

#include "BaseGizmos/GizmoRectangleComponent.h"
#include "BaseGizmos/HitTargets.h"

ATHsAttachGizmoActor::ATHsAttachGizmoActor():
mGizmoRoot(),mGizmoCenterRectPlane()/*,GizmoCenterMeshComponent()*/,mTargetObject()
{
	PrimaryActorTick.bCanEverTick = true;

	mGizmoRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GizmoRoot"));
	RootComponent = mGizmoRoot;
	//mGizmoCenterPlaneComponent->SetupAttachment(mGizmoRoot);
	mGizmoCenterMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GizmoCenterMesh"));
	mGizmoCenterMeshComponent->SetupAttachment(mGizmoRoot);
}

void ATHsAttachGizmoActor::BeginPlay()
{
	Super::BeginPlay();
	if (mGizmoCenterMeshComponent)
	{
		mGizmoCenterMeshComponent->SetVisibility(true);
		if (UStaticMesh* cubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
		{
			mGizmoCenterMeshComponent->SetStaticMesh(cubeMesh);
		}
	}
	if (mGizmoCenterRectPlane == nullptr && false)
	{
		mGizmoRoot->SetWorldTransform(
		FTransform(FRotator(0,45,0),FVector(-100,-100,100),FVector(1.0f,1.0f,1.0f))
		);
		
		mGizmoCenterRectPlane = NewObject<UGizmoRectangleComponent>(this, TEXT("GizmoCenterPlane"));
		mGizmoCenterRectPlane->RegisterComponent();
		mGizmoCenterRectPlane->AttachToComponent(mGizmoRoot,FAttachmentTransformRules::KeepRelativeTransform);
		FVector min, max;
		mGizmoCenterMeshComponent->GetLocalBounds(min,max);
		const FVector size = ( max - min);
		UE_LOG(LogTemp, Warning, TEXT("gimochi size: %s"), *size.ToString());
		mGizmoCenterRectPlane->LengthX = size.X;
		mGizmoCenterRectPlane->LengthY = size.Y;
		//mGizmoCenterRectPlane->OffsetX = -size.X / 2;
		//mGizmoCenterRectPlane->OffsetY = size.Y / 2;
		mGizmoCenterRectPlane->DirectionX = FVector::ForwardVector;
		mGizmoCenterRectPlane->DirectionY = FVector::RightVector;
		mGizmoCenterRectPlane->Color = FLinearColor::Blue;
		mGizmoCenterRectPlane->SegmentFlags =  0x1 | 0x2 | 0x4 | 0x8;
		//mGizmoCenterRectPlane->SegmentFlags =  0x2 | 0x4 | 0x8;
		//mGizmoCenterRectPlane->SegmentFlags = 0x1 | 0x4 | 0x8;
		//mGizmoCenterRectPlane->SegmentFlags = 0x1 | 0x2 | 0x8;
		//mGizmoCenterRectPlane->SegmentFlags = 0x1 | 0x2 | 0x4;
		mGizmoCenterRectPlane->Thickness = 5.0f;
		mGizmoCenterRectPlane->NotifyExternalPropertyUpdates();

		mGizmoCenterRectPlane->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		mGizmoCenterRectPlane->SetCollisionResponseToChannels(ECR_Ignore);
		mGizmoCenterRectPlane->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	}
	
}

void ATHsAttachTestingActor::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = this;
	spawnParameters.Name = TEXT("GizmoActor");
	
	mGizmoActor = GetWorld()->SpawnActor<ATHsAttachGizmoActor>(spawnParameters);
	
	
}

void ATHsAttachTestingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	
}
