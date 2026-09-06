// Fill out your copyright notice in the Description page of Project Settings.


#include "Loading/NPLoadingSubsystem.h"
#include "NeonProject.h"
#include "Loading/NPLoadingSettings.h"
#include "Loading/UI/SNPLoadingScreen.h"

#include "MoviePlayer.h"

/// @brief EnterStageHandler에서 호출될 함수
void UNPLoadingSubsystem::PrepareStageLoading()
{
	PrepareLoadingScreen(TEXT("Default"));

}

/// @brief EnterStageHandler에서 ScreenSubsystem으로부터 FadeOut 완료시 호출되도록 바인드될 함수
void UNPLoadingSubsystem::BeginStageTransition(const TSoftObjectPtr<UWorld>& StageLevel)
{
	check(!StageLevel.IsNull());
	check(LoadingScreen);
	if (!LoadingScreen->IsReady())
	{
		LoadingScreen->OnLoadingScreenReady.BindLambda([this, StageLevel]()
		{
			BeginStageTransition(StageLevel);
		});
		return;
	}
	LoadingScreen->OnLoadingScreenReady.Unbind();

	BeginLoadingScreen();

	const FString LevelPackageName = StageLevel.ToSoftObjectPath().GetLongPackageName();
	checkf(!LevelPackageName.IsEmpty(), TEXT("Stage Level 경로가 유효하지 않습니다."));
	UGameplayStatics::OpenLevel(this, FName(*LevelPackageName));
}

void UNPLoadingSubsystem::PrepareLoadingScreen(const FName& ArtworkId)
{
	IGameMoviePlayer* MoviePlayer = GetMoviePlayer();
	check(MoviePlayer);

	const FNPLoadingScreenResources LoadingScreenResources =
		UNPLoadingSettings::GetChecked()->GetLoadingScreenResources(ArtworkId);
	SAssignNew(LoadingScreen, SNPLoadingScreen).Resources(LoadingScreenResources);

	FLoadingScreenAttributes Attributes;
	Attributes.WidgetLoadingScreen = LoadingScreen;
	Attributes.bAutoCompleteWhenLoadingCompletes = false;
	Attributes.bWaitForManualStop = true;
	Attributes.bMoviesAreSkippable = false;

	MoviePlayer->SetupLoadingScreen(Attributes);

	LoadingScreen->OnLoadingScreenFinished.BindUObject(this, &UNPLoadingSubsystem::HandleLoadingScreenFinished);
}

void UNPLoadingSubsystem::BeginLoadingScreen()
{
	IGameMoviePlayer* MoviePlayer = GetMoviePlayer();
	check(MoviePlayer);
	check(LoadingScreen);

	MoviePlayer->PlayMovie();
	LoadingScreen->StartLoadingScreen();
}

void UNPLoadingSubsystem::HandleLoadingScreenFinished()
{
	IGameMoviePlayer* MoviePlayer = GetMoviePlayer();
	check(MoviePlayer);

	MoviePlayer->StopMovie();

	if (LoadingScreen.IsValid())
	{
		LoadingScreen->Shutdown();
		LoadingScreen.Reset();
	}
}
