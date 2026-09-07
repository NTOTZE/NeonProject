// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/NPBattleGameMode.h"
#include "NeonProject.h"
#include "Character/Player/NPPlayerCharacterBase.h"
#include "Core/Controller/NPBattlePlayerController.h"
#include "Interface/NPBattleHUDInterface.h"

#include "Combat/NPEncounterManager.h"
#include "Kismet/GameplayStatics.h"

ANPBattleGameMode::ANPBattleGameMode()
{
	PlayerControllerClass = ANPBattlePlayerController::StaticClass();
}

void ANPBattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPEncounterManager::StaticClass(), Found);
	if (Found.IsValidIndex(0))
	{
		if(ANPEncounterManager* manager = Cast<ANPEncounterManager>(Found[0]))
		EncounterManager = manager;
		EncounterManager->OnWaveStarted.AddDynamic(this, &ANPBattleGameMode::OnWaveStarted);
		EncounterManager->OnWaveCleared.AddDynamic(this, &ANPBattleGameMode::OnWaveCleared);
		EncounterManager->OnEncounterCleared.AddDynamic(this, &ANPBattleGameMode::OnEncounterCleared);
	}
}


void ANPBattleGameMode::ApplyTimeDilation(float Dilation, float Duration)
{
	UWorld* World = GetWorld();
	if (!World) return;

	const float AppliedDilation = FMath::Clamp(Dilation, 0.01f, 1.0f);
	const float AppliedDuration = FMath::Clamp(Duration, 0.1f, 10.f) * AppliedDilation;

	UGameplayStatics::SetGlobalTimeDilation(World, AppliedDilation);

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle,
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

		//UWorld* World = GetWorld();
		//if (!World) return;
		//FTimerHandle TimerHandle;
		//World->GetTimerManager().SetTimer(TimerHandle,
		//	[this]()
		//	{
		//		this->BattleHUD->EnableBlackScreen(true, 2.f);
		//	}
		//, 3.f, false);
	}

}
