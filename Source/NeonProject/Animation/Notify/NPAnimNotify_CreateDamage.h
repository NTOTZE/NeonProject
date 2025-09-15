// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DataType/NPCombatTypes.h"
#include "NPAnimNotify_CreateDamage.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPAnimNotify_CreateDamage : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
    void DrawDebugBySpec(const FNPDamageAreaSpec& Spec, UWorld* World, const FVector& Location, const FQuat& Rotation);

public:
    // 위치 기준 소켓. 미설정 시 Mesh 위치
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SocketName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAttach = false;

    // 소켓 기준 위치 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector LocationOffset = FVector(100.f, 0.f, 80.f);

    // 캐릭터 기준 회전 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator RotationOffset = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FNPDamageAreaSpec DamageAreaSpec;

    // 디버그 박스 그리기
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDebugDraw = false;

    FTimerHandle TestTimer;
};
