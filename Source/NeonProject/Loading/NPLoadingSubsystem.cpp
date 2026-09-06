#include "Loading/NPLoadingSubsystem.h"
#include "Loading/NPLoadingSettings.h"
#include "Loading/UI/SNPLoadingScreen.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MoviePlayer.h"

void UNPLoadingSubsystem::Deinitialize()
{
	CancelStageLoading();
	Super::Deinitialize();
}

TSharedPtr<FStreamableHandle> UNPLoadingSubsystem::LoadAssets(
	const TArray<FSoftObjectPath>& AssetPaths, FStreamableDelegate OnLoaded)
{
	if (AssetPaths.IsEmpty())
	{
		OnLoaded.ExecuteIfBound();
		return nullptr;
	}
	return UAssetManager::GetStreamableManager().RequestAsyncLoad(AssetPaths, MoveTemp(OnLoaded));
}

bool UNPLoadingSubsystem::PrepareStageLoading(const FName& ArtworkId)
{
	check(!LoadingScreen);
	if (LoadingScreen || !GetMoviePlayer())
		return false;

	const FNPLoadingScreenResources Resources =
		UNPLoadingSettings::GetChecked()->GetLoadingScreenResources(ArtworkId);
	SAssignNew(LoadingScreen, SNPLoadingScreen).Resources(Resources);
	const uint64 RequestId = ++LoadingRequestId;
	LoadingScreen->OnLoadingScreenReady.BindWeakLambda(this, [this, RequestId](bool bSucceeded)
	{
		if (RequestId == LoadingRequestId)
			HandleLoadingScreenReady(bSucceeded);
	});
	LoadingScreen->OnLoadingScreenFinished.BindWeakLambda(this, [this, RequestId]()
	{
		if (RequestId == LoadingRequestId)
			HandleLoadingScreenFinished();
	});

	FLoadingScreenAttributes Attributes;
	Attributes.WidgetLoadingScreen = LoadingScreen;
	Attributes.bAutoCompleteWhenLoadingCompletes = false;
	Attributes.bWaitForManualStop = true;
	Attributes.bMoviesAreSkippable = false;
	// 로딩스크린 종료 대기 중에도 에셋 로딩 완료 콜백과 새 월드 초기화가 진행되도록 Tick 허용
	Attributes.bAllowEngineTick = true;
	GetMoviePlayer()->SetupLoadingScreen(Attributes);
	return !LoadingScreen->HasResourceLoadFailed();
}

void UNPLoadingSubsystem::BeginStageTransition(const TSoftObjectPtr<UWorld>& StageLevel,
	FNPStageLevelLoadedDelegate OnLevelLoaded, bool bInAutoFinishScreen)
{
	check(!bTransitionRequested);
	LevelLoadedDelegate = MoveTemp(OnLevelLoaded);
	PendingLevel = StageLevel;
	bAutoFinishScreen = bInAutoFinishScreen;
	bTransitionRequested = true;
	if (!LoadingScreen || StageLevel.IsNull() || LoadingScreen->HasResourceLoadFailed())
	{
		CompleteLevelLoading(nullptr, false);
		return;
	}
	if (LoadingScreen->IsReady())
		OpenPreparedLevel();
}

void UNPLoadingSubsystem::HandleLoadingScreenReady(bool bSucceeded)
{
	if (!bTransitionRequested)
		return;
	if (!bSucceeded)
	{
		CompleteLevelLoading(nullptr, false);
		return;
	}
	OpenPreparedLevel();
}

void UNPLoadingSubsystem::OpenPreparedLevel()
{
	check(LoadingScreen && LoadingScreen->IsReady());
	LoadingScreen->OnLoadingScreenReady.Unbind();
	const FString PackageName = PendingLevel.ToSoftObjectPath().GetLongPackageName();
	if (PackageName.IsEmpty())
	{
		CompleteLevelLoading(nullptr, false);
		return;
	}

	// 레벨 이동 요청 전에 로딩 완료 및 실패 델리게이트 바인딩
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);
	if (GEngine)
		TravelFailureHandle = GEngine->OnTravelFailure().AddUObject(this, &ThisClass::HandleTravelFailure);
	IGameMoviePlayer* MoviePlayer = GetMoviePlayer();
	MoviePlaybackStartedHandle = MoviePlayer->OnMoviePlaybackStarted().AddUObject(
		this, &ThisClass::HandleMoviePlaybackStarted);
	MoviePlaybackFinishedHandle = MoviePlayer->OnMoviePlaybackFinished().AddUObject(
		this, &ThisClass::HandleMoviePlaybackFinished);
	LoadingScreen->StartLoadingScreen();
	// 재생은 엔진의 PreLoadMap에 맡겨 일반 프레임과 로딩 렌더링이 겹치지 않게 한다.
	UGameplayStatics::OpenLevel(this, FName(*PackageName));
}

void UNPLoadingSubsystem::HandleMoviePlaybackStarted()
{
	bScreenPlaying = true;
}

void UNPLoadingSubsystem::HandlePostLoadMap(UWorld* LoadedWorld)
{
	if (LoadedWorld && LoadedWorld->GetGameInstance() != GetGameInstance())
		return;
	CompleteLevelLoading(LoadedWorld, IsValid(LoadedWorld));
}

void UNPLoadingSubsystem::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& Error)
{
	if (World && World->GetGameInstance() != GetGameInstance())
		return;
	UE_LOG(LogTemp, Error, TEXT("Stage 레벨 이동에 실패했습니다. 사유: [%s]"), *Error);
	CompleteLevelLoading(nullptr, false);
}

void UNPLoadingSubsystem::CompleteLevelLoading(UWorld* LoadedWorld, bool bSucceeded)
{
	RemoveTravelDelegates();
	bTransitionRequested = false;
	const bool bShouldAutoFinish = bAutoFinishScreen;
	FNPStageLevelLoadedDelegate Callback = MoveTemp(LevelLoadedDelegate);
	LevelLoadedDelegate.Unbind();
	Callback.ExecuteIfBound(LoadedWorld, bSucceeded);
	if (bShouldAutoFinish && !bFinishRequested && LoadingScreen)
		RequestFinishLoadingScreen(FSimpleDelegate());
}

void UNPLoadingSubsystem::RequestFinishLoadingScreen(FSimpleDelegate OnFinished)
{
	check(!bFinishRequested);
	bFinishRequested = true;
	ScreenFinishedDelegate = MoveTemp(OnFinished);
	if (!LoadingScreen || !bScreenPlaying)
	{
		HandleMoviePlaybackFinished();
		return;
	}
	LoadingScreen->BeginFinishLoadingScreen();
}

void UNPLoadingSubsystem::HandleLoadingScreenFinished()
{
	// 로딩스크린 연출은 종료됐지만 MoviePlayer가 아직 위젯을 보유하고 있으므로 재생 종료 요청
	if (bFinishRequested && bScreenPlaying)
		GetMoviePlayer()->StopMovie();
}

void UNPLoadingSubsystem::HandleMoviePlaybackFinished()
{
	bScreenPlaying = false;
	if (!bFinishRequested)
		return;
	FSimpleDelegate Callback = MoveTemp(ScreenFinishedDelegate);
	ScreenFinishedDelegate.Unbind();
	ResetLoadingScreen();
	Callback.ExecuteIfBound();
}

void UNPLoadingSubsystem::RemoveTravelDelegates()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
	PostLoadMapHandle.Reset();
	if (GEngine)
		GEngine->OnTravelFailure().Remove(TravelFailureHandle);
	TravelFailureHandle.Reset();
}

void UNPLoadingSubsystem::ResetLoadingScreen()
{
	++LoadingRequestId;
	RemoveTravelDelegates();
	if (IGameMoviePlayer* MoviePlayer = GetMoviePlayer())
	{
		MoviePlayer->OnMoviePlaybackStarted().Remove(MoviePlaybackStartedHandle);
		MoviePlaybackStartedHandle.Reset();
		MoviePlayer->OnMoviePlaybackFinished().Remove(MoviePlaybackFinishedHandle);
		MoviePlaybackFinishedHandle.Reset();
		MoviePlayer->SetupLoadingScreen(FLoadingScreenAttributes());
	}
	if (LoadingScreen)
	{
		LoadingScreen->Shutdown();
		LoadingScreen.Reset();
	}
	PendingLevel.Reset();
	LevelLoadedDelegate.Unbind();
	bTransitionRequested = false;
	bScreenPlaying = false;
	bAutoFinishScreen = false;
	bFinishRequested = false;
}

void UNPLoadingSubsystem::CancelStageLoading()
{
	++LoadingRequestId;
	RemoveTravelDelegates();
	LevelLoadedDelegate.Unbind();
	ScreenFinishedDelegate.Unbind();
	if (bScreenPlaying && GetMoviePlayer())
	{
		GetMoviePlayer()->OnMoviePlaybackFinished().Remove(MoviePlaybackFinishedHandle);
		GetMoviePlayer()->StopMovie();
		// Slate 리소스 정리 전에 로딩 렌더링 종료까지 대기
		GetMoviePlayer()->WaitForMovieToFinish();
	}
	ResetLoadingScreen();
}

void UNPLoadingSubsystem::SetLoadingDisplayData(const FNPLoadingDisplayData& DisplayData)
{
	check(IsInGameThread());
	if (LoadingScreen)
		LoadingScreen->SetLoadingDisplayData(DisplayData);
}
