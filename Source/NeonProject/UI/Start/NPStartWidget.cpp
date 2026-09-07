// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Start/NPStartWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Styling/SlateTypes.h"

void UNPStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bCanStartGame = false;
	SetIsFocusable(true);

	if (BeginAnimation)
	{
		UnbindAllFromAnimationFinished(BeginAnimation);

		FWidgetAnimationDynamicEvent BeginFinishedDelegate;
		BeginFinishedDelegate.BindDynamic(this, &UNPStartWidget::HandleBeginAnimationFinished);
		BindToAnimationFinished(BeginAnimation, BeginFinishedDelegate);
		PlayAnimation(BeginAnimation);
	}
	else
	{
		HandleBeginAnimationFinished();
	}
}

void UNPStartWidget::HandleStartButtonClicked()
{
	if (bCanStartGame)
	{
		OnStartGameClicked.Broadcast();
	}
}

void UNPStartWidget::HandleBeginAnimationFinished()
{
	if (BeginAnimation)
	{
		UnbindAllFromAnimationFinished(BeginAnimation);
	}

	bCanStartGame = true;
	SetKeyboardFocus();
}

FReply UNPStartWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bCanStartGame)
	{
		OnStartGameClicked.Broadcast();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
