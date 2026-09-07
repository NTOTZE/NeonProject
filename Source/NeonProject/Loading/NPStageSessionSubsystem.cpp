#include "Loading/NPStageSessionSubsystem.h"
#include "Engine/StreamableManager.h"

void UNPStageSessionSubsystem::InitializeSession(FNPStageSessionData&& InSessionData,
	TArray<TSharedPtr<FStreamableHandle>>&& InAssetHandles)
{
	SessionData = MoveTemp(InSessionData);
	AssetHandles = MoveTemp(InAssetHandles);

	for (const TSharedPtr<FStreamableHandle>& Handle : AssetHandles)
	{
		check(!Handle || Handle->HasLoadCompleted());
	}

	bSessionInitialized = true;
	OnSessionReady.Broadcast();
}

void UNPStageSessionSubsystem::Deinitialize()
{
	// StreamableManager가 핸들을 보관하지 않으므로, 마지막 참조 제거시 에셋 유지 해제
	AssetHandles.Reset();
	SessionData = FNPStageSessionData();
	bSessionInitialized = false;
	Super::Deinitialize();
}
