// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NPCharacterBase.h"
#include "NeonProject.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ANPCharacterBase::ANPCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

	GetCapsuleComponent()->SetCapsuleHalfHeight(90.f);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));
	WeaponMesh->SetGenerateOverlapEvents(false);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

	GetCharacterMovement()->bEnablePhysicsInteraction = false;
}

// Called when the game starts or when spawned
void ANPCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	State = static_cast<ENPCharacterState>(0);

	GetMesh()->GetAnimInstance()->OnMontageBlendingOut.AddDynamic(this, &ANPCharacterBase::OnMontageBlendingOut);
	OnTakeAnyDamage.AddDynamic(this, &ANPCharacterBase::HandleTakeAnyDamage);
}

// Called every frame
void ANPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANPCharacterBase::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	TryPlayHitReaction(Damage);
}

void ANPCharacterBase::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{

}

void ANPCharacterBase::TryPlayHitReaction(float Damage)
{
	if (Damage <= 0.f) return;

	if (!GetMesh()) return;
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (!AnimInst) return;

	if (HasAnyState(ENPCharacterState::Dead))
	{
		AnimInst->Montage_Play(DeathMontage);
		return;
	}

	if (Damage >= HeavyStaggerDamage && HeavyStaggerMontage)
	{
		//AddState(ENPCharacterState::Stagger);
		AnimInst->Montage_Play(HeavyStaggerMontage);
		return;
	}
	
	if (Damage >= LightStaggerDamage && LightStaggerMontage)
	{
		//AddState(ENPCharacterState::Stagger);
		AnimInst->Montage_Play(LightStaggerMontage);

		return;
	}
}

void ANPCharacterBase::TryPlayAttackImpactEffect(AActor* DamageActor, AActor* Target, float Damage)
{
	if (Damage <= 0.f) return;

	const FVector TargetLocation = Target->GetActorLocation();
	const FVector DamageLocation = DamageActor->GetActorLocation();

	FVector dir = TargetLocation - DamageLocation;
	dir.Z = 0.f;
	const float distance = dir.Size();
	dir.Normalize();
	const float Radius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector EffectLocation = DamageLocation + (dir * FMath::Max(0.f, distance - Radius));
	
	SpawnEffect(AttackImpactEffect, EffectLocation, (-dir).Rotation());
}

float ANPCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentHP <= 0.f)
		return 0.f;

	const float Applied = FMath::Max(0.f, DamageAmount);
	const float NewHP = FMath::Clamp(CurrentHP - Applied, 0.f, MaxHP);
	SetHP(NewHP);

	BP_OnDamaged(CurrentHP, Applied, DamageCauser);

	if (CurrentHP <= 0.f)
		Die();

	NP_LOG(NPLog, Warning, TEXT("%f"), Applied);
	
	const float SuperReturned = Super::TakeDamage(Applied, DamageEvent, EventInstigator, DamageCauser);
	
	return SuperReturned;
}

void ANPCharacterBase::Die()
{
	BP_OnDied(nullptr);

	AddState(ENPCharacterState::Dead);
	OnDead.Broadcast(this);
	GetWorldTimerManager().SetTimer(TimerHandle_DeathDissolve, this, &ANPCharacterBase::BeginDeathDissolve, DissolveDelayAfterDeath, false);
}

void ANPCharacterBase::SpawnEffect(UNiagaraSystem* Effect, const FVector& Location, const FRotator& Rotation)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Effect, Location, Rotation, FVector(AttackImpactEffectScale), true, true);
}

void ANPCharacterBase::InitDissolve()
{
	DissolveMIDs.Reset();

	TArray<UMeshComponent*> MeshArr;
	GetComponents<UMeshComponent>(MeshArr);
	
	for (UMeshComponent* MeshComp : MeshArr)
	{
		if (!MeshComp) continue;

		const int8 MaterialNum = MeshComp->GetNumMaterials();
		for (int8 i = 0; i < MaterialNum; ++i)
		{
			UMaterialInterface* MaterialInterface = MeshComp->GetMaterial(i);
			if (!MaterialInterface) continue;

			UMaterialInstanceDynamic* MID = MeshComp->CreateAndSetMaterialInstanceDynamic(i);
			if (!MID) continue;
		
			DissolveMIDs.Add(MID);
		}
	}
}

void ANPCharacterBase::SetDissolveAppearance(float Value)
{
	const float Appearance = FMath::Clamp(Value, 0.f, 1.f);
	CachedAppearance = Appearance;
	for (auto MID : DissolveMIDs)
	{
		if (!IsValid(MID)) continue;

		MID->SetScalarParameterValue(DissolveParamName, Appearance);
	}
}

void ANPCharacterBase::DissolveTimerCallback()
{
	const float Elapsed = GetWorld()->GetTimeSeconds() - DissolveStartTime;
	
	float AlphaValue = 0.f;
	if (FMath::IsNearlyZero(DissolveDuration))
		AlphaValue = 1.f;

	AlphaValue = FMath::Clamp(Elapsed / DissolveDuration, 0.f, 1.f);

	const float CurrentAppearance = FMath::Lerp(StartAppearance, EndAppearance, AlphaValue);
	SetDissolveAppearance(CurrentAppearance);

	if (AlphaValue >= 1.f)
	{
		if (FMath::IsNearlyZero(CachedAppearance))
			SetActorHiddenInGame(true);

		GetWorldTimerManager().ClearTimer(TimerHandle_Dissolve);
	}
}

void ANPCharacterBase::PlayDissolve(bool bAppear, float Duration)
{
	if (bAppear)
	{
		SetActorHiddenInGame(false);
	}

	if (DissolveMIDs.Num() == 0)
		InitDissolve();

	GetWorldTimerManager().ClearTimer(TimerHandle_Dissolve);

	StartAppearance = CachedAppearance;
	EndAppearance = bAppear ? 1.f : 0.f;

	const float AppearanceDiff = FMath::Abs(EndAppearance - StartAppearance) ;
	DissolveDuration = FMath::Clamp(AppearanceDiff, 0.f, 1.f) * Duration;
	DissolveStartTime = GetWorld()->GetTimeSeconds();


	GetWorldTimerManager().SetTimer(TimerHandle_Dissolve, this, &ANPCharacterBase::DissolveTimerCallback, 0.02f, true);
}



void ANPCharacterBase::BeginDeathDissolve()
{
	RemoveState(ENPCharacterState::Active);

	SetActorTickEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PlayDissolve(false, DeathDissolveDuration);

	//GetWorldTimerManager().ClearTimer(TimerHandle_DeathDissolve);
}

void ANPCharacterBase::AddState(ENPCharacterState flags)
{
	ENPCharacterState Applied = (~State & flags);
	if ((uint8)Applied == 0) return;

	FString str = GetStateFlagsName(Applied);
	NP_LOG(NPLog, Warning, TEXT("Add State : %s"), *str);
	
	EnumAddFlags(State, Applied);
	OnStateChange.Broadcast(Applied, true);
}

void ANPCharacterBase::RemoveState(ENPCharacterState flags)
{
	ENPCharacterState Applied = (State & flags);
	if ((uint8)Applied == 0) return;

	FString str = GetStateFlagsName(Applied);
	NP_LOG(NPLog, Warning, TEXT("Remove State : %s"), *str);

	EnumRemoveFlags(State, Applied);
	OnStateChange.Broadcast(Applied, false);
}

bool ANPCharacterBase::HasAnyState(ENPCharacterState flags)
{
	return EnumHasAnyFlags(State, flags);
}

bool ANPCharacterBase::HasAllState(ENPCharacterState flags)
{
	return EnumHasAllFlags(State, flags);
}

void ANPCharacterBase::SetHP(float NewHP)
{
	const float OldHP = CurrentHP;
	CurrentHP = FMath::Clamp(NewHP, 0.f, MaxHP);

	OnHPChange.Broadcast(OldHP, CurrentHP);
}

