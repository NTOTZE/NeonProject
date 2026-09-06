// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPProgressBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetPercent(float value);
	FORCEINLINE float GetPercent() { return CurrentPercent; };

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> IMG_ProgressBar = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|ProgressBar")
	FName ProgressParameterName = TEXT("Progress");

	UPROPERTY(Transient)
	TObjectPtr<class UMaterialInstanceDynamic> ProgressMID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|ProgressBar")
	float InitialPercent = 0.f;

private:
	UPROPERTY(Transient)
	float CurrentPercent = 0.f;
};
