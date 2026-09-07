// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/NPANS_EnableStateFlags.h"
#include "Character/NPCharacterBase.h"

#include "DataType/NPCombatTypes.h"

void UNPANS_EnableStateFlags::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    ANPCharacterBase* Char = Cast<ANPCharacterBase>(MeshComp->GetOwner());
    if (!Char) return;

    if (FlagsToEnable != 0)
        Char->AddState((ENPCharacterState)FlagsToEnable);
}

void UNPANS_EnableStateFlags::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    ANPCharacterBase* Char = Cast<ANPCharacterBase>(MeshComp->GetOwner());
    if (!Char) return;

    if (FlagsToEnable != 0)
        Char->RemoveState((ENPCharacterState)FlagsToEnable);
}

FString UNPANS_EnableStateFlags::GetNotifyName_Implementation() const
{
    
    return GetStateFlagsName((ENPCharacterState)FlagsToEnable);
}
