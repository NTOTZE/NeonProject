// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlow/NPGameFlowHandlerBase.h"
#include "Loading/NPLoadingDisplayData.h"
#include "NPEnterStageHandler.generated.h"

enum class ENPStageType : uint8;
struct FStreamableHandle;
struct FNPStageSessionData;

USTRUCT(BlueprintType)
struct FNPEnterStageHandlerData : public FNPGameFlowHandlerDataBase
{
	GENERATED_BODY()

public:
	FNPEnterStageHandlerData() = default;

	static TSharedPtr<const TInstancedStruct<FNPGameFlowHandlerDataBase>> Make(ENPStageType stageType, const FName& stageId,
		const TArray<FName>& PartyCharacterIds = {},
		const TArray<FSoftObjectPath>& AdditionalAssets = {})
	{
		TSharedPtr<TInstancedStruct<FNPGameFlowHandlerDataBase>> HandlerDataPtr
			= MakeShared<TInstancedStruct<FNPGameFlowHandlerDataBase>>();
		HandlerDataPtr->InitializeAs<FNPEnterStageHandlerData>();
		
		FNPEnterStageHandlerData& HandlerData = HandlerDataPtr->GetMutable<FNPEnterStageHandlerData>();
		HandlerData.StageType = stageType;
		HandlerData.StageId = stageId;
		HandlerData.PartyCharacterIds = PartyCharacterIds;
		HandlerData.AdditionalAssets = AdditionalAssets;

		return HandlerDataPtr;
	}

public:
	ENPStageType StageType = static_cast<ENPStageType>(0);
	FName StageId;
	// 파티 캐릭터 ID는 PlayerDataTable에서 조회
	TArray<FName> PartyCharacterIds;
	// 전용 ID 조회가 없는 오브젝트 등은 에셋 경로로 전달
	TArray<FSoftObjectPath> AdditionalAssets;
};

UCLASS()
class NEONPROJECT_API UNPEnterStageHandler : public UNPGameFlowHandlerBase
{
	GENERATED_BODY()
	
private:
	virtual void PrepareExecuteInternal() override;
	virtual void ExecuteInternal() override;
	virtual void FinishInternal() override;
	virtual void CleanupInternal() override;

	void HandleAssetLoaded(int32 AssetIndex);
	void AddAsset(const FSoftObjectPath& Path, const FString& DisplayName);
	void UpdateLoadingDisplay();
	void HandleLevelLoaded(UWorld* LoadedWorld, bool bSucceeded);
	void TryCompleteStageEntry();
	void HandleLoadingScreenClosed();

private:
	TSoftObjectPtr<UWorld> StageLevel;
	TArray<FSoftObjectPath> AssetPaths;
	TArray<TSharedPtr<FStreamableHandle>> AssetHandles;
	FNPLoadingDisplayData LoadingDisplay;
	bool bSubmittingAssets = false;
	TWeakObjectPtr<UWorld> DestinationWorld;
	bool bExecuting = false;
	bool bAssetsLoaded = false;
	bool bLevelLoaded = false;
	bool bFailed = false;
	bool bClosingScreen = false;
	TSharedPtr<const FNPStageSessionData> PreviousSession;
};
