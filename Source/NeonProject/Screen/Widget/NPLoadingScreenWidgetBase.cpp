// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen/Widget/NPLoadingScreenWidgetBase.h"

#include "NeonProject.h"

void UNPLoadingScreenWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ProgressFinishedDelegate.BindDynamic(this, &UNPLoadingScreenWidgetBase::HandleProgressFinished);
	FadeOutFinishedDelegate.BindDynamic(this, &UNPLoadingScreenWidgetBase::HandleFadeOutFinished);
}

void UNPLoadingScreenWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	
}

bool UNPLoadingScreenWidgetBase::PlayLoading()
{
	if (!IsInViewport())
		AddToViewport(GetZOrder());

	//FadeIn
	//PlayAnimation(LoadingFadeInAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);

	BindToAnimationFinished(LoadingProgressAnimation, ProgressFinishedDelegate);
	if (PlayAnimation(LoadingProgressAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f))
		return true;

	UnbindAllFromAnimationFinished(LoadingProgressAnimation);
	OnLoadingScreenFinished.Unbind();
	RemoveFromParent();
	NP_LOG(NPLog, Error, TEXT("LoadingProgressAnimation 재생 실패"));

	return false;
}

void UNPLoadingScreenWidgetBase::HandleProgressFinished()
{
	UnbindAllFromAnimationFinished(LoadingProgressAnimation);

	BindToAnimationFinished(LoadingFadeOutAnimation, FadeOutFinishedDelegate);
	if (PlayAnimation(LoadingFadeOutAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f))
		return;

	NP_LOG(NPLog, Error, TEXT("LoadingFadeOutAnimation 재생 실패"));
	HandleFadeOutFinished();
}

void UNPLoadingScreenWidgetBase::HandleFadeOutFinished()
{
	UnbindAllFromAnimationFinished(LoadingFadeOutAnimation);
	RemoveFromParent();

	OnLoadingScreenFinished.ExecuteIfBound();
}
