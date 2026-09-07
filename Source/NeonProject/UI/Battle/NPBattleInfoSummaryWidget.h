// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPBattleInfoSummaryWidget.generated.h"

struct FNPBattleStageData;

UCLASS()
class NEONPROJECT_API UNPBattleInfoSummaryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitBattleInfo(FName id);
	const FNPBattleStageData* GetBattleInfo();

protected:
	virtual void SetBattleInfo(const FNPBattleStageData* battleStageData);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> BattleImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> BattleLevel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> BattleName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> BattleDescription;

	FName BattleInfoId;
};
