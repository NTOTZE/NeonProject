// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/NPAnimNotifyState_AILookAtTarget.h"

#include "Character/NPMonsterCharacter.h"

#include "AIController.h"

void UNPAnimNotifyState_AILookAtTarget::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;

    ANPMonsterCharacter* Monster = Cast<ANPMonsterCharacter>(MeshComp->GetOwner());
    if (!Monster) return;

    Monster->LookAtTarget(true);
}

void UNPAnimNotifyState_AILookAtTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    
    ANPMonsterCharacter* Monster = Cast<ANPMonsterCharacter>(MeshComp->GetOwner());
    if (!Monster) return;

    Monster->LookAtTarget(false);
}
