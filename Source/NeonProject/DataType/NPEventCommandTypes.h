#pragma once

#include "CoreMinimal.h"
#include "DataType/NPStageData.h"
#include "GameData/NPGameDataSubsystem.h"
#include "NPEventCommandTypes.generated.h"

UENUM(BlueprintType)
enum class ENPEventCommandType : uint8
{
	None,
	Dialogue,
	EnterStage,
	Quest,
};

USTRUCT(BlueprintType)
struct FNPDialogueEventData
{
	GENERATED_BODY()

public:
	bool IsValid(const UObject* WorldContextObject) const
	{
		return true;
	}

public:
};

USTRUCT(BlueprintType)
struct FNPEnterStageEventData
{
	GENERATED_BODY()

public:
	bool IsValid(const UObject* WorldContextObject) const
	{
		switch (StageType)
		{
		case ENPStageType::None:
		{
			break;
		}
		case ENPStageType::Hub:
			if (UNPGameDataSubsystem::GetGameData<FNPHubStageData>(WorldContextObject, Id))
			{
				return true;
			}
			break;
		case ENPStageType::Battle:
			if (UNPGameDataSubsystem::GetGameData<FNPBattleStageData>(WorldContextObject, Id))
			{
				return true;
			}
			break;
		default:
			break;
		}
		return false;
	}

public:
	ENPStageType StageType;
	FName Id;
};

USTRUCT(BlueprintType)
struct FNPQuestEventData
{
	GENERATED_BODY()

public:
	bool IsValid(const UObject* WorldContextObject) const
	{
		return true;
	}

public:
};

USTRUCT(BlueprintType)
struct FNPEventCommand
{
	GENERATED_BODY()

public:
	static FNPEventCommand MakeDialogueEvent(const FNPDialogueEventData& dialogueEventData)
	{
		FNPEventCommand command;
		command.CommandType = ENPEventCommandType::Dialogue;
		command.DialogueEventData = dialogueEventData;
		return command;
	}

	static FNPEventCommand MakeEnterStageEvent(const FNPEnterStageEventData& levelEventData)
	{
		FNPEventCommand command;
		command.CommandType = ENPEventCommandType::EnterStage;
		command.EnterStageEventData = levelEventData;
		return command;
	}

	static FNPEventCommand MakeQuestEvent(const FNPQuestEventData& questEventData)
	{
		FNPEventCommand command;
		command.CommandType = ENPEventCommandType::Quest;
		command.QuestEventData = questEventData;
		return command;
	}

	bool IsValid(const UObject* WorldContextObject) const
	{
		switch (CommandType)
		{
		case ENPEventCommandType::Dialogue:
			return DialogueEventData.IsValid(WorldContextObject);
		case ENPEventCommandType::EnterStage:
			return EnterStageEventData.IsValid(WorldContextObject);
		case ENPEventCommandType::Quest:
			return QuestEventData.IsValid(WorldContextObject);
		}

		return false;
	}

public:
	ENPEventCommandType CommandType;

	FNPDialogueEventData DialogueEventData;
	FNPEnterStageEventData EnterStageEventData;
	FNPQuestEventData QuestEventData;

};