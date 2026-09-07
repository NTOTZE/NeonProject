// Fill out your copyright notice in the Description page of Project Settings.


#include "Dialogue/NPDialogueSubsystem.h"
#include "DataAsset/NPDialogueDataAsset.h"
#include "DataType/NPDialogueTypes.h"


UNPDialogueSubsystem::UNPDialogueSubsystem()
{
	ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(TEXT("/Script/Engine.DataTable'/Game/NeonProject/Blueprint/DataTable/DT_DialogueTable.DT_DialogueTable'"));
	if (TableFinder.Succeeded())
	{
		DialogueTable = TableFinder.Object;
	}
}

UNPDialogueDataAsset* UNPDialogueSubsystem::FindDialogueData(const FName& InDialogueId)
{
	if (DialogueTable == nullptr)
	{
		return nullptr;
	}
	const FNPDialogueRegistryRow* FoundRow = DialogueTable->FindRow<FNPDialogueRegistryRow>(InDialogueId, TEXT("FindDialogue"));
	if (FoundRow == nullptr)
	{
		return nullptr;
	}

	return FoundRow->DialogueAsset.LoadSynchronous();
}
