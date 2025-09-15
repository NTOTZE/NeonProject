// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controller/NPCameraRig.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ANPCameraRig::ANPCameraRig()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));

	RootComponent = SceneComp;
	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp);

	SpringArmComp->TargetArmLength = 600.f;
	SpringArmComp->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	SpringArmComp->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));

	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->bEnableCameraRotationLag = true;
	SpringArmComp->CameraLagSpeed = 10.f;
	SpringArmComp->CameraRotationLagSpeed = 10.f;

	CameraComp->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void ANPCameraRig::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPCameraRig::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FollowTarget.Get()) return;

	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (!PlayerController) return;

	const FVector TargetLocation = FollowTarget.Get()->GetActorLocation();
	SetActorLocation(TargetLocation);

	if (LockOnTarget.IsValid())
	{
		const FVector TargetPos = LockOnTarget->GetActorLocation();
		const FVector ToTarget = TargetPos - GetActorLocation();
		SetActorRotation(ToTarget.Rotation());
		PlayerController->SetControlRotation(GetActorRotation());
	}
	else
	{
		const FRotator ControlRot = PlayerController->GetControlRotation();
		SetActorRotation(ControlRot);
	}
}

void ANPCameraRig::SetFollowTarget(AActor* NewTarget)
{
	FollowTarget = NewTarget;

	if (NewTarget)
	{
		SetActorLocation(NewTarget->GetActorLocation());
	}
}

void ANPCameraRig::SetLockOnTarget(AActor* NewLockOnTarget)
{
	LockOnTarget = NewLockOnTarget;
}
