// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPBTDecorator_TriggerWithinRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UNPBTDecorator_TriggerWithinRange::UNPBTDecorator_TriggerWithinRange()
{
	NodeName = TEXT("NP Trigger Within Range");
	MinRange = 0.f;
	MaxRange = 200.f;
}

bool UNPBTDecorator_TriggerWithinRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return false;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter) return false;

	const float Dist = FVector::Dist2D(AIPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
	if (Dist > MaxRange || Dist < MinRange)	return false;

	return true;
}
