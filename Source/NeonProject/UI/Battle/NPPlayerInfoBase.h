// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPPlayerInfoBase.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPPlayerInfoBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	void SetHpBar(float current, float max);
	void SetStaminaBar(float current, float max);
	void SetSkillCostBar(float current, float max);
	void SetUltimateCostBar(float current, float max);

private:
	void SetHpText(float current, float max);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPProgressBarWidget> HpBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> HpText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPProgressBarWidget> SkillCostBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPProgressBarWidget> UltimateCostBar;
};
