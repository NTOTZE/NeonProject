// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NPPlayerCharacter.h"
#include "NeonProject.h"
#include "Component/NPCombatComponent.h"

#include "Misc/EnumClassFlags.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ANPPlayerCharacter::ANPPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CombatComp = CreateDefaultSubobject<UNPCombatComponent>(TEXT("CombatComp"));

	
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

void ANPPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitDissolve();
	SetDissolveAppearance(0.f);

	CombatComp->InitializeCombat(this);

	OnStateChange.AddUObject(this, &ANPPlayerCharacter::HandleStateChange);
	CombatComp->OnSkillPlayed.AddUObject(this, &ANPPlayerCharacter::HandleSkillPlayed);
}

void ANPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


float ANPPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Applied = FMath::Max(0.f, DamageAmount);

	if (EnumHasAnyFlags(State, ENPCharacterState::Invincible))
	{
		Applied = 0.f;
		CombatComp->OnAttackIgnored(EventInstigator);
	}

	const float SuperReturned = Super::TakeDamage(Applied, DamageEvent, EventInstigator, DamageCauser);

	return SuperReturned;
}

void ANPPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	EnumAddFlags(State, ENPCharacterState::Active);
}

void ANPPlayerCharacter::UnPossessed()
{
	Super::UnPossessed();
	EnumRemoveFlags(State, ENPCharacterState::Active);
}


void ANPPlayerCharacter::Die()
{
	Super::Die();

	// SwapNext
}

void ANPPlayerCharacter::OnMoveForwardInput(FVector ForwardVector, float ForwardInput)
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
	CombatComp->HandleMoveInput();
}

void ANPPlayerCharacter::OnMoveRightInput(FVector RightVector, float RightInput)
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
	CombatComp->HandleMoveInput();
}

void ANPPlayerCharacter::OnMoveStop()
{
	RemoveState(ENPCharacterState::Move | ENPCharacterState::Run);
}

void ANPPlayerCharacter::HandleStateChange(ENPCharacterState Flags, bool Value)
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

void ANPPlayerCharacter::HandleSkillPlayed(ENPAbilityType AbilityType, uint8 Combo, bool bExtra)
{
	if (AbilityType == ENPAbilityType::Dash && Combo == 1 && bExtra == false)
	{
		AddState(ENPCharacterState::Run);
	}
	else
	{
		RemoveState(ENPCharacterState::Run);
	}
}

void ANPPlayerCharacter::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageBlendingOut(Montage, bInterrupted);

	if (HasAnyState(ENPCharacterState::Dead)) return;

	if (!HasAnyState(ENPCharacterState::Active))
		CharacterHiddenWithDissolve(true, SwapDissolveDuration);

}

bool ANPPlayerCharacter::TryExecuteAbility(ENPAbilityType AbilityType)
{
	if (!CombatComp) return false;

	return CombatComp->TryExecuteAbility(AbilityType);
}

void ANPPlayerCharacter::OnAttackInput()
{
	if (!CombatComp) return;

	CombatComp->HandleAttackInput();
}

void ANPPlayerCharacter::OnDashInput()
{
	if (!CombatComp) return;

	CombatComp->HandleDashInput();
}

void ANPPlayerCharacter::CharacterHiddenWithDissolve(bool bCharHidden, float fDissolveDuration)
{
	if (bCharHidden)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorTickEnabled(false);
		PlayDissolve(false, SwapDissolveDuration);
	}
	else
	{
		PlayDissolve(true, SwapDissolveDuration);
		SetActorTickEnabled(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ANPPlayerCharacter::CharacterActivate()
{

	AddState(ENPCharacterState::Active);
	
	CharacterHiddenWithDissolve(false, SwapDissolveDuration);
	// 등장 애니메이션 재생이나 위치보정 등
}

void ANPPlayerCharacter::CharacterDeativate()
{
	RemoveState(ENPCharacterState::Active);

	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr)) return;

	CharacterHiddenWithDissolve(true, SwapDissolveDuration);
}

void ANPPlayerCharacter::SwapIn(const FTransform& NewTransform)
{
	if (IsHidden())
	{
		SetActorTransform(NewTransform);
		//CombatComp->OnSwapIn();
	}

	CharacterActivate();
}

void ANPPlayerCharacter::SwapOut()
{
	CharacterDeativate();
}
