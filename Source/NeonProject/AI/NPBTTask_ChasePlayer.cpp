// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPBTTask_ChasePlayer.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"

UNPBTTask_ChasePlayer::UNPBTTask_ChasePlayer()
{
	NodeName = TEXT("NP Chase Player");
	bNotifyTick = false;
}

EBTNodeResult::Type UNPBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(AIController->GetWorld(), 0);
	if (!PlayerCharacter) return EBTNodeResult::Failed;

	const float Dist = FVector::Dist(AIPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
	if (Dist > AcceptanceRadius)
	{
		AIController->MoveToLocation(PlayerCharacter->GetActorLocation(), AcceptanceRadius, true);
	}

	return bReturnSucceeded ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
