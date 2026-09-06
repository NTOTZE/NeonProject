// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Controller/NPPlayerControllerBase.h"
#include "NPHubPlayerController.generated.h"

class UNPDialogueDataAsset;

UCLASS()
class NEONPROJECT_API ANPHubPlayerController : public ANPPlayerControllerBase
{
	GENERATED_BODY()

public:
	ANPHubPlayerController();

	virtual void BeginPlay() override;

private:
	void SetDialogueData(const UNPDialogueDataAsset* DialogueDA);
	void ShowDialogue();
	void HideDialogue();

	// InteractionComp
	void OnInteractableActorsChanged(const TArray<TWeakObjectPtr<class ANPInteractableActorBase>>& interactableActors);

protected:
	virtual void SetupInputComponent() override;
	virtual bool ExecuteInputCommand(FNPInputCommand Command) override;
	void HandleInteractionInput();
	virtual void HandleScrollInput(const FInputActionValue& Value) override;
	virtual void HandleEscapeInput() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NP|Interaction")
	TObjectPtr<class UNPInteractionComponent> InteractionComp;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<class UInputAction> IA_Interaction;

private:
	UPROPERTY()
	TSubclassOf<class UUserWidget> HubHUDClass;

	UPROPERTY()
	TObjectPtr<class UNPHubHUDBase> HubHUD;


};
