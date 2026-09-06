// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utility/NPMacros.h"
#include "NPDialogueSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	NP_DECLARE_GAMEINSTANCE_SUBSYSTEM_GETTER()

public:
	UNPDialogueSubsystem();

	class UNPDialogueDataAsset* FindDialogueData(const FName& InDialogueId);

public:
	UPROPERTY()
	TObjectPtr<class UDataTable> DialogueTable;
};
