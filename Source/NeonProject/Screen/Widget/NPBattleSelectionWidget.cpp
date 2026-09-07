// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen/Widget/NPBattleSelectionWidget.h"
#include "Character/Player/NPPlayerCharacterBase.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "DataType/NPCharacterData.h"
#include "GameFlow/Handler/NPEnterStageHandler.h"
#include "GameFlow/NPGameFlowCommand.h"
#include "GameFlow/NPGameFlowSubsystem.h"
#include "GameData/NPGameDataSubsystem.h"
#include "NeonProject.h"
#include "Input/Reply.h"
#include "Screen/Widget/Element/NPBattleSelectionListEntryWidget.h"

void UNPBattleSelectionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetIsFocusable(true);

	if (IsValid(EntryButton))
		EntryButton->OnClicked.AddDynamic(this, &UNPBattleSelectionWidget::OnEntryButtonClicked);
}

void UNPBattleSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetRenderOpacity(0.f);
	SetIsEnabled(true);
	StartFade(1.f, false);

	if (!IsValid(BattleList))
	{
		NP_LOG(NPLog, Warning, TEXT("전투 선택 목록 BattleList가 바인딩되어 있지 않습니다."));
		return;
	}

	BattleList->ClearListItems();
	BattleItems.Empty();
	SelectedBattleItem = nullptr;
	UpdateCharacterList();
	UpdateEntryButtonState();

	const UNPGameDataSubsystem* GameDataSubsystem = UNPGameDataSubsystem::GetChecked(this);
	const UDataTable* BattleStageDataTable = GameDataSubsystem->GetBattleStageDataTable();
	if (!IsValid(BattleStageDataTable))
	{
		NP_LOG(NPLog, Warning, TEXT("전투 스테이지 데이터테이블이 설정되어 있지 않습니다."));
		return;
	}

	TArray<FName> BattleStageIds;
	BattleStageDataTable->GetRowMap().GetKeys(BattleStageIds);
	BattleStageIds.Sort(FNameLexicalLess());

	for (const FName& BattleStageId : BattleStageIds)
	{
		const FNPBattleStageData* BattleStageData = BattleStageDataTable->FindRow<FNPBattleStageData>(
			BattleStageId, TEXT("BattleSelection"), false);
		if (!BattleStageData)
			continue;

		UNPBattleSelectionItem* BattleItem = NewObject<UNPBattleSelectionItem>(this);
		BattleItem->BattleStageId = BattleStageId;
		BattleItem->StageName = BattleStageData->StageName;
		BattleItem->Description = BattleStageData->Description;
		BattleItem->Difficulty = BattleStageData->Difficulty;
		BattleItem->ThumbnailTexture = BattleStageData->ThumbnailTexture;
		BattleItem->OnClicked.AddUObject(this, &UNPBattleSelectionWidget::OnBattleItemClicked);
		BattleItems.Add(BattleItem);
		BattleList->AddItem(BattleItem);
	}

}

void UNPBattleSelectionWidget::NativeDestruct()
{
	if (IsValid(EntryButton))
		EntryButton->OnClicked.RemoveDynamic(this, &UNPBattleSelectionWidget::OnEntryButtonClicked);
	for (UNPBattleSelectionItem* BattleItem : BattleItems)
	{
		if (IsValid(BattleItem))
			BattleItem->OnClicked.RemoveAll(this);
	}
	for (UNPBattleSelectionCharacterItem* CharacterItem : CharacterItems)
	{
		if (IsValid(CharacterItem))
			CharacterItem->OnClicked.RemoveAll(this);
	}
	SelectedBattleItem = nullptr;
	BattleItems.Empty();
	CharacterItems.Empty();
	SelectedCharacterSlots.Empty();

	Super::NativeDestruct();
}

void UNPBattleSelectionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bFadeActive)
		return;

	FadeElapsedTime += InDeltaTime;
	const float Alpha = FadeDuration > 0.f
		? FMath::Clamp(FadeElapsedTime / FadeDuration, 0.f, 1.f)
		: 1.f;
	SetRenderOpacity(FMath::Lerp(FadeStartOpacity, FadeTargetOpacity, Alpha));

	if (Alpha < 1.f)
		return;

	bFadeActive = false;
	if (bCloseAfterFade)
	{
		bCloseAfterFade = false;
		RequestCloseScreen();
	}
}

FReply UNPBattleSelectionWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		BeginCloseScreen();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UNPBattleSelectionWidget::OnBattleItemClicked(UObject* BattleItem)
{
	UNPBattleSelectionItem* SelectedItem = Cast<UNPBattleSelectionItem>(BattleItem);
	SetSelectedBattleItem(SelectedItem);
}

void UNPBattleSelectionWidget::OnCharacterItemClicked(UObject* CharacterItem)
{
	UNPBattleSelectionCharacterItem* SelectedItem = Cast<UNPBattleSelectionCharacterItem>(CharacterItem);
	if (!IsValid(SelectedItem))
	{
		NP_LOG(NPLog, Warning, TEXT("선택한 캐릭터 목록 항목이 유효하지 않습니다."));
		return;
	}

	const int32 ExistingIndex = SelectedCharacterSlots.IndexOfByKey(SelectedItem->CharacterId);
	if (ExistingIndex != INDEX_NONE)
	{
		SelectedCharacterSlots[ExistingIndex] = NAME_None;
		SelectedItem->bIsSelected = false;
		SelectedItem->SelectionOrder = 0;
	}
	else
	{
		const int32 EmptySlotIndex = SelectedCharacterSlots.IndexOfByPredicate(
			[](const FName& CharacterId) { return CharacterId.IsNone(); });
		if (EmptySlotIndex == INDEX_NONE)
		{
			NP_LOG(NPLog, Warning, TEXT("파티는 최대 %d명까지 선택할 수 있습니다."), MaxPartyMemberCount);
			return;
		}

		SelectedCharacterSlots[EmptySlotIndex] = SelectedItem->CharacterId;
		SelectedItem->bIsSelected = true;
		SelectedItem->SelectionOrder = EmptySlotIndex + 1;
	}

	UpdateCharacterSelectionState();
	BP_OnCharacterSelectionChanged(GetPartyCharacterIds());
	UpdateEntryButtonState();
}

void UNPBattleSelectionWidget::UpdateCharacterList()
{
	CharacterItems.Empty();
	SelectedCharacterSlots.Init(NAME_None, MaxPartyMemberCount);

	if (!IsValid(CharacterList))
	{
		NP_LOG(NPLog, Warning, TEXT("캐릭터 선택 목록 CharacterList가 바인딩되어 있지 않습니다."));
		return;
	}

	CharacterList->ClearListItems();

	const UNPGameDataSubsystem* GameDataSubsystem = UNPGameDataSubsystem::GetChecked(this);
	const UDataTable* PlayerDataTable = GameDataSubsystem->GetPlayerDataTable();
	if (!IsValid(PlayerDataTable))
	{
		NP_LOG(NPLog, Warning, TEXT("플레이어 데이터테이블이 설정되어 있지 않습니다."));
		return;
	}

	TArray<FName> CharacterIds;
	PlayerDataTable->GetRowMap().GetKeys(CharacterIds);
	CharacterIds.Sort(FNameLexicalLess());

	for (const FName& CharacterId : CharacterIds)
	{
		const FNPCharacterData* CharacterData = PlayerDataTable->FindRow<FNPCharacterData>(
			CharacterId, TEXT("BattleSelection"), false);
		if (!CharacterData)
			continue;

		UNPBattleSelectionCharacterItem* CharacterItem = NewObject<UNPBattleSelectionCharacterItem>(this);
		CharacterItem->CharacterId = CharacterId;
		CharacterItem->CharacterName = CharacterData->CharacterName;
		if (UClass* CharacterClass = CharacterData->CharacterClass.LoadSynchronous())
		{
			if (ANPPlayerCharacterBase* CharacterDefaultObject = Cast<ANPPlayerCharacterBase>(CharacterClass->GetDefaultObject()))
			{
				CharacterItem->ThumbnailTexture = CharacterDefaultObject->GetCharacterSoftTexture();
			}
		}
		CharacterItem->bIsSelected = false;
		CharacterItem->bIsSelectionLocked = false;
		CharacterItem->SelectionOrder = 0;
		CharacterItem->OnClicked.AddUObject(this, &UNPBattleSelectionWidget::OnCharacterItemClicked);

		CharacterItems.Add(CharacterItem);
		CharacterList->AddItem(CharacterItem);
	}

	UpdateCharacterSelectionState();
	BP_OnCharacterSelectionChanged(GetPartyCharacterIds());
}

void UNPBattleSelectionWidget::UpdateCharacterSelectionState()
{
	const bool bPartyFull = !SelectedCharacterSlots.Contains(NAME_None);
	for (UNPBattleSelectionCharacterItem* CharacterItem : CharacterItems)
	{
		if (!IsValid(CharacterItem))
			continue;

		CharacterItem->bIsSelectionLocked = bPartyFull && !CharacterItem->bIsSelected;
		CharacterItem->OnSelectionStateChanged.Broadcast();
	}
}

void UNPBattleSelectionWidget::SetSelectedBattleItem(UNPBattleSelectionItem* BattleItem)
{
	if (!IsValid(BattleItem))
	{
		NP_LOG(NPLog, Warning, TEXT("선택한 전투 목록 항목이 유효하지 않습니다."));
		return;
	}

	UNPBattleSelectionItem* PreviousBattleItem = SelectedBattleItem;
	if (IsValid(PreviousBattleItem))
		PreviousBattleItem->bIsSelected = false;

	SelectedBattleItem = BattleItem;
	SelectedBattleItem->bIsSelected = true;

	if (IsValid(BattleList))
	{
		if (IsValid(PreviousBattleItem))
		{
			if (UNPBattleSelectionListEntryWidget* PreviousEntry = Cast<UNPBattleSelectionListEntryWidget>(
				BattleList->GetEntryWidgetFromItem(PreviousBattleItem)))
			{
				PreviousEntry->RefreshSelectionVisual();
			}
		}
		if (UNPBattleSelectionListEntryWidget* SelectedEntry = Cast<UNPBattleSelectionListEntryWidget>(
			BattleList->GetEntryWidgetFromItem(SelectedBattleItem)))
		{
			SelectedEntry->RefreshSelectionVisual();
		}
	}
	BP_OnBattleStageSelected(BattleItem);
	UpdateEntryButtonState();
}

void UNPBattleSelectionWidget::UpdateEntryButtonState()
{
	if (IsValid(EntryButton))
		EntryButton->SetIsEnabled(IsValid(SelectedBattleItem) && !GetPartyCharacterIds().IsEmpty());
}

TArray<FName> UNPBattleSelectionWidget::GetPartyCharacterIds() const
{
	TArray<FName> PartyCharacterIds;
	PartyCharacterIds.Reserve(SelectedCharacterSlots.Num());
	for (const FName& CharacterId : SelectedCharacterSlots)
	{
		if (!CharacterId.IsNone())
			PartyCharacterIds.Add(CharacterId);
	}

	return PartyCharacterIds;
}

void UNPBattleSelectionWidget::StartFade(float TargetOpacity, bool bCloseWhenFinished)
{
	FadeElapsedTime = 0.f;
	FadeStartOpacity = GetRenderOpacity();
	FadeTargetOpacity = FMath::Clamp(TargetOpacity, 0.f, 1.f);
	bCloseAfterFade = bCloseWhenFinished;
	bFadeActive = FadeDuration > 0.f && !FMath::IsNearlyEqual(FadeStartOpacity, FadeTargetOpacity);

	if (!bFadeActive)
	{
		SetRenderOpacity(FadeTargetOpacity);
		if (bCloseAfterFade)
		{
			bCloseAfterFade = false;
			RequestCloseScreen();
		}
	}
}

void UNPBattleSelectionWidget::BeginCloseScreen()
{
	if (bCloseAfterFade)
		return;

	SetIsEnabled(false);
	StartFade(0.f, true);
}

void UNPBattleSelectionWidget::OnEntryButtonClicked()
{
	if (!IsValid(SelectedBattleItem))
	{
		NP_LOG(NPLog, Warning, TEXT("진입할 전투 스테이지가 선택되어 있지 않습니다."));
		return;
	}

	RequestEnterBattle(SelectedBattleItem->BattleStageId);
}

void UNPBattleSelectionWidget::RequestEnterBattle(const FName& BattleStageId)
{
	if (BattleStageId.IsNone())
	{
		NP_LOG(NPLog, Warning, TEXT("전투 진입에 사용할 스테이지 ID가 유효하지 않습니다."));
		return;
	}

	const TArray<FName> PartyCharacterIds = GetPartyCharacterIds();
	if (PartyCharacterIds.IsEmpty())
	{
		NP_LOG(NPLog, Warning, TEXT("전투에 진입할 캐릭터가 선택되어 있지 않습니다."));
		return;
	}

	const FNPGameFlowCommand EnterStageCommand = FNPGameFlowCommand::Make(
		FNPGameFlowCommandOptions::Make(false, true, 1.f),
		FNPEnterStageHandlerData::Make(ENPStageType::Battle, BattleStageId, PartyCharacterIds));
	if (UNPGameFlowSubsystem::GetChecked(this)->RequestExecuteCommand(EnterStageCommand))
	{
		// 진입 요청이 수락되면 페이드 후 선택 화면을 닫고 게임 입력을 복원한다.
		BeginCloseScreen();
	}
	else
	{
		NP_LOG(NPLog, Warning, TEXT("전투 스테이지 [%s] 진입 요청이 거절되었습니다."), *BattleStageId.ToString());
	}
}
