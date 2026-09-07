// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen/NPScreenSubsystem.h"
#include "Screen/Widget/NPScreenWidgetBase.h"
#include "Screen/Widget/NPScreenFadeWidgetBase.h"
#include "Screen/Widget/NPBattleSelectionWidget.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"

UNPScreenSubsystem::UNPScreenSubsystem()
{
	ConstructorHelpers::FClassFinder<UNPScreenFadeWidgetBase> ScreenFadeWidgetFinder(
		TEXT("/Game/NeonProject/Blueprint/UI/FullScreen/WBP_ScreenFadeWidget"));
	if (ScreenFadeWidgetFinder.Succeeded())
	{
		ScreenFadeWidgetClass = ScreenFadeWidgetFinder.Class;
	}

	ConstructorHelpers::FClassFinder<UNPBattleSelectionWidget> BattleSelectionWidgetFinder(
		TEXT("/Game/NeonProject/Blueprint/UI/FullScreen/BattleEntry/WBP_BattleSelectionWidget"));
	if (BattleSelectionWidgetFinder.Succeeded())
	{
		ScreenWidgetClasses.Add(ENPScreenType::BattleSelection, BattleSelectionWidgetFinder.Class);
	}
}

void UNPScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	WorldCleanupHandle = FWorldDelegates::OnWorldCleanup.AddUObject(this, &ThisClass::HandleWorldCleanup);
}

void UNPScreenSubsystem::Deinitialize()
{
	FWorldDelegates::OnWorldCleanup.Remove(WorldCleanupHandle);
	WorldCleanupHandle.Reset();
	ClearWidgets();
	Super::Deinitialize();
}

bool UNPScreenSubsystem::OpenScreen(ENPScreenType screenType)
{
	if (bPlayingFade)
		return false;

	APlayerController* PlayerController = GetPlayerController();
	const TSubclassOf<UNPScreenWidgetBase>* ScreenWidgetClass = ScreenWidgetClasses.Find(screenType);
	if (!IsValid(PlayerController) || !ScreenWidgetClass || !*ScreenWidgetClass)
		return false;

	UNPScreenWidgetBase* ScreenWidget = CreateWidget<UNPScreenWidgetBase>(PlayerController, *ScreenWidgetClass);
	if (!IsValid(ScreenWidget))
		return false;

	if (IsValid(ActiveScreenWidget))
		CloseScreen(ActiveScreenWidget);

	ActiveScreenWidget = ScreenWidget;
	ScreenWidget->AddToViewport(ScreenWidget->GetZOrder());

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ScreenWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(ScreenWidget->GetScreenOptions().bShowMouseCursor);
	return true;
}

bool UNPScreenSubsystem::CloseScreen(UNPScreenWidgetBase* screenWidget)
{
	if (!IsValid(screenWidget) || screenWidget != ActiveScreenWidget)
		return false;

	ActiveScreenWidget = nullptr;
	screenWidget->RemoveFromParent();

	APlayerController* PlayerController = GetPlayerController();
	if (IsValid(PlayerController) && screenWidget->GetOwningPlayer() == PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
	}
	return true;
}

bool UNPScreenSubsystem::ShowBlackScreen()
{
	return PlayFadeOut(0.f, 0.f);
}

bool UNPScreenSubsystem::PlayFadeIn(float fadeDuration, float fadeStartDelay)
{
	return PlayFade(ENPFadeAnimationType::FadeIn, fadeDuration, fadeStartDelay);
}

bool UNPScreenSubsystem::PlayFadeOut(float fadeDuration, float fadeStartDelay)
{
	return PlayFade(ENPFadeAnimationType::FadeOut, fadeDuration, fadeStartDelay);
}

bool UNPScreenSubsystem::PlayFade(ENPFadeAnimationType fadeAnimationType, float fadeDuration, float fadeStartDelay)
{
	if (bPlayingFade)
		return false;

	APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController) || !ScreenFadeWidgetClass)
		return false;

	if (IsValid(ScreenFadeWidget) && ScreenFadeWidget->GetOwningPlayer() != PlayerController)
	{
		ScreenFadeWidget->OnFadeAnimationFinished.Unbind();
		ScreenFadeWidget->RemoveFromParent();
		ScreenFadeWidget = nullptr;
	}

	if (!IsValid(ScreenFadeWidget))
		ScreenFadeWidget = CreateWidget<UNPScreenFadeWidgetBase>(PlayerController, ScreenFadeWidgetClass);
	if (!IsValid(ScreenFadeWidget))
		return false;

	bPlayingFade = true;
	ScreenFadeWidget->OnFadeAnimationFinished.BindUObject(this, &ThisClass::HandleFadeFinished);
	if (ScreenFadeWidget->PlayFade(fadeAnimationType, fadeDuration, fadeStartDelay))
		return true;

	ScreenFadeWidget->OnFadeAnimationFinished.Unbind();
	bPlayingFade = false;
	return false;
}

void UNPScreenSubsystem::HandleFadeFinished(ENPFadeAnimationType CompletedFadeType)
{
	ScreenFadeWidget->OnFadeAnimationFinished.Unbind();
	bPlayingFade = false;

	// 콜백에서 다음 Fade를 요청할 수 있도록 기존 요청부터 정리한다.
	FNPScreenFadeFinishedDelegate FinishedDelegate = OnFadeFinished;
	OnFadeFinished.Unbind();
	FinishedDelegate.ExecuteIfBound(CompletedFadeType);
}

void UNPScreenSubsystem::HandleWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	if (World && World->GetGameInstance() == GetGameInstance())
		ClearWidgets();
}

void UNPScreenSubsystem::ClearWidgets()
{
	OnFadeFinished.Unbind();
	bPlayingFade = false;
	if (IsValid(ActiveScreenWidget))
		ActiveScreenWidget->RemoveFromParent();
	ActiveScreenWidget = nullptr;

	if (IsValid(ScreenFadeWidget))
	{
		ScreenFadeWidget->OnFadeAnimationFinished.Unbind();
		ScreenFadeWidget->StopAllAnimations();
		ScreenFadeWidget->RemoveFromParent();
	}
	ScreenFadeWidget = nullptr;
}

APlayerController* UNPScreenSubsystem::GetPlayerController() const
{
	return UGameplayStatics::GetPlayerController(this, 0);
}
