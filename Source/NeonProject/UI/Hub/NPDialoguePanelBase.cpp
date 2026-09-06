// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Hub/NPDialoguePanelBase.h"
#include "DataType/NPDialogueTypes.h"

#include "Components/TextBlock.h"

void UNPDialoguePanelBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	SetSpeakerName(FText());
	SetDialogueText(FText());
}

void UNPDialoguePanelBase::SetLines(const TArray<FNPDialogueLine>& LinesRef)
{
	Lines = LinesRef;
	NextIndex = 0;

	SetSpeakerName(FText());
	SetDialogueText(FText());
}

void UNPDialoguePanelBase::ShowDialogue()
{
	AdvanceDialogue();
	SetVisibility(ESlateVisibility::Visible);
}

void UNPDialoguePanelBase::HideDialogue()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UNPDialoguePanelBase::AdvanceDialogue()
{
	if (Lines.IsValidIndex(NextIndex))
	{
		SetSpeakerName(Lines[NextIndex].SpeakerName);
		SetDialogueText(Lines[NextIndex].DialogueText);
		NextIndex = Lines[NextIndex].NextIndex;
	}
	else
	{
		DialogueEnded();
	}
}

void UNPDialoguePanelBase::SetSpeakerName(const FText& str)
{
	SpeakerName->SetText(str);
}

void UNPDialoguePanelBase::SetDialogueText(const FText& str)
{
	DialogueText->SetText(str);
}

void UNPDialoguePanelBase::DialogueEnded()
{
	Lines.Empty();
	NextIndex = 0;

	SetSpeakerName(FText());
	SetDialogueText(FText());
}
