// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "NPBattleSelectionCharacterListEntryWidget.generated.h"

UCLASS()
class NEONPROJECT_API UNPBattleSelectionCharacterListEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	void RefreshSelectionVisual();

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "NP|Battle")
	void BP_OnCharacterItemSet(class UNPBattleSelectionCharacterItem* CharacterItem);

private:
	void UpdateSelectionVisual(bool bHovered = false);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CharacterNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CharacterThumbnailImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> SelectionOrderText;

	UPROPERTY(Transient)
	TObjectPtr<class UNPBattleSelectionCharacterItem> CachedCharacterItem;
};
