// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/NPBattleGameMode.h"
#include "NeonProject.h"
#include "Character/Player/NPPlayerCharacterBase.h"
#include "Core/Controller/NPBattlePlayerController.h"
#include "Interface/NPBattleHUDInterface.h"
#include "Loading/NPStageSessionSubsystem.h"

#include "Combat/NPEncounterManager.h"
#include "Component/NPPartyComponent.h"
#include "DataType/NPStageData.h"
#include "GameFlow/Command/Factory/NPEnterStageCommandFactory.h"
#include "GameFlow/NPGameFlowSubsystem.h"
#include "Kismet/GameplayStatics.h"

ANPBattleGameMode::ANPBattleGameMode()
{
	PlayerControllerClass = ANPBattlePlayerController::StaticClass();
}

void ANPBattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	UNPStageSessionSubsystem* StageSession = UNPStageSessionSubsystem::GetChecked(this);
	StageSession->OnSessionReady.AddUObject(this, &ThisClass::InitializeEncounterManager);
	if (StageSession->IsSessionInitialized())
		InitializeEncounterManager();
}

void ANPBattleGameMode::InitializeEncounterManager()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPEncounterManager::StaticClass(), Found);
	if (Found.IsValidIndex(0))
	{
		if (ANPEncounterManager* manager = Cast<ANPEncounterManager>(Found[0]))
			EncounterManager = manager;
		EncounterManager->OnWaveStarted.AddDynamic(this, &ANPBattleGameMode::OnWaveStarted);
		EncounterManager->OnWaveCleared.AddDynamic(this, &ANPBattleGameMode::OnWaveCleared);
		EncounterManager->OnEncounterCleared.AddDynamic(this, &ANPBattleGameMode::OnEncounterCleared);
		EncounterManager->OnEncounterFailed.AddDynamic(this, &ANPBattleGameMode::OnEncounterFailed);

		if (ANPBattlePlayerController* PlayerController = Cast<ANPBattlePlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			PlayerController->GetPartyComponent()->OnPartyMemberDead.AddUObject(EncounterManager, &ANPEncounterManager::HandlePartyMemberDead);
		}
	}
}

void ANPBattleGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	check(World);
	World->GetTimerManager().ClearTimer(TimeDilationHandler);

	Super::EndPlay(EndPlayReason);
}


void ANPBattleGameMode::ApplyTimeDilation(float Dilation, float Duration)
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(TimeDilationHandler);

	const float AppliedDilation = FMath::Clamp(Dilation, 0.01f, 1.0f);
	const float AppliedDuration = FMath::Clamp(Duration, 0.1f, 10.f) * AppliedDilation;

	UGameplayStatics::SetGlobalTimeDilation(World, AppliedDilation);


	World->GetTimerManager().SetTimer(TimeDilationHandler,
		[World]()
		{
			if (!World) return;
			UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
		}
	, AppliedDuration, false);
}


void ANPBattleGameMode::OnWaveStarted(int32 CurrentWave, int32 TotalWave)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (INPBattleHUDInterface* BattleHUDInterface = Cast<INPBattleHUDInterface>(GetWorld()->GetFirstPlayerController()))
	{
		FString str = FString::Printf(TEXT("WAVE START %d / %d"), CurrentWave, TotalWave);
		BattleHUDInterface->SetWaveText(str);
	}
}

void ANPBattleGameMode::OnWaveCleared(int32 CurrentWave, int32 TotalWave)
{
	NP_LOG(NPLog, Warning, TEXT("WAVE CLEAR %d / %d"), CurrentWave, TotalWave);
	
	if (CurrentWave >= TotalWave) return;
		
	ApplyTimeDilation(0.1f, 2.f);
}

void ANPBattleGameMode::OnEncounterCleared()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (INPBattleHUDInterface* BattleHUDInterface = Cast<INPBattleHUDInterface>(GetWorld()->GetFirstPlayerController()))
	{
		FString str = FString::Printf(TEXT("CLEAR"));
		BattleHUDInterface->SetWaveText(str);
		ApplyTimeDilation(0.1f, 3.f);
	}

	const TOptional<FNPGameFlowCommand> ReturnCommand = NPEnterStageCommandFactory::MakeReturnToPreviousStage(
		this, FNPGameFlowCommandOptions::Make(false, true, 2.f));
	if (!ReturnCommand.IsSet() || !UNPGameFlowSubsystem::GetChecked(this)->RequestExecuteCommand(ReturnCommand.GetValue()))
	{
		NP_LOG(NPLog, Warning, TEXT("클리어 후 이전 스테이지 복귀 요청에 실패했습니다."));
	}

}

void ANPBattleGameMode::OnEncounterFailed()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (INPBattleHUDInterface* BattleHUDInterface = Cast<INPBattleHUDInterface>(GetWorld()->GetFirstPlayerController()))
	{
		BattleHUDInterface->SetWaveText(TEXT("FAIL"));
	}

	const TOptional<FNPGameFlowCommand> ReturnCommand = NPEnterStageCommandFactory::MakeReturnToPreviousStage(
		this, FNPGameFlowCommandOptions::Make(false, true, 2.f));
	if (!ReturnCommand.IsSet() || !UNPGameFlowSubsystem::GetChecked(this)->RequestExecuteCommand(ReturnCommand.GetValue()))
	{
		NP_LOG(NPLog, Warning, TEXT("실패 후 이전 스테이지 복귀 요청에 실패했습니다."));
	}
}
