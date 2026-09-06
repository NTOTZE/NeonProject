// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GameMode/NPGameModeBase.h"
#include "Interface/NPBattleHUDInterface.h"
#include "NPBattleGameMode.generated.h"

class ANPEncounterManager;

UCLASS()
class NEONPROJECT_API ANPBattleGameMode : public ANPGameModeBase
{
	GENERATED_BODY()
	
public:
	ANPBattleGameMode();

	void BeginPlay() override;

private:
	void ApplyTimeDilation(float Dilation, float Duration);

	UFUNCTION()	void OnWaveStarted(int32 CurrentWave, int32 TotalWave);
	UFUNCTION()	void OnWaveCleared(int32 CurrentWave, int32 TotalWave);
	UFUNCTION()	void OnEncounterCleared();

public:
	const ANPEncounterManager* GetEncounterManager() { return EncounterManager; }

private:
	UPROPERTY()
	TObjectPtr<ANPEncounterManager> EncounterManager;
};
