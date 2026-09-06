// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen/Widget/Element/NPBattleSelectionListEntryWidget.h"
#include "NeonProject.h"

void UNPBattleSelectionListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	NP_LOG(NPLog, Warning, TEXT(""));

	// Cached전투구성엑터 = Cast<전투 구성 엑터>(ListItemObject);
	// if(Cached전투구성엑터.IsValid())
	//	{
	//	전투 데이터 파싱
	//	}
}

void UNPBattleSelectionListEntryWidget::NativeOnEntryReleased()
{
	/*
	Cached전투구성엑터 메모리 해제
	
	*/

	NP_LOG(NPLog, Warning, TEXT(""));

	IUserObjectListEntry::NativeOnEntryReleased();
}
