// Fill out your copyright notice in the Description page of Project Settings.


#include "Builds/THsFloor.h"

#include "ActionSystem/THsEnhancedActionSystem.h"
#include "BaseGizmos/AxisPositionGizmo.h"
#include "BaseGizmos/GizmoCircleComponent.h"
#include "BaseGizmos/GizmoLineHandleComponent.h"
#include "BaseGizmos/GizmoRectangleComponent.h"

//
// UInteractiveGizmo* UTHsFloorGizmoBuilder::BuildGizmo(const FToolBuilderState& SceneState) const
// {
// 	check(SceneState.GizmoManager);
// 	check(SceneState.World);
// 	UInteractiveGizmo* herit = Super::BuildGizmo(SceneState);
// 	if (herit == nullptr || not IsValid(herit))
// 	{
// 		herit = nullptr;
// 		TSubclassOf<ATHsFloorGizmoActor> useCls = mViewActorClass ? mViewActorClass : ATHsFloorGizmoActor::StaticClass();
// 		FActorSpawnParameters spawnParams = {};
// 		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
// 		spawnParams.ObjectFlags |= RF_Transient;     
// 		ATHsFloorGizmoActor* gizmoViewActor = SceneState.World->SpawnActor<ATHsFloorGizmoActor>(useCls, FTransform::Identity, spawnParams);
// 		if (not ensure(gizmoViewActor))
// 		{
// 			return nullptr;
// 		}
//
// 		UTHsFloorGizmo* gizmo = NewObject<UTHsFloorGizmo>(SceneState.GizmoManager);
// 		if (not ensure(gizmo))
// 		{
// 			if (gizmoViewActor)
// 			{
// 				gizmoViewActor->Destroy();
// 			}
// 			return nullptr;
// 		}
// 		
// 		gizmo->InitFloorGizmo(
// 			mTempDependency.Get(),
// 			mOwner.Get(),
// 			gizmoViewActor);
// 	}
// 	return herit;
// }
//
// FInputRayHit UTHsFloorGizmo::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
// {
// 	return FInputRayHit();
// }
//
// void UTHsFloorGizmo::OnClickPress(const FInputDeviceRay& PressPos)
// {
// }
//
// void UTHsFloorGizmo::OnClickDrag(const FInputDeviceRay& DragPos)
// {
// }
//
// void UTHsFloorGizmo::OnClickRelease(const FInputDeviceRay& ReleasePos)
// {
// }
//
// void UTHsFloorGizmo::OnTerminateDragSequence()
// {
// }
//
// FInputRayHit UTHsFloorGizmo::BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos)
// {
// 	return FInputRayHit();
// }
//
// void UTHsFloorGizmo::OnBeginHover(const FInputDeviceRay& DevicePos)
// {
// }
//
// bool UTHsFloorGizmo::OnUpdateHover(const FInputDeviceRay& DevicePos)
// {
// 	return false;
// }
//
// void UTHsFloorGizmo::OnEndHover()
// {
// }
//
// void UTHsFloorGizmo::Setup()
// {
// 	Super::Setup();
//
// 	UTHsSettingSubsystem* settingSystem = TIHUtils::GetSettingSubsystem();
// 	bool isValid = false;
// 	if (UTHsDependencyBase* di = settingSystem->TryGetGizmoDependency())
// 	{
// 		if (di->IsValidDependency())
// 		{
// 			isValid = true;
// 			/*
// 			 *	빌더가 spawn 부터 해야 온전히 됨.
// 			 * 
// 			 */
// 		}
// 	}
// }
// void UTHsFloorGizmo::Shutdown()
// {
// 	Super::Shutdown();
// 	
// 	
// 	
// }
//
// ATHsFloor::ATHsFloor()
// {
// }
//
// void ATHsFloor::BeginPlay()
// {
// 	Super::BeginPlay();
// }
//
// void ATHsFloor::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }
//
// /*
//  *	----------------------------------------------------------------
//  *	Testing Only
//  *
//  * 
//  */
//
// bool UTHsCircleGizmoSimple::RayPlaneIntersect(
// 	const FVector& R0, const FVector& Rd,
//     const FVector& P0, const FVector& N,
//     float& OutT, FVector& OutPoint)
// {
// 	const float denom = FVector::DotProduct(N, Rd);
// 	if (FMath::IsNearlyZero(denom, 1e-6f))
// 	{
// 		return false; // 평행
// 	}
// 	OutT = FVector::DotProduct(N, (P0 - R0)) / denom;
// 	if (OutT < 0.f)
// 	{
// 		return false; // 뒤쪽
// 	}
// 	OutPoint = R0 + OutT * Rd;
// 	return true;
// }
//
// float UTHsCircleGizmoSimple::SafePixelToWorld(const FVector& At, TFunction<float(const FVector&)> Fn)
// {
// 	if (Fn)
// 	{
// 		const float S = Fn(At);
// 		return (S > KINDA_SMALL_NUMBER) ? S : 1.f;
// 	}
// 	return 1.f;
// }
//
// bool UTHsCircleGizmoSimple::RaycastRing(const FVector& RayOrigin,
//                                         const FVector& RayDir,
//                                         TFunction<float(const FVector&)> PixelToWorldAt,
//                                         FTHsCircleGizmoHit& OutHit,
//                                         UWorld* WorldForOcclusion,
//                                         ECollisionChannel Channel) const
// {
// 	OutHit = FTHsCircleGizmoHit{};
//
// 	// 1) 레이-평면 교차
// 	float t = 0.f;
// 	FVector P;
// 	if (!RayPlaneIntersect(RayOrigin, RayDir, Center, Normal, t, P))
// 	{
// 		return false;
// 	}
//
// 	// 2) 원 중심에서의 반지름 거리
// 	const FVector CP = P - Center;
// 	// 평면 성분만 보정(수치오차 제거)
// 	const FVector CPInPlane = CP - FVector::DotProduct(CP, Normal) * Normal;
// 	const float r = CPInPlane.Length();
//
// 	// 3) 링 두께(band) 판정
// 	float halfWidthW = RingHalfWidthWorld;
// 	if (bUseScreenThickness)
// 	{
// 		const float pixelToWorld = SafePixelToWorld(P, PixelToWorldAt); // cm per pixel
// 		halfWidthW = FMath::Max(0.5f, RingHalfWidthPx * pixelToWorld);
// 	}
//
// 	const float band = FMath::Abs(r - Radius);
// 	if (band > halfWidthW)
// 	{
// 		return false;
// 	}
//
// 	// 4) (옵션) 월드 가림 체크
// 	if (bEnableWorldOcclusion && WorldForOcclusion)
// 	{
// 		FHitResult Hit;
// 		const FVector End = P;
// 		FCollisionQueryParams Params(SCENE_QUERY_STAT(THsCircleGizmoSimple), /*bTraceComplex=*/true);
// 		Params.bReturnPhysicalMaterial = false;
//
// 		// 기즈모 자신을 무시하고 싶다면 여기서 AddIgnoredActor/Component 해주면 됨.
//
// 		if (WorldForOcclusion->LineTraceSingleByChannel(Hit, RayOrigin, End, Channel, Params))
// 		{
// 			// 레이 경로 중간에 뭔가 먼저 맞았음 → 링 가려짐
// 			const float hitT = (Hit.ImpactPoint - RayOrigin).Size() / RayDir.Size(); // RayDir는 보통 정규화
// 			if (hitT + KINDA_SMALL_NUMBER < t)
// 			{
// 				return false;
// 			}
// 		}
// 	}
//
// 	// 5) 추가 정보 채우기
// 	OutHit.bHit = true;
// 	OutHit.RayT = t;
// 	OutHit.WorldPoint = P;
//
// 	// 각도(원 위 좌표계 하나 만들어서 투영)
// 	// 임의의 접선축 구성
// 	FVector U = FVector::CrossProduct(Normal, FVector::UpVector);
// 	if (U.SquaredLength() < 1e-3f)
// 	{
// 		U = FVector::CrossProduct(Normal, FVector::RightVector);
// 	}
// 	U.Normalize();
// 	const FVector V = FVector::CrossProduct(Normal, U);
// 	const FVector PV = CPInPlane.GetSafeNormal();
//
// 	const float u = FVector::DotProduct(PV, U);
// 	const float v = FVector::DotProduct(PV, V);
// 	OutHit.AngleRadians = FMath::Atan2(v, u); // [-pi, pi]
//
// 	return true;
// }