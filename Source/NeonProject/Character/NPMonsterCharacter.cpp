// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NPMonsterCharacter.h"
#include "NeonProject.h"
#include "DataType/NPCombatTypes.h"
#include "AI/NPAIController.h"
#include "UI/NPMonsterInfoBase.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/WidgetComponent.h"

ANPMonsterCharacter::ANPMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(GetCapsuleComponent());
	WidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	ConstructorHelpers::FClassFinder<UNPMonsterInfoBase> UiFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/NeonProject/Blueprint/UI/WBP_MonsterInfo.WBP_MonsterInfo_C'"));
	if (UiFinder.Succeeded())
	{
		WidgetComp->SetWidgetClass(UiFinder.Class);
	}
	WidgetComp->SetDrawSize(FVector2D(200.f, 200.f));
	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/NeonProject/Blueprint/Character/Monster/UI/NP_Widget3DPassThrough_Masked_OneSided.NP_Widget3DPassThrough_Masked_OneSided'"));
	if (MatFinder.Succeeded())
	{
		WidgetComp->SetMaterial(0, MatFinder.Object);
	}

	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCapsuleComponent()->SetCollisionProfileName(NPCharacterCollisionProfileName[(uint8)ENPCharacterCollisionType::MonsterCapsule]);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ANPAIController::StaticClass();

	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

void ANPMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UNPMonsterInfoBase* MonUI = Cast<UNPMonsterInfoBase>(WidgetComp->GetWidget());
	if (MonUI)
	{
		MonsterUI = MonUI;
		OnHPChange.AddUObject(this, &ANPMonsterCharacter::OnHPChanged);
		MonsterUI->SetHPBarRatio(CurrentHP / MaxHP);
	}
	InitDissolve();
	SetDissolveAppearance(0.f);
	PlayDissolve(true, 1.f);
}

void ANPMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bLookAtTarget && GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		if (!GetTarget()) return;

		const FVector StartLocation = GetActorLocation();
		const FVector TargetLocation = GetTarget()->GetActorLocation();

		FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		LookAt.Pitch = 0.f;
		LookAt.Roll = 0.f;

		const float StartYaw = GetActorRotation().Yaw;
		const float TargetYaw = LookAt.Yaw;

		const float DeltaYaw = FMath::FindDeltaAngleDegrees(StartYaw, TargetYaw);
		const float YawRate = GetCharacterMovement()->RotationRate.Yaw;

		//const float NewYaw = FMath::FInterpConstantTo(StartYaw, TargetYaw, DeltaTime, YawRate);
		
		const float NewYaw = FMath::Clamp(DeltaYaw, -YawRate* DeltaTime, YawRate* DeltaTime) + StartYaw;

		SetActorRotation(FRotator(0.f, NewYaw, 0.f));
	}
}


void ANPMonsterCharacter::LookAtTarget(bool b)
{
	bLookAtTarget = b;
}

void ANPMonsterCharacter::Die()
{
	LookAtTarget(false);
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->DisableMovement();
	}
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();

		if (UPathFollowingComponent* Path = AICon->GetPathFollowingComponent())
		{
			Path->AbortMove(*this, FPathFollowingResultFlags::ForcedScript);
		}

		if (UBehaviorTreeComponent* BT = Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()))
		{
			BT->StopLogic(TEXT("Death"));
		}
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WidgetComp->SetVisibility(false);
	SetLifeSpan(DeathLifeSpan);

	Super::Die();
}

AActor* ANPMonsterCharacter::GetTarget()
{
	return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void ANPMonsterCharacter::OnHPChanged(float OldHP, float NewHP)
{
	if (!MonsterUI) return;

	const float ratio = NewHP / MaxHP;

	MonsterUI->SetHPBarRatio(ratio);
}
