// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NPScreenManagerComponent.h"
#include "NeonProject.h"
#include "GameFlow/NPGameFlowSubsystem.h"
#include "Screen/Widget/NPScreenWidgetBase.h"
#include "Screen/Widget/NPScreenFadeWidgetBase.h"
#include "Screen/Widget/NPLoadingScreenWidgetBase.h"
#include "Screen/Widget/NPBattleSelectionWidget.h"


// Sets default values for this component's properties
UNPScreenManagerComponent::UNPScreenManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	
	/*ConstructorHelpers::FClassFinder<UNPScreenWidgetBase> SettingsWidgetFinder(TEXT(""));
	if (SettingsWidgetFinder.Succeeded())
	{
		ScreenWidgetClasses[ENPScreenType::Settings] = SettingsWidgetFinder.Class;
	}*/

	ConstructorHelpers::FClassFinder<UNPScreenFadeWidgetBase> ScreenFadeWidgetFinder(
		TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/NeonProject/Blueprint/UI/FullScreen/WBP_ScreenFadeWidget.WBP_ScreenFadeWidget_C'"));
	if (ScreenFadeWidgetFinder.Succeeded())
	{
		ScreenFadeWidgetClass = ScreenFadeWidgetFinder.Class;
	}

	ConstructorHelpers::FClassFinder<UNPLoadingScreenWidgetBase> LoadingScreenWidgetFinder(
		TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/NeonProject/Blueprint/UI/FullScreen/WBP_LoadingScreenWidget.WBP_LoadingScreenWidget_C'"));
	if (LoadingScreenWidgetFinder.Succeeded())
	{
		LoadingScreenWidgetClass = LoadingScreenWidgetFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UNPBattleSelectionWidget> BattleSelectionWidgetFinder(
		TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/NeonProject/Blueprint/UI/FullScreen/BattleEntry/WBP_BattleSelectionWidget.WBP_BattleSelectionWidget_C'"));
	if (BattleSelectionWidgetFinder.Succeeded())
	{
		ScreenWidgetClasses.Add(ENPScreenType::BattleSelection, BattleSelectionWidgetFinder.Class);
	}
}


// Called when the game starts
void UNPScreenManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ShowAndPlayLoadingScreen();

	// FadeIn 완료 후 조작 시작같은, Delegate 필요
}


// Called every frame
void UNPScreenManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UNPScreenManagerComponent::OpenScreen(ENPScreenType screenType)
{
	TSubclassOf<UNPScreenWidgetBase>* ScreenWidgetClass = ScreenWidgetClasses.Find(screenType);
	check(ScreenWidgetClass);

	UNPScreenWidgetBase* ScreenWidget = CreateWidget<UNPScreenWidgetBase>(GetPlayerControllerChecked(), *ScreenWidgetClass);
	check(ScreenWidget);

	ScreenWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	//InputMode.SetWidgetToFocus(ScreenWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	GetPlayerControllerChecked()->SetInputMode(InputMode);

	ApplyScreenOpenOptions(ScreenWidget->GetScreenOptions());
}

void UNPScreenManagerComponent::CloseScreen(UNPScreenWidgetBase* screenWidget)
{
	check(IsValid(screenWidget));

	screenWidget->RemoveFromParent();

	// screenWidget의 종료 요청 수행

	//스크린이 없을경우
	FInputModeGameOnly InputMode;
	GetPlayerControllerChecked()->SetInputMode(InputMode);
	ApplyScreenOpenOptions(FNPScreenOptions::MakeCloseOptions());
}

bool UNPScreenManagerComponent::ExcuteCommandAfterFade(const FNPEventCommand& eventCommand, ENPFadeAnimationType fadeAnimationType, float fadeDuration, float fadeStartDelay)
{


	return false;
}

void UNPScreenManagerComponent::ApplyScreenOpenOptions(const FNPScreenOptions& openRequest)
{
	APlayerController* PlayerController = GetPlayerControllerChecked();

	PlayerController->SetShowMouseCursor(openRequest.bShowMouseCursor);
	//PlayerController->SetPause(openRequest.bPauseGame);
	//PlayerController->SetPause(true);
	//openRequest.bCloseOnEscape
}

bool UNPScreenManagerComponent::ShowAndPlayLoadingScreen()
{
	check(LoadingScreenWidgetClass);
	UNPLoadingScreenWidgetBase* LoadingScreenWidget = CreateWidget<UNPLoadingScreenWidgetBase>(GetPlayerControllerChecked(), LoadingScreenWidgetClass);

	check(LoadingScreenWidget);
	LoadingScreenWidget->OnLoadingScreenFinished.BindUObject(this, &UNPScreenManagerComponent::OnLoadingScreenFinished);

	if (!LoadingScreenWidget->PlayLoading())
		return false;
	//



	//
	return true;
}

void UNPScreenManagerComponent::OnLoadingScreenFinished()
{
	ShowAndPlayFadeScreen(ENPFadeAnimationType::FadeIn, GameStartFadeInDuration, GameStartFadeInDelay);
}

bool UNPScreenManagerComponent::ShowAndPlayFadeScreen(ENPFadeAnimationType fadeAnimationType, float fadeDuration, float fadeStartDelay)
{
	if (!IsValid(ScreenFadeWidget))
	{
		check(ScreenFadeWidgetClass);
		ScreenFadeWidget = CreateWidget<UNPScreenFadeWidgetBase>(GetPlayerControllerChecked(), ScreenFadeWidgetClass);
	}

	bool bSucceeded = false;
	ScreenFadeWidget->OnFadeAnimationFinished.BindUObject(this, &UNPScreenManagerComponent::OnFadeScreenFinished);

	if (ScreenFadeWidget->PlayFade(fadeAnimationType, fadeDuration, fadeStartDelay))
		bSucceeded = true;

	if (bSucceeded)
		return true;

	ScreenFadeWidget->OnFadeAnimationFinished.Unbind();

	return false;
}

void UNPScreenManagerComponent::OnFadeScreenFinished(ENPFadeAnimationType CompletedFadeType)
{
	check(ScreenFadeWidget);
	ScreenFadeWidget->OnFadeAnimationFinished.Unbind();

	//if (UNPGameFlowSubsystem* EventManager = GetOwner()->GetGameInstance()->GetSubsystem<UNPGameFlowSubsystem>())
	//{
	//	EventManager->ExecuteCommand(PendingEventCommand);
	//}
	//PendingEventCommand = FNPEventCommand();
}

APlayerController* UNPScreenManagerComponent::GetPlayerController()
{
	if (!IsValid(CachedController))
	{
		CachedController = Cast<APlayerController>(GetOwner());
	}
		
	return CachedController;
}

APlayerController* UNPScreenManagerComponent::GetPlayerControllerChecked()
{
	check(GetPlayerController());

	return CachedController;
}

