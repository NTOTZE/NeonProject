// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPBTService_DetectPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UNPBTService_DetectPlayer::UNPBTService_DetectPlayer()
{
	NodeName = TEXT("NP Detect Player");

	Interval = 0.5f;

	PlayerDetectedKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UNPBTService_DetectPlayer, PlayerDetectedKey));
	PlayerDetectedKey.SelectedKeyName = TEXT("bIsPlayerDetected");
}

void UNPBTService_DetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter) return;

	const float Distance = FVector::Dist(AIPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());

	// 한번 발견하면 끝까지 True
	if (Distance <= DetectRange)
	{
		if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
		{
			BB->SetValueAsBool(PlayerDetectedKey.SelectedKeyName, true);
		}
	}
}
