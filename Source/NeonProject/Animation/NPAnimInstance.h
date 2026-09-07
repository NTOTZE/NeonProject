// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NPAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float _fDeltaTime) override;

private:

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowprivateAccess = "true"))
	FVector	Velocity;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowprivateAccess = "true"))
	float	Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowprivateAccess = "true"))
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowprivateAccess = "true"))
	bool	bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation", meta = (AllowprivateAccess = "true"))
	float ControllerYawOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Rotation", meta = (AllowprivateAccess = "true"))
	float AimPitch;


};
