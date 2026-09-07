// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Utility/NPMacros.h"
#include "NPGameFlowSettings.generated.h"

class UNPGameFlowHandlerBase;
class ANPCharacterBase;

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Game Flow Settings"))
class NEONPROJECT_API UNPGameFlowSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	NP_DECLARE_SETTINGS()

public:
	const TMap<TSoftObjectPtr<UScriptStruct>, TSoftClassPtr<UNPGameFlowHandlerBase>>& GetHandlerMap() const { return HandlerMap; }
	const FName& GetDefaultHubStageId() const { return DefaultHubStageId; }
	const TSoftClassPtr<ANPCharacterBase>& GetDefaultHubCharacterClass() const { return DefaultHubCharacterClass; }

private:
	UPROPERTY(EditAnywhere, Config, Category = "Handler")
	TMap<TSoftObjectPtr<UScriptStruct>, TSoftClassPtr<UNPGameFlowHandlerBase>> HandlerMap;

	UPROPERTY(EditAnywhere, Config, Category = "Stage")
	FName DefaultHubStageId = TEXT("Hub0001");

	UPROPERTY(EditAnywhere, Config, Category = "Stage")
	TSoftClassPtr<ANPCharacterBase> DefaultHubCharacterClass;
};
