// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DataType/NPCombatTypes.h"
#include "NPAnimNotify_SpawnProjectile.generated.h"


UCLASS()
class NEONPROJECT_API UNPAnimNotify_SpawnProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	// 위치 기준 소켓. 미설정 시 Mesh 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SocketName = NAME_None;

	// 소켓 기준 위치 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector LocationOffset = FVector::ZeroVector;

	// 캐릭터 기준 회전 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FNPHomingProjectileSpec ProjectileSpec;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FNPDamageAreaSpec DamageSpec;

	// 디버그 화살표 그리기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDebugDraw = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition="bDebugDraw"))
	float DebugDrawDuration = 0.3f;
};
