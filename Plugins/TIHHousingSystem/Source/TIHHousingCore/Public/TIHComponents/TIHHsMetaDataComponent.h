// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TIHHsCore.h"
#include "Components/SceneComponent.h"
#include "TIHHsMetaDataComponent.generated.h"
/*
 *	상태저장
 *	순수하게 읽기만 하는 컴포넌트
 *	이거 토대로 state를 만들어야할거 같은데
 *	
 * 
 */

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TIHHOUSINGCORE_API UTIHHsMetaDataComponent : public USceneComponent, public ITIHHsBaseObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTIHHsMetaDataComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;

	static void AnalysisBaseObject(AActor* target);

protected:
	TSet<FName> mMetaTags; // if(contain(test)) 면 그 태그는 되어있다라는 말임
};
