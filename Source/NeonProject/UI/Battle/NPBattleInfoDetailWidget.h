// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Battle/NPBattleInfoSummaryWidget.h"
#include "NPBattleInfoDetailWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPBattleInfoDetailWidget : public UNPBattleInfoSummaryWidget
{
	GENERATED_BODY()

protected:
	virtual void SetBattleInfo(const FNPBattleStageData* battleStageData) override;
};
