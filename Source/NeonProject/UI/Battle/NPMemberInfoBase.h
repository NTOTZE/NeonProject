// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPMemberInfoBase.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPMemberInfoBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetHpBar(float current, float max);
	void SetSkillCostBar(float current, float max);
	void SetUltimateCostBar(float current, float max);

	void SetImage(TSoftObjectPtr<class UTexture2D> SoftTexture);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPProgressBarWidget> HpBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPProgressBarWidget> UltimateCostBar;

	FVector2D DefaultSize;
};
