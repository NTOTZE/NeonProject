// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controller/NPCameraRig.h"

#include "NeonProject.h"
#include "DataType/NPInputCommandTypes.h"
#include "GameFramework/Controller.h"

#include "Engine/OverlapResult.h"
#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

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

	SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->bEnableCameraRotationLag = true;
	SpringArmComp->CameraLagSpeed = 10.f;
	SpringArmComp->CameraRotationLagSpeed = 10.f;

	CameraComp->bUsePawnControlRotation = true;
	CameraComp->bConstrainAspectRatio = true;
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
		SetActorRotation(FRotator(0.f, ControlRot.Yaw, 0.f));
		SpringArmComp->SetRelativeRotation(FRotator(ControlRot.Pitch, 0.f, 0.f));
	}
}

bool ANPCameraRig::HandleInputCommand(AController* InstigatorController, FNPInputCommand Command)
{
	bool bSucceeded = false;
	switch (Command.CommandType)
	{
	case ENPInputCommandType::LockOn:
	{
		FRotator Rot = InstigatorController->GetControlRotation();
		AActor* Target = SearchNearTargetWithinSector(
			NPDamageCollisionTraceChannel[(uint8)ENPDamageCollisionType::MonsterHit]
			, 3500.f
			, Rot.RotateVector(FVector(1.f, 0.f, 0.f))
			, 180);

		if (Target)
		{
			SetLockOnTarget(Target);
		}
		bSucceeded = true;
		break;
	}
	case ENPInputCommandType::Unlock:
	{
		SetLockOnTarget(nullptr);
		bSucceeded = true;
		break;
	}
	}
	return bSucceeded;
}

void ANPCameraRig::SetFollowTarget(ACharacter* NewTarget)
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

AActor* ANPCameraRig::SearchNearTargetWithinCircle(ECollisionChannel TraceChannel, float Radius)
{
	TArray<FOverlapResult> Overlaps;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	const FVector CharacterLocation = GetActorLocation();

	const bool bHit = World->OverlapMultiByChannel(
		Overlaps,
		CharacterLocation,
		FQuat(),
		TraceChannel,
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams::DefaultQueryParam
	);

	TWeakObjectPtr<AActor> ResultActor = nullptr;

	for (const FOverlapResult& Overlap : Overlaps)
	{

		AActor* OverlapActor = Overlap.GetActor();

		if (!ResultActor.Get())
		{
			ResultActor = OverlapActor;
			continue;
		}

		// 최소거리 엑터 찾기
		const float ResultDist = FVector::Dist2D(ResultActor->GetActorLocation(), CharacterLocation);
		const float OverlapDist = FVector::Dist2D(OverlapActor->GetActorLocation(), CharacterLocation);
		if (OverlapDist < ResultDist)
		{
			ResultActor = OverlapActor;
		}
	}

	return ResultActor.Get();
}

AActor* ANPCameraRig::SearchNearTargetWithinSector(ECollisionChannel TraceChannel, float Radius, const FVector& Direction, float AngleDeg)
{
	TArray<FOverlapResult> Overlaps;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	const FVector CharacterLocation = GetActorLocation();

	const bool bHit = World->OverlapMultiByChannel(
		Overlaps,
		CharacterLocation,
		FQuat(),
		TraceChannel,
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams::DefaultQueryParam
	);

	TWeakObjectPtr<AActor> ResultActor = nullptr;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* OverlapActor = Overlap.GetActor();

		// 부채꼴 판단
		const FVector TargetLoc = OverlapActor->GetActorLocation();
		const FVector OwnerLoc = GetActorLocation();

		FVector ToTarget = TargetLoc - OwnerLoc;
		ToTarget.Z = 0.f;
		ToTarget = ToTarget.GetSafeNormal2D();

		FVector Forward = Direction;
		Forward.Z = 0.f;
		Forward = Forward.GetSafeNormal2D();

		const float SectorCos = FMath::Cos(FMath::DegreesToRadians(AngleDeg * 0.5f));
		const float TargetCos = FVector::DotProduct(Forward, ToTarget);

		if (TargetCos < SectorCos)
			continue;

		if (!ResultActor.Get())
		{
			ResultActor = OverlapActor;
			continue;
		}

		// 최소거리 엑터 찾기
		const float ResultDist = FVector::Dist2D(ResultActor->GetActorLocation(), CharacterLocation);
		const float OverlapDist = FVector::Dist2D(OverlapActor->GetActorLocation(), CharacterLocation);
		if (OverlapDist < ResultDist)
		{
			ResultActor = OverlapActor;
		}
	}

	return ResultActor.Get();
}