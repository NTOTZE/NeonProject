// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/NPPlayerCharacterBase.h"
#include "NeonProject.h"

#include "Component/NPCombatComponent.h"
#include "Core/Controller/NPCameraRig.h"
#include "Core/Controller/NPBattlePlayerController.h"
#include "DataType/NPInputCommandTypes.h"

#include "Misc/EnumClassFlags.h"
#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ANPPlayerCharacterBase::ANPPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionProfileName(NPCharacterCollisionProfileName[(uint8)ENPCharacterCollisionType::PlayerCapsule]);

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 170.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	DissolveDelayAfterDeath = 1.5f;
}

void ANPPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	OnStateChange.AddUObject(this, &ANPPlayerCharacterBase::HandleStateChange);

	InitDissolve();
	SetDissolveAppearance(0.f);

}

void ANPPlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANPPlayerCharacterBase::OnMoveForwardInput(FVector ForwardVector, float ForwardInput)
{
	if (FMath::IsNearlyZero(ForwardInput))
	{
		if (FMath::IsNearlyZero(MoveInput.Y) &&
			!FMath::IsNearlyZero(MoveInput.X))
		{
			OnMoveStop();
		}
		MoveInput.X = 0.f;
		return;
	}
	MoveInput.X = ForwardInput;

	AddState(ENPCharacterState::Move);
	AddMovementInput(ForwardVector, ForwardInput);
}

void ANPPlayerCharacterBase::OnMoveRightInput(FVector RightVector, float RightInput)
{
	if (FMath::IsNearlyZero(RightInput))
	{
		if (FMath::IsNearlyZero(MoveInput.X) &&
			!FMath::IsNearlyZero(MoveInput.Y))
		{
			OnMoveStop();
		}
		MoveInput.Y = 0.f;
		return;
	}
	MoveInput.Y = RightInput;

	AddState(ENPCharacterState::Move);
	AddMovementInput(RightVector, RightInput);
}

void ANPPlayerCharacterBase::OnMoveStop()
{
	RemoveState(ENPCharacterState::Move | ENPCharacterState::Run);
}

void ANPPlayerCharacterBase::HandleStateChange(ENPCharacterState Flags, bool Value)
{
	if (EnumHasAnyFlags(Flags, ENPCharacterState::Run))
	{
		if (Value)
		{
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
	}
}

void ANPPlayerCharacterBase::NP_SeqEventTest(float BlendTime)
{
	NP_LOG(NPLog, Warning, TEXT("%f"), BlendTime);
	ANPBattlePlayerController* NPController = Cast<ANPBattlePlayerController>(GetController());
	if (NPController)
	{
		NPController->SetViewTargetToRig();
	}
}

bool ANPPlayerCharacterBase::HandleInputCommand(AController* InstigatorController, FNPInputCommand Command)
{
	bool bSucceeded = false;

	switch (Command.CommandType)
	{
	case ENPInputCommandType::Dash:
	{
		if (HasAnyState(ENPCharacterState::Move))
		{
			AddState(ENPCharacterState::Run);
		}
		bSucceeded = true;
	}
	default:
		break;
	}

	return bSucceeded;
}
