// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPAIController.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API ANPAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* _Owner) override;
	virtual void OnUnPossess() override;
};
