// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controller/NPHubPlayerController.h"
#include "NeonProject.h"
#include "Interface/NPInputCommandReceiver.h"
#include "UI/Hub/NPHubHUDBase.h"
#include "Component/NPInteractionComponent.h"
#include "Screen/Widget/NPScreenFadeWidgetBase.h"

#include "GameFlow/NPGameFlowCommand.h"
#include "DataType/NPStageData.h"
#include "GameFlow/NPGameFlowSubsystem.h"
#include "GameFlow/Handler/NPEnterStageHandler.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"

ANPHubPlayerController::ANPHubPlayerController()
{
	ConstructorHelpers::FObjectFinder<UInputAction> IA_InteractionFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_Interaction.IA_Interaction"));
	if (IA_InteractionFinder.Succeeded())
	{
		IA_Interaction = IA_InteractionFinder.Object;
	}

	ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder
	(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/NeonProject/Blueprint/UI/Hub/WBP_HubHUD.WBP_HubHUD_C'"));
	if (WidgetFinder.Succeeded())
	{
		HubHUDClass = WidgetFinder.Class;
	}

	InteractionComp = CreateDefaultSubobject<UNPInteractionComponent>(TEXT("InteractionComp"));

}

void ANPHubPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HubHUDClass))
	{
		HubHUD = Cast<UNPHubHUDBase>(CreateWidget(GetWorld(), HubHUDClass));
	}

	if (IsValid(HubHUD))
	{
		HubHUD->AddToViewport();
		//HubHUD->GetScreenFadeWidget()->ScreenFadeIn(1.f);
	}

	if (InteractionComp)
	{
		InteractionComp->OnInteractableActorsChanged.AddUObject(this, &ANPHubPlayerController::OnInteractableActorsChanged);
	}
}

void ANPHubPlayerController::SetDialogueData(const UNPDialogueDataAsset* DialogueDA)
{
	HubHUD->SetDialogueData(DialogueDA);
}

void ANPHubPlayerController::ShowDialogue()
{
	HubHUD->ShowDialogue();
}

void ANPHubPlayerController::HideDialogue()
{
	HubHUD->HideDialogue();
}

void ANPHubPlayerController::OnInteractableActorsChanged(const TArray<TWeakObjectPtr<class ANPInteractableActorBase>>& interactableActors)
{
	if (!IsValid(HubHUD)) return;

	HubHUD->UpdateInteractionItems(interactableActors);
}

void ANPHubPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComp->BindAction(IA_Interaction, ETriggerEvent::Triggered, this,
			&ANPHubPlayerController::HandleInteractionInput);
	}
}

bool ANPHubPlayerController::ExecuteInputCommand(FNPInputCommand Command)
{
	bool bSucceeded = Super::ExecuteInputCommand(Command);
	if (bSucceeded)
		return true;

	INPInputCommandReceiver* Receiver = nullptr;

	switch (Command.Receiver)
	{
		case ENPInputCommandReceiver::Character:
		{
			ACharacter* PlayerCharacter = GetCharacter();
			if (PlayerCharacter)
			{
				Receiver = Cast<INPInputCommandReceiver>(PlayerCharacter);
			}

			break;
		}
	}

	if (Receiver)
	{
		bSucceeded = Receiver->HandleInputCommand(this, Command);
	}

	return bSucceeded;
}

void ANPHubPlayerController::HandleInteractionInput()
{
	bool bSucceeded = InteractionComp->ExecuteFocusedInteraction(this);

	if (bSucceeded)
	{
		NP_LOG(NPLog, Warning, TEXT("Interaction Succeeded."));




	}
	else
	{
		NP_LOG(NPLog, Warning, TEXT("Interaction failed."));
	}
}

void ANPHubPlayerController::HandleScrollInput(const FInputActionValue& Value)
{
	if (InteractionComp->NumInteractableActor() <= 0)
	{
		Super::HandleScrollInput(Value);
		return;
	}

	float ScrollValue = Value.Get<float>();

	if (ScrollValue > 0.f)
		InteractionComp->ShiftFocus(-1);
	else if (ScrollValue < 0.f)
		InteractionComp->ShiftFocus(1);
}

void ANPHubPlayerController::HandleEscapeInput()
{
	Super::HandleEscapeInput();

	FNPGameFlowCommand EnterStageCommand = FNPGameFlowCommand::Make(
		FNPGameFlowCommandOptions::Make(true, true, 1.f),
		FNPEnterStageHandlerData::Make(ENPStageType::Battle, TEXT("Battle0001"))
	);
	UNPGameFlowSubsystem::GetChecked(this)->RequestExecuteCommand(EnterStageCommand);
}

