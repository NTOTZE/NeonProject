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

	void SetHPBarRatio(float Ratio);
	void SetImage(TSoftObjectPtr<class UTexture2D> SoftTexture);
	
private:
	TObjectPtr<class UImage> Image;
	FName ImageWidgetName = TEXT("Image");

	TObjectPtr<class UProgressBar> HPBar;
	FName HPBarWidgetName = TEXT("HPBar");

	FVector2D DefaultSize;
};
