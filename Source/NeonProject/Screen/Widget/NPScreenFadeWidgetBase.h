// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataType/NPScreenTypes.h"
#include "NPScreenFadeWidgetBase.generated.h"

DECLARE_DELEGATE_OneParam(FNPFadeAnimationFinishedDelegate, ENPFadeAnimationType /*CompletedFadeType*/);

class UImage;

UCLASS()
class NEONPROJECT_API UNPScreenFadeWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	/// <summary>
	/// Fade 애니메이션 재생
	/// </summary>
	/// <param name="fadeAnimationType"> : FadeIn or FadeOut </param>
	/// <param name="playbackSpeed"> : 0보다 작은 수 입력 시, 애니메이션이 시작 즉시 종료됩니다. </param>
	/// <param name="fadeStartDelay"> : Fade애니메이션이 첫 프레임에서 fadeStartDelay만큼 대기 후 재생됩니다. </param>
	/// <returns>이미 재생중인 애니메이션이 있거나 애니메이션 재생에 실패한다면 false를 출력합니다.</returns>
	bool PlayFade(ENPFadeAnimationType fadeAnimationType, float fadeDuration = 1.f, float fadeStartDelay = 0.f);

private:
	bool PlayWidgetAnimation(ENPFadeAnimationType fadeAnimationType, float fadeDuration, float fadeStartDelay);

	UFUNCTION()	void HandleFadeInFinished();
	UFUNCTION() void HandleFadeOutFinished();

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> FadeImage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnimation;

private:
	FWidgetAnimationDynamicEvent FadeOutFinishedDelegate;
	FWidgetAnimationDynamicEvent FadeInFinishedDelegate;
	FTimerHandle FadeDelayTimer;

public:
	FNPFadeAnimationFinishedDelegate OnFadeAnimationFinished;
	static constexpr int32 ZOrder = 1001;
};
