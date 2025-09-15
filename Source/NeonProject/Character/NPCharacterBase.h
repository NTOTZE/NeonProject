// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataType/NPCombatTypes.h"
#include "NPCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStateChangeDelegate, ENPCharacterState /*Flags*/, bool /*Value*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChangeDelegate, float /*OldHP*/, float/*NewHP*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeadDelegate, AActor*, DeadActor);

UCLASS()
class NEONPROJECT_API ANPCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	virtual void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void TryPlayHitReaction(float Damage);

public:
	void TryPlayAttackImpactEffect(AActor* DamageActor, AActor* DamageCauser, float Damage);

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Die();

	// 블루프린트 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "NP|Events")
	void BP_OnDamaged(float NewHP, float Damage, AActor* Causer);

	// 블루프린트 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "NP|Events")
	void BP_OnDied(AActor* Causer);

/////////////////////
//	Niagara
private:
	void SpawnEffect(UNiagaraSystem* Effect, const FVector& Location, const FRotator& Rotation);

/////////////////////
//	Dissolve
protected: 
	void InitDissolve();
	void SetDissolveAppearance(float Value);
private:
	void DissolveTimerCallback();
public:
	void PlayDissolve(bool bAppear, float Duration);

private:
	void BeginDeathDissolve();

// State
public:
	void AddState(ENPCharacterState flags);
	void RemoveState(ENPCharacterState flags);
	bool HasAnyState(ENPCharacterState flags);
	bool HasAllState(ENPCharacterState flags);

// Debug
public:
	bool IsSkillDebugDrawEnabled() { return bSkillDebugDrawEnable; }

//	UI
	float GetMaxHP() { return MaxHP; }
	float GetCurrentHP() { return CurrentHP; }
	TSoftObjectPtr<class UTexture2D> GetCharacterSoftTexture() { return CharacterImage; }

private:
	void SetHP(float NewHP);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "NP|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Stats", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<class UTexture2D> CharacterImage;

	// ─ HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Stats", meta = (AllowPrivateAccess = "true"))
	float CurrentHP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Animation|LightStagger", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> LightStaggerMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Animation|LightStagger", meta = (AllowPrivateAccess = "true"))
	float LightStaggerDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Animation|HeavyStagger", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> HeavyStaggerMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Animation|HeavyStagger", meta = (AllowPrivateAccess = "true"))
	float HeavyStaggerDamage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Animation|Death", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Animation|Death", meta = (AllowPrivateAccess = "true"))
	float DissolveDelayAfterDeath = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Animation|Death", meta = (AllowPrivateAccess = "true"))
	float DeathDissolveDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Effect|AttackImpact", meta = (AllowPrivateAccess = "true"))

	TObjectPtr<class UNiagaraSystem> AttackImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Effect|AttackImpact", meta = (AllowPrivateAccess = "true", ClampMin = "0.1", UIMin = "0.1"))
	float AttackImpactEffectScale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Debug|Skill", meta = (AllowPrivateAccess = "true"))
	bool bSkillDebugDrawEnable = false;

private:
	FName DissolveParamName = TEXT("Appearance"); 

public:
	virtual AActor* GetTarget() { return nullptr; }
////////////////////////////////
// 내부 변수

private:  // 사망 처리
	FTimerHandle TimerHandle_DeathDissolve;

private:  //Dissolve 내부 변수
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UMaterialInstanceDynamic>> DissolveMIDs;

	FTimerHandle TimerHandle_Dissolve;
	float StartAppearance;
	float EndAppearance;
	float CachedAppearance = 0.f;
	float DissolveDuration;
	float DissolveStartTime;


public: //State
	FOnStateChangeDelegate OnStateChange;
	FOnHPChangeDelegate OnHPChange;
	FOnDeadDelegate OnDead;
protected:
	ENPCharacterState State = ENPCharacterState::None;
};
