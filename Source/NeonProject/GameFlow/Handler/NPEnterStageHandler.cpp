// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/Handler/NPEnterStageHandler.h"
#include "DataType/NPStageData.h"
#include "GameData/NPGameDataSubsystem.h"
#include "Loading/NPLoadingSubsystem.h"

void UNPEnterStageHandler::PrepareExecuteInternal()
{
	const FNPEnterStageHandlerData& EnterStageData = GetHandlerData().Get<FNPEnterStageHandlerData>();

	switch (EnterStageData.StageType)
	{
	case ENPStageType::Hub:
	{
		const FNPHubStageData* StageData = UNPGameDataSubsystem::GetGameData<FNPHubStageData>(this, EnterStageData.StageId);
		if (StageData)
		{
			StageLevel = StageData->Level;
		}
		break;
	}
	case ENPStageType::Battle:
	{
		const FNPBattleStageData* StageData = UNPGameDataSubsystem::GetGameData<FNPBattleStageData>(this, EnterStageData.StageId);
		if (StageData)
		{
			StageLevel = StageData->Level;
		}
		break;
	}
	default:
		break;
	}

	checkf(!StageLevel.IsNull(), TEXT("Stage [%s]의 Level이 설정되어 있지 않습니다."), *EnterStageData.StageId.ToString());

	UNPLoadingSubsystem::GetChecked(this)->PrepareStageLoading();
}

void UNPEnterStageHandler::ExecuteInternal()
{
	checkf(!StageLevel.IsNull(), TEXT("PrepareExecuteInternal에서 StageLevel을 설정해야 합니다."));

	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this,
		&UNPEnterStageHandler::HandlePostLoadMap
	);

	UNPLoadingSubsystem::GetChecked(this)->BeginStageTransition(StageLevel);
}

void UNPEnterStageHandler::FinishInternal()
{
}

void UNPEnterStageHandler::CleanupInternal()
{
	if (PostLoadMapHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
		PostLoadMapHandle.Reset();
	}

	StageLevel.Reset();
}

void UNPEnterStageHandler::HandlePostLoadMap(UWorld* LoadedWorld)
{
	Finish(IsValid(LoadedWorld));
}
