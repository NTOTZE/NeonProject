// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controller/NPPlayerControllerBase.h"
#include "NeonProject.h"
#include "Character/Player/NPPlayerCharacterBase.h"
#include "Core/PlayerSettings.h"

#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ANPPlayerControllerBase::ANPPlayerControllerBase()
{

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_CharacterFinder
		(TEXT("/Game/NeonProject/Input/CharacterMovement/IMC_CharacterMovement.IMC_CharacterMovement"));
	if (IMC_CharacterFinder.Succeeded())
	{
		IMC_Character = IMC_CharacterFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveForwardFinder
		(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_MoveForward.IA_MoveForward"));
	if (IA_MoveForwardFinder.Succeeded())
	{
		IA_MoveForward = IA_MoveForwardFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveRightFinder
		(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_MoveRight.IA_MoveRight"));
	if (IA_MoveRightFinder.Succeeded())
	{
		IA_MoveRight = IA_MoveRightFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LookUpFinder
		(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_LookUp.IA_LookUp"));
	if (IA_LookUpFinder.Succeeded())
	{
		IA_LookUp = IA_LookUpFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_DashFinder
		(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_Dash.IA_Dash"));
	if (IA_DashFinder.Succeeded())
	{
		IA_Dash = IA_DashFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ScrollFinder
		(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_Scroll.IA_Scroll"));
	if (IA_ScrollFinder.Succeeded())
	{
		IA_Scroll = IA_ScrollFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_EscapeFinder
		(TEXT("/Script/EnhancedInput.InputAction'/Game/NeonProject/Input/CharacterMovement/IA_Escape.IA_Escape'"));
	if (IA_EscapeFinder.Succeeded())
	{
		IA_Escape = IA_EscapeFinder.Object;
	}


	DefaultControlRotation = FRotator(-20.f, 0.f, 0.f);
}

void ANPPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(IMC_Character, 0);
		}
	}

	PlayerCameraManager->PrimaryActorTick.bTickEvenWhenPaused = true;
	PlayerCameraManager->ViewPitchMax = 85.f;
	PlayerCameraManager->ViewPitchMin = -85.f;

	ControlRotation = DefaultControlRotation;
	CachedControlRotation = ControlRotation;
	CachedMouseSensitivity = GetDefault<UPlayerSettings>()->MouseSensitivity;
}

void ANPPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComp->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &ANPPlayerControllerBase::HandleMoveForwardInput);
		EnhancedInputComp->BindAction(IA_MoveForward, ETriggerEvent::Completed, this, &ANPPlayerControllerBase::HandleMoveForwardInput);
		EnhancedInputComp->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &ANPPlayerControllerBase::HandleMoveRightInput);
		EnhancedInputComp->BindAction(IA_MoveRight, ETriggerEvent::Completed, this, &ANPPlayerControllerBase::HandleMoveRightInput);
		EnhancedInputComp->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &ANPPlayerControllerBase::HandleLookUpInput);
		EnhancedInputComp->BindAction(IA_Dash, ETriggerEvent::Triggered, this, &ANPPlayerControllerBase::TryExecuteInputCommand, FNPInputCommand::MakeDash());
		EnhancedInputComp->BindAction(IA_Scroll, ETriggerEvent::Triggered, this, &ANPPlayerControllerBase::HandleScrollInput);
		EnhancedInputComp->BindAction(IA_Escape, ETriggerEvent::Triggered, this, &ANPPlayerControllerBase::HandleEscapeInput);
	}
}

void ANPPlayerControllerBase::TryExecuteInputCommand(FNPInputCommand Command)
{
	ExecuteInputCommand(Command);
}

bool ANPPlayerControllerBase::ExecuteInputCommand(FNPInputCommand Command)
{
	INPInputCommandReceiver* Receiver = nullptr;
	bool bSucceeded = false;

	switch (Command.Receiver)
	{
		case ENPInputCommandReceiver::Character:
		{
			ANPCharacterBase* PlayerCharacter = Cast<ANPCharacterBase>(GetCharacter());
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

void ANPPlayerControllerBase::HandleMoveForwardInput(const FInputActionValue& Value)
{
	ANPPlayerCharacterBase* PlayerCharacter = Cast<ANPPlayerCharacterBase>(GetCharacter());
	if (!PlayerCharacter)
		return;

	const FVector2D VectorValue = Value.Get<FVector2D>();
	const float ForwardInput = VectorValue.X - VectorValue.Y;

	const float ControlYaw = GetControlRotation().Yaw;
	const FVector ForwardVector = FRotator(0.f, ControlYaw, 0.f).RotateVector(FVector(1.f, 0.f, 0.f));

	PlayerCharacter->OnMoveForwardInput(ForwardVector, ForwardInput);
}

void ANPPlayerControllerBase::HandleMoveRightInput(const FInputActionValue& Value)
{
	ANPPlayerCharacterBase* PlayerCharacter = Cast<ANPPlayerCharacterBase>(GetCharacter());
	if (!PlayerCharacter)
		return;

	const FVector2D VectorValue = Value.Get<FVector2D>();
	const float RightInput = VectorValue.X - VectorValue.Y;

	const float ControlYaw = GetControlRotation().Yaw;
	const FVector RightVector = FRotator(0.f, ControlYaw, 0.f).RotateVector(FVector(0.f, 1.f, 0.f));
	PlayerCharacter->OnMoveRightInput(RightVector, RightInput);
}

void ANPPlayerControllerBase::HandleLookUpInput(const FInputActionValue& Value)
{
	const FVector2D VectorValue = Value.Get<FVector2D>();

	AddYawInput(VectorValue.X * CachedMouseSensitivity);
	AddPitchInput(-VectorValue.Y * CachedMouseSensitivity);
}

void ANPPlayerControllerBase::HandleScrollInput(const FInputActionValue& Value)
{
	NP_LOG(NPLog, Warning, TEXT(""));
}

void ANPPlayerControllerBase::HandleEscapeInput()
{
	NP_LOG(NPLog, Warning, TEXT(""));
}
