// Fill out your copyright notice in the Description page of Project Settings.


#include "Screen/Widget/NPScreenWidgetBase.h"
#include "NeonProject.h"
#include "Screen/NPScreenSubsystem.h"
#include "DataType/NPScreenTypes.h"

#include "Components/Button.h"
#include "Input/Reply.h"


UNPScreenWidgetBase::UNPScreenWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNPScreenWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
}

void UNPScreenWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	
}

bool UNPScreenWidgetBase::RequestCloseScreen()
{
	if (UNPScreenSubsystem* ScreenSubsystem = GetScreenSubsystem())
	{
		return ScreenSubsystem->CloseScreen(this);
	}
	
	return false;
}

APlayerController* UNPScreenWidgetBase::GetPlayerController()
{
	if (!IsValid(CachedController))
	{
		CachedController = GetOwningPlayer();
	}

	return CachedController;
}

APlayerController* UNPScreenWidgetBase::GetPlayerControllerChecked()
{
	check(GetPlayerController());

	return CachedController;
}

UNPScreenSubsystem* UNPScreenWidgetBase::GetScreenSubsystem()
{
	UGameInstance* GameInstance = GetGameInstance();
	return IsValid(GameInstance) ? GameInstance->GetSubsystem<UNPScreenSubsystem>() : nullptr;
}

UNPScreenSubsystem* UNPScreenWidgetBase::GetScreenSubsystemChecked()
{
	return UNPScreenSubsystem::GetChecked(this);
}
