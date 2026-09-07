// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/NPHomingProjectile.h"

#include "NeonProject.h"
#include "Combat/NPDamageAreaActor.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANPHomingProjectile::ANPHomingProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	CollisionComp->InitSphereRadius(10.f);
	CollisionComp->SetCollisionProfileName(TEXT("PlayerProjectile"));
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->SetLineThickness(3.f);
	RootComponent = CollisionComp;

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(RootComponent);
	NiagaraComp->bAutoActivate = false;
	NiagaraComp->SetAutoDestroy(false);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	MovementComp->InitialSpeed = 1800.f;
	MovementComp->MaxSpeed = 1800.f;
	MovementComp->ProjectileGravityScale = 0.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bShouldBounce = false;

	MovementComp->bShouldBounce = false;
	MovementComp->HomingAccelerationMagnitude = 6000.f;

	InitialLifeSpan = 10.f;
}

// Called when the game starts or when spawned
void ANPHomingProjectile::BeginPlay()
{
	Super::BeginPlay();


	MovementComp->InitialSpeed = Spec.InitialSpeed;
	MovementComp->Velocity = GetActorForwardVector() * MovementComp->InitialSpeed;
	MovementComp->MaxSpeed = Spec.MaxSpeed;
	MovementComp->HomingAccelerationMagnitude = Spec.HomingAccelBase;
	MovementComp->bIsHomingProjectile = true;

	CollisionComp->SetCollisionProfileName(NPDamageCollisionProfileName[(uint8)Spec.CollisionType]);
	CollisionComp->SetSphereRadius(Spec.CollisionRadius);
	CollisionComp->SetHiddenInGame(!bDrawDebug);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ANPHomingProjectile::OnBeginOverlap);

	if (Spec.NigaraSpec.NiagaraSystem)
	{
		NiagaraComp->SetRelativeTransform(Spec.NigaraSpec.Offset);
		if (!Spec.NigaraSpec.bAttach)
		{
			NiagaraComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
		NiagaraComp->SetAsset(Spec.NigaraSpec.NiagaraSystem);
		NiagaraComp->Activate(true);
	}

	if (IsValid(TargetActor))
	{
		TargetActor->OnDestroyed.AddDynamic(this, &ANPHomingProjectile::OnTargetDistroyed);
		MovementComp->HomingTargetComponent = TargetActor->GetRootComponent();
	}
	else
	{
		const FVector OwnerLocation = GetOwner()->GetActorLocation();
		const FVector OwnerForward = GetOwner()->GetActorForwardVector();
		const FVector TargetLocation = OwnerLocation + (OwnerForward * Spec.MaxRange);

		SetHomingLocation(TargetLocation);
	}

	InitialOwnerLocation = GetOwner()->GetActorLocation();
	bHasImpacted = false;
}

// Called every frame
void ANPHomingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (ElapsedTime >= Spec.ExplosionTimeout ||
		GetDistanceToOwner() >= Spec.MaxRange ||
		GetDistanceToTarget() <= CollisionComp->GetScaledSphereRadius() * 0.5f)
	{
		HandleImpact(nullptr);
		return;
	}
	ElapsedTime += DeltaTime;

	const float HomingAccel = Spec.HomingAccelBase + (Spec.MaxRange - GetDistanceToTarget()) / Spec.MaxRange * Spec.HomingAccelBonusMax;
	MovementComp->HomingAccelerationMagnitude = HomingAccel;

}

void ANPHomingProjectile::InitProjectile(const FNPHomingProjectileSpec& ProjectileSpec, const FNPDamageAreaSpec& DamageSpec, AActor* Target, bool DrawDebug)
{
	TargetActor = Target;
	Spec = ProjectileSpec;
	DamageAreaSpec = DamageSpec;
	bDrawDebug = DrawDebug;
}

void ANPHomingProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	NP_LOG(NPLog, Warning, TEXT(""));

	HandleImpact(OtherActor);
}

void ANPHomingProjectile::OnTargetDistroyed(AActor* DestroyedActor)
{
	const FVector TargetLocation = DestroyedActor->GetActorLocation();
	SetHomingLocation(TargetLocation);
}

void ANPHomingProjectile::HandleImpact(AActor* HitActor)
{
	if (bHasImpacted)
		return;
	
	bHasImpacted = true;

	MovementComp->bIsHomingProjectile = false;
	MovementComp->SetUpdatedComponent(nullptr);
	MovementComp->StopMovementImmediately();

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->SetGenerateOverlapEvents(false);

	FActorSpawnParameters Params;
	Params.Owner = GetInstigator();
	Params.Instigator = GetInstigator();
	Params.bDeferConstruction = true;

	FTransform DamageTransform = GetActorTransform();

	// 명중 시 이펙트가 몬스터 근처 허공에서 터진다는 느낌이어서.
	// Projectile 캡슐 크기만큼 몬스터 방향으로 Damage 생성 위치를 변경함
	if (HitActor)
	{
		const FVector ActorLocation = GetActorLocation();
		FVector ToTarget = HitActor->GetActorLocation() - ActorLocation;
		ToTarget.Normalize();
		const FQuat ImpactRotation = ToTarget.ToOrientationQuat();
		const FVector ImpactPos = ToTarget * Spec.CollisionRadius + ActorLocation;
		
		DamageTransform.SetLocation(ImpactPos);
		DamageTransform.SetRotation(ImpactRotation);
	}
	else
	{
		const FVector ImpactPos = GetActorLocation() + (GetActorForwardVector() * Spec.CollisionRadius);
		DamageTransform.SetLocation(ImpactPos);
	}
	
	ANPDamageAreaActor* AreaActor = GetWorld()->SpawnActor<ANPDamageAreaActor>(ANPDamageAreaActor::StaticClass(), DamageTransform, Params);
	if (!AreaActor) return;
	AreaActor->InitializeFromSpec(DamageAreaSpec);
	AreaActor->FinishSpawning(DamageTransform, false, nullptr, ESpawnActorScaleMethod::OverrideRootScale);
	
	NiagaraComp->Deactivate();
	
	if (Spec.PostImpactLifeSpan <= 0.f)
	{
		Destroy();
		return;
	}
	SetLifeSpan(Spec.PostImpactLifeSpan);
}

void ANPHomingProjectile::SetHomingLocation(const FVector& TargetLocation)
{
	TargetActor = nullptr;
	
	if (!HomingLocationProxy)
	{
		HomingLocationProxy = NewObject<USceneComponent>(this, TEXT("HomingLocationProxy"));
		HomingLocationProxy->RegisterComponent();
	}
	HomingLocationProxy->SetWorldLocation(TargetLocation);

	MovementComp->HomingTargetComponent = HomingLocationProxy;
}

float ANPHomingProjectile::GetDistanceToTarget()
{
	if (IsValid(TargetActor))
	{
		const FVector TargetLocation = TargetActor->GetActorLocation();
		const FVector CurrentLocation = GetActorLocation();
		return FVector::Distance(TargetLocation, CurrentLocation);
	}

	if (HomingLocationProxy)
	{
		const FVector TargetLocation = HomingLocationProxy->GetComponentLocation();
		const FVector CurrentLocation = GetActorLocation();
		return FVector::Distance(TargetLocation, CurrentLocation);
	}

	return 0.f;
}

float ANPHomingProjectile::GetDistanceToOwner()
{
	return FVector::Distance(GetActorLocation(), InitialOwnerLocation);
}
