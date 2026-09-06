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
	CharacterDataTable = Settings->GetCharacterDataTable();

	checkf(HubStageDataTable, TEXT("Game Data Settings에 HubStageDataTable이 설정되어 있지 않습니다."));
	checkf(BattleStageDataTable, TEXT("Game Data Settings에 BattleStageDataTable이 설정되어 있지 않습니다."));
	checkf(CharacterDataTable, TEXT("Game Data Settings에 CharacterDataTable이 설정되어 있지 않습니다."));
}
