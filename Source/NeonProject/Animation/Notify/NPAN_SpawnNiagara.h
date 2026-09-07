// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NPAN_SpawnNiagara.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "NP Spawn Niagara (with PlayRate)")
class NEONPROJECT_API UNPAN_SpawnNiagara : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

    // 스폰할 나이아가라 시스템
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
    TObjectPtr<class UNiagaraSystem> NiagaraSystem = nullptr;

    // 소켓 이름(비어있으면 루트 기준)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
    FName SocketName = NAME_None;

    // 스폰 방식: 부착 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
    bool bAttachToMesh = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
    FVector LocationOffset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
    FRotator RotationOffset = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara",
        meta = (ClampMin = "0.01", UIMin = "0.01"))
	FVector Scale = FVector(1.f, 1.f, 1.f);
};
