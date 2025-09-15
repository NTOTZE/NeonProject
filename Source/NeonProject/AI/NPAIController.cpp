// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPAIController.h"

#include "Character/NPMonsterCharacter.h"

#include "Kismet/GameplayStatics.h"

void ANPAIController::OnPossess(APawn* _Owner)
{
	Super::OnPossess(_Owner);

	ANPMonsterCharacter* Monster = Cast<ANPMonsterCharacter>(GetPawn());
	if (Monster)
	{
		if (Monster->BehaviorTreeAsset)
		{
			RunBehaviorTree(Monster->BehaviorTreeAsset);
		}
	}
}

void ANPAIController::OnUnPossess()
{
	Super::OnUnPossess();

}
