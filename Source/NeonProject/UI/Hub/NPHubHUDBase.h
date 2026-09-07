// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPHubHUDBase.generated.h"

class UNPDialogueDataAsset;
class ANPInteractableActorBase;

UCLASS()
class NEONPROJECT_API UNPHubHUDBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetDialogueData(const UNPDialogueDataAsset* DialogueDA);
	void ShowDialogue();
	void HideDialogue();

	void UpdateInteractionItems(const TArray<TWeakObjectPtr<ANPInteractableActorBase>>& interactableActors);
	void UpdateInteractionFocus(int32 focusIndex);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPDialoguePanelBase> DialoguePanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPInteractionListViewBase> InteractionList;
};
