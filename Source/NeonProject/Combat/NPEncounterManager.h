// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataAsset/NPEncounterData.h"
#include "NPEncounterManager.generated.h"

class ANPSpawnPoint;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPWaveStarted, int32, CurrentWave, int32, TotalWave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPWaveCleared, int32, CurrentWave, int32, TotalWave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNPEncounterCleared);

UCLASS()
class NEONPROJECT_API ANPEncounterManager : public AActor
{
	GENERATED_BODY()
	
public:
	ANPEncounterManager();

protected:
	virtual void BeginPlay() override;

public:
	int32 GetCurrentWave() { return LastStartedWaveIndex + 1; }
	int32 GetTotalWave() { return TotalWaves; }

public:
	UFUNCTION(BlueprintCallable, Category = "NP|Encounter")
	void StartEncounter();

protected:
	// 내부 함수
	void GatherSpawnPoints();
	void ScheduleWaves();
	void StartWave(int32 WaveIndex);
	void MaybeMarkWaveCleared(int32 WaveIndex);
	void TryFinishEncounter();

	// 스폰 관련
	void ScheduleSpawnUnit(int32 WaveIndex, const FNPSpawnUnit& Unit);
	void DoSpawnOne(int32 WaveIndex, const FNPSpawnUnit UnitCopy);

	// 사망처리
	UFUNCTION()
	void OnSpawnedEnemyDestroyed(AActor* DeadOrDestroyedActor);

public:
	UPROPERTY(BlueprintAssignable, Category = "NP|Encounter")
	FOnNPWaveStarted OnWaveStarted;

	UPROPERTY(BlueprintAssignable, Category = "NP|Encounter")
	FOnNPWaveCleared OnWaveCleared;

	UPROPERTY(BlueprintAssignable, Category = "NP|Encounter")
	FOnNPEncounterCleared OnEncounterCleared;

protected:
	UPROPERTY(EditAnywhere, Category = "NP|Encounter")
	TObjectPtr<UNPEncounterData> EncounterData;

	// false일경우 BP에서 StartEncounter함수 호출해주어야 함
	UPROPERTY(EditAnywhere, Category = "NP|Encounter")
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, Category = "NP|Encounter")
	TSubclassOf<ANPSpawnPoint> SpawnPointClass;

protected:
	TMap<FName, TArray<TWeakObjectPtr<ANPSpawnPoint>>> SpawnPointsByTag;

	// 웨이브별 카운트
	TArray<int32> WaveAlive;          // 현재 살아있는 수
	TArray<int32> WavePendingToSpawn; // 아직 스폰 대기 중(타이머 큐)에 있는 수
	TArray<bool>  WaveStarted;
	TArray<bool>  WaveCleared;

	int32 TotalWaves = 0;
	int32 LastStartedWaveIndex = INDEX_NONE;

};
