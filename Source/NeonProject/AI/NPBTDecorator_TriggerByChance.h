// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "NPBTDecorator_TriggerByChance.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPBTDecorator_TriggerByChance : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UNPBTDecorator_TriggerByChance();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, Category = "NP", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float Chance;
};
