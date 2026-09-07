#pragma once

#include "CoreMinimal.h"
#include "GameFlow/Command/NPGameFlowCommand.h"

enum class ENPStageType : uint8;

namespace NPEnterStageCommandFactory
{
	FNPGameFlowCommand MakeEnterStage(
		ENPStageType StageType,
		FName StageId,
		const TArray<FName>& PartyCharacterIds,
		TSharedPtr<const FNPGameFlowCommandOptions> Options = FNPGameFlowCommandOptions::Make(false, true, 1.f));

	FNPGameFlowCommand MakeEnterDefaultHub(
		TSharedPtr<const FNPGameFlowCommandOptions> Options = FNPGameFlowCommandOptions::Make(false, true, 1.f));

	TOptional<FNPGameFlowCommand> MakeReturnToPreviousStage(
		const UObject* WorldContext,
		TSharedPtr<const FNPGameFlowCommandOptions> Options = FNPGameFlowCommandOptions::Make(false, true, 1.f));
}
