// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/NPPlayerCharacterBase.h"
#include "NPHubPlayerCharacter.generated.h"

UCLASS()
class NEONPROJECT_API ANPHubPlayerCharacter : public ANPPlayerCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPHubPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual bool HandleInputCommand(class AController* InstigatorController, FNPInputCommand Command) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true))
	TObjectPtr<class USpringArmComponent> SpringArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = true))
	TObjectPtr<class UCameraComponent> CameraComp;
};
