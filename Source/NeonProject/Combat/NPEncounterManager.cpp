// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/NPEncounterManager.h"
#include "NeonProject.h"

#include "Character/NPMonsterCharacter.h"
#include "Combat/NPSpawnPoint.h"
#include "Kismet/GameplayStatics.h" 

ANPEncounterManager::ANPEncounterManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SpawnPointClass = ANPSpawnPoint::StaticClass();
}

void ANPEncounterManager::BeginPlay()
{
	Super::BeginPlay();
	
	GatherSpawnPoints();

	if (EncounterData)
	{
		TotalWaves = EncounterData->Waves.Num();
		WaveAlive.Init(0, TotalWaves);
		WavePendingToSpawn.Init(0, TotalWaves);
		WaveStarted.Init(false, TotalWaves);
		WaveCleared.Init(false, TotalWaves);
	}

	if (bAutoStart)
	{
		StartEncounter();
	}
}

void ANPEncounterManager::StartEncounter()
{
	if (!EncounterData || TotalWaves == 0)
	{
		NP_LOG(NPLog, Error, TEXT("EncounterData 설정 안됨."));
		return;
	}
	ScheduleWaves();
}

void ANPEncounterManager::GatherSpawnPoints()
{
	SpawnPointsByTag.Reset();

	if (!SpawnPointClass) return;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnPointClass, Found);

	for (AActor* Actor : Found)
	{
		ANPSpawnPoint* SP = Cast<ANPSpawnPoint>(Actor);
		if (!SP) continue;

		for (const FName& Tag : SP->Tags)
		{
			SpawnPointsByTag.FindOrAdd(Tag).Add(SP);
		}
	}
}

void ANPEncounterManager::ScheduleWaves()
{
	for (int32 i = 0; i < TotalWaves; ++i)
	{
		const FNPEncounterWave& Wave = EncounterData->Waves[i];

		if (Wave.bWaitForPrevWaveClear && i > 0)
		{
			// 클리어 될때 StartWave가 이미 호출 된상태
			continue;
		}
		else
		{
			// 시작 시간
			FTimerHandle Handle;
			GetWorldTimerManager().SetTimer(
				Handle,
				FTimerDelegate::CreateUObject(this, &ANPEncounterManager::StartWave, i),
				FMath::Max(0.f, Wave.StartTime),
				false
			);
		}
	}
}

void ANPEncounterManager::StartWave(int32 WaveIndex)
{
	if (!EncounterData || !EncounterData->Waves.IsValidIndex(WaveIndex)) return;
	if (WaveStarted[WaveIndex]) return;

	WaveStarted[WaveIndex] = true;
	LastStartedWaveIndex = FMath::Max(LastStartedWaveIndex, WaveIndex);
	OnWaveStarted.Broadcast(WaveIndex + 1, TotalWaves);

	const FNPEncounterWave& Wave = EncounterData->Waves[WaveIndex];

	// 각 유닛 묶음마다 스케줄링
	for (const FNPSpawnUnit& Unit : Wave.Units)
	{
		// Count만큼 Pending 누적(스폰될 예정 수)
		WavePendingToSpawn[WaveIndex] += FMath::Max(0, Unit.Count);
		ScheduleSpawnUnit(WaveIndex, Unit);
	}

	// 유닛이 하나도 없으면 즉시 클리어 처리
	if (WavePendingToSpawn[WaveIndex] == 0 && WaveAlive[WaveIndex] == 0)
	{
		MaybeMarkWaveCleared(WaveIndex);
	}
}

void ANPEncounterManager::MaybeMarkWaveCleared(int32 WaveIndex)
{
	if (!EncounterData || !EncounterData->Waves.IsValidIndex(WaveIndex)) return;
	if (WaveCleared[WaveIndex]) return;

	if (WaveAlive[WaveIndex] == 0 && WavePendingToSpawn[WaveIndex] == 0 && WaveStarted[WaveIndex])
	{
		WaveCleared[WaveIndex] = true;
		OnWaveCleared.Broadcast(WaveIndex+1, TotalWaves);

		// 다음 웨이브가 "이전 웨이브 클리어 대기" 모드였다면 여기서 시작
		const int32 NextIndex = WaveIndex + 1;
		if (EncounterData->Waves.IsValidIndex(NextIndex))
		{
			const FNPEncounterWave& NextWave = EncounterData->Waves[NextIndex];
			if (NextWave.bWaitForPrevWaveClear)
			{
				if (NextWave.StartTime > 0.f)
				{
					FTimerHandle Handle;
					GetWorldTimerManager().SetTimer(
						Handle,
						FTimerDelegate::CreateUObject(this, &ANPEncounterManager::StartWave, NextIndex),
						FMath::Max(0.f, NextWave.StartTime),
						false
					);
				}
				else
				{
					StartWave(NextIndex);
				}
			}
		}

		TryFinishEncounter();
	}
}

void ANPEncounterManager::TryFinishEncounter()
{
	for (int32 i = 0; i < TotalWaves; ++i)
	{
		if (!WaveCleared[i]) return;
	}
	OnEncounterCleared.Broadcast();
}

void ANPEncounterManager::ScheduleSpawnUnit(int32 WaveIndex, const FNPSpawnUnit& Unit)
{
	for (int32 i = 0; i < Unit.Count; ++i)
	{
		const float Delay = Unit.FirstDelay + (i * Unit.Interval);

		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(
			Handle,
			FTimerDelegate::CreateUObject(this, &ANPEncounterManager::DoSpawnOne, WaveIndex, Unit),
			Delay,
			false
		);
	}
}

void ANPEncounterManager::DoSpawnOne(int32 WaveIndex, const FNPSpawnUnit UnitCopy)
{
	if (!EncounterData || !EncounterData->Waves.IsValidIndex(WaveIndex)) return;

	// 스폰 포인트를 태그로 찾기
	TArray<TWeakObjectPtr<ANPSpawnPoint>>* PointsPtr = SpawnPointsByTag.Find(UnitCopy.SpawnPointTag);
	if (!PointsPtr || PointsPtr->Num() == 0)
	{
		NP_LOG(NPLog, Error, TEXT("해당 태그 없음 '%s'"), *UnitCopy.SpawnPointTag.ToString());
		WavePendingToSpawn[WaveIndex] = FMath::Max(0, WavePendingToSpawn[WaveIndex] - 1);
		MaybeMarkWaveCleared(WaveIndex);
		return;
	}

	static TMap<FName, int32> TagCursor;
	int32& Cursor = TagCursor.FindOrAdd(UnitCopy.SpawnPointTag);
	ANPSpawnPoint* Chosen = nullptr;

	for (int32 Attempt = 0; Attempt < PointsPtr->Num(); ++Attempt)
	{
		ANPSpawnPoint* Candidate = Cast<ANPSpawnPoint>((*PointsPtr)[(Cursor + Attempt) % PointsPtr->Num()].Get());
		if (IsValid(Candidate))
		{
			Chosen = Candidate;
			Cursor = (Cursor + Attempt + 1) % PointsPtr->Num();
			break;
		}
	}

	if (!Chosen)
	{
		WavePendingToSpawn[WaveIndex] = FMath::Max(0, WavePendingToSpawn[WaveIndex] - 1);
		MaybeMarkWaveCleared(WaveIndex);
		return;
	}

	const FTransform SpawnTM = Chosen->GetSpawnTransform();

	APawn* Spawned = nullptr;

	if (UnitCopy.MonsterClass)
	{
		Spawned = GetWorld()->SpawnActorDeferred<APawn>(UnitCopy.MonsterClass, SpawnTM, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (Spawned)
		{
			Spawned->SetActorHiddenInGame(true);
			Spawned->FinishSpawning(SpawnTM);

			ANPMonsterCharacter* Monster = Cast<ANPMonsterCharacter>(Spawned);
			if (Monster)
			{
				Monster->OnDead.AddDynamic(this, &ANPEncounterManager::OnSpawnedEnemyDestroyed);
			}
			else
			{
				Spawned->OnDestroyed.AddDynamic(this, &ANPEncounterManager::OnSpawnedEnemyDestroyed);
			}

			WaveAlive[WaveIndex] += 1;
		}
	}

	WavePendingToSpawn[WaveIndex] = FMath::Max(0, WavePendingToSpawn[WaveIndex] - 1);
	MaybeMarkWaveCleared(WaveIndex);
}

void ANPEncounterManager::OnSpawnedEnemyDestroyed(AActor* DeadOrDestroyedActor)
{
	const int32 WaveIndex = LastStartedWaveIndex;
	if (!EncounterData || !EncounterData->Waves.IsValidIndex(WaveIndex)) return;

	WaveAlive[WaveIndex] = FMath::Max(0, WaveAlive[WaveIndex] - 1);
	MaybeMarkWaveCleared(WaveIndex);
}
