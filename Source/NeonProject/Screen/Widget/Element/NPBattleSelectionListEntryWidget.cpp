// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen/Widget/Element/NPBattleSelectionListEntryWidget.h"
#include "Screen/Widget/NPBattleSelectionWidget.h"
#include "UI/Battle/NPBattleInfoSummaryWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "InputCoreTypes.h"
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

	UpdateSelectionVisual();
	BP_OnBattleStageItemSet(CachedBattleItem);
}

void UNPBattleSelectionListEntryWidget::NativeOnEntryReleased()
{
	CachedBattleItem = nullptr;

	IUserObjectListEntry::NativeOnEntryReleased();
}

FReply UNPBattleSelectionListEntryWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && IsValid(CachedBattleItem))
	{
		CachedBattleItem->OnClicked.Broadcast(CachedBattleItem);
		UpdateSelectionVisual(true);
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UNPBattleSelectionListEntryWidget::NativeOnMouseEnter(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	UpdateSelectionVisual(true);
}

void UNPBattleSelectionListEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	UpdateSelectionVisual();
}

void UNPBattleSelectionListEntryWidget::UpdateSelectionVisual(bool bHovered)
{
	if (!IsValid(CachedBattleItem))
		return;

	const float Brightness = CachedBattleItem->bIsSelected ? 1.f : (bHovered ? 0.55f : 0.2f);
	SetRenderOpacity(Brightness);
}

void UNPBattleSelectionListEntryWidget::RefreshSelectionVisual()
{
	UpdateSelectionVisual(IsHovered());
}
