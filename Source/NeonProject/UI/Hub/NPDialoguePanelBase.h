// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPDialoguePanelBase.generated.h"

struct FNPDialogueLine;

UCLASS()
class NEONPROJECT_API UNPDialoguePanelBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetLines(const TArray<FNPDialogueLine>& LinesRef);
	void ShowDialogue();
	void HideDialogue();
	void AdvanceDialogue();

private:
	void SetSpeakerName(const FText& str);
	void SetDialogueText(const FText& str);

	void DialogueEnded();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SpeakerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DialogueText;

	TArray<FNPDialogueLine> Lines;
	int32 NextIndex = 0;
};
