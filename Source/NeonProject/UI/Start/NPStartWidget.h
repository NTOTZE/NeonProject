// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPStartWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNPStartGameClickedDelegate);

UCLASS()
class NEONPROJECT_API UNPStartWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(BlueprintAssignable, Category = "NP|Start")
	FNPStartGameClickedDelegate OnStartGameClicked;

private:
	UFUNCTION()
	void HandleStartButtonClicked();

	UFUNCTION()
	void HandleBeginAnimationFinished();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> BeginAnimation;

	bool bCanStartGame = false;
};
