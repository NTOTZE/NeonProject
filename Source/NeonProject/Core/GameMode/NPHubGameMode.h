// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GameMode/NPGameModeBase.h"
#include "NPHubGameMode.generated.h"

class UNPDialogueDataAsset;
class ANPInteractableActorBase;

UCLASS()
class NEONPROJECT_API ANPHubGameMode : public ANPGameModeBase
{
	GENERATED_BODY()
	
public:
	ANPHubGameMode();
};
