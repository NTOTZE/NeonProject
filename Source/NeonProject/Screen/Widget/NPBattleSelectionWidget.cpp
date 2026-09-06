// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen/Widget/NPBattleSelectionWidget.h"
#include "Components\ListView.h"

#include "NeonProject.h"

void UNPBattleSelectionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//initialized 혹은 Construct에서 BattleList의
	//아이템 클릭(하이라이트)에 대한 델리게이트 함수 바인딩

	BattleList->OnItemClicked().AddUObject(this, &UNPBattleSelectionWidget::OnBattleItemClicked);
}

void UNPBattleSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//construct에서든 따로 함수를 두든
	//전투 목록을 전투ID 형태로 받아와 
	//BattleList에 넣어주는 기능 필요
}

void UNPBattleSelectionWidget::NativeDestruct()
{
	//리스트 항목 삭제?
	//BattleList->ClearListItems();

	Super::NativeDestruct();
}

void UNPBattleSelectionWidget::OnBattleItemClicked(UObject* BattleItem)
{
	NP_LOG(NPLog, Warning, TEXT(""));
}
