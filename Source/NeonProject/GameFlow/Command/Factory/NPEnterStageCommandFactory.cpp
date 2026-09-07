#include "GameFlow/Command/Factory/NPEnterStageCommandFactory.h"

#include "NeonProject.h"
#include "GameFlow/Handler/NPEnterStageHandler.h"
#include "GameFlow/NPGameFlowSettings.h"
#include "Loading/NPStageSessionSubsystem.h"

FNPGameFlowCommand NPEnterStageCommandFactory::MakeEnterStage(
	ENPStageType StageType,
	FName StageId,
	const TArray<FName>& PartyCharacterIds,
	TSharedPtr<const FNPGameFlowCommandOptions> Options)
{
	return FNPGameFlowCommand::Make(
		Options,
		FNPEnterStageHandlerData::Make(StageType, StageId, PartyCharacterIds));
}

FNPGameFlowCommand NPEnterStageCommandFactory::MakeEnterDefaultHub(
	TSharedPtr<const FNPGameFlowCommandOptions> Options)
{
	const UNPGameFlowSettings* Settings = UNPGameFlowSettings::GetChecked();
	return MakeEnterStage(
		ENPStageType::Hub,
		Settings->GetDefaultHubStageId(),
		{},
		MoveTemp(Options));
}

TOptional<FNPGameFlowCommand> NPEnterStageCommandFactory::MakeReturnToPreviousStage(
	const UObject* WorldContext,
	TSharedPtr<const FNPGameFlowCommandOptions> Options)
{
	const UNPStageSessionSubsystem* StageSession = UNPStageSessionSubsystem::GetChecked(WorldContext);
	if (!StageSession->IsSessionInitialized())
	{
		NP_LOG(NPLog, Warning, TEXT("이전 스테이지로 복귀할 현재 세션이 초기화되어 있지 않습니다."));
		return {};
	}

	const TSharedPtr<const FNPStageSessionData>& PreviousSession = StageSession->GetPreviousSession();
	if (!PreviousSession.IsValid())
	{
		NP_LOG(NPLog, Warning, TEXT("이전 스테이지 세션이 설정되어 있지 않습니다."));
		return {};
	}

	return MakeEnterStage(
		PreviousSession->StageType,
		PreviousSession->StageId,
		PreviousSession->PartyCharacterIds,
		Options);
}
