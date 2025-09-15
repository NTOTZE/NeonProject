// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "NPBTService_DetectPlayer.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPBTService_DetectPlayer : public UBTService
{
	GENERATED_BODY()

public:
	UNPBTService_DetectPlayer();

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    struct FBlackboardKeySelector PlayerDetectedKey;
    
    /** 감지 반경 (cm) */
    UPROPERTY(EditAnywhere, Category = "Detect", meta = (ClampMin = "0.0", UIMin = "0.0"))
    float DetectRange = 1200.f;
};
