// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Utility/NPMacros.h"
#include "NPGameFlowSettings.generated.h"

class UNPGameFlowHandlerBase;

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Game Flow Settings"))
class NEONPROJECT_API UNPGameFlowSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	NP_DECLARE_SETTINGS()

public:
	const TMap<TSoftObjectPtr<UScriptStruct>, TSoftClassPtr<UNPGameFlowHandlerBase>>& GetHandlerMap() const { return HandlerMap; }

private:
	UPROPERTY(EditAnywhere, Config, Category = "Handler")
	TMap<TSoftObjectPtr<UScriptStruct>, TSoftClassPtr<UNPGameFlowHandlerBase>> HandlerMap;
};
