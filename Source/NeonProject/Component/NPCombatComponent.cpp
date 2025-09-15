// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NPCombatComponent.h"
#include "NeonProject.h"
#include "Character/NPPlayerCharacter.h"
#include "Core/Controller/NPPlayerController.h"

#include "Engine/OverlapResult.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UNPCombatComponent::UNPCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UNPCombatComponent::InitializeCombat(ANPPlayerCharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
	OwnerCharacter->GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &UNPCombatComponent::OnSkillEnded);
	CurrentSkill = ENPAbilityType::None;
	for (auto SkillData : SkillDataMap)
	{
		if (!SkillData.Value)
		{
			SkillDataMap.Remove(SkillData.Key);
			continue;
		}
		RegisterAbility(SkillData.Value);
	}
}

void UNPCombatComponent::RegisterAbility(const UNPSkillData* Skill)
{
	if (!Skill) return;

	FNPAbilityRuntime& R = Abilities.FindOrAdd(Skill->AbilityType);
	R.Data = Skill;
}

const UNPSkillData* UNPCombatComponent::FindAbility(ENPAbilityType AbilityType) const
{
	if (const FNPAbilityRuntime* Found = Abilities.Find(AbilityType))
	{
		return Found->Data;
	}
	return nullptr;
}

bool UNPCombatComponent::TryExecuteAbility(ENPAbilityType AbilityType, bool bExtra)
{
	const UNPSkillData* Skill = FindAbility(AbilityType);
	if (!OwnerCharacter || !Skill) return false;
	
	//콤보 윈도우 체크
	if (CurrentSkill != ENPAbilityType::None && bComboWindowOpen == false)
	{
		SetBufferedSkill(AbilityType);
		return false;
	}

	// 쿨다운 체크
	if (CurrentSectionIndex == 0 && IsOnCooldown(Skill))
	{
		NP_LOG(NPLog, Warning, TEXT("쿨다운 중."));
		return false;
	}

	// 방향
	FRotator TargetRot;
	const FVector InputVector = OwnerCharacter->GetLastMovementInputVector();

	if (!InputVector.IsNearlyZero() && !Skill->bAutoTarget)
		TargetRot = InputVector.Rotation();
	else
		TargetRot = GetTargetRotation();

	if (!ExecuteAbility(AbilityType, TargetRot, bExtra)) return false;

	// 쿨다운 적용
	if (CurrentSectionIndex <= 1)
	{
		if (FNPAbilityRuntime* R = Abilities.Find(AbilityType))
		{
			R->LastUsedTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		}
	}

	return true;
}

bool UNPCombatComponent::ExecuteAbility(ENPAbilityType AbilityType, const FRotator& Rotation, bool bExtra)
{
	const UNPSkillData* Skill = FindAbility(AbilityType);
	if (!OwnerCharacter || !Skill || !OwnerCharacter->GetMesh()) return false;
	UAnimInstance* Anim = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!Anim) return false;
	if (!Skill->Montage) return false;
	if (OwnerCharacter->HasAnyState(ENPCharacterState::Stagger) && bComboWindowOpen == false) return false;

	if (!PlaySkill(AbilityType, Rotation, bExtra)) return false;

	return true;
}

bool UNPCombatComponent::PlaySkill(ENPAbilityType AbilityType, const FRotator& Rotation, bool bExtra)
{
	const FNPAbilityRuntime* Skill = Abilities.Find(AbilityType);
	if (!OwnerCharacter || !Skill || !OwnerCharacter->GetMesh()) return false;
	UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInst) return false;

	if (CurrentSkill != AbilityType)
	{
		if (AnimInst->Montage_Play(Skill->Data->Montage, 1.f) <= 0.f)
		{
			NP_LOG(NPLog, Warning, TEXT("몽타주 실행 실패"));
			return false;
		}
		CurrentSectionIndex = 0;
	}

	if (CurrentSectionIndex >= Skill->Data->MaxCombo) return false;

	FName SectionName = ComboSections[CurrentSectionIndex];
	if (bExtra)
		SectionName = ExtraComboSections[CurrentSectionIndex];

	SetCurrentSkill(AbilityType, bExtra);
	SetBufferedSkill(ENPAbilityType::None);
	OwnerCharacter->SetActorRotation(Rotation);
	AnimInst->Montage_JumpToSection(SectionName, Skill->Data->Montage);
	CurrentSectionIndex++;
	OnSkillPlayed.Broadcast(AbilityType, CurrentSectionIndex, bExtra);

	if (CurrentSectionIndex < Skill->Data->MaxCombo)
	{
		SetNextSkill(AbilityType);
	}
	else
	{
		CurrentSectionIndex = 0;
		SetNextSkill(ENPAbilityType::NormalAttack);
	}

	return true;
}

void UNPCombatComponent::HandleAttackInput()
{   
	TryExecuteAbility(NextSkill);
}

void UNPCombatComponent::HandleDashInput()
{
	const UNPSkillData* Dash = FindAbility(ENPAbilityType::Dash);
	if (!Dash) return;
	// 첫 대쉬 반복해서 사용 불가
	
	if (CurrentSkill == ENPAbilityType::Dash && (CurrentSectionIndex == 1 || Dash->MaxCombo <= 1))
		return;

	bool Extra = false;
	
	const FVector InputVector = OwnerCharacter->GetLastMovementInputVector();
	if (InputVector.IsNearlyZero())
		Extra = true;

	TryExecuteAbility(ENPAbilityType::Dash, Extra);
}

void UNPCombatComponent::HandleMoveInput()
{
	if (!OwnerCharacter) return;
	if (!OwnerCharacter->GetMesh()) return;

	if (!bMoveWindowOpen || CurrentSkill == ENPAbilityType::None) return;
	UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
	AnimInst->Montage_Stop(0.1f);
}

void UNPCombatComponent::OnComboWindowBegin()
{
	if (!OwnerCharacter->GetController()) return;

	bComboWindowOpen = true;
	if (TryExecuteAbility(BufferedSkill)) return;

	SetBufferedSkill(ENPAbilityType::None);
}

void UNPCombatComponent::OnComboWindowEnd()
{
	bComboWindowOpen = false;
}

void UNPCombatComponent::OnMoveWindowBegin()
{
	bMoveWindowOpen = true;
}

void UNPCombatComponent::OnMoveWindowEnd()
{
	bMoveWindowOpen = false;
}

bool UNPCombatComponent::IsOnCooldown(const UNPSkillData* Skill) const
{
	if (!Skill) return true;

	if (const FNPAbilityRuntime* R = Abilities.Find(Skill->AbilityType))
	{
		const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		return (Now - R->LastUsedTime) < Skill->Cooldown;
	}
	return false;
}

float UNPCombatComponent::GetRemainingCooldown(const UNPSkillData* Skill) const
{
	if (!Skill) return 0.f;

	if (const FNPAbilityRuntime* R = Abilities.Find(Skill->AbilityType))
	{
		const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		return FMath::Max(0.f, Skill->Cooldown - (Now - R->LastUsedTime));
	}
	return 0.f;
}

void UNPCombatComponent::OnAttackIgnored(AController* EventInstigator)
{
	switch (CurrentSkill)
	{
	case ENPAbilityType::Dash:
		OnDodgeSuccess();
		break;
	default:
		break;
	}
}

void UNPCombatComponent::OnDodgeSuccess()
{
	if (!OwnerCharacter) return;
	ExecuteAbility(ENPAbilityType::DodgeSuccess, OwnerCharacter->GetActorRotation(), bCurrentSkillIsExtra);
}

const FRotator UNPCombatComponent::GetTargetRotation()
{
	ANPPlayerController* PlayerController = Cast<ANPPlayerController>(OwnerCharacter->GetController());
	if (!PlayerController) return OwnerCharacter->GetActorRotation();

	const AActor* LockOnTarget = PlayerController->GetLockOnTarget();
	if (LockOnTarget)
	{
		return FRotator(0.f, PlayerController->GetControlRotation().Yaw, 0.f);
	}

	const FVector InputVector = OwnerCharacter->GetLastMovementInputVector();
	if (!InputVector.IsNearlyZero())
	{
		AActor* TargetActor = PlayerController->SearchNearTargetWithinSector(MonsterTraceChannel, AutoTargetingRange, InputVector, 45.f);

		if (ANPPlayerCharacter* NPChar = Cast<ANPPlayerCharacter>(OwnerCharacter))
			NPChar->SetTarget(TargetActor);

		if (!TargetActor)
		{
			return InputVector.Rotation();
		}
		return UKismetMathLibrary::FindLookAtRotation(OwnerCharacter->GetActorLocation(), TargetActor->GetActorLocation());
	}

	AActor* Target = PlayerController->SearchNearTargetWithinCircle(MonsterTraceChannel, AutoTargetingRange);

	if (ANPPlayerCharacter* NPChar = Cast<ANPPlayerCharacter>(OwnerCharacter))
		NPChar->SetTarget(Target);

	if (Target)
	{
		const FVector CharacterLocation = OwnerCharacter->GetActorLocation();
		const FVector TargetLocation = Target->GetActorLocation();
		const FRotator LookAt = (TargetLocation - CharacterLocation).Rotation();
		return FRotator(0.f, LookAt.Yaw, 0.f);
	}
	
	return OwnerCharacter->GetActorRotation();

}

void UNPCombatComponent::OnSkillEnded(UAnimMontage* Montage, bool bInterrupted)
{
	const FNPAbilityRuntime* Skill = Abilities.Find(CurrentSkill);
	if (!Skill) return;

	if (Montage != Skill->Data->Montage) return;

	// 콤보 종료/리셋
	SetCurrentSkill(ENPAbilityType::None, false);
	SetNextSkill(ENPAbilityType::NormalAttack);
	CurrentSectionIndex = 0;
	//bComboWindowOpen = false;
}

void UNPCombatComponent::SetCurrentSkill(ENPAbilityType SkillType, bool bIsExtra)
{
	CurrentSkill = SkillType;
	bCurrentSkillIsExtra = bIsExtra;

	//FString str = StaticEnum<ENPAbilityType>()->GetNameStringByValue((int64)CurrentSkill);
	//NP_LOG(NPLog, Warning, TEXT("CurrentSkill = %s"), *str);
}

void UNPCombatComponent::SetNextSkill(ENPAbilityType SkillType)
{
	NextSkill = SkillType;

	//FString str = StaticEnum<ENPAbilityType>()->GetNameStringByValue((int64)SkillType);
	//NP_LOG(NPLog, Warning, TEXT("NextSkill = %s"), *str);
}

void UNPCombatComponent::SetBufferedSkill(ENPAbilityType SkillType)
{
	BufferedSkill = SkillType;

	//FString str = StaticEnum<ENPAbilityType>()->GetNameStringByValue((int64)SkillType);
	//NP_LOG(NPLog, Warning, TEXT("BufferedSkill = %s"), *str);
}
