// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen/Widget/NPScreenFadeWidgetBase.h"
#include "NeonProject.h"

#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "NPLoadingScreenWidgetBase.h"


void UNPScreenFadeWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FadeInFinishedDelegate.BindDynamic(this, &UNPScreenFadeWidgetBase::HandleFadeInFinished);
	FadeOutFinishedDelegate.BindDynamic(this, &UNPScreenFadeWidgetBase::HandleFadeOutFinished);
}

void UNPScreenFadeWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	NP_LOG(NPLog, Warning, TEXT(""));
}

void UNPScreenFadeWidgetBase::NativeDestruct()
{
	if (UWorld* World = GetWorld())
		World->GetTimerManager().ClearTimer(FadeDelayTimer);
	Super::NativeDestruct();
}

bool UNPScreenFadeWidgetBase::PlayFade(ENPFadeAnimationType fadeAnimationType, float fadeDuration, float fadeStartDelay)
{
	return PlayWidgetAnimation(fadeAnimationType, fadeDuration, fadeStartDelay);
}

bool UNPScreenFadeWidgetBase::PlayWidgetAnimation(ENPFadeAnimationType fadeAnimationType, float fadeDuration, float fadeStartDelay)
{
	UWidgetAnimation* WidgetAnimation = nullptr;
	FWidgetAnimationDynamicEvent* AnimationFinishedDelegate = nullptr;

	switch (fadeAnimationType)
	{
	case ENPFadeAnimationType::FadeIn:
		WidgetAnimation = FadeInAnimation;
		AnimationFinishedDelegate = &FadeInFinishedDelegate;
		break;
	case ENPFadeAnimationType::FadeOut:
		WidgetAnimation = FadeOutAnimation;
		AnimationFinishedDelegate = &FadeOutFinishedDelegate;
		break;
	default:
		NP_LOG(NPLog, Warning, TEXT("fadeAnimationType이 None입니다."));
		return false;
	}

	if (!WidgetAnimation)
	{
		NP_LOG(NPLog, Warning, TEXT("WidgetAnimation이 nullptr입니다."));
		return false;
	}

	if (!AnimationFinishedDelegate)
	{
		NP_LOG(NPLog, Warning, TEXT("AnimationFinishedDelegate이 nullptr입니다."));
		return false;
	}

	if (IsPlayingAnimation())
	{
		NP_LOG(NPLog, Warning, TEXT("이미 위젯 애니메이션이 재생중입니다."))
		return false;
	}
	
	BindToAnimationFinished(WidgetAnimation, *AnimationFinishedDelegate);

	if(!IsInViewport())
		AddToViewport(ZOrder);

	float StartAtTime = 0.f;
	const float EndTime = WidgetAnimation->GetEndTime();
	bool bSucceeded = false;

	float PlaybackSpeed = EndTime;
	if (fadeDuration <= 0.f)
		StartAtTime = EndTime;
	else
	{
		PlaybackSpeed = EndTime / fadeDuration;
	}

	if (fadeStartDelay <= 0.f)
	{
		if (fadeDuration <= 0.f)
		{
			StartAtTime = EndTime;
		}
	
		if (PlayAnimation(WidgetAnimation, StartAtTime, 1, EUMGSequencePlayMode::Type::Forward, PlaybackSpeed))
			bSucceeded = true;
	}
	else
	{
		if (UWorld* World = GetWorld())
		{
			if (PlayAnimation(WidgetAnimation, 0.f, 1, EUMGSequencePlayMode::Type::Forward, 0.f))
			{
				World->GetTimerManager().SetTimer(
					FadeDelayTimer,
					FTimerDelegate::CreateWeakLambda(this, [this, WidgetAnimation, StartAtTime, PlaybackSpeed]()
						{
							SetAnimationCurrentTime(WidgetAnimation, StartAtTime);
							SetPlaybackSpeed(WidgetAnimation, PlaybackSpeed);
						}
					),
					fadeStartDelay,
					false);

				bSucceeded = true;
			}
		}
	}

	if (bSucceeded)
		return true;

	UnbindAllFromAnimationFinished(WidgetAnimation);
	RemoveFromParent();
	NP_LOG(NPLog, Warning, TEXT("WidgetAnimation 재생 실패"));

	return false;
}

void UNPScreenFadeWidgetBase::HandleFadeInFinished()
{
	NP_LOG(NPLog, Warning, TEXT(""));

	UnbindAllFromAnimationFinished(FadeInAnimation);
	RemoveFromParent();

	OnFadeAnimationFinished.ExecuteIfBound(ENPFadeAnimationType::FadeIn);
}

void UNPScreenFadeWidgetBase::HandleFadeOutFinished()
{
	NP_LOG(NPLog, Warning, TEXT(""));

	UnbindAllFromAnimationFinished(FadeOutAnimation);

	OnFadeAnimationFinished.ExecuteIfBound(ENPFadeAnimationType::FadeOut);
}
