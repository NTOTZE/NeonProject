// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataType/NPInputCommandTypes.h"
#include "NPInputCommandReceiver.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNPInputCommandReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NEONPROJECT_API INPInputCommandReceiver
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool HandleInputCommand(class AController* InstigatorController, FNPInputCommand Command) = 0;
};
