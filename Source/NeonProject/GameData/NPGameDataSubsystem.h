// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utility/NPMacros.h"
#include "NPGameDataSubsystem.generated.h"


struct FNPHubStageData;
struct FNPBattleStageData;
struct FNPCharacterData;

UCLASS()
class NEONPROJECT_API UNPGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	NP_DECLARE_GAMEINSTANCE_SUBSYSTEM_GETTER()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	const UDataTable* GetBattleStageDataTable() const { return BattleStageDataTable; }
	const UDataTable* GetPlayerDataTable() const { return PlayerDataTable; }
	static const FNPCharacterData* GetPlayerData(const UObject* WorldContext, FName Id);
	static const FNPCharacterData* GetMonsterData(const UObject* WorldContext, FName Id);

	template<typename T>
	static const T* GetGameData(const UObject* WorldContext, FName id)
	{
		static_assert(
			std::is_same_v<T, FNPHubStageData> ||
			std::is_same_v<T, FNPBattleStageData>,
			"Unsupported game data type."
		);

		const UNPGameDataSubsystem* Subsystem = GetChecked(WorldContext);
		const UDataTable* GameDataTable = nullptr;

		if constexpr (std::is_same_v<T, FNPHubStageData>)
		{
			GameDataTable = Subsystem->HubStageDataTable;
		}
		else if constexpr (std::is_same_v<T, FNPBattleStageData>)
		{
			GameDataTable = Subsystem->BattleStageDataTable;
		}
		check(GameDataTable);

		const T* Data = GameDataTable->FindRow<T>(
			id,
			ANSI_TO_TCHAR(__FUNCTION__),
			true
		);

		if (!ensure(Data))
			return nullptr;

		return Data;
	}

private:

	UPROPERTY()	TObjectPtr<UDataTable> HubStageDataTable;

	UPROPERTY()	TObjectPtr<UDataTable> BattleStageDataTable;

	UPROPERTY()	TObjectPtr<UDataTable> PlayerDataTable;

	UPROPERTY()	TObjectPtr<UDataTable> MonsterDataTable;

};
