// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NPCombatComponent.h"
#include "NeonProject.h"
#include "Character/Player/NPPlayerCharacterBase.h"
#include "Core/Controller/NPBattlePlayerController.h"
#include "Component/NPCharacterStatComponent.h"
#include "Combat/Cutscene/NPSkillCutsceneSubsystem.h"
#include "Interface/NPTargetingInterface.h"

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
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UNPCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerCharacter) 
		return;

	FVector CurrentLocation = OwnerCharacter->GetActorLocation();

	if (CurrentSkill != ENPAbilityType::None)
		if (AActor* Target = OwnerCharacter->GetTarget())
			if (const UNPSkillData* CurrentData = FindAbility(CurrentSkill))
				if (CurrentData->bAutoTarget)
				{
					const FVector TargetLocation = Target->GetActorLocation();
					double PrevDist = FVector::Dist2D(TargetLocation, PrevLocation);
					double CurrentDist = FVector::Dist2D(TargetLocation, CurrentLocation);
					const double TestRange = 250;
					if (CurrentDist <= TestRange)
					{
						if (CurrentDist < PrevDist)
						{
							OwnerCharacter->SetActorLocation(PrevLocation);
							CurrentLocation = PrevLocation;
						}
					}
				}

	PrevLocation = CurrentLocation;
}

void UNPCombatComponent::InitializeCombat(ANPPlayerCharacterBase* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
	StatComp = OwnerCharacter->FindComponentByClass<UNPCharacterStatComponent>();
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

	if (!StatComp)
	{
		NP_LOG(NPLog, Warning, TEXT("StatComp 없음"));
		return false;
	}

	if (AbilityType == ENPAbilityType::Skill)
	{
		if (!StatComp->ConsumeResourceStat(ENPResourceStatType::SkillCost, Skill->Cost))
		{
			NP_LOG(NPLog, Warning, TEXT("SkillCost 부족함"));
			return false;
		}
	}
	else if (AbilityType == ENPAbilityType::Ultimate)
	{
		if (!StatComp->ConsumeResourceStat(ENPResourceStatType::UltimateCost, Skill->Cost))
		{
			NP_LOG(NPLog, Warning, TEXT("UltimateCost 부족함"));
			return false;
		}
	}
	else
	{
		if (!StatComp->ConsumeResourceStat(ENPResourceStatType::Stamina, Skill->Cost))
		{
			NP_LOG(NPLog, Warning, TEXT("Stamina 부족함"));
			return false;
		}
	}

	// 방향
	AActor* Target = nullptr;
	FRotator TargetRot = OwnerCharacter->GetActorRotation();

	const FVector InputVector = OwnerCharacter->GetLastMovementInputVector();
	if (!InputVector.IsNearlyZero())
		TargetRot = InputVector.Rotation();

	if (Skill->bAutoTarget)
		Target = FindTarget();

	if (!ExecuteAbility(AbilityType, TargetRot, bExtra, Target))
		return false;

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

bool UNPCombatComponent::ExecuteAbility(ENPAbilityType AbilityType, const FRotator& Rotation, bool bExtra, AActor* Target)
{
	const UNPSkillData* Skill = FindAbility(AbilityType);
	if (!OwnerCharacter || !Skill || !OwnerCharacter->GetMesh()) return false;
	UAnimInstance* Anim = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!Anim) return false;
	if (!Skill->Montage) return false;
	if (OwnerCharacter->HasAnyState(ENPCharacterState::Stagger) && bComboWindowOpen == false) return false;

	// 타겟이 없다면 매개변수 Rotation방향으로 PlaySkill
	FRotator TargetRotation = Rotation;
	OwnerCharacter->SetAnimRootMotionTranslationScale(1.f);
	if (Target)
	{	// 타겟이 있다면 타겟 방향으로 PlaySkill
		TargetRotation = UKismetMathLibrary::FindLookAtRotation(OwnerCharacter->GetActorLocation(), Target->GetActorLocation());
		OwnerCharacter->SetAnimRootMotionTranslationScale(1.7f);
	}
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;
	OwnerCharacter->SetTarget(Target);

	if (!PlaySkill(AbilityType, TargetRotation, bExtra)) return false;

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
		bool bPlaySucceeded = false;
		// 컷씬 있을경우
		if (Skill->Data->bPlaySkillCutscene && Skill->Data->SkillCutscene)
		{
			//블랜드없이 재생
			bPlaySucceeded = (AnimInst->Montage_PlayWithBlendIn(Skill->Data->Montage, FAlphaBlendArgs(0.f)) > 0.f);
			OwnerCharacter->SetActorRotation(Rotation);
			UNPSkillCutsceneSubsystem::GetChecked(this)->PlaySkillCutscene(Skill->Data->SkillCutscene ,OwnerCharacter, Skill->Data->RestoreViewBlendTime);
		}
		else // 컷씬 없을경우
		{
			// 몽타주의 블렌드 설정값 그대로 재생
			bPlaySucceeded = (AnimInst->Montage_Play(Skill->Data->Montage) > 0.f);
		}

		if (!bPlaySucceeded)
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

AActor* UNPCombatComponent::FindTarget()
{
	INPTargetingInterface* TargetingInterface = Cast<INPTargetingInterface>(OwnerCharacter->GetController());
	if (!TargetingInterface)
		return nullptr;

	//락온 중인 타겟
	AActor* TargetActor = TargetingInterface->GetLockOnTarget();
	if (!TargetActor)
	{	//락온중인 타겟 없을시

		const FVector InputVector = OwnerCharacter->GetLastMovementInputVector();
		if (InputVector.IsNearlyZero())
		{
			//방향키 미입력 시 주변 탐색
			TargetActor = TargetingInterface->FindTargetAround(MonsterTraceChannel, AutoTargetingRange);
		}
		else
		{
			//방향키 입력 시 입력한 방향 기준 정면 탐색
			TargetActor = TargetingInterface->FindTargetInFront(MonsterTraceChannel, AutoTargetingRange, InputVector, 45.f);
		}
	}
	return TargetActor;
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
