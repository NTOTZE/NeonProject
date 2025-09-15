// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NPAnimInstance.h"
#include "NeonProject.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UNPAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();


}

void UNPAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();


}

void UNPAnimInstance::NativeUpdateAnimation(float _fDeltaTime)
{
	Super::NativeUpdateAnimation(_fDeltaTime);

	ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	if (!Character) return;

	Velocity = Character->GetActorRotation().UnrotateVector(Character->GetMovementComponent()->Velocity);
	Speed = Velocity.Size2D();
	Direction = Velocity.Rotation().Yaw;
	bIsFalling = Character->GetMovementComponent()->IsFalling();

	ControllerYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(
		Character->GetControlRotation(),
		Character->GetActorRotation()
	).Yaw;

	AimPitch = Character->GetBaseAimRotation().Pitch;
}
