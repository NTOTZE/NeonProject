// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataType/NPStageData.h"
#include "Screen/Widget/NPScreenWidgetBase.h"
#include "NPBattleSelectionWidget.generated.h"

UCLASS(BlueprintType)
class NEONPROJECT_API UNPBattleSelectionItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	FName BattleStageId;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	FText StageName;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	int32 Difficulty = 0;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	TSoftObjectPtr<UTexture2D> ThumbnailTexture;
};

UCLASS()
class NEONPROJECT_API UNPBattleSelectionWidget : public UNPScreenWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void OnBattleItemClicked(UObject* BattleItem);
	void SetSelectedBattleItem(UNPBattleSelectionItem* BattleItem);
	void RequestEnterBattle(const FName& BattleStageId);

	UFUNCTION(BlueprintImplementableEvent, Category = "NP|Battle")
	void BP_OnBattleStageSelected(UNPBattleSelectionItem* BattleItem);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UListView> BattleList;

	UPROPERTY(Transient)
	TObjectPtr<UNPBattleSelectionItem> SelectedBattleItem;
};
