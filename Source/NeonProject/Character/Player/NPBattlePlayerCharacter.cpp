// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/NPBattlePlayerCharacter.h"

#include "Component/NPCombatComponent.h"
#include "Interface/NPInputCommandReceiver.h"

#include "Components/CapsuleComponent.h"

ANPBattlePlayerCharacter::ANPBattlePlayerCharacter()
{
	CombatComp = CreateDefaultSubobject<UNPCombatComponent>(TEXT("CombatComp"));

}

void ANPBattlePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CombatComp->InitializeCombat(this);

	CombatComp->OnSkillPlayed.AddUObject(this, &ANPBattlePlayerCharacter::HandleSkillPlayed);

	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &ANPBattlePlayerCharacter::OnMontageBlendingOut);
}

float ANPBattlePlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

void ANPBattlePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	EnumAddFlags(State, ENPCharacterState::Active);
}

void ANPBattlePlayerCharacter::UnPossessed()
{
	Super::UnPossessed();
	EnumRemoveFlags(State, ENPCharacterState::Active);
}


void ANPBattlePlayerCharacter::Die()
{
	Super::Die();

	// SwapNext
}

void ANPBattlePlayerCharacter::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (HasAnyState(ENPCharacterState::Dead)) return;

	if (!HasAnyState(ENPCharacterState::Active))
		CharacterHiddenWithDissolve(true, SwapDissolveDuration);

}

void ANPBattlePlayerCharacter::OnMoveForwardInput(FVector ForwardVector, float ForwardInput)
{
	Super::OnMoveForwardInput(ForwardVector, ForwardInput);

	CombatComp->HandleMoveInput();
}

void ANPBattlePlayerCharacter::OnMoveRightInput(FVector RightVector, float RightInput)
{
	Super::OnMoveRightInput(RightVector, RightInput);

	CombatComp->HandleMoveInput();
}

void ANPBattlePlayerCharacter::OnMoveStop()
{
	Super::OnMoveStop();
}

bool ANPBattlePlayerCharacter::HandleInputCommand(AController* InstigatorController, FNPInputCommand Command)
{
	bool bSucceeded = Super::HandleInputCommand(InstigatorController, Command);

	if (!CombatComp) return bSucceeded;


	switch (Command.CommandType)
	{
	case ENPInputCommandType::Attack:
	{
		CombatComp->HandleAttackInput();
		bSucceeded = true;
		break;
	}
	case ENPInputCommandType::Dash:
	{
		CombatComp->HandleDashInput();
		bSucceeded = true;
		break;
	}
	case ENPInputCommandType::Ability:
	{
		CombatComp->TryExecuteAbility(Command.AbilityType);
		bSucceeded = true;
		break;
	}
	default:
		break;
	}

	return bSucceeded;
}

void ANPBattlePlayerCharacter::HandleSkillPlayed(ENPAbilityType AbilityType, uint8 Combo, bool bExtra)
{

}

void ANPBattlePlayerCharacter::SwapIn(const FTransform& NewTransform)
{
	if (IsHidden())
	{
		SetActorTransform(NewTransform);
		//CombatComp->OnSwapIn();
	}

	CharacterActivate();
}

void ANPBattlePlayerCharacter::SwapOut()
{
	CharacterDeativate();
}

void ANPBattlePlayerCharacter::CharacterActivate()
{

	AddState(ENPCharacterState::Active);

	CharacterHiddenWithDissolve(false, SwapDissolveDuration);
	// 등장 애니메이션 재생이나 위치보정 등
}

void ANPBattlePlayerCharacter::CharacterDeativate()
{
	RemoveState(ENPCharacterState::Active);

	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr)) return;

	CharacterHiddenWithDissolve(true, SwapDissolveDuration);
}

void ANPBattlePlayerCharacter::CharacterHiddenWithDissolve(bool bCharHidden, float fDissolveDuration)
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
