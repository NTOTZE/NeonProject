// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NPStartPlayerController.generated.h"

UCLASS()
class NEONPROJECT_API ANPStartPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	/// @brief 시작 화면의 게임 시작 버튼에서 호출
	UFUNCTION(BlueprintCallable, Category = "NP|Start")
	void StartGame();

private:
	UFUNCTION()
	void HandleStartGameClicked();

private:
	UPROPERTY(Transient)
	TObjectPtr<class UNPStartWidget> StartWidget;

	bool bIsStartingGame = false;
};
