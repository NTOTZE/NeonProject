// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NPAnimNotifyState_SetAnimRate.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPAnimNotifyState_SetAnimRate : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "NP", meta = (AllowPrivateAccess = "true"))
	float RateScale = 1.f;

	// 노티파이스테이트가 종료된 이후 적용 될 Rate. 원래의 Rate값을 입력해야함
	UPROPERTY(EditAnywhere, Category = "NP", meta = (AllowPrivateAccess = "true"))
	float RateScaleOnEnd = 1.f;
};
