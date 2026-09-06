// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NPTargetingInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNPTargetingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NEONPROJECT_API INPTargetingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual AActor* GetLockOnTarget() const = 0;
	virtual AActor* FindTargetAround(ECollisionChannel TraceChannel, float Radius) const = 0;
	virtual AActor* FindTargetInFront(ECollisionChannel TraceChannel, float Radius, const FVector& Direction, float AngleDeg) const = 0;

};
