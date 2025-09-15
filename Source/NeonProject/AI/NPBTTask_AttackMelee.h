// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NPBTTask_AttackMelee.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPBTTask_AttackMelee : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UNPBTTask_AttackMelee();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

public:
	UPROPERTY(EditAnywhere, Category = "Attack")
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;
};
