// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GameMode/NPStartGameMode.h"
#include "Core/Controller/NPStartPlayerController.h"

ANPStartGameMode::ANPStartGameMode()
{
	PlayerControllerClass = ANPStartPlayerController::StaticClass();
}
