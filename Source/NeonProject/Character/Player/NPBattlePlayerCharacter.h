// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/NPPlayerCharacterBase.h"
#include "NPBattlePlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API ANPBattlePlayerCharacter : public ANPPlayerCharacterBase
{
	GENERATED_BODY()
	
public:
	ANPBattlePlayerCharacter();

protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void Die() override;

private:
	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

public:
	virtual void OnMoveForwardInput(FVector ForwardVector, float ForwardInput);
	virtual void OnMoveRightInput(FVector RightVector, float RightInput);
	virtual void OnMoveStop();

	virtual bool HandleInputCommand(class AController* InstigatorController, FNPInputCommand Command) override;
	void HandleSkillPlayed(ENPAbilityType AbilityType, uint8 Combo, bool bExtra);

	void SwapIn(const FTransform& NewTransform);
	void SwapOut();

private:
	void CharacterActivate();
	void CharacterDeativate();
	void CharacterHiddenWithDissolve(bool bCharHidden, float fDissolveDuration);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "NP|Player|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UNPCombatComponent> CombatComp;

private:
	UPROPERTY(EditDefaultsOnly, Category = "NP|Player|Dissolve", meta = (AllowPrivateAccess = "true"))
	float SwapDissolveDuration = 0.3f;
};
