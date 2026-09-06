// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Loading/UI/NPLoadingScreenResources.h"
#include "Loading/NPLoadingDisplayData.h"
#include "Utility/NPInputBlockMacros.h"

class UTexture2D;
class SOverlay;
class SVerticalBox;
class STextBlock;
class FNPInputBlockProcessor;
struct FStreamableHandle;

DECLARE_DELEGATE_OneParam(FNPLoadingScreenReadyDelegate, bool);
DECLARE_DELEGATE(FNPLoadingScreenFinishedDelegate);

class NEONPROJECT_API SNPLoadingScreen : public SCompoundWidget
{
	NP_SLATE_BLOCK_ALL_INPUT()

#if WITH_DEV_AUTOMATION_TESTS
	friend class FNPLoadingScreenFadeTest;
	friend class FNPLoadingDisplaySnapshotTest;
#endif

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
	void SetLoadingDisplayData(const FNPLoadingDisplayData& DisplayData);
	bool IsReady() const { return bIsReady; }
	bool HasResourceLoadFailed() const { return bResourceLoadFailed; }
	virtual void Tick(const FGeometry& AllottedGeometry, double CurrentTime, float DeltaTime) override;

private:
	void AsyncLoadResources();
	void HandleResourcesLoaded();
	void ApplyPendingDisplayData();

	void PlayFadeIn();
	void PlayFadeOut();

	TOptional<float> GetLoadingProgress() const;
	FText GetLoadingProgressText() const;

	
public:
	FNPLoadingScreenReadyDelegate OnLoadingScreenReady;
	FNPLoadingScreenFinishedDelegate OnLoadingScreenFinished;

private:
	TSharedPtr<SOverlay> LoadingContent;
	TSharedPtr<SVerticalBox> AssetList;
	TSharedPtr<STextBlock> LoadingStatusText;

	// 게임 스레드에서 받은 데이터를 잠금 안에서 복사하고 위젯 변경은 Slate Tick에서 처리
	FCriticalSection DisplayDataMutex;
	FNPLoadingDisplayData PendingDisplayData;
	FNPLoadingDisplayData DisplayData;
	bool bDisplayDataPending = false;
	TSharedPtr<FStreamableHandle> LoadingScreenAssetHandle;

	FSlateBrush ArtworkBrush;
	FSlateBrush VignetteBrush;
	FSlateBrush ThrobberBrush;

	FNPLoadingScreenResources Resources;


private:
	float FadeElapsedTime = 0.f;

	static constexpr float FadeDuration = 0.5f;

	TAtomic<float> LoadingProgress { 0.0f };

	bool bIsReady = false;

	TAtomic<bool> bFinishRequested { false };
	bool bResourceLoadFailed = false;
	bool bPlatformCursorHidden = false;

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
