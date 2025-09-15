// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPBTDecorator_TriggerByChance.h"

UNPBTDecorator_TriggerByChance::UNPBTDecorator_TriggerByChance()
{
	NodeName = TEXT("NP Trigger By Chance");
	Chance = 0.5f;
}

bool UNPBTDecorator_TriggerByChance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const float RandomValue = FMath::FRand();

	return RandomValue <= Chance;
}
