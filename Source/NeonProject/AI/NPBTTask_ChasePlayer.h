// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NPBTTask_ChasePlayer.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPBTTask_ChasePlayer : public UBTTaskNode
{
	GENERATED_BODY()


public:
	UNPBTTask_ChasePlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = "Chase")
	float AcceptanceRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Chase")
	bool bReturnSucceeded = false;
};
