#pragma once

#include "CoreMinimal.h"
#include "NPInteractionTypes.generated.h"

UENUM(BlueprintType)
enum class ENPInteractionType : uint8
{
	None,
	Quest,
	MissionBoard,
	BattleEntry,
	Shop,
	Dialogue,
};