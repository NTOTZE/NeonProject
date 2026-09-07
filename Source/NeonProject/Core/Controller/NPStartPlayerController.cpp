// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Controller/NPStartPlayerController.h"
#include "DataType/NPStageData.h"
#include "GameFlow/Handler/NPEnterStageHandler.h"
#include "GameFlow/NPGameFlowCommand.h"
#include "GameFlow/NPGameFlowSubsystem.h"
#include "Loading/NPLoadingSettings.h"
#include "UI/Start/NPStartWidget.h"

ANPStartPlayerController::ANPStartPlayerController()
{
	PartyCharacterIds = {
		TEXT("Player0000")
	};
}

void ANPStartPlayerController::BeginPlay()
{
	Super::BeginPlay();

	const TSubclassOf<UNPStartWidget> StartWidgetClass = UNPLoadingSettings::GetChecked()->GetStartWidgetClass();
	if (StartWidgetClass)
	{
		StartWidget = CreateWidget<UNPStartWidget>(this, StartWidgetClass);
	}
	else
	{
		ensureMsgf(false, TEXT("Loading Settings에 StartWidgetClass가 설정되어 있지 않습니다."));
	}

	if (StartWidget)
	{
		StartWidget->OnStartGameClicked.AddDynamic(this, &ThisClass::HandleStartGameClicked);
		StartWidget->AddToViewport();
	}

	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	if (StartWidget)
		InputMode.SetWidgetToFocus(StartWidget->TakeWidget());
	SetInputMode(InputMode);
}

void ANPStartPlayerController::HandleStartGameClicked()
{
	StartGame();
}

void ANPStartPlayerController::StartGame()
{
	if (bIsStartingGame)
		return;

	bIsStartingGame = true;
	if (StartWidget)
		StartWidget->SetIsEnabled(false);
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;

	const FNPGameFlowCommand EnterHubCommand = FNPGameFlowCommand::Make(
		FNPGameFlowCommandOptions::Make(false, true, 1.f),
		FNPEnterStageHandlerData::Make(ENPStageType::Hub, HubStageId, PartyCharacterIds));

	if (!UNPGameFlowSubsystem::GetChecked(this)->RequestExecuteCommand(EnterHubCommand))
	{
		UE_LOG(LogTemp, Warning, TEXT("허브 스테이지 진입 요청에 실패했습니다."));
		bIsStartingGame = false;
		if (StartWidget)
			StartWidget->SetIsEnabled(true);
		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}
}
