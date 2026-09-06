// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/GameInstance.h"
#include "Utility/NPMacros.h"
#include "DataType/NPScreenTypes.h"
#include "NPScreenSubsystem.generated.h"

class UNPScreenFadeWidgetBase;
class UNPScreenWidgetBase;

DECLARE_DELEGATE_OneParam(FNPScreenFadeFinishedDelegate, ENPFadeAnimationType /*CompletedFadeType*/);

UCLASS()
class NEONPROJECT_API UNPScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	NP_DECLARE_GAMEINSTANCE_SUBSYSTEM_GETTER()

public:
	UNPScreenSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	bool OpenScreen(ENPScreenType screenType);
	bool CloseScreen(UNPScreenWidgetBase* screenWidget);

	bool ShowBlackScreen();

	bool PlayFadeIn(float fadeDuration = 1.f, float fadeStartDelay = 0.f);
	bool PlayFadeOut(float fadeDuration = 1.f, float fadeStartDelay = 0.f);

private:
	bool PlayFade(ENPFadeAnimationType fadeAnimationType, float fadeDuration, float fadeStartDelay);
	void HandleFadeFinished(ENPFadeAnimationType CompletedFadeType);
	void HandleWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	void ClearWidgets();
	APlayerController* GetPlayerController() const;

private:
	UPROPERTY()
	TMap<ENPScreenType, TSubclassOf<UNPScreenWidgetBase>> ScreenWidgetClasses;

	UPROPERTY()
	TSubclassOf<UNPScreenFadeWidgetBase> ScreenFadeWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UNPScreenWidgetBase> ActiveScreenWidget;

	UPROPERTY(Transient)
	TObjectPtr<UNPScreenFadeWidgetBase> ScreenFadeWidget;

	FDelegateHandle WorldCleanupHandle;
	bool bPlayingFade = false;

public:
	FNPScreenFadeFinishedDelegate OnFadeFinished;
};
