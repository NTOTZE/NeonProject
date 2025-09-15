// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/AnimNotifyState_NPComboWindow.h"
#include "Component/NPCombatComponent.h"

void UAnimNotifyState_NPComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    if (AActor* Owner = MeshComp->GetOwner())
    {
        if (UNPCombatComponent* CC = Owner->FindComponentByClass<UNPCombatComponent>())
        {
            CC->OnComboWindowBegin();
        }
    }
}

void UAnimNotifyState_NPComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    if (AActor* Owner = MeshComp->GetOwner())
    {
        if (UNPCombatComponent* CC = Owner->FindComponentByClass<UNPCombatComponent>())
        {
            CC->OnComboWindowEnd();
        }
    }
}
