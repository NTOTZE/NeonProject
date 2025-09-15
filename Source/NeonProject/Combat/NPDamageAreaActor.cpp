// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/NPDamageAreaActor.h"
#include "NeonProject.h"

#include "Character/NPCharacterBase.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANPDamageAreaActor::ANPDamageAreaActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
    RootComponent = SceneComp;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComp->SetGenerateOverlapEvents(false);

    NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
    NiagaraComp->SetupAttachment(RootComponent);
    NiagaraComp->bAutoActivate = false;
    NiagaraComp->SetAutoDestroy(false);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(GetRootComponent());
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComp->SetGenerateOverlapEvents(false);

    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    CapsuleComp->SetupAttachment(GetRootComponent());
    CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CapsuleComp->SetGenerateOverlapEvents(false);

}

// Called when the game starts or when spawned
void ANPDamageAreaActor::BeginPlay()
{
	Super::BeginPlay();

    ANPCharacterBase* Character = Cast<ANPCharacterBase>(GetInstigator());
    if (Character)
    {
        bDrawDebug = Character->IsSkillDebugDrawEnabled();
    }

    PlayNiagara();

    const float Delay = FMath::Max(0.f, DamageSpec.DamageDelay);
    if (Delay > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(DelayTimer, this, &ANPDamageAreaActor::DamageActivate, Delay, false);
        return;
    }
    DamageActivate();
}

void ANPDamageAreaActor::InitializeFromSpec(const FNPDamageAreaSpec& Spec)
{
	DamageSpec = Spec;
}

void ANPDamageAreaActor::PlayNiagara()
{
    if (DamageSpec.NigaraSpec.NiagaraSystem)
    {
        NiagaraComp->SetRelativeTransform(DamageSpec.NigaraSpec.Offset);
        if (!DamageSpec.NigaraSpec.bAttach)
            NiagaraComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        NiagaraComp->SetAsset(DamageSpec.NigaraSpec.NiagaraSystem);
        NiagaraComp->Activate(true);
    }
}

void ANPDamageAreaActor::DamageActivate()
{
    if (DamageSpec.Mode == ENPDamageMode::TraceOnce)
    {
        RunTraceOnce();
        OnDamageEnded();
    }
    else if (DamageSpec.Mode == ENPDamageMode::Persistent)
    {
        ActivatePersistent();
        GetWorld()->GetTimerManager().SetTimer(
            DamageTimer, this, &ANPDamageAreaActor::DeactivatePersistent,
            FMath::Max(0.0f, DamageSpec.DurationSeconds), false);
    }
    else if (DamageSpec.Mode == ENPDamageMode::Periodic)
    {
        DebugDuration = DamageSpec.DamageInterval * 0.5f;
        CurrentCount = 0;

        GetWorld()->GetTimerManager().SetTimer(
            DamageTimer, this, &ANPDamageAreaActor::RunPeriodic,
            FMath::Max(0.05f, DamageSpec.DamageInterval), true, 0.f);
    }
}

void ANPDamageAreaActor::RunTraceOnce()
{
    UWorld* World = GetWorld();
    if (!World) return;

    const FTransform Transform = GetActorTransform();
    const FVector Start = Transform.GetLocation();
    const FVector End = Start;

    TArray<FHitResult> Hits;
    bool bHit = false;
    
    if (DamageSpec.Shape == ENPShapeType::Box)
    {
        const FCollisionShape Shape = FCollisionShape::MakeBox(DamageSpec.BoxExtent);
        bHit = World->SweepMultiByChannel(
            Hits, Start, End, Transform.GetRotation(),
            NPDamageCollisionTraceChannel[(uint8)DamageSpec.CollisionType], Shape
        );

        if (bDrawDebug)
        {
            DrawDebugBox(World, Start, DamageSpec.BoxExtent, Transform.GetRotation(), FColor::Red, false, DebugDuration);
        }
    }
    else if (DamageSpec.Shape == ENPShapeType::Sphere)
    {
        const FCollisionShape Shape = FCollisionShape::MakeSphere(DamageSpec.SphereRadius);
        bHit = World->SweepMultiByChannel(
            Hits, Start, End, Transform.GetRotation(),
            NPDamageCollisionTraceChannel[(uint8)DamageSpec.CollisionType], Shape
        );

        if (bDrawDebug)
        {
            DrawDebugSphere(World, Start, DamageSpec.SphereRadius, 16, FColor::Red, false, DebugDuration);
        }
    }
    else //Capsule
    {
        const FCollisionShape Shape = FCollisionShape::MakeCapsule(DamageSpec.CapsuleRadius,DamageSpec.CapsuleHalfHeight);
        bHit = World->SweepMultiByChannel(
            Hits, Start, End, Transform.GetRotation(),
            NPDamageCollisionTraceChannel[(uint8)DamageSpec.CollisionType], Shape
        );
        if (bDrawDebug)
        {
            DrawDebugCapsule(World, Start, DamageSpec.CapsuleHalfHeight, DamageSpec.CapsuleRadius, Transform.GetRotation(), FColor::Red, false, DebugDuration);
        }
    }

    if (bHit)
    {
        for (const FHitResult& HR : Hits)
        {
            if (AActor* Other = HR.GetActor())
            {
                if (Other != this && Other != GetInstigator())
                {
                    ApplyDamageTo(Other);
                }
            }
        }
    }
}

void ANPDamageAreaActor::RunPeriodic()
{
    RunTraceOnce();

    CurrentCount++;

    if (CurrentCount >= DamageSpec.DamageCount)
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
        OnDamageEnded();
    }
}

void ANPDamageAreaActor::ActivatePersistent()
{
    UShapeComponent* ShapeComp = GetActivatedShapeComponent();
    if (!ShapeComp) return;

    AlreadyHitSet.Reset();

    SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SphereComp->SetGenerateOverlapEvents(false);
    SphereComp->bHiddenInGame = true;
    ShapeComp->SetVisibility(false);
    BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BoxComp->SetGenerateOverlapEvents(false);
    BoxComp->bHiddenInGame = true;
    BoxComp->SetVisibility(false);
    CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CapsuleComp->SetGenerateOverlapEvents(false);
    CapsuleComp->bHiddenInGame = true;
    CapsuleComp->SetVisibility(false);

    SphereComp->SetSphereRadius(DamageSpec.SphereRadius);
    BoxComp->SetBoxExtent(DamageSpec.BoxExtent);
    CapsuleComp->SetCapsuleHalfHeight(DamageSpec.CapsuleHalfHeight);
    CapsuleComp->SetCapsuleRadius(DamageSpec.CapsuleRadius);

    ShapeComp->SetCollisionProfileName(NPDamageCollisionProfileName[(uint8)DamageSpec.CollisionType]);
    ShapeComp->OnComponentBeginOverlap.AddDynamic(this, &ANPDamageAreaActor::OnBeginOverlap);
    ShapeComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ShapeComp->SetGenerateOverlapEvents(true);
    ShapeComp->SetVisibility(true);

    if (bDrawDebug)
        ShapeComp->bHiddenInGame = false;
}

void ANPDamageAreaActor::DeactivatePersistent()
{
    if (UShapeComponent* ShapeComp = GetActivatedShapeComponent())
    {
        ShapeComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        ShapeComp->SetGenerateOverlapEvents(false);
        ShapeComp->bHiddenInGame = true;
        ShapeComp->SetVisibility(false);
    }

    OnDamageEnded();
}

void ANPDamageAreaActor::OnBeginOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIdx, bool bFromSweep, const FHitResult& Hit)
{
    if (!Other || Other == this || Other == GetInstigator()) 
        return;

    if (AlreadyHitSet.Contains(Other)) 
        return;

    AlreadyHitSet.Add(Other);
    ApplyDamageTo(Other);
}

void ANPDamageAreaActor::ApplyDamageTo(AActor* Other)
{
    float ApplyDamage = UGameplayStatics::ApplyDamage(
        Other,
        DamageSpec.Damage,
        GetInstigator() ? GetInstigator()->GetController() : nullptr,
        this,
        UDamageType::StaticClass()
    );

    ANPCharacterBase* Char = Cast<ANPCharacterBase>(GetInstigator());
    if (Char)
    {
        Char->TryPlayAttackImpactEffect(this, Other, ApplyDamage);
    }

    NP_LOG(NPLog, Warning, TEXT("%f"), ApplyDamage);
}

void ANPDamageAreaActor::OnDamageEnded()
{
    if (DamageSpec.PostDamageLifeSpan > 0.f)
        SetLifeSpan(DamageSpec.PostDamageLifeSpan);
    else
        Destroy();
}

UShapeComponent* ANPDamageAreaActor::GetActivatedShapeComponent()
{
    switch (DamageSpec.Shape)
    {
    case ENPShapeType::Box:
        return BoxComp;
    case ENPShapeType::Sphere:
        return SphereComp;
    case ENPShapeType::Capsule:
        return CapsuleComp;
    }

    return nullptr;
}

/*
    WITH EDITOR
*/
#if WITH_EDITOR
void ANPDamageAreaActor::PreviewDebugPlay(bool bPreviewDebugDraw)
{
    PlayNiagara();
    PreviewDebugDrawEnabled = bPreviewDebugDraw;
    SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SphereComp->SetGenerateOverlapEvents(false);
    SphereComp->bHiddenInGame = true;
    SphereComp->SetSphereRadius(DamageSpec.SphereRadius);
    SphereComp->SetVisibility(false);
    BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BoxComp->SetGenerateOverlapEvents(false);
    BoxComp->bHiddenInGame = true;
    BoxComp->SetBoxExtent(DamageSpec.BoxExtent);
    BoxComp->SetVisibility(false);
    CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CapsuleComp->SetGenerateOverlapEvents(false);
    CapsuleComp->bHiddenInGame = true;
    CapsuleComp->SetCapsuleHalfHeight(DamageSpec.CapsuleHalfHeight);
    CapsuleComp->SetCapsuleRadius(DamageSpec.CapsuleRadius);
    CapsuleComp->SetVisibility(false);

    const float Delay = FMath::Max(0.f, DamageSpec.DamageDelay);
    if (Delay > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(PreivewDelayTimer, this, &ANPDamageAreaActor::PreviewDebugDelayCallback, Delay, false);
        return;
    }
    PreviewDebugDelayCallback();
}
void ANPDamageAreaActor::PreviewDebugDelayCallback()
{
    if (DamageSpec.Mode == ENPDamageMode::TraceOnce)
    {
        PreviewDebugDraw(0.1f);

        if (DamageSpec.PostDamageLifeSpan > 0.f)
            SetLifeSpan(DamageSpec.PostDamageLifeSpan);
        else
            Destroy();
    }
    else if (DamageSpec.Mode == ENPDamageMode::Persistent)
    {
        if (PreviewDebugDrawEnabled)
        {
            if (DamageSpec.Shape == ENPShapeType::Box)
            {
                BoxComp->SetVisibility(true);
            }
            else if (DamageSpec.Shape == ENPShapeType::Sphere)
            {
                SphereComp->SetVisibility(true);
            }
            else if (DamageSpec.Shape == ENPShapeType::Capsule)
            {
                CapsuleComp->SetVisibility(true);
            }
        }

        GetWorld()->GetTimerManager().SetTimer(
            PreviewDamageTimer, this, &ANPDamageAreaActor::PreviewPersistentTimerCallback,
            FMath::Max(0.0f, DamageSpec.DurationSeconds), false);
    }
    else if (DamageSpec.Mode == ENPDamageMode::Periodic)
    {
        DebugDuration = DamageSpec.DamageInterval * 0.5f;
        PreviewCount = 0;

        GetWorld()->GetTimerManager().SetTimer(
            PreviewDamageTimer, this, &ANPDamageAreaActor::PreviewPeriodicTimerCallback,
            FMath::Max(0.05f, DamageSpec.DamageInterval), true, 0.f);
    }
}
void ANPDamageAreaActor::PreviewPersistentTimerCallback()
{
    if (PreviewDebugDrawEnabled)
    {
        SphereComp->SetVisibility(true);
        BoxComp->SetVisibility(true);
        CapsuleComp->SetVisibility(true);
    }

    if (DamageSpec.PostDamageLifeSpan > 0.f)
        SetLifeSpan(DamageSpec.PostDamageLifeSpan);
    else
        Destroy();
}
void ANPDamageAreaActor::PreviewPeriodicTimerCallback()
{
	PreviewDebugDraw(DamageSpec.DamageInterval * 0.5f);

    PreviewCount++;
    if (PreviewCount >= DamageSpec.DamageCount)
    {
        GetWorld()->GetTimerManager().ClearTimer(PreviewDamageTimer);
        if (DamageSpec.PostDamageLifeSpan > 0.f)
            SetLifeSpan(DamageSpec.PostDamageLifeSpan);
        else
            Destroy();
    }
}
void ANPDamageAreaActor::PreviewDebugDraw(float Duration)
{
    if (!PreviewDebugDrawEnabled) return;

    UWorld* World = GetWorld();
    if (!World) return;

    const FTransform Transform = GetActorTransform();
    const FVector Start = Transform.GetLocation();
    const FVector End = Start;

    if (DamageSpec.Shape == ENPShapeType::Box)
    {
        DrawDebugBox(World, Start, DamageSpec.BoxExtent, Transform.GetRotation(), FColor::Red, false, Duration);
    }
    else if (DamageSpec.Shape == ENPShapeType::Sphere)
    {
        DrawDebugSphere(World, Start, DamageSpec.SphereRadius, 16, FColor::Red, false, Duration);
    }
    else //Capsule
    {
        DrawDebugCapsule(World, Start, DamageSpec.CapsuleHalfHeight, DamageSpec.CapsuleRadius, Transform.GetRotation(), FColor::Red, false, Duration);
    }
}
#endif