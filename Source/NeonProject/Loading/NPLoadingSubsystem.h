#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/StreamableManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utility/NPMacros.h"
#include "NPLoadingSubsystem.generated.h"

class SNPLoadingScreen;
struct FNPLoadingDisplayData;

DECLARE_DELEGATE_TwoParams(FNPStageLevelLoadedDelegate, UWorld*, bool);

UCLASS()
class NEONPROJECT_API UNPLoadingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	NP_DECLARE_GAMEINSTANCE_SUBSYSTEM_GETTER()

public:
	virtual void Deinitialize() override;

	TSharedPtr<FStreamableHandle> LoadAssets(
		const TArray<FSoftObjectPath>& AssetPaths, FStreamableDelegate OnLoaded);
	bool PrepareStageLoading(const FName& ArtworkId = TEXT("Default"));
	void BeginStageTransition(const TSoftObjectPtr<UWorld>& StageLevel,
		FNPStageLevelLoadedDelegate OnLevelLoaded, bool bAutoFinishScreen = false);
	void RequestFinishLoadingScreen(FSimpleDelegate OnFinished);
	void CancelStageLoading();
	void SetLoadingDisplayData(const FNPLoadingDisplayData& DisplayData);
	bool IsStageTransitionPending() const { return bTransitionRequested; }

private:
	void HandleLoadingScreenReady(bool bSucceeded);
	void OpenPreparedLevel();
	void HandlePostLoadMap(UWorld* LoadedWorld);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& Error);
	void CompleteLevelLoading(UWorld* LoadedWorld, bool bSucceeded);
	void HandleLoadingScreenFinished();
	void HandleMoviePlaybackStarted();
	void HandleMoviePlaybackFinished();
	void RemoveTravelDelegates();
	void ResetLoadingScreen();

private:
	TSharedPtr<SNPLoadingScreen> LoadingScreen;
	TSoftObjectPtr<UWorld> PendingLevel;
	FNPStageLevelLoadedDelegate LevelLoadedDelegate;
	FSimpleDelegate ScreenFinishedDelegate;
	FDelegateHandle PostLoadMapHandle;
	FDelegateHandle TravelFailureHandle;
	FDelegateHandle MoviePlaybackFinishedHandle;
	FDelegateHandle MoviePlaybackStartedHandle;
	uint64 LoadingRequestId = 0;
	bool bTransitionRequested = false;
	bool bScreenPlaying = false;
	bool bAutoFinishScreen = false;
	bool bFinishRequested = false;
};
