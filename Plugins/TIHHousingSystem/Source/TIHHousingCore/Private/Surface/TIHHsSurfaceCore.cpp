// Fill out your copyright notice in the Description page of Project Settings.


#include "Surface/TIHHsSurfaceCore.h"


void UTIHHsSurfaceProcRegistrySubSystem::RegisterProcessor(FName name, UTIHHsSurfaceRuleProcBase* processor)
{
	if (processor == nullptr)
	{
		/*
		 * TODO: 나중에 datatable에 이름으로 검색해서 class를 가져오자. 지금은 그냥 return
		 */
		return;
	}
	if (mProcessorTable.Contains(name))
	{
		UTIHHsSurfaceRuleProcBase* existingProcessor = mProcessorTable[name];
		existingProcessor->RemoveFromRoot();
		existingProcessor->UnRegisterProcessorCallBack();
	}
	mProcessorTable.FindOrAdd(name) = processor;
}

void UTIHHsSurfaceProcRegistrySubSystem::UnregisterProcessor(FName name)
{
	if (mProcessorTable.Contains(name))
	{
		UTIHHsSurfaceRuleProcBase* existingProcessor = mProcessorTable[name];
		mProcessorTable.Remove(name);
		existingProcessor->UnRegisterProcessorCallBack();
		existingProcessor->RemoveFromRoot();
	}
}

void UTIHHsSurfaceProcRegistrySubSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

FTIHHsSurfaceRuleProcResult UTIHHsSurfaceBase::ExecutePipeline(const FTIHHsSurfaceRuleProcResult& input)
{
	FTIHHsSurfaceRuleProcResult output = input;

	//	todo: UTIHHsSurfaceProcessorRegistry 이거 subsytem으로 변경하고 인터페이스를 변경.
	//UTIHHsSurfaceProcessorRegistry* registry = UTIHHsSurfaceProcessorRegistry::GetInstance();
	if (UTIHHsSurfaceProcRegistrySubSystem* registry = GetWorld()->GetSubsystem<UTIHHsSurfaceProcRegistrySubSystem>())
	{
		for (const FName& processorName : mProcessorNames)
		{
			UTIHHsSurfaceRuleProcBase* processor = registry->GetProcessor(processorName);
			if (processor && processor->VerifyRequestedContext(GetSurfaceContextTable()))
			{
				if (output.Result < 0)
				{
					break;
				}
				output = processor->Process(output, const_cast<UTIHHsSurfaceBase*>(this));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTIHHsSurfaceBase::ExecutePipeline: ProcessorRegistry is not initialized!"));
	}
	return output;
}

/*
*	
template<> const FVector3f FVector3f::ZeroVector(0, 0, 0);
template<> const FVector3f FVector3f::OneVector(1, 1, 1);
template<> const FVector3f FVector3f::UpVector(0, 0, 1);
template<> const FVector3f FVector3f::DownVector(0, 0, -1);
template<> const FVector3f FVector3f::ForwardVector(1, 0, 0);
template<> const FVector3f FVector3f::BackwardVector(-1, 0, 0);
template<> const FVector3f FVector3f::RightVector(0, 1, 0);
template<> const FVector3f FVector3f::LeftVector(0, -1, 0);
template<> const FVector3f FVector3f::XAxisVector(1, 0, 0);
template<> const FVector3f FVector3f::YAxisVector(0, 1, 0);
template<> const FVector3f FVector3f::ZAxisVector(0, 0, 1);
 *
 * 
 */
FTIHHsSurfaceRuleProcResult UTIHHsSurfaceGridSnapProc::Process(const FTIHHsSurfaceRuleProcResult& inResult, UTIHHsSurfaceBase* surface) const
{
	float gridSize = mRefGridSize;
	float rotationSnapAngle = mRefRotationSnapAngle;
	const FName& processorName = GetProcessorName();
	//	grid-check
	if (surface && surface->HasSurfaceContext(processorName) )
	{
		const FTIHHsSurfaceProcGridSnapContext& snapContext = surface->GetSurfaceContext(processorName).Get<FTIHHsSurfaceProcGridSnapContext>();
		gridSize = snapContext.GridSize;
		rotationSnapAngle = snapContext.RotationSnapAngle;
	}
	
	FVector snappedLocation = inResult.Transform.GetLocation();
	snappedLocation.X = FMath::GridSnap(snappedLocation.X, gridSize);
	snappedLocation.Y = FMath::GridSnap(snappedLocation.Y, gridSize);
	snappedLocation.Z = FMath::GridSnap(snappedLocation.Z, gridSize);

	FRotator snappedRotation = inResult.Transform.GetRotation().Rotator();
	snappedRotation.Yaw = FMath::GridSnap(snappedRotation.Yaw, rotationSnapAngle);
	// snappedRotation.Pitch = FMath::GridSnap(snappedRotation.Pitch, mRefRotationSnapAngle);
	// snappedRotation.Roll = FMath::GridSnap(snappedRotation.Roll, mRefRotationSnapAngle);
	
	UE_LOG(LogTemp, Warning, TEXT("UTIHHsSurfaceGridSnapProcessor::Process: %s"), *snappedLocation.ToString());
	return FTransform(snappedRotation, snappedLocation, inResult.Transform.GetScale3D());
}

// FTIHHsSurfaceProcResult UTIHHsSurfaceAttachTestProc::Process(const FTIHHsSurfaceProcResult& inResult,
// 	UTIHHsSurfaceBase* surface) const
// {
// 	FTIHHsSurfaceProcResult output = inResult;
// 	if (surface == nullptr)
// 	{
// 		output.Result = -1;
// 		return output;
// 	}
// 	if (surface->GetSurfaceOwner() == nullptr)
// 	{
// 		output.Result = -2;
// 		return output;
// 	}
// 	if (ITIHHsBaseObject* baseObj = Cast<ITIHHsBaseObject>(surface->GetSurfaceOwner().GetObject()))
// 	{
// 		if (baseObj->DetectPlacementTarget(mHitResult))
// 		{
// 			if (mHitResult.bBlockingHit)
// 			{
// 				output.Result = 1; // 성공적으로 충돌 감지
// 				output.Transform = FTransform(mHitResult.ImpactNormal.Rotation(), mHitResult.ImpactPoint);
// 				output.HitResult = mHitResult;
// 			}
// 			else
// 			{
// 				output.Result = 0; // 충돌 없음
// 			}
// 		}
// 	}
// 	
// 	
// 	return Super::Process(inResult, surface);
// }

void UTIHHsPlaneSurface::DrawDebug(UWorld* world, float duration) const
{
	if (world)
	{
		const FVector planeNormal(mPlane.X,mPlane.Y,mPlane.Z);
		const FVector normal = planeNormal.GetSafeNormal();
		if (normal.IsNearlyZero())
		{
			return;
		}
		const FVector origin = -mPlane.W * normal; // 평면 방정식에서 D는 -W로 표현됨
		const float size = 200.0f; // 최소한의 평면의 크기
		DrawDebugSolidPlane(
			world,
			FPlane(origin,normal),
			origin,
			size,
			FColor::Cyan,
			false,
			duration
			);
		FVector TangentX, TangentY;
		normal.FindBestAxisVectors(TangentX, TangentY); // n에 수직인 직교기저
		const int32 Grid = 4;
		const float Step = size / Grid;
		for (int32 i = -Grid; i <= Grid; ++i)
		{
			const FVector A0 = origin + TangentX * (i * Step) - TangentY * size;
			const FVector A1 = origin + TangentX * (i * Step) + TangentY * size;
			const FVector B0 = origin + TangentY * (i * Step) - TangentX * size;
			const FVector B1 = origin + TangentY * (i * Step) + TangentX * size;

			DrawDebugLine(world, A0, A1, FColor::Blue,   false, duration, 0, 0.5f);
			DrawDebugLine(world, B0, B1, FColor::Blue,   false, duration, 0, 0.5f);
		}
	}
}

bool UTIHHsPlaneSurface::Intersect(const FRay& ray, FHitResult& outHitResult) const
{
	//	이걸 다르게 해야할 필요가 있을까? 
	bool result = true;
	/* 수학적 배경:
		 * 평면 방정식: Ax + By + Cz + D = 0
		 * 레이 방정식: P = Origin + t * Direction
		 * 
		 * 교차점 구하기:
		 * t = -(D + dot(Normal, Origin)) / dot(Normal, Direction)
		 * 
		 * t > 0: 레이 앞쪽에 교차
		 * t < 0: 레이 뒤쪽 (무시)
		 * denominator = 0: 레이와 평면이 평행
		 */
	float denominator = FVector::DotProduct(mPlane.GetNormal(),ray.Direction);

	if (FMath::Abs((denominator)) < KINDA_SMALL_NUMBER)
	{
		// 평면과 레이가 평행한 경우
		result = false;
	}
	else
	{
		float t = -mPlane.PlaneDot(ray.Origin) / denominator;

		if (t < 0.0f)
		{
			result = false;
		}
		else
		{
			outHitResult.Location = ray.Origin + (ray.Direction * t);
			outHitResult.ImpactPoint = outHitResult.Location;
			outHitResult.Normal = outHitResult.Location.GetSafeNormal();
			outHitResult.ImpactNormal = outHitResult.Normal;
			outHitResult.Distance = t;
			outHitResult.bBlockingHit = true;
		}
	}
	return result;
}

FVector UTIHHsPlaneSurface::GetClosestPoint(const FVector& point) const
{
	/* 수학적 배경:
		 * 점에서 평면까지 거리: d = dot(Normal, Point) + D
		 * 투영점 = Point - d * Normal
		 */
	float distance = mPlane.PlaneDot(point);
	return point - mPlane.GetNormal() * distance;
	
}

bool UTIHHsPlaneSurface::SupportsSnapping() const
{
	return Super::SupportsSnapping();
}

float UTIHHsPlaneSurface::GetSnapSize() const
{
	return Super::GetSnapSize();
}

FVector UTIHHsPlaneSurface::GetNormalAtPoint(const FVector& point) const
{
	return mPlane.GetNormal();
}

UTIHHsPlaneSurface* UTIHHsSurfaceUtility::CreateAxisPlan(EAxis::Type ueAxis, const FVector& origin, UWorld* worldContext,UObject* outer)
{
	FVector normal;
	switch (ueAxis) {
	case EAxis::None:
		break;
	case EAxis::X:	//	yz
		normal = FVector::ForwardVector;
		break;
	case EAxis::Y:	//	xz
		normal = FVector::RightVector;
		break;
	case EAxis::Z:	//	xy
		normal = FVector::UpVector;
		break;
	}
	auto* surface = NewObject<UTIHHsPlaneSurface>(outer);
	surface->SetWorldContext(worldContext);
	surface->SetPlaneFromPointNormal(origin, normal);
	return surface;
}

UTIHHsPlaneSurface* UTIHHsSurfaceUtility::CreateCameraPlan(
	const FVector& cameraLocation,
	const FVector& cameraForward,
	const FVector& targetPoint,UWorld* worldContext,
	UObject* outer)
{
	/*
	* 수학적 배경:
	* 카메라에서 TargetPoint까지 거리를 유지하는 평면
	* Normal = CameraForward (카메라가 보는 방향)
	* Point = TargetPoint (평면이 지나는 점)
	*/
	
	float distanceFromCamera = FVector::Dist(cameraLocation, targetPoint);
	FVector planePoint = cameraLocation + cameraForward * distanceFromCamera;
	
	auto* surface = NewObject<UTIHHsPlaneSurface>(outer);
	surface->SetWorldContext(worldContext);
	surface->SetPlaneFromPointNormal(planePoint, cameraForward);
	return surface;
}

UTIHHsPlaneSurface* UTIHHsSurfaceUtility::CreatePlaneFrom3Points(const FVector& pointA, const FVector& pointB,
	const FVector& pointC,UWorld* worldContext, UObject* outer)
{
	/* 수학적 배경:
	  * 두 벡터의 외적으로 법선 구하기
	  * V1 = PointB - PointA
	  * V2 = PointC - PointA
	  * Normal = V1 × V2 (normalized)
	  */
	FVector vec1 = pointB - pointA;
	FVector vec2 = pointC - pointA;
	FVector normal = FVector::CrossProduct(vec1, vec2).GetSafeNormal();
	
	if (normal.IsNearlyZero())
	{
		return nullptr;
	}

	auto* surface = NewObject<UTIHHsPlaneSurface>(outer);
	surface->SetWorldContext(worldContext);
	surface->SetPlaneFromPointNormal(pointA, normal);
	return surface;
}

UTIHHsPlaneSurface* UTIHHsSurfaceUtility::CreatePlaneFromHit(const FHitResult& hit,UWorld* worldContext, UObject* outer)
{
	if (!hit.bBlockingHit)
		return nullptr;
        
	auto* surface = NewObject<UTIHHsPlaneSurface>(outer);
	surface->SetWorldContext(worldContext);
	surface->SetPlaneFromPointNormal(hit.Location, hit.ImpactNormal);
	return surface;
}

UTIHHsPlaneSurface* UTIHHsSurfaceUtility::CreateLocalAxisPlane(const FTransform& transform, EAxis::Type localAxis,UWorld* worldContext,
	UObject* outer)
{
	/* 수학적 배경:
	  * 로컬 축을 월드 공간으로 변환
	  * LocalX = Transform.GetUnitAxis(EAxis::X)
	  */
	if (localAxis == EAxis::None)
	{
		return nullptr;
	}
	FVector worldNormal = transform.GetUnitAxis(localAxis);

	auto* surface = NewObject<UTIHHsPlaneSurface>(outer);
	surface->SetWorldContext(worldContext);
	surface->SetPlaneFromPointNormal(transform.GetLocation(), worldNormal);
	return surface;
}

