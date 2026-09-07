// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/NPAnimNotify_SpawnProjectile.h"

#include "Character/NPCharacterBase.h"
#include "Combat/NPHomingProjectile.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

void UNPAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;
    
    // 기준 트랜스폼 결정 (소켓 우선 → 없으면 메시에 붙은 컴포넌트 기준)
    FTransform BaseTransform;
    if (SocketName != NAME_None && MeshComp->DoesSocketExist(SocketName))
    {
        BaseTransform = MeshComp->GetSocketTransform(SocketName, RTS_World);
        BaseTransform.SetRotation(MeshComp->GetComponentQuat());
    }
    else
    {
        BaseTransform = MeshComp->GetComponentTransform();
    }

    // Mesh는 FRotator(0.f, 0.f, 0.f)가 캐릭터 정면이 아니라 Yaw값이 -90 틀어져있음.
    // 이를 위해 조정
    const FRotator Rot90 = FRotator(0.f, 90.f, 0.f);

    const FTransform LocalTransform(RotationOffset + Rot90, Rot90.RotateVector(LocationOffset));
    const FTransform SpawnTransform = LocalTransform * BaseTransform;

    UWorld* World = Owner->GetWorld();
    if (!World) return;

    if (bDebugDraw)
    {
        const FVector ArrowStart = SpawnTransform.GetLocation();
        const FVector ArrowEnd = ArrowStart + SpawnTransform.GetRotation().GetForwardVector() * 80.f;
        DrawDebugSphere(World, ArrowStart, 15.f, 4, FColor::Red, false, DebugDrawDuration, 0, 0.5f);
        DrawDebugDirectionalArrow(World, ArrowStart, ArrowEnd, 50.f, FColor::Red, false, DebugDrawDuration, 0, 2.f);
    }
   
    ANPCharacterBase* NPChar = Cast<ANPCharacterBase>(Owner);
    if (!NPChar) return;

    FActorSpawnParameters Params;
    Params.Owner = Owner;
    Params.Instigator = Cast<APawn>(Owner);
    Params.bDeferConstruction = true;

    ANPHomingProjectile* Proj = World->SpawnActor<ANPHomingProjectile>(ANPHomingProjectile::StaticClass(), SpawnTransform, Params);
    if (!Proj) return;

    AActor* Target = NPChar->GetTarget();
    Proj->InitProjectile(ProjectileSpec, DamageSpec, Target, bDebugDraw);
    Proj->FinishSpawning(SpawnTransform, false, nullptr, ESpawnActorScaleMethod::MultiplyWithRoot);
}
