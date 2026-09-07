
#pragma once

#include "CoreMinimal.h"
#include "DataType/NPCombatTypes.h"
#include "NPInputCommandTypes.generated.h"

UENUM(BlueprintType)
enum class ENPInputCommandReceiver : uint8
{
	None,
	Character,
	Party,
	CameraRig
};

UENUM(BlueprintType)
enum class ENPInputCommandType : uint8
{
	None,
	Attack,
	Dash,
	Ability,
	Swap,
	LockOn,
	Unlock,
};

USTRUCT(BlueprintType)
struct FNPInputCommand
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ENPInputCommandReceiver Receiver = ENPInputCommandReceiver::None;

	UPROPERTY()
	ENPInputCommandType CommandType = ENPInputCommandType::None;

	UPROPERTY()
	ENPAbilityType AbilityType = ENPAbilityType::None;

	UPROPERTY()
	int32 SwapIndex = INDEX_NONE;

	static FNPInputCommand MakeAttack()
	{
		FNPInputCommand Cmd;
		Cmd.Receiver = ENPInputCommandReceiver::Character;
		Cmd.CommandType = ENPInputCommandType::Attack;
		return Cmd;
	}

	static FNPInputCommand MakeDash()
	{
		FNPInputCommand Cmd;
		Cmd.Receiver = ENPInputCommandReceiver::Character;
		Cmd.CommandType = ENPInputCommandType::Dash;
		return Cmd;
	}

	static FNPInputCommand MakeAbility(ENPAbilityType InAbilityType)
	{
		FNPInputCommand Cmd;
		Cmd.Receiver = ENPInputCommandReceiver::Character;
		Cmd.CommandType = ENPInputCommandType::Ability;
		Cmd.AbilityType = InAbilityType;
		return Cmd;
	}

	static FNPInputCommand MakeSwap(int32 InSwapIndex)
	{
		FNPInputCommand Cmd;
		Cmd.Receiver = ENPInputCommandReceiver::Party;
		Cmd.CommandType = ENPInputCommandType::Swap;
		Cmd.SwapIndex = InSwapIndex;
		return Cmd;
	}

	static FNPInputCommand MakeLockOn()
	{
		FNPInputCommand Cmd;
		Cmd.Receiver = ENPInputCommandReceiver::CameraRig;
		Cmd.CommandType = ENPInputCommandType::LockOn;
		return Cmd;
	}

	static FNPInputCommand MakeUnlock()
	{
		FNPInputCommand Cmd;
		Cmd.Receiver = ENPInputCommandReceiver::CameraRig;
		Cmd.CommandType = ENPInputCommandType::Unlock;
		return Cmd;
	}
};