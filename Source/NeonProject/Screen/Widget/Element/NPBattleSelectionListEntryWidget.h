// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "NPBattleSelectionListEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPBattleSelectionListEntryWidget : public UUserWidget, public IUserObjectListEntry
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
	void BP_OnBattleStageItemSet(class UNPBattleSelectionItem* BattleItem);

private:
	void UpdateSelectionVisual(bool bHovered = false);

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> StageNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> DifficultyText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> StageDescriptionText;

	UPROPERTY(Transient)
	TObjectPtr<class UNPBattleSelectionItem> CachedBattleItem;
};
