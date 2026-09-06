// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/NPHubGameMode.h"
#include "Core/Controller/NPHubPlayerController.h"


ANPHubGameMode::ANPHubGameMode()
{
	PlayerControllerClass = ANPHubPlayerController::StaticClass();

}
