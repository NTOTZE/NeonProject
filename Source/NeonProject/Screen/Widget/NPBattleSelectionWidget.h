// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Screen/Widget/NPScreenWidgetBase.h"
#include "NPBattleSelectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPBattleSelectionWidget : public UNPScreenWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void OnBattleItemClicked(UObject* BattleItem);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UListView> BattleList;
};
