// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/NPCharacterBase.h"
#include "InputAction.h"

#include "DataType/NPCombatTypes.h"

#include "NPPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API ANPPlayerCharacter : public ANPCharacterBase
{
	GENERATED_BODY()

public:
	ANPPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void Die() override;
public:
	void OnMoveForwardInput(FVector ForwardVector, float ForwardInput);
	void OnMoveRightInput(FVector RightVector, float RightInput);
	void OnMoveStop();
	void HandleStateChange(ENPCharacterState Flags, bool Value);
	void HandleSkillPlayed(ENPAbilityType AbilityType, uint8 Combo, bool bExtra);

protected:
	virtual void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted) override;

public:
	bool TryExecuteAbility(ENPAbilityType AbilityType);
	void OnAttackInput();
	void OnDashInput();

	void CharacterHiddenWithDissolve(bool bCharHidden, float fDissolveDuration);

private:
	void CharacterActivate();
	void CharacterDeativate();
public:
	void SwapIn(const FTransform& NewTransform);
	void SwapOut();

	void SetTarget(AActor* Target) { RecentTarget = Target; }
	virtual AActor* GetTarget() override { return RecentTarget.IsValid() ? RecentTarget.Get() : nullptr; }
protected:
	UPROPERTY(EditDefaultsOnly, Category = "NP|Player|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNPCombatComponent> CombatComp;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Player|Dissolve", meta = (AllowPrivateAccess = "true"))
	float SwapDissolveDuration = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Player", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 240.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "NP|Player", meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 630.f;

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> RecentTarget = nullptr;

	FVector2D MoveInput;
};
