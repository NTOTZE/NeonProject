// Fill out your copyright notice in the Description page of Project Settings.


#include "Loading/UI/SNPLoadingScreen.h"
#include "SlateOptMacros.h"
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
#include "Utility/NPInputBlockProcessor.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SNPLoadingScreen::Construct(const FArguments& InArgs)
{
	FNPInputBlockProcessor::Enable(InputBlockProcessor);
	if (FSlateApplication::IsInitialized())
		FSlateApplication::Get().GetPlatformCursor()->Show(false);

	Resources = InArgs._Resources;
	Resources.CheckPath();

	FadeInCurveHandle = FadeInSequence.AddCurve(0.f, FadeDuration, ECurveEaseFunction::QuadOut);
	FadeOutCurveHandle = FadeOutSequence.AddCurve(0.f, FadeDuration, ECurveEaseFunction::QuadIn);

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

	SetLoadingProgress(0.5f);
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

	if (FadeTimerHandle.IsValid())
	{
		UnRegisterActiveTimer(FadeTimerHandle.ToSharedRef());
		FadeTimerHandle.Reset();
	}

	if (LoadingScreenAssetHandle.IsValid())
	{
		LoadingScreenAssetHandle->CancelHandle();
		LoadingScreenAssetHandle.Reset();
	}

	OnLoadingScreenFinished.Unbind();

	LoadingContent.Reset();

	ArtworkBrush.SetResourceObject(nullptr);

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
	bFinishRequested = true;

	if (LoadingScreenState != ELoadingScreenState::Visible)
		return;

	PlayFadeOut();
}

void SNPLoadingScreen::FinishLoadingScreen()
{
	LoadingScreenState = ELoadingScreenState::Finished;

	OnLoadingScreenFinished.ExecuteIfBound();
}


void SNPLoadingScreen::SetLoadingProgress(float InProgress)
{
	LoadingProgress = FMath::Clamp(InProgress, 0.0f, 1.0f);
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
}

void SNPLoadingScreen::HandleResourcesLoaded()
{
	NP_LOG(NPLog, Warning, TEXT(""));
	Resources.CheckLoaded();

	ArtworkBrush.SetResourceObject(Resources.ArtworkTexture.Get());
	VignetteBrush.SetResourceObject(Resources.VignetteTexture.Get());
	ThrobberBrush.SetResourceObject(Resources.ThrobberTexture.Get());

	Invalidate(EInvalidateWidgetReason::Paint);

	bIsReady = true;

	OnLoadingScreenReady.ExecuteIfBound();
}


void SNPLoadingScreen::PlayFadeIn()
{
	check(LoadingContent.IsValid());
	check(LoadingScreenState == ELoadingScreenState::Hidden);
	check(!FadeTimerHandle.IsValid());

	LoadingScreenState = ELoadingScreenState::FadingIn;

	LoadingContent->SetRenderOpacity(0.0f);

	FadeInSequence.Play(AsShared());

	FadeTimerHandle = RegisterActiveTimer(
		0.0f,
		FWidgetActiveTimerDelegate::CreateSP(
			this,
			&SNPLoadingScreen::HandleFadeInTimer
		)
	);
}

void SNPLoadingScreen::PlayFadeOut()
{
	check(LoadingContent.IsValid());
	check(LoadingScreenState == ELoadingScreenState::Visible);
	check(!FadeTimerHandle.IsValid());

	LoadingScreenState = ELoadingScreenState::FadingOut;

	LoadingContent->SetRenderOpacity(1.0f);

	FadeOutSequence.Play(AsShared());

	FadeTimerHandle = RegisterActiveTimer(
		0.0f,
		FWidgetActiveTimerDelegate::CreateSP(
			this,
			&SNPLoadingScreen::HandleFadeOutTimer
		)
	);
}

TOptional<float> SNPLoadingScreen::GetLoadingProgress() const
{
	return LoadingProgress;
}

FText SNPLoadingScreen::GetLoadingProgressText() const
{
	return FText::AsPercent(LoadingProgress);
}


EActiveTimerReturnType SNPLoadingScreen::HandleFadeInTimer(double CurrentTime, float DeltaTime)
{
	check(LoadingContent.IsValid());

	LoadingContent->SetRenderOpacity(FadeInCurveHandle.GetLerp());

	if (FadeInSequence.IsAtEnd())
	{
		FadeTimerHandle.Reset();

		LoadingContent->SetRenderOpacity(1.0f);

		LoadingScreenState = ELoadingScreenState::Visible;

		if (bFinishRequested)
		{
			PlayFadeOut();
		}

		return EActiveTimerReturnType::Stop;
	}

	return EActiveTimerReturnType::Continue;
}

EActiveTimerReturnType SNPLoadingScreen::HandleFadeOutTimer(double CurrentTime, float DeltaTime)
{
	check(LoadingContent.IsValid());

	LoadingContent->SetRenderOpacity(1.f - FadeOutCurveHandle.GetLerp());

	if (FadeOutSequence.IsAtEnd())
	{
		FadeTimerHandle.Reset();

		LoadingContent->SetRenderOpacity(0.0f);

		FinishLoadingScreen();

		return EActiveTimerReturnType::Stop;
	}

	return EActiveTimerReturnType::Continue;
}
