// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class NEONPROJECT_API UPlayerSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "Input")
	float MouseSensitivity = 0.5f;
};
