// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Loading/UI/NPLoadingScreenResources.h"
#include "Utility/NPInputBlockMacros.h"

class UTexture2D;
class SOverlay;
class FNPInputBlockProcessor;
struct FStreamableHandle;

DECLARE_DELEGATE(FNPLoadingScreenReadyDelegate);
DECLARE_DELEGATE(FNPLoadingScreenFinishedDelegate);

class NEONPROJECT_API SNPLoadingScreen : public SCompoundWidget
{
	NP_SLATE_BLOCK_ALL_INPUT()

public:
	SLATE_BEGIN_ARGS(SNPLoadingScreen)
		: _Resources()
	{
	}
		SLATE_ARGUMENT(FNPLoadingScreenResources, Resources)

	SLATE_END_ARGS()

	~SNPLoadingScreen();

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	/// @brief MoviePlayer에서 제거 된 후 호출해야함
	void Shutdown();

public:
	void StartLoadingScreen();
	void BeginFinishLoadingScreen();
	void FinishLoadingScreen();

	void SetLoadingProgress(float InProgress);
	bool IsReady() { return bIsReady; }

private:
	void AsyncLoadResources();
	void HandleResourcesLoaded();

	void PlayFadeIn();
	void PlayFadeOut();

	TOptional<float> GetLoadingProgress() const;
	FText GetLoadingProgressText() const;

	EActiveTimerReturnType HandleFadeInTimer(double CurrentTime, float DeltaTime);
	EActiveTimerReturnType HandleFadeOutTimer(double CurrentTime, float DeltaTime);
	
public:
	FNPLoadingScreenReadyDelegate OnLoadingScreenReady;
	FNPLoadingScreenFinishedDelegate OnLoadingScreenFinished;

private:
	TSharedPtr<SOverlay> LoadingContent;
	TSharedPtr<FStreamableHandle> LoadingScreenAssetHandle;

	FSlateBrush ArtworkBrush;
	FSlateBrush VignetteBrush;
	FSlateBrush ThrobberBrush;

	FNPLoadingScreenResources Resources;


private:
	FCurveSequence FadeInSequence;
	FCurveHandle FadeInCurveHandle;

	FCurveSequence FadeOutSequence;
	FCurveHandle FadeOutCurveHandle;

	TSharedPtr<FActiveTimerHandle> FadeTimerHandle;

	static constexpr float FadeDuration = 0.5f;

	float LoadingProgress = 0.0f;

	bool bIsReady = false;

	bool bFinishRequested = false;

private:
	enum class ELoadingScreenState : uint8
	{
		Hidden,
		FadingIn,
		Visible,
		FadingOut,
		Finished,
	};
	ELoadingScreenState LoadingScreenState = ELoadingScreenState::Hidden;

private:
	TSharedPtr<FNPInputBlockProcessor> InputBlockProcessor;
};
