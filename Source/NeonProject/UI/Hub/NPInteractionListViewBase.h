// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPInteractionListViewBase.generated.h"

class ANPInteractableActorBase;

UCLASS()
class NEONPROJECT_API UNPInteractionListViewBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateItems(const TArray<TWeakObjectPtr<class ANPInteractableActorBase>>& interactableActors);
	void UpdateFocusIndex(int32 newIndex);

private:
	void SetFocusIndex(int32 idx);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UListView> InteractionList;

	int32 FocusIdx = -1;
};
