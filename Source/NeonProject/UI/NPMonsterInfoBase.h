// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPMonsterInfoBase.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPMonsterInfoBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetHPBarRatio(float Ratio);

private:
	TObjectPtr<class UProgressBar> HPBar;
	FName HPBarWidgetName = TEXT("HPBar");

	float HPRatio = 1.f;
};
