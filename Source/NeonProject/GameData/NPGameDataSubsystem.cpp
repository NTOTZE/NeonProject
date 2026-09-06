// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/NPGameDataSubsystem.h"
#include "NeonProject.h"
#include "DataType/NPStageData.h"
#include "DataType/NPCharacterData.h"

UNPGameDataSubsystem::UNPGameDataSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> BattleStageDataTableFinder(
		TEXT("/Script/Engine.DataTable'/Game/NeonProject/Blueprint/DataTable/DT_BattleStage.DT_BattleStage'"));
	check(BattleStageDataTableFinder.Succeeded());
	BattleStageDataTable = BattleStageDataTableFinder.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> CharacterDataTableFinder(
		TEXT("/Script/Engine.DataTable'/Game/NeonProject/Blueprint/DataTable/DT_Character.DT_Character'"));
	check(CharacterDataTableFinder.Succeeded());
	CharacterDataTable = CharacterDataTableFinder.Object;
}
