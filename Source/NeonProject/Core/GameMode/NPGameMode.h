// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API ANPGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ANPGameMode();

	void BeginPlay() override;

	void SetPlayCharacterHPBarRatio(float Ratio);
	void SetMemberHPBarRatio(int32 Idx, float Ratio);
	void SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture);
	void SetMemberOpacity(int32 Idx, float Opacity);

private:
	void ApplyTimeDilation(float Dilation, float Duration);

	UFUNCTION()	void OnWaveStarted(int32 CurrentWave, int32 TotalWave);
	UFUNCTION()	void OnWaveCleared(int32 CurrentWave, int32 TotalWave);
	UFUNCTION()	void OnEncounterCleared();


private:
	TSubclassOf<class UUserWidget>	MainHudClass;
	TObjectPtr<class UNPMainHUDBase> MainHUD;
	TObjectPtr<class ANPEncounterManager> EncounterManager;
};
