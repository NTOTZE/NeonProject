// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPBTDecorator_HasAnyState.h"

#include "Character/NPCharacterBase.h"
#include "DataType/NPCombatTypes.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UNPBTDecorator_HasAnyState::UNPBTDecorator_HasAnyState()
{
	NodeName = TEXT("NP Has Any State");
}

bool UNPBTDecorator_HasAnyState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return false;

	ANPCharacterBase* Character = Cast<ANPCharacterBase>(AIPawn);
	if (!Character) return false;

	return Character->HasAnyState((ENPCharacterState)StateFlags);
}
