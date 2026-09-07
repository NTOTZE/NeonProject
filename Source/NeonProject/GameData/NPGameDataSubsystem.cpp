// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/NPGameDataSubsystem.h"
#include "DataType/NPStageData.h"
#include "DataType/NPCharacterData.h"
#include "GameData/NPGameDataSettings.h"

void UNPGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UNPGameDataSettings* Settings = UNPGameDataSettings::GetChecked();
	HubStageDataTable = Settings->GetHubStageDataTable();
	BattleStageDataTable = Settings->GetBattleStageDataTable();
	PlayerDataTable = Settings->GetPlayerDataTable();
	MonsterDataTable = Settings->GetMonsterDataTable();

	checkf(HubStageDataTable, TEXT("Game Data Settings에 HubStageDataTable이 설정되어 있지 않습니다."));
	checkf(BattleStageDataTable, TEXT("Game Data Settings에 BattleStageDataTable이 설정되어 있지 않습니다."));
	checkf(PlayerDataTable, TEXT("Game Data Settings에 PlayerDataTable이 설정되어 있지 않습니다."));
	checkf(MonsterDataTable, TEXT("Game Data Settings에 MonsterDataTable이 설정되어 있지 않습니다."));
}

const FNPCharacterData* UNPGameDataSubsystem::GetPlayerData(const UObject* WorldContext, FName Id)
{
	const UNPGameDataSubsystem* Subsystem = GetChecked(WorldContext);
	check(Subsystem->PlayerDataTable);
	return Subsystem->PlayerDataTable->FindRow<FNPCharacterData>(Id, ANSI_TO_TCHAR(__FUNCTION__), true);
}

const FNPCharacterData* UNPGameDataSubsystem::GetMonsterData(const UObject* WorldContext, FName Id)
{
	const UNPGameDataSubsystem* Subsystem = GetChecked(WorldContext);
	check(Subsystem->MonsterDataTable);
	return Subsystem->MonsterDataTable->FindRow<FNPCharacterData>(Id, ANSI_TO_TCHAR(__FUNCTION__), true);
}
