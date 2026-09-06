// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Hub/NPInteractionListViewBase.h"
#include "NeonProject.h"
#include "Interaction/NPInteractableActorBase.h"
#include "UI/Hub//NPInteractionListViewElementBase.h"

#include "Components/ListView.h"

void UNPInteractionListViewBase::UpdateItems(const TArray<TWeakObjectPtr<ANPInteractableActorBase>>& interactableActors)
{
	if (!IsValid(InteractionList)) return;

	InteractionList->ClearListItems();

	for (TWeakObjectPtr<ANPInteractableActorBase> Actor : interactableActors)
	{
		if (!Actor.IsValid())
			continue;

		InteractionList->AddItem(Actor.Get());
	}
}

void UNPInteractionListViewBase::UpdateFocusIndex(int32 newIndex)
{
	SetFocusIndex(newIndex);
}

void UNPInteractionListViewBase::SetFocusIndex(int32 idx)
{
	//FocusIdx = idx;

	//const TArray<UObject*> Items = InteractionList->GetListItems();
	//int32 count = 0;
	//for (const UObject* Item : Items)
	//{
	//	UNPInteractionListViewElementBase* element = Cast<UNPInteractionListViewElementBase>(InteractionList->GetEntryWidgetFromItem(Item));
	//	if (element)
	//	{
	//		bool bFocus = false;
	//		if (FocusIdx == count)
	//			bFocus = true;

	//		element->SetInteractionFocused(bFocus);
	//	}
	//	count++;
	//}

	//NP_LOG(NPLog, Warning, TEXT("%d"), FocusIdx);
}

