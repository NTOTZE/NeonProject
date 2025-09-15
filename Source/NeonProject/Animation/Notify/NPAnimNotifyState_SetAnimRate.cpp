// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/NPAnimNotifyState_SetAnimRate.h"

void UNPAnimNotifyState_SetAnimRate::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;

    MeshComp->GlobalAnimRateScale = RateScale;
}

void UNPAnimNotifyState_SetAnimRate::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;

    MeshComp->GlobalAnimRateScale = RateScaleOnEnd;
}
