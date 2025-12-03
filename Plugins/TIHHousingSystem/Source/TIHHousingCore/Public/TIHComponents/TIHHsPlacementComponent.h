// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TIHHsCore.h"
#include "UObject/Object.h"
#include "TIHHsPlacementComponent.generated.h"


UCLASS()
class TIHHOUSINGCORE_API UTIHHsPlacementComponent : public UStaticMeshComponent, public ITIHHsBaseObject
	//, public ITIHHsInteractionObject
{
	GENERATED_BODY()
public:
	bool CheckPlacement(USceneComponent* target){return false;}
	

	/*
	 *	OnRegisterdCreate 이거를 여기 상위에서 없애줘야함.
	 *	PostInitializeComponents 이거도 없애줘야함.
	 * 이놈은 측정만 하는거임! 그럼 바닥면을 알아야한다? 노노 그냥 점선면만 알면 된다.그래서 sceneComponent 를 파라미터로 받는거다.
	 * OnGrabModeHover 가 호출되면 액터에서 ScenenComponent 를 가져와서 CheckPlacement에 넣어보고 그걸 메타오브젝트에 기록한다. 즉 상태변화
	 * 오로지 그것만 할거임. 다른거 생각 ㄴㄴ
	 * 근데 이러면 결국 그걸 해주는 곳이 있어야하는데?ㅋㅋㅋㅋ
	 * 그러면 프리올리티를 넣어볼까? 사실 이거도 정말 어렵긴한데, 어떤 동작 혹은 특정 컴포넌트 뒤라고 생각을 하자. 실행의 관계성임
	 * 근데 겹치는 부분을 줄이고 싶은데, 그럴려면 그냥 시불 cmd 상속받아서 만들던가.
	 * 직교성을 지키고 싶어서 cmd 를 만든거임. 그걸 직교성이라  불러야하는지도 지금앎. 그럼 차라리 
	 * 
	 */

	
	
};
