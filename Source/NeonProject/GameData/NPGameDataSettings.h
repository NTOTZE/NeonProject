// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "Utility/NPMacros.h"
#include "NPGameDataSettings.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Game Data Settings"))
class NEONPROJECT_API UNPGameDataSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	NP_DECLARE_SETTINGS()

public:
	UDataTable* GetHubStageDataTable() const { return HubStageDataTable.LoadSynchronous(); }
	UDataTable* GetBattleStageDataTable() const { return BattleStageDataTable.LoadSynchronous(); }
	UDataTable* GetCharacterDataTable() const { return CharacterDataTable.LoadSynchronous(); }

private:
	UPROPERTY(EditAnywhere, Config, Category = "Game Data")
	TSoftObjectPtr<UDataTable> HubStageDataTable;

	UPROPERTY(EditAnywhere, Config, Category = "Game Data")
	TSoftObjectPtr<UDataTable> BattleStageDataTable;

	UPROPERTY(EditAnywhere, Config, Category = "Game Data")
	TSoftObjectPtr<UDataTable> CharacterDataTable;
};
