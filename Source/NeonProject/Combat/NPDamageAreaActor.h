// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataType/NPCombatTypes.h"
#include "NPDamageAreaActor.generated.h"



UCLASS()
class NEONPROJECT_API ANPDamageAreaActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPDamageAreaActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void InitializeFromSpec(const FNPDamageAreaSpec& Spec);
	void PlayNiagara();
	void DamageActivate();

private:
	// === 내부 구현 ===
	void RunTraceOnce();
	void RunPeriodic();
	void ActivatePersistent();
	void DeactivatePersistent();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
		UPrimitiveComponent* OtherComp, int32 BodyIdx, bool bFromSweep, const FHitResult& Hit);

	void ApplyDamageTo(AActor* Other);

	void OnDamageEnded();

	class UShapeComponent* GetActivatedShapeComponent();
private:
	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> NiagaraComp;
	
	UPROPERTY()
	TObjectPtr<class USceneComponent> SceneComp;

	UPROPERTY()
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY()
	TObjectPtr<class USphereComponent> SphereComp;

	UPROPERTY()
	TObjectPtr<class UCapsuleComponent> CapsuleComp;

private:
	// 데미지 스펙
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP", meta=(AllowPrivateAccess = "true"))
	FNPDamageAreaSpec DamageSpec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebug = false;

private:
	TSet<TWeakObjectPtr<AActor>> AlreadyHitSet;	// 중복 타격 방지 TSet
	FTimerHandle DamageTimer;	// 데미지 판정 타이머
	FTimerHandle DelayTimer;	// 데미지 판정 지속 시간 타이머
	int32 CurrentCount = 0;
	float DebugDuration = 0.1f;


#if WITH_EDITOR
public:
	void PreviewDebugPlay(bool bPreviewDebugDraw);
private:
	void PreviewDebugDelayCallback();
	void PreviewPersistentTimerCallback();
	void PreviewPeriodicTimerCallback();
	void PreviewDebugDraw(float Duration);
	bool PreviewDebugDrawEnabled = false;
	int32 PreviewCount = 0;
	FTimerHandle PreviewDamageTimer;	// 데미지 판정 타이머
	FTimerHandle PreivewDelayTimer;	// 데미지 판정 지속 시간 타이머
#endif
};
