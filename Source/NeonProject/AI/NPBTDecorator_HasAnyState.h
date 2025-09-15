// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "NPBTDecorator_HasAnyState.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPBTDecorator_HasAnyState : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UNPBTDecorator_HasAnyState();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "ENPCharacterState", UseEnumValuesAsMaskValuesInEditor = "true"))
	uint8 StateFlags;
};
