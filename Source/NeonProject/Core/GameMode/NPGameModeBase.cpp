// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/NPGameModeBase.h"

#include "Core/NPGameState.h"
#include "Core/NPPlayerState.h"
#include "Core/Controller/NPPlayerControllerBase.h"

ANPGameModeBase::ANPGameModeBase()
{
	GameStateClass = ANPGameState::StaticClass();
	PlayerControllerClass = ANPPlayerControllerBase::StaticClass();
	PlayerStateClass = ANPPlayerState::StaticClass();
	DefaultPawnClass = nullptr;
}
