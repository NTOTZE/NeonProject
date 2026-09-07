#include "GameFlow/Handler/NPEnterStageHandler.h"
#include "DataType/NPStageData.h"
#include "DataType/NPCharacterData.h"
#include "DataAsset/NPEncounterData.h"
#include "GameData/NPGameDataSubsystem.h"
#include "GameFlow/NPGameFlowSettings.h"
#include "Loading/NPLoadingSubsystem.h"
#include "Loading/NPStageSessionSubsystem.h"
#include "Screen/NPScreenSubsystem.h"
#include "Engine/World.h"

void UNPEnterStageHandler::PrepareExecuteInternal()
{
	const UNPStageSessionSubsystem* CurrentSession = UNPStageSessionSubsystem::GetChecked(this);
	if (CurrentSession->IsSessionInitialized())
		PreviousSession = MakeShared<FNPStageSessionData>(CurrentSession->GetSessionData());

	const FNPEnterStageHandlerData& Data = GetHandlerData().Get<FNPEnterStageHandlerData>();
	switch (Data.StageType)
	{
	case ENPStageType::Hub:
		if (const FNPHubStageData* Stage = UNPGameDataSubsystem::GetGameData<FNPHubStageData>(this, Data.StageId))
			StageLevel = Stage->Level;
		break;
	case ENPStageType::Battle:
		if (const FNPBattleStageData* Stage = UNPGameDataSubsystem::GetGameData<FNPBattleStageData>(this, Data.StageId))
		{
			StageLevel = Stage->Level;
			if (!Stage->EncounterData.IsNull())
			{
				AddAsset(Stage->EncounterData.ToSoftObjectPath(), TEXT("스테이지 전투 데이터"));
				const UNPEncounterData* EncounterData = Stage->EncounterData.LoadSynchronous();
				checkf(EncounterData, TEXT("Stage [%s]의 EncounterData를 불러오지 못했습니다."), *Data.StageId.ToString());
				if (EncounterData)
				{
					for (const FNPEncounterWave& Wave : EncounterData->Waves)
					{
						for (const FNPSpawnUnit& Unit : Wave.Units)
						{
							const FNPCharacterData* Monster = UNPGameDataSubsystem::GetMonsterData(this, Unit.MonsterId);
							checkf(Monster && !Monster->CharacterClass.IsNull(), TEXT("Monster [%s]의 데이터가 없거나 CharacterClass가 설정되어 있지 않습니다."), *Unit.MonsterId.ToString());
							if (Monster && !Monster->CharacterClass.IsNull())
							{
								const FString Name = Monster->CharacterName.IsEmpty() ? Unit.MonsterId.ToString() : Monster->CharacterName.ToString();
								AddAsset(Monster->CharacterClass.ToSoftObjectPath(), FString::Printf(TEXT("몬스터 · %s"), *Name));
							}
							else
							{
								bFailed = true;
							}
						}
					}
				}
			}
		}
		break;
	default:
		break;
	}
	checkf(!StageLevel.IsNull(), TEXT("Stage [%s]의 Level이 설정되어 있지 않습니다."), *Data.StageId.ToString());
	bFailed = StageLevel.IsNull();
	if (Data.StageType == ENPStageType::Hub && Data.PartyCharacterIds.IsEmpty())
	{
		const TSoftClassPtr<ANPCharacterBase>& DefaultHubCharacterClass =
			UNPGameFlowSettings::GetChecked()->GetDefaultHubCharacterClass();
		checkf(!DefaultHubCharacterClass.IsNull(), TEXT("기본 허브 캐릭터 클래스가 설정되어 있지 않습니다."));
		if (DefaultHubCharacterClass.IsNull())
		{
			bFailed = true;
		}
		else
		{
			AddAsset(DefaultHubCharacterClass.ToSoftObjectPath(), TEXT("기본 허브 캐릭터"));
		}
	}

	auto AddPlayerAssets = [this](const TArray<FName>& Ids)
	{
		for (const FName& Id : Ids)
		{
			const FNPCharacterData* Character = UNPGameDataSubsystem::GetPlayerData(this, Id);
			checkf(Character && !Character->CharacterClass.IsNull(), TEXT("Player [%s]의 데이터가 없거나 CharacterClass가 설정되어 있지 않습니다."), *Id.ToString());
			if (!Character || Character->CharacterClass.IsNull())
			{
				bFailed = true;
				continue;
			}
			const FString Name = Character->CharacterName.IsEmpty() ? Id.ToString() : Character->CharacterName.ToString();
			AddAsset(Character->CharacterClass.ToSoftObjectPath(), FString::Printf(TEXT("플레이어 캐릭터 · %s"), *Name));
		}
	};
	AddPlayerAssets(Data.PartyCharacterIds);
	for (const FSoftObjectPath& Path : Data.AdditionalAssets)
	{
		check(Path.IsValid());
		if (!Path.IsValid())
			bFailed = true;
		else
			AddAsset(Path, FString::Printf(TEXT("오브젝트 · %s"), *Path.GetAssetName()));
	}

	UNPLoadingSubsystem* Loading = UNPLoadingSubsystem::GetChecked(this);
	bFailed |= !Loading->PrepareStageLoading();
	UpdateLoadingDisplay();
	if (!bFailed)
	{
		// 에셋별 핸들로 완료 상태를 기록하고, 중복 경로는 한 번만 요청
		bSubmittingAssets = true;
		AssetHandles.SetNum(AssetPaths.Num());
		for (int32 Index = 0; Index < AssetPaths.Num(); ++Index)
		{
			AssetHandles[Index] = Loading->LoadAssets({ AssetPaths[Index] },
				FStreamableDelegate::CreateUObject(this, &ThisClass::HandleAssetLoaded, Index));
			if (!AssetHandles[Index])
			{
				LoadingDisplay.Assets[Index].State = ENPLoadingAssetState::Failed;
				bFailed = true;
			}
		}
		bSubmittingAssets = false;
		bAssetsLoaded = LoadingDisplay.GetCompletedCount() == AssetPaths.Num();
		UpdateLoadingDisplay();
	}
}

void UNPEnterStageHandler::AddAsset(const FSoftObjectPath& Path, const FString& DisplayName)
{
	if (AssetPaths.Contains(Path))
		return;
	AssetPaths.Add(Path);
	FNPLoadingAssetDisplay& Asset = LoadingDisplay.Assets.AddDefaulted_GetRef();
	Asset.DisplayName = DisplayName;
}

void UNPEnterStageHandler::UpdateLoadingDisplay()
{
	if (bFailed)
		LoadingDisplay.StatusText = TEXT("스테이지 준비에 실패했습니다.");
	else if (!bAssetsLoaded && !bLevelLoaded)
		LoadingDisplay.StatusText = bExecuting ? TEXT("에셋 준비 및 레벨 이동 중") : TEXT("에셋 준비 중");
	else if (!bAssetsLoaded)
		LoadingDisplay.StatusText = TEXT("에셋 준비 중");
	else if (!bLevelLoaded)
		LoadingDisplay.StatusText = bExecuting ? TEXT("에셋 준비 완료 · 레벨 이동 중") : TEXT("에셋 준비 완료 · 레벨 이동 대기 중");
	else
		LoadingDisplay.StatusText = TEXT("스테이지 준비 완료");
	UNPLoadingSubsystem::GetChecked(this)->SetLoadingDisplayData(LoadingDisplay);
}
void UNPEnterStageHandler::ExecuteInternal()
{
	bExecuting = true;
	UpdateLoadingDisplay();
	if (bFailed)
	{
		TryCompleteStageEntry();
		return;
	}
	UNPLoadingSubsystem::GetChecked(this)->BeginStageTransition(StageLevel,
		FNPStageLevelLoadedDelegate::CreateUObject(this, &ThisClass::HandleLevelLoaded), false);
}

void UNPEnterStageHandler::HandleAssetLoaded(int32 AssetIndex)
{
	check(IsInGameThread());
	if (bClosingScreen || !LoadingDisplay.Assets.IsValidIndex(AssetIndex))
		return;

	const bool bSucceeded = AssetPaths[AssetIndex].ResolveObject() != nullptr;
	LoadingDisplay.Assets[AssetIndex].State =
		bSucceeded ? ENPLoadingAssetState::Loaded : ENPLoadingAssetState::Failed;
	if (!bSucceeded)
	{
		UE_LOG(LogTemp, Error, TEXT("Stage 에셋 [%s] 로딩에 실패했습니다."), *AssetPaths[AssetIndex].ToString());
		bFailed = true;
	}
	bAssetsLoaded = LoadingDisplay.GetCompletedCount() == AssetPaths.Num();
	UpdateLoadingDisplay();
	if (!bSubmittingAssets)
		TryCompleteStageEntry();
}
void UNPEnterStageHandler::HandleLevelLoaded(UWorld* LoadedWorld, bool bSucceeded)
{
	bLevelLoaded = true;
	DestinationWorld = LoadedWorld;
	bFailed |= !bSucceeded || !IsValid(LoadedWorld);
	UpdateLoadingDisplay();
	TryCompleteStageEntry();
}

void UNPEnterStageHandler::TryCompleteStageEntry()
{
	if (!bExecuting || bClosingScreen)
		return;
	// 레벨 이동이 요청된 상태라면 에셋 로딩에 실패해도 레벨 이동 완료까지 대기
	if (!bFailed && (!bAssetsLoaded || !bLevelLoaded))
		return;
	if (bFailed && !bLevelLoaded && !StageLevel.IsNull()
		&& UNPLoadingSubsystem::GetChecked(this)->IsStageTransitionPending())
		return;

	bClosingScreen = true;
	if (bFailed)
	{
		for (int32 Index = 0; Index < LoadingDisplay.Assets.Num(); ++Index)
		{
			if (LoadingDisplay.Assets[Index].State == ENPLoadingAssetState::Requested)
			{
				if (AssetHandles.IsValidIndex(Index) && AssetHandles[Index])
					AssetHandles[Index]->CancelHandle();
				LoadingDisplay.Assets[Index].State = ENPLoadingAssetState::Canceled;
			}
		}
	}
	UpdateLoadingDisplay();
	// 월드 정리시 기존 Fade 위젯이 제거되므로, 로딩스크린 종료 전에 새 월드에 검은 화면 표시
	bFailed |= !UNPScreenSubsystem::GetChecked(this)->ShowBlackScreen();
	UNPLoadingSubsystem::GetChecked(this)->RequestFinishLoadingScreen(
		FSimpleDelegate::CreateUObject(this, &ThisClass::HandleLoadingScreenClosed));
}

void UNPEnterStageHandler::HandleLoadingScreenClosed()
{
	Finish(!bFailed);
}

void UNPEnterStageHandler::FinishInternal()
{
	if (!bFailed && DestinationWorld.IsValid())
	{
		UNPStageSessionSubsystem* Session = DestinationWorld->GetSubsystem<UNPStageSessionSubsystem>();
		check(Session);
		if (Session)
		{
			const FNPEnterStageHandlerData& Data = GetHandlerData().Get<FNPEnterStageHandlerData>();
			FNPStageSessionData SessionData;
			SessionData.StageType = Data.StageType;
			SessionData.StageId = Data.StageId;
			SessionData.PartyCharacterIds = Data.PartyCharacterIds;
			SessionData.AdditionalAssets = Data.AdditionalAssets;
			SessionData.PreviousSession = MoveTemp(PreviousSession);
			Session->InitializeSession(MoveTemp(SessionData), MoveTemp(AssetHandles));
		}
	}
}

void UNPEnterStageHandler::CleanupInternal()
{
	bExecuting = false;
	UNPLoadingSubsystem::GetChecked(this)->CancelStageLoading();
	for (TSharedPtr<FStreamableHandle>& Handle : AssetHandles)
	{
		if (Handle && !Handle->HasLoadCompleted())
			Handle->CancelHandle();
	}
	AssetHandles.Reset();
	LoadingDisplay = FNPLoadingDisplayData();
	AssetPaths.Reset();
	DestinationWorld.Reset();
	StageLevel.Reset();
	PreviousSession.Reset();
}
