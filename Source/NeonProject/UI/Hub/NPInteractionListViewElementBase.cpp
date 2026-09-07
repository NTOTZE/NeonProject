// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Hub/NPInteractionListViewElementBase.h"
#include "NeonProject.h"
#include "Interaction/NPInteractableActorBase.h"

#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UNPInteractionListViewElementBase::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (!IsValid(InteractionDisplayText)) return;

	CachedInteractionActor = Cast<ANPInteractableActorBase>(ListItemObject);
	if (!CachedInteractionActor.IsValid()) return;
	CachedInteractionActor->OnInteractionFocusChanged.RemoveAll(this);
	CachedInteractionActor->OnInteractionFocusChanged.AddUObject(this, &UNPInteractionListViewElementBase::OnInteractionFocusChanged);

	if(CachedInteractionActor->GetInteractionFocused())
		BP_OnInteractionFocused();

	InteractionDisplayText->SetText(CachedInteractionActor->GetInteractionDisplayText());
}

void UNPInteractionListViewElementBase::NativeOnEntryReleased()
{
	if (CachedInteractionActor.IsValid())
	{

		CachedInteractionActor->OnInteractionFocusChanged.RemoveAll(this);
	}
	IUserObjectListEntry::NativeOnEntryReleased();
}

void UNPInteractionListViewElementBase::OnInteractionFocusChanged(bool bIsFocused)
{
	if (bIsFocused)
		BP_OnInteractionFocused();
	else
		BP_OnInteractionFocusReleased();
}