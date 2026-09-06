// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen/Widget/Element/NPBattleSelectionListEntryWidget.h"
#include "Screen/Widget/NPBattleSelectionWidget.h"
#include "UI/Battle/NPBattleInfoSummaryWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "NeonProject.h"

void UNPBattleSelectionListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CachedBattleItem = Cast<UNPBattleSelectionItem>(ListItemObject);
	if (!IsValid(CachedBattleItem))
	{
		NP_LOG(NPLog, Warning, TEXT("전투 목록 항목 데이터가 유효하지 않습니다."));
		return;
	}

	if (IsValid(StageNameText))
		StageNameText->SetText(CachedBattleItem->StageName);
	if (IsValid(DifficultyText))
		DifficultyText->SetText(FText::Format(NSLOCTEXT("BattleSelection", "Difficulty", "난이도 {0}"), CachedBattleItem->Difficulty));
	if (IsValid(StageDescriptionText))
		StageDescriptionText->SetText(CachedBattleItem->Description);

	if (WidgetTree)
	{
		TArray<UWidget*> ChildWidgets;
		WidgetTree->GetAllWidgets(ChildWidgets);
		for (UWidget* ChildWidget : ChildWidgets)
		{
			if (UNPBattleInfoSummaryWidget* BattleInfoSummary = Cast<UNPBattleInfoSummaryWidget>(ChildWidget))
			{
				BattleInfoSummary->InitBattleInfo(CachedBattleItem->BattleStageId);
				break;
			}
		}
	}

	BP_OnBattleStageItemSet(CachedBattleItem);
}

void UNPBattleSelectionListEntryWidget::NativeOnEntryReleased()
{
	CachedBattleItem = nullptr;

	IUserObjectListEntry::NativeOnEntryReleased();
}
