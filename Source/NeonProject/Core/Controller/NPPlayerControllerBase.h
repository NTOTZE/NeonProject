// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NPPlayerControllerBase.generated.h"

struct FInputActionValue;
struct FNPInputCommand;

UCLASS()
class NEONPROJECT_API ANPPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
public:
	ANPPlayerControllerBase();

	virtual void BeginPlay() override;

protected:
	virtual void SetupInputComponent() override;
	void TryExecuteInputCommand(FNPInputCommand Command);
	virtual bool ExecuteInputCommand(FNPInputCommand Command);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<class UInputMappingContext> IMC_Character;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<class UInputAction> IA_MoveForward;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_MoveRight;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_LookUp;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_Dash;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<class UInputAction> IA_Scroll;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<class UInputAction> IA_Escape;

protected:
	virtual void HandleMoveForwardInput(const FInputActionValue& Value);
	virtual void HandleMoveRightInput(const FInputActionValue& Value);
	virtual void HandleLookUpInput(const FInputActionValue& Value);
	virtual void HandleScrollInput(const FInputActionValue& Value);
	virtual void HandleEscapeInput();

protected:
	FRotator CachedControlRotation;
	FRotator DefaultControlRotation;

	UPROPERTY()
	float CachedMouseSensitivity = 1.f;
};
