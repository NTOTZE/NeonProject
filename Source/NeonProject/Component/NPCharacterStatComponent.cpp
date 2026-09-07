// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NPCharacterStatComponent.h"
#include "NeonProject.h"

// Sets default values for this component's properties
UNPCharacterStatComponent::UNPCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ResourceStat[(uint8)ENPResourceStatType::Hp].MaxValue = 100.f;
	ResourceStat[(uint8)ENPResourceStatType::Hp].RegenPerSec = 1.f;
	
	ResourceStat[(uint8)ENPResourceStatType::Stamina].MaxValue = 100.f;
	ResourceStat[(uint8)ENPResourceStatType::Stamina].RegenPerSec = 1.f;

	ResourceStat[(uint8)ENPResourceStatType::SkillCost].MaxValue = 100.f;
	ResourceStat[(uint8)ENPResourceStatType::SkillCost].RegenPerSec = 1.f;

	ResourceStat[(uint8)ENPResourceStatType::UltimateCost].MaxValue = 100.f;
	ResourceStat[(uint8)ENPResourceStatType::UltimateCost].RegenPerSec = 1.f;
}


// Called when the game starts
void UNPCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// 테이블에서 스텟 불러오기


	///////

	// ...
	SetResourceCurrentValue(ENPResourceStatType::Hp, ResourceStat[(uint8)ENPResourceStatType::Hp].MaxValue);
	SetResourceCurrentValue(ENPResourceStatType::Stamina, ResourceStat[(uint8)ENPResourceStatType::Stamina].MaxValue);
	SetResourceCurrentValue(ENPResourceStatType::SkillCost, 0.f);
	SetResourceCurrentValue(ENPResourceStatType::UltimateCost, 0.f);
}


// Called every frame
void UNPCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UNPCharacterStatComponent::ApplyDamage(float NewDamage)
{
	const FNPResourceStat& PrevHpStat = GetResourceStat(ENPResourceStatType::Hp);
	const float PrevHp = PrevHpStat.CurrentValue;
	if (PrevHp <= KINDA_SMALL_NUMBER)
		return 0.f;

	const float ActualDamage = FMath::Clamp(NewDamage, 0.f, PrevHp);

	FNPResourceStat NewStat = PrevHpStat;
	NewStat.CurrentValue = PrevHp - ActualDamage;

	SetResourceStat(ENPResourceStatType::Hp, NewStat);
	if (GetResourceStat(ENPResourceStatType::Hp).CurrentValue <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();

	}

	return ActualDamage;
}

void UNPCharacterStatComponent::ApplyNaturalRecovery(FNPNaturalRecoveryContext context, float DeltaSeconds)
{
	for (int i = 0; i < (int)ENPResourceStatType::Count; ++i)
	{
		const float Amount = ResourceStat[i].RegenPerSec * DeltaSeconds;
		RecoverResourceStat((ENPResourceStatType)i, Amount);
	}

}

void UNPCharacterStatComponent::RecoverUltimateCostOnHit()
{
	const FNPResourceStat& UltimateCostStat = GetResourceStat(ENPResourceStatType::UltimateCost);
	RecoverResourceStat(ENPResourceStatType::UltimateCost, UltimateCostStat.HitRecoveryAmount);
}

void UNPCharacterStatComponent::RecoverResourceStat(ENPResourceStatType Type, float Amount)
{
	if (Amount < 0.f)
	{
		NP_LOG(NPLog, Error, TEXT("Amount가 0보다 작을 수 없습니다."));
		return;
	}

	const FNPResourceStat& PrevStat = GetResourceStat(Type);
	const float PrevValue = PrevStat.CurrentValue;
	const float NewValue = FMath::Clamp(PrevValue + Amount, 0.f, PrevStat.MaxValue);

	if (PrevValue != NewValue)
		SetResourceCurrentValue(Type, NewValue);
}

bool UNPCharacterStatComponent::ConsumeResourceStat(ENPResourceStatType Type, float Amount)
{
	if (Amount < 0.f)
	{
		NP_LOG(NPLog, Error, TEXT("Amount가 0보다 작을 수 없습니다."));
		return false;
	}
	const FNPResourceStat& PrevStat = GetResourceStat(Type);
	const float PrevValue = PrevStat.CurrentValue;
	float NewValue = PrevValue - Amount;

	if (NewValue < 0.f)
		return false;

	// 만약 HP를 사용하는 공격이 있다면, 사용 후 최소 1hp는 남아야함
	if (Type == ENPResourceStatType::Hp)
	{
		if (NewValue < 1.f)
			return false;
	}
	NewValue = FMath::Clamp(NewValue, 0.f, PrevStat.MaxValue);

	if (PrevValue != NewValue)
		SetResourceCurrentValue(Type, NewValue);

	return true;
}

void UNPCharacterStatComponent::SetResourceStat(ENPResourceStatType Type, FNPResourceStat NewStat)
{
	ResourceStat[(uint8)Type].MaxValue = FMath::Max(NewStat.MaxValue, 1.f);
	ResourceStat[(uint8)Type].CurrentValue = FMath::Clamp(NewStat.CurrentValue, 0.f, ResourceStat[(uint8)Type].MaxValue);
	ResourceStat[(uint8)Type].RegenPerSec = NewStat.RegenPerSec;

	OnResourceStatChanged.Broadcast(Type, ResourceStat[(uint8)Type].CurrentValue, ResourceStat[(uint8)Type].MaxValue);
}

void UNPCharacterStatComponent::SetResourceCurrentValue(ENPResourceStatType Type, float NewValue)
{
	ResourceStat[(uint8)Type].CurrentValue = FMath::Clamp(NewValue, 0.f, ResourceStat[(uint8)Type].MaxValue);

	OnResourceStatChanged.Broadcast(Type, ResourceStat[(uint8)Type].CurrentValue, ResourceStat[(uint8)Type].MaxValue);
}

