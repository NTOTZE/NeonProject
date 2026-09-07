// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controller/NPHubPlayerController.h"
#include "NeonProject.h"
#include "Interface/NPInputCommandReceiver.h"
#include "UI/Hub/NPHubHUDBase.h"
#include "Component/NPInteractionComponent.h"
#include "Screen/Widget/NPScreenFadeWidgetBase.h"

#include "GameFlow/Command/Factory/NPEnterStageCommandFactory.h"
#include "GameFlow/NPGameFlowSettings.h"
#include "DataType/NPStageData.h"
#include "GameFlow/NPGameFlowSubsystem.h"
#include "Loading/NPStageSessionSubsystem.h"
#include "DataType/NPCharacterData.h"
#include "GameData/NPGameDataSubsystem.h"
#include "Character/NPCharacterBase.h"
#include "Character/Player/NPPlayerCharacterBase.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"

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

	UNPStageSessionSubsystem* StageSession = GetWorld()->GetSubsystem<UNPStageSessionSubsystem>();
	check(StageSession);
	StageSession->OnSessionReady.AddUObject(this, &ThisClass::HandleStageSessionReady);
	if (StageSession->IsSessionInitialized())
		HandleStageSessionReady();
}

void ANPHubPlayerController::HandleStageSessionReady()
{
	if (bHubCharacterSpawned)
		return;

	const UNPStageSessionSubsystem* StageSession = GetWorld()->GetSubsystem<UNPStageSessionSubsystem>();
	check(StageSession);
	const TArray<FName>& PartyCharacterIds = StageSession->GetSessionData().PartyCharacterIds;
	const FNPCharacterData* CharacterData = nullptr;
	UClass* CharacterClass = nullptr;
	FName CharacterId = NAME_None;
	if (!PartyCharacterIds.IsEmpty())
	{
		CharacterId = PartyCharacterIds[0];
		CharacterData = UNPGameDataSubsystem::GetPlayerData(this, CharacterId);
		CharacterClass = CharacterData ? CharacterData->CharacterClass.Get() : nullptr;
	}
	else
	{
		CharacterClass = UNPGameFlowSettings::GetChecked()->GetDefaultHubCharacterClass().Get();
	}
	if (!CharacterClass || !CharacterClass->IsChildOf(ANPCharacterBase::StaticClass()))
	{
		if (CharacterId.IsNone())
		{
			NP_LOG(NPLog, Warning, TEXT("기본 허브 캐릭터 클래스가 설정되어 있지 않거나 유효하지 않습니다."));
		}
		else
		{
			NP_LOG(NPLog, Warning, TEXT("허브 캐릭터 [%s]의 클래스가 유효하지 않습니다."), *CharacterId.ToString());
		}
		return;
	}

	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	AActor* StartPoint = GameMode ? GameMode->FindPlayerStart(this) : nullptr;
	const FTransform SpawnTransform = StartPoint ? StartPoint->GetActorTransform() : FTransform::Identity;
	ANPCharacterBase* HubCharacter = GetWorld()->SpawnActorDeferred<ANPCharacterBase>(CharacterClass, SpawnTransform);
	if (!HubCharacter)
	{
		NP_LOG(NPLog, Warning, TEXT("허브 캐릭터 [%s] 스폰에 실패했습니다."), CharacterId.IsNone() ? TEXT("기본 캐릭터") : *CharacterId.ToString());
		return;
	}
	if (CharacterData)
	{
		if (ANPPlayerCharacterBase* PlayerCharacter = Cast<ANPPlayerCharacterBase>(HubCharacter))
			PlayerCharacter->SetCharacterSoftTexture(CharacterData->ThumbnailTexture);
	}
	HubCharacter->FinishSpawning(SpawnTransform);

	Possess(HubCharacter);
	bHubCharacterSpawned = true;
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


	TArray<FName> PartyCharacterIds;
	PartyCharacterIds.Reserve(3);
	PartyCharacterIds.Add(TEXT("Player0001"));
	PartyCharacterIds.Add(TEXT("Player0002"));
	PartyCharacterIds.Add(TEXT("Player0003"));

	const FNPGameFlowCommand EnterStageCommand = NPEnterStageCommandFactory::MakeEnterStage(
		ENPStageType::Battle, TEXT("Battle0001"), PartyCharacterIds);
	UNPGameFlowSubsystem::GetChecked(this)->RequestExecuteCommand(EnterStageCommand);
}

