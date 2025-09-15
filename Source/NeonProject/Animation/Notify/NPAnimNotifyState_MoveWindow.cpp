// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/NPAnimNotifyState_MoveWindow.h"
#include "Component/NPCombatComponent.h"

void UNPAnimNotifyState_MoveWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    if (AActor* Owner = MeshComp->GetOwner())
    {
        if (UNPCombatComponent* CC = Owner->FindComponentByClass<UNPCombatComponent>())
        {
            CC->OnMoveWindowBegin();
        }
    }
}

void UNPAnimNotifyState_MoveWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;
    if (AActor* Owner = MeshComp->GetOwner())
    {
        if (UNPCombatComponent* CC = Owner->FindComponentByClass<UNPCombatComponent>())
        {
            CC->OnMoveWindowEnd();
        }
    }
}
