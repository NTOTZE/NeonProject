// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/NPGameInstance.h"
#include "Engine/Engine.h"

UNPGameInstance::UNPGameInstance()
{
}

void UNPGameInstance::Init()
{
	Super::Init();

	if (GEngine)
	{
		GEngine->bEnableOnScreenDebugMessages = false;
		GEngine->bEnableOnScreenDebugMessagesDisplay = false;
		GEngine->Exec(GetWorld(), TEXT("DisableAllScreenMessages"));
	}
}
