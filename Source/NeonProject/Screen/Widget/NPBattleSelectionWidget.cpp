// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen/Widget/NPBattleSelectionWidget.h"
#include "Components\ListView.h"
#include "GameFlow/Handler/NPEnterStageHandler.h"
#include "GameFlow/NPGameFlowCommand.h"
#include "GameFlow/NPGameFlowSubsystem.h"
#include "GameData/NPGameDataSubsystem.h"
#include "NeonProject.h"
#include "Input/Reply.h"

void UNPBattleSelectionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetIsFocusable(true);

	if (IsValid(BattleList))
		BattleList->OnItemClicked().AddUObject(this, &UNPBattleSelectionWidget::OnBattleItemClicked);
}

void UNPBattleSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(BattleList))
	{
		NP_LOG(NPLog, Warning, TEXT("전투 선택 목록 BattleList가 바인딩되어 있지 않습니다."));
		return;
	}

	BattleList->ClearListItems();
	SelectedBattleItem = nullptr;

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
		BattleList->AddItem(BattleItem);
	}

	if (BattleList->GetNumItems() > 0)
	{
		UNPBattleSelectionItem* FirstBattleItem = Cast<UNPBattleSelectionItem>(BattleList->GetItemAt(0));
		BattleList->SetSelectedItem(FirstBattleItem);
		SetSelectedBattleItem(FirstBattleItem);
	}
}

void UNPBattleSelectionWidget::NativeDestruct()
{
	if (IsValid(BattleList))
		BattleList->OnItemClicked().RemoveAll(this);
	SelectedBattleItem = nullptr;

	Super::NativeDestruct();
}

FReply UNPBattleSelectionWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		RequestCloseScreen();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UNPBattleSelectionWidget::OnBattleItemClicked(UObject* BattleItem)
{
	UNPBattleSelectionItem* SelectedItem = Cast<UNPBattleSelectionItem>(BattleItem);
	SetSelectedBattleItem(SelectedItem);
	if (IsValid(SelectedItem))
		RequestEnterBattle(SelectedItem->BattleStageId);
}

void UNPBattleSelectionWidget::SetSelectedBattleItem(UNPBattleSelectionItem* BattleItem)
{
	if (!IsValid(BattleItem))
	{
		NP_LOG(NPLog, Warning, TEXT("선택한 전투 목록 항목이 유효하지 않습니다."));
		return;
	}

	SelectedBattleItem = BattleItem;
	BP_OnBattleStageSelected(BattleItem);
}

void UNPBattleSelectionWidget::RequestEnterBattle(const FName& BattleStageId)
{
	if (BattleStageId.IsNone())
	{
		NP_LOG(NPLog, Warning, TEXT("전투 진입에 사용할 스테이지 ID가 유효하지 않습니다."));
		return;
	}

	const TArray<FName> PartyCharacterIds = {
		TEXT("Player0001"),
		TEXT("Player0002"),
		TEXT("Player0003"),
	};
	const TArray<FName> MonsterIds = {
		TEXT("Monster0001"),
		TEXT("Monster0002"),
		TEXT("Monster0003"),
	};

	const FNPGameFlowCommand EnterStageCommand = FNPGameFlowCommand::Make(
		FNPGameFlowCommandOptions::Make(false, true, 1.f),
		FNPEnterStageHandlerData::Make(ENPStageType::Battle, BattleStageId, PartyCharacterIds, MonsterIds));
	if (UNPGameFlowSubsystem::GetChecked(this)->RequestExecuteCommand(EnterStageCommand))
	{
		// 진입 요청이 수락되면 선택 화면을 닫고 게임 입력을 복원한다.
		RequestCloseScreen();
	}
	else
	{
		NP_LOG(NPLog, Warning, TEXT("전투 스테이지 [%s] 진입 요청이 거절되었습니다."), *BattleStageId.ToString());
	}
}
