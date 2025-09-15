// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NPEncounterData.generated.h"

class APawn;

/** 개별 스폰 단위: 무엇을, 어디서, 몇 마리, 언제, 간격 */
USTRUCT(BlueprintType)
struct FNPSpawnUnit
{
	GENERATED_BODY()

	// 스폰할 몬스터(보통 ANPEnemyCharacter 파생)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Encounter")
	TSubclassOf<APawn> MonsterClass = nullptr;

	// 스폰 위치 식별용: 레벨의 ActorTags에서 이 이름을 가진 ANPSpawnPoint들을 찾음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Encounter")
	FName SpawnPointTag;

	// 이 유닛 묶음을 몇 마리 뽑을지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Encounter", meta = (ClampMin = "1"))
	int32 Count = 1;

	// 웨이브 시작 시점으로부터 이 유닛의 첫 스폰까지 지연(초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Encounter", meta = (ClampMin = "0.1"))
	float FirstDelay = 0.1f;

	// 같은 유닛 내 개체 간 간격(초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Encounter", meta = (ClampMin = "0"))
	float Interval = 0.2f;
};

/** 웨이브: 언제 시작할지 + 어떤 유닛들을 뿌릴지 */
USTRUCT(BlueprintType)
struct FNPEncounterWave
{
	GENERATED_BODY()

	// 인카운터 시작으로부터의 절대 시간(초) — 타임라인형
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Encounter", meta = (ClampMin = "0.1"))
	float StartTime = 0.1f;

	// true면 이전 웨이브가 정리(모두 사망)될 때까지 기다렸다가 시작(순차형)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Encounter")
	bool bWaitForPrevWaveClear = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Encounter")
	TArray<FNPSpawnUnit> Units;
};

UCLASS()
class NEONPROJECT_API UNPEncounterData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Encounter")
	TArray<FNPEncounterWave> Waves;
};
