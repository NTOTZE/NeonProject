// Fill out your copyright notice in the Description page of Project Settings.


#include "Loading/UI/SNPLoadingScreen.h"
#include "SlateOptMacros.h"
#include "Async/Async.h"
#include "NeonProject.h"

#include "Engine/AssetManager.h"
#include "Engine/Texture2D.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Misc/ScopeLock.h"
#include "Utility/NPInputBlockProcessor.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SNPLoadingScreen::Construct(const FArguments& InArgs)
{
	FNPInputBlockProcessor::Enable(InputBlockProcessor);
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetPlatformCursor()->Show(false);
		bPlatformCursorHidden = true;
	}

	Resources = InArgs._Resources;
	Resources.CheckPath();

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(FLinearColor::Black)
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFill)
			.StretchDirection(EStretchDirection::Both)
			[
				SNew(SBox)
				.WidthOverride(1920.0f)
				.HeightOverride(1080.0f)
				[
					SAssignNew(LoadingContent, SOverlay)
					.RenderOpacity(0.f)

					// Artwork
					+ SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SImage)
						.Image(&ArtworkBrush)
					]

					// vignette
					+ SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SImage)
						.Image(&VignetteBrush)
					]

					+ SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Bottom)
					.Padding(100.0f, 0.0f, 100.0f, 30.0f)
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Left)
						.Padding(0.f, 0.f, 0.f, 20.f)
						[
							SNew(SBox)
							.WidthOverride(1000.f)
							[
								SNew(SBorder)
								.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
								.BorderBackgroundColor(FLinearColor(0.015f, 0.025f, 0.04f, 0.85f))
								.Padding(24.f)
								[
									SNew(SVerticalBox)
									+ SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0.f, 0.f, 0.f, 14.f)
									[
										SAssignNew(LoadingStatusText, STextBlock)
										.Text(NSLOCTEXT("NPLoading", "Preparing", "스테이지 준비 중"))
										.Font(FCoreStyle::GetDefaultFontStyle("Bold", 22))
										.ColorAndOpacity(FLinearColor(0.45f, 0.9f, 1.f))
									]
									+ SVerticalBox::Slot()
									.AutoHeight()
									[
										SNew(SBox)
										.MaxDesiredHeight(240.f)
										[
											SNew(SScrollBox)
											+ SScrollBox::Slot()
											[
												SAssignNew(AssetList, SVerticalBox)
											]
										]
									]
								]
							]
						]

						// Loading Throbber
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Right)
						[
							SNew(SBox)
							.WidthOverride(64.0f)
							.HeightOverride(64.0f)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							[
								SNew(SCircularThrobber)
								.PieceImage(&ThrobberBrush)
								.NumPieces(8)
								.Period(1.0f)
								.Radius(80.0f)
							]
						]

						// Progress Bar
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SProgressBar)
							.Percent(this, &SNPLoadingScreen::GetLoadingProgress)
						]

						// Progress Bar Text
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(0.f, 8.f, 0.f, 0.f)
						[
							SNew(STextBlock)
							.Text(this, &SNPLoadingScreen::GetLoadingProgressText)
						]
					]
				]
			]
		]
	];

	SetLoadingProgress(0.f);
	AsyncLoadResources();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

SNPLoadingScreen::~SNPLoadingScreen()
{
	Shutdown();
}

void SNPLoadingScreen::Shutdown()
{
	FNPInputBlockProcessor::Disable(InputBlockProcessor);

	// 종료와 소멸자가 중복 호출되어도 플랫폼 커서는 한 번만 복원한다.
	if (bPlatformCursorHidden)
	{
		if (FSlateApplication::IsInitialized())
			FSlateApplication::Get().GetPlatformCursor()->Show(true);
		bPlatformCursorHidden = false;
	}

	if (LoadingScreenAssetHandle.IsValid())
	{
		LoadingScreenAssetHandle->CancelHandle();
		LoadingScreenAssetHandle.Reset();
	}

	OnLoadingScreenReady.Unbind();
	OnLoadingScreenFinished.Unbind();

	LoadingContent.Reset();
	AssetList.Reset();
	LoadingStatusText.Reset();

	ArtworkBrush.SetResourceObject(nullptr);
	VignetteBrush.SetResourceObject(nullptr);
	ThrobberBrush.SetResourceObject(nullptr);

	bFinishRequested = false;
	bIsReady = false;

	LoadingScreenState = ELoadingScreenState::Finished;
}

void SNPLoadingScreen::StartLoadingScreen()
{
	if (!bIsReady)
		return;

	if (LoadingScreenState != ELoadingScreenState::Hidden)
		return;

	PlayFadeIn();
}


void SNPLoadingScreen::BeginFinishLoadingScreen()
{
	// 종료 요청만 기록하고, 실제 연출 상태 변경은 로딩 Slate 스레드에서 처리
	bFinishRequested.Store(true);
}

void SNPLoadingScreen::Tick(const FGeometry& AllottedGeometry, double CurrentTime, float DeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, CurrentTime, DeltaTime);
	ApplyPendingDisplayData();

	// MoviePlayer 로딩 중에는 ActiveTimer에 의존하지 않고 위젯 Tick에서 Fade 진행
	if (LoadingScreenState == ELoadingScreenState::FadingIn
		|| LoadingScreenState == ELoadingScreenState::FadingOut)
	{
		FadeElapsedTime += FMath::Max(DeltaTime, 0.f);
		const float Alpha = FMath::Clamp(FadeElapsedTime / FadeDuration, 0.f, 1.f);
		if (LoadingScreenState == ELoadingScreenState::FadingIn)
		{
			LoadingContent->SetRenderOpacity(1.f - FMath::Square(1.f - Alpha));
			if (Alpha >= 1.f)
				LoadingScreenState = ELoadingScreenState::Visible;
		}
		else
		{
			LoadingContent->SetRenderOpacity(1.f - FMath::Square(Alpha));
			if (Alpha >= 1.f)
				FinishLoadingScreen();
		}
	}

	// 로딩이 먼저 완료돼도 FadeIn 종료 후 FadeOut 시작
	if (bFinishRequested.Load() && LoadingScreenState == ELoadingScreenState::Visible)
		PlayFadeOut();
}

void SNPLoadingScreen::FinishLoadingScreen()
{
	LoadingScreenState = ELoadingScreenState::Finished;
	FNPLoadingScreenFinishedDelegate Callback = OnLoadingScreenFinished;
	// MoviePlayer 종료와 위젯 정리는 게임 스레드에서 처리하도록 완료 델리게이트 전달
	AsyncTask(ENamedThreads::GameThread, [Callback]() { Callback.ExecuteIfBound(); });
}

void SNPLoadingScreen::SetLoadingProgress(float InProgress)
{
	LoadingProgress.Store(FMath::Clamp(InProgress, 0.0f, 1.0f));
}


void SNPLoadingScreen::AsyncLoadResources()
{
	NP_LOG(NPLog, Warning, TEXT(""));
	Resources.CheckPath();
	
	FStreamableManager& StreamableManager =
		UAssetManager::GetStreamableManager();

	LoadingScreenAssetHandle = StreamableManager.RequestAsyncLoad(
		Resources.GetAssetPaths(),
		FStreamableDelegate::CreateSP(
			this,
			&SNPLoadingScreen::HandleResourcesLoaded
		)
	);
	if (!LoadingScreenAssetHandle)
		bResourceLoadFailed = true;
}

void SNPLoadingScreen::HandleResourcesLoaded()
{
	NP_LOG(NPLog, Warning, TEXT(""));
	bResourceLoadFailed = !Resources.ArtworkTexture.IsValid()
		|| !Resources.VignetteTexture.IsValid() || !Resources.ThrobberTexture.IsValid();
	if (bResourceLoadFailed)
	{
		UE_LOG(LogTemp, Error, TEXT("로딩스크린 리소스 로딩에 실패했습니다."));
		OnLoadingScreenReady.ExecuteIfBound(false);
		return;
	}

	ArtworkBrush.SetResourceObject(Resources.ArtworkTexture.Get());
	VignetteBrush.SetResourceObject(Resources.VignetteTexture.Get());
	ThrobberBrush.SetResourceObject(Resources.ThrobberTexture.Get());

	Invalidate(EInvalidateWidgetReason::Paint);

	bIsReady = true;

	OnLoadingScreenReady.ExecuteIfBound(true);
}


void SNPLoadingScreen::PlayFadeIn()
{
	check(LoadingContent.IsValid());
	check(LoadingScreenState == ELoadingScreenState::Hidden);

	LoadingScreenState = ELoadingScreenState::FadingIn;
	FadeElapsedTime = 0.f;
	LoadingContent->SetRenderOpacity(0.f);
}

void SNPLoadingScreen::PlayFadeOut()
{
	check(LoadingContent.IsValid());
	check(LoadingScreenState == ELoadingScreenState::Visible);

	LoadingScreenState = ELoadingScreenState::FadingOut;
	FadeElapsedTime = 0.f;
	LoadingContent->SetRenderOpacity(1.f);
}

TOptional<float> SNPLoadingScreen::GetLoadingProgress() const
{
	return LoadingProgress.Load();
}

FText SNPLoadingScreen::GetLoadingProgressText() const
{
		if (DisplayData.Assets.IsEmpty())
		return NSLOCTEXT("NPLoading", "NoAssetRequests", "추가 에셋 요청 없음");
	return FText::Format(NSLOCTEXT("NPLoading", "AssetProgress", "에셋 처리 {0} / {1} · {2}"),
		FText::AsNumber(DisplayData.GetCompletedCount()), FText::AsNumber(DisplayData.Assets.Num()),
		FText::AsPercent(LoadingProgress.Load()));
}

void SNPLoadingScreen::SetLoadingDisplayData(const FNPLoadingDisplayData& InDisplayData)
{
	FScopeLock Lock(&DisplayDataMutex);
	PendingDisplayData = InDisplayData;
	bDisplayDataPending = true;
}

void SNPLoadingScreen::ApplyPendingDisplayData()
{
	{
		FScopeLock Lock(&DisplayDataMutex);
		if (!bDisplayDataPending)
			return;
		DisplayData = MoveTemp(PendingDisplayData);
		bDisplayDataPending = false;
	}
	if (!AssetList || !LoadingStatusText)
		return;

	SetLoadingProgress(DisplayData.GetProgress());
	LoadingStatusText->SetText(FText::FromString(DisplayData.StatusText));
	AssetList->ClearChildren();
	if (DisplayData.Assets.IsEmpty())
	{
		AssetList->AddSlot().AutoHeight()
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("NPLoading", "EmptyAssets", "별도로 요청한 에셋이 없습니다."))
			.ColorAndOpacity(FLinearColor(0.65f, 0.7f, 0.75f))
		];
		return;
	}

	for (const FNPLoadingAssetDisplay& Asset : DisplayData.Assets)
	{
		FText StateText;
		FLinearColor StateColor;
		switch (Asset.State)
		{
		case ENPLoadingAssetState::Loaded:
			StateText = NSLOCTEXT("NPLoading", "AssetLoaded", "로딩 완료");
			StateColor = FLinearColor(0.35f, 0.9f, 0.65f);
			break;
		case ENPLoadingAssetState::Failed:
			StateText = NSLOCTEXT("NPLoading", "AssetFailed", "로딩 실패");
			StateColor = FLinearColor(1.f, 0.35f, 0.35f);
			break;
		case ENPLoadingAssetState::Canceled:
			StateText = NSLOCTEXT("NPLoading", "AssetCanceled", "요청 취소");
			StateColor = FLinearColor(0.7f, 0.7f, 0.7f);
			break;
		default:
			StateText = NSLOCTEXT("NPLoading", "AssetRequested", "로딩 요청 중");
			StateColor = FLinearColor(0.5f, 0.8f, 1.f);
			break;
		}

		AssetList->AddSlot().AutoHeight().Padding(0.f, 5.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 12.f, 0.f)
			[
				SNew(SBox).WidthOverride(6.f).HeightOverride(6.f)
				[
					SNew(SBorder)
					.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.BorderBackgroundColor(StateColor)
				]
			]
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Asset.DisplayName))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 18))
				.AutoWrapText(true)
				.ColorAndOpacity(FLinearColor(0.9f, 0.93f, 0.97f))
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(20.f, 0.f, 0.f, 0.f)
			[
				SNew(STextBlock)
				.Text(StateText)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
				.ColorAndOpacity(StateColor)
			]
		];
	}
}
