// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPLoadingScreenWidgetBase.generated.h"

DECLARE_DELEGATE(FNPLoadingScreenFinishedDelegate);

UCLASS()
class NEONPROJECT_API UNPLoadingScreenWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

public:
	bool PlayLoading();
	
	virtual int32 GetZOrder() const { return 900; }

private:
	// 애니메이션 진행 순서 : Progress -> FadeOut
	UFUNCTION()	void HandleProgressFinished();
	UFUNCTION() void HandleFadeOutFinished();

private:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LoadingFadeInAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LoadingProgressAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LoadingFadeOutAnimation;

private:
	FWidgetAnimationDynamicEvent ProgressFinishedDelegate;
	FWidgetAnimationDynamicEvent FadeOutFinishedDelegate;
	
public:
	FNPLoadingScreenFinishedDelegate OnLoadingScreenFinished;
};
