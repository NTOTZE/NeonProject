// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Battle/NPBattleInfoSummaryWidget.h"
#include "DataType/NPStageData.h"
#include "GameData/NPGameDataSubsystem.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UNPBattleInfoSummaryWidget::InitBattleInfo(FName id)
{
	BattleInfoId = id;

	SetBattleInfo(GetBattleInfo());
}

const FNPBattleStageData* UNPBattleInfoSummaryWidget::GetBattleInfo()
{
	return UNPGameDataSubsystem::GetGameData<FNPBattleStageData>(this, BattleInfoId);
}

void UNPBattleInfoSummaryWidget::SetBattleInfo(const FNPBattleStageData* battleStageData)
{
	if (!ensure(battleStageData))
		return;

	BattleName->SetText(battleStageData->StageName);
	BattleLevel->SetText(FText::AsNumber(battleStageData->Difficulty));
	BattleDescription->SetText(battleStageData->Description);
	BattleImage->SetBrushFromTexture(battleStageData->ThumbnailTexture.LoadSynchronous());
}
