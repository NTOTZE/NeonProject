// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataType/NPCombatTypes.h"
#include "NPHomingProjectile.generated.h"

UCLASS()
class NEONPROJECT_API ANPHomingProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPHomingProjectile();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    void InitProjectile(const FNPHomingProjectileSpec& ProjectileSpec, const FNPDamageAreaSpec& DamageSpec, AActor* Target, bool DrawDebug = false);

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
   
    UFUNCTION()
    void OnTargetDistroyed(AActor* DestroyedActor);

private:
    void HandleImpact(AActor* HitActor);
    void SetHomingLocation(const FVector& TargetLocation);
    float GetDistanceToTarget();
    float GetDistanceToOwner();

protected:  // Component
    UPROPERTY()
    TObjectPtr<class UNiagaraComponent> NiagaraComp;

    UPROPERTY()
    TObjectPtr<class USphereComponent> CollisionComp;

    UPROPERTY()
    TObjectPtr<class UProjectileMovementComponent> MovementComp;

private:    // Edit
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP", meta = (AllowPrivateAccess = "true"))
    FNPHomingProjectileSpec Spec;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP", meta = (AllowPrivateAccess = "true"))
    FNPDamageAreaSpec DamageAreaSpec;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Debug", meta = (AllowPrivateAccess = "true"))
    bool bDrawDebug = false;

private:
    // 좌표기반 추적용 더미
    UPROPERTY()
    TObjectPtr<USceneComponent> HomingLocationProxy = nullptr;

    UPROPERTY()
    TObjectPtr<AActor> TargetActor;

    FVector InitialOwnerLocation;
    bool bHasImpacted = false;
    float ElapsedTime = 0.f;


};
