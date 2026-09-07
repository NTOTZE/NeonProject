// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NPDialogueDataAsset.generated.h"


struct FNPDialogueLine;

UCLASS()
class NEONPROJECT_API UNPDialogueDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FNPDialogueLine> Lines;
};
