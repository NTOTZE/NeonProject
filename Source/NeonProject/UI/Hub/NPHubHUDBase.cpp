// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Hub/NPHubHUDBase.h"
#include "UI/Hub/NPDialoguePanelBase.h"
#include "UI/Hub/NPInteractionListViewBase.h"
#include "DataAsset/NPDialogueDataAsset.h"

void UNPHubHUDBase::SetDialogueData(const UNPDialogueDataAsset* DialogueDA)
{
	if (!IsValid(DialoguePanel)) return;

	DialoguePanel->SetLines(DialogueDA->Lines);
}

void UNPHubHUDBase::ShowDialogue()
{
	if (!IsValid(DialoguePanel)) return;

	DialoguePanel->ShowDialogue();
}

void UNPHubHUDBase::HideDialogue()
{
	if (!IsValid(DialoguePanel)) return;

	DialoguePanel->HideDialogue();
}

void UNPHubHUDBase::UpdateInteractionItems(const TArray<TWeakObjectPtr<ANPInteractableActorBase>>& interactableActors)
{
	InteractionList->UpdateItems(interactableActors);
}

void UNPHubHUDBase::UpdateInteractionFocus(int32 focusIndex)
{
	InteractionList->UpdateFocusIndex(focusIndex);
}
