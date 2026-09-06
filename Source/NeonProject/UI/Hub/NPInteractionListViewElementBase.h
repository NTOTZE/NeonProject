// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "NPInteractionListViewElementBase.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPInteractionListViewElementBase : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnInteractionFocused();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnInteractionFocusReleased();

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;
private:
	void OnInteractionFocusChanged(bool bIsFocused);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> InteractionDisplayText;

	TWeakObjectPtr<class ANPInteractableActorBase> CachedInteractionActor;
};
