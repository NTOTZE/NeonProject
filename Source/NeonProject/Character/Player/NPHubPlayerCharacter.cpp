// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/NPHubPlayerCharacter.h"
#include "NeonProject.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ANPHubPlayerCharacter::ANPHubPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));

	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->TargetArmLength = 400.f;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = false;

	CameraComp->SetupAttachment(SpringArmComp);

	GetCapsuleComponent()->SetCapsuleHalfHeight(90.f);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	GetCharacterMovement()->bEnablePhysicsInteraction = false;
}

// Called when the game starts or when spawned
void ANPHubPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPHubPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	const FRotator ControlRotation = GetControlRotation();
	SpringArmComp->SetRelativeRotation(ControlRotation);*/
}

// Called to bind functionality to input
void ANPHubPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ANPHubPlayerCharacter::HandleInputCommand(AController* InstigatorController, FNPInputCommand Command)
{
	bool bSucceeded = Super::HandleInputCommand(InstigatorController, Command);



	return bSucceeded;
}

