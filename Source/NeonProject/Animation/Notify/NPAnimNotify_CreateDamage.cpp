// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/NPAnimNotify_CreateDamage.h"
#include "NeonProject.h"
#include "Combat/NPDamageAreaActor.h"

#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

void UNPAnimNotify_CreateDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    // 기준 트랜스폼 결정 (소켓 우선 → 없으면 메시에 붙은 컴포넌트 기준)
    FTransform BaseTransform;
    if (SocketName != NAME_None && MeshComp->DoesSocketExist(SocketName))
    {
        BaseTransform = MeshComp->GetSocketTransform(SocketName, RTS_World);
        //BaseTransform.SetRotation(MeshComp->GetComponentQuat());
    }
    else
    {
        BaseTransform = MeshComp->GetComponentTransform();
    }


    // Mesh 기준이라 그런가 Yaw 90도가 돌아가있음. 조정
    const FRotator Rot90 = FRotator(0.f, 90.f, 0.f);

    // 로컬(캐릭터/소켓 기준) 오프셋/회전을 월드로 변환
    const FRotator LocalRotation = RotationOffset + Rot90;
    const FVector LocalLocation = Rot90.RotateVector(LocationOffset);

    const FTransform LocalXform(LocalRotation, LocalLocation);
    const FTransform DamageXform = LocalXform * BaseTransform;

    const FVector Center = DamageXform.GetLocation();
    const FQuat   RotQ = DamageXform.GetRotation();


    UWorld* World = Owner->GetWorld();
    if (!World) return;

    FActorSpawnParameters Params;
    Params.Owner = Owner;
    Params.Instigator = Cast<APawn>(Owner);
    Params.bDeferConstruction = true;

    ANPDamageAreaActor* AreaActor = GetWorld()->SpawnActor<ANPDamageAreaActor>(ANPDamageAreaActor::StaticClass(), DamageXform, Params);
    if (!AreaActor) return;
    AreaActor->InitializeFromSpec(DamageAreaSpec);

    if (bAttach)
    {
        if (SocketName != NAME_None && MeshComp->DoesSocketExist(SocketName))
        {
            AreaActor->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepWorldTransform, SocketName);
        }
        else
        {
            AreaActor->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepWorldTransform);
        }
    }

    AreaActor->FinishSpawning(DamageXform, false, nullptr, ESpawnActorScaleMethod::OverrideRootScale);



#if WITH_EDITOR
    if (World->IsPreviewWorld())
    {
        AreaActor->PreviewDebugPlay(bDebugDraw);
    }
#endif
}

void UNPAnimNotify_CreateDamage::DrawDebugBySpec(const FNPDamageAreaSpec& Spec, UWorld* World, const FVector& Location, const FQuat& Rotation)
{
    float Duration = 0.1f;

    if (Spec.Shape == ENPShapeType::Box)
    {
        DrawDebugBox(World, Location, Spec.BoxExtent, Rotation, FColor::Red, false, Duration, 0, 1.f);
    }
    else if (Spec.Shape == ENPShapeType::Sphere)
    {
        DrawDebugSphere(World, Location, Spec.SphereRadius, 8, FColor::Red, false, Duration, 0, 1.f);
    }
    else if (Spec.Shape == ENPShapeType::Capsule)
    {
        DrawDebugCapsule(World, Location, Spec.CapsuleHalfHeight, Spec.CapsuleRadius,Rotation, FColor::Red, false, Duration, 0, 1.f);
    }
}
