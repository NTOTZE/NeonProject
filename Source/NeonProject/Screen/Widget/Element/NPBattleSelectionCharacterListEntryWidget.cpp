// Fill out your copyright notice in the Description page of Project Settings.

#include "Screen/Widget/Element/NPBattleSelectionCharacterListEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InputCoreTypes.h"
#include "NeonProject.h"
#include "Screen/Widget/NPBattleSelectionWidget.h"

void UNPBattleSelectionCharacterListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (IsValid(CachedCharacterItem))
		CachedCharacterItem->OnSelectionStateChanged.RemoveAll(this);

	CachedCharacterItem = Cast<UNPBattleSelectionCharacterItem>(ListItemObject);
	if (!IsValid(CachedCharacterItem))
	{
		NP_LOG(NPLog, Warning, TEXT("캐릭터 목록 항목 데이터가 유효하지 않습니다."));
		return;
	}

	if (IsValid(CharacterNameText))
		CharacterNameText->SetText(CachedCharacterItem->CharacterName);
	if (IsValid(CharacterThumbnailImage))
		CharacterThumbnailImage->SetBrushFromSoftTexture(CachedCharacterItem->ThumbnailTexture, true);
	CachedCharacterItem->OnSelectionStateChanged.AddUObject(
		this, &UNPBattleSelectionCharacterListEntryWidget::RefreshSelectionVisual);

	UpdateSelectionVisual();
	BP_OnCharacterItemSet(CachedCharacterItem);
}

void UNPBattleSelectionCharacterListEntryWidget::NativeOnEntryReleased()
{
	if (IsValid(CachedCharacterItem))
		CachedCharacterItem->OnSelectionStateChanged.RemoveAll(this);
	CachedCharacterItem = nullptr;

	IUserObjectListEntry::NativeOnEntryReleased();
}

FReply UNPBattleSelectionCharacterListEntryWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && IsValid(CachedCharacterItem))
	{
		CachedCharacterItem->OnClicked.Broadcast(CachedCharacterItem);
		UpdateSelectionVisual(true);
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UNPBattleSelectionCharacterListEntryWidget::NativeOnMouseEnter(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	UpdateSelectionVisual(true);
}

void UNPBattleSelectionCharacterListEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	UpdateSelectionVisual();
}

void UNPBattleSelectionCharacterListEntryWidget::UpdateSelectionVisual(bool bHovered)
{
	if (!IsValid(CachedCharacterItem))
		return;

	const bool bUseHoverHighlight = bHovered && !CachedCharacterItem->bIsSelectionLocked;
	const float Brightness = CachedCharacterItem->bIsSelected ? 1.f : (bUseHoverHighlight ? 0.55f : 0.2f);
	const FLinearColor TintColor(Brightness, Brightness, Brightness, 1.f);
	if (IsValid(CharacterThumbnailImage))
		CharacterThumbnailImage->SetColorAndOpacity(TintColor);
	if (IsValid(CharacterNameText))
		CharacterNameText->SetColorAndOpacity(FSlateColor(TintColor));
	if (IsValid(SelectionOrderText))
	{
		SelectionOrderText->SetText(FText::AsNumber(CachedCharacterItem->SelectionOrder));
		SelectionOrderText->SetVisibility(
			CachedCharacterItem->bIsSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UNPBattleSelectionCharacterListEntryWidget::RefreshSelectionVisual()
{
	UpdateSelectionVisual(IsHovered());
}
