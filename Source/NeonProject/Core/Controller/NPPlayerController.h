// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "DataType/NPCombatTypes.h"

#include "NPPlayerController.generated.h"

/**
 * 
 */

struct FInputActionValue;
enum class ETriggerEvent : uint8;

UCLASS()
class NEONPROJECT_API ANPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ANPPlayerController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void SetupInputComponent() override;

public:	// Party
	void SpawnAndInitParty(FTransform SpawnTransform, float SpawnDelay);
	void BeginSpawnCharacter();
	void HandleMemberStateChange(ENPCharacterState Flags, bool bValue, int32 idx);
	void HandleMemberHPChange(float OldHP, float NewHP, int32 Idx);

public:	// Target
	AActor* GetLockOnTarget() const;
	AActor* SearchNearTargetWithinCircle(ECollisionChannel TraceChannel, float Radius);
	AActor* SearchNearTargetWithinSector(ECollisionChannel TraceChannel, float Radius, const FVector& Direction, float AngleDeg);

public:
	void ApplyTimeDilation(float Dilation, float Duration);


private:	// Input
	UFUNCTION() 
	void HandlePossessedPawnChanged(APawn* PrevPawn, APawn* NextPawn);

	void HandleMoveForward(const FInputActionValue& Value);
	void HandleMoveRight(const FInputActionValue& Value);
	void HandleLookUp(const FInputActionValue& Value);

	void HandleAttack();
	void HandleDash();
	void HandleCombatAction(ENPAbilityType AbilityType);

	void HandleSwap(int SwapNumber);

	void HandleLockOn();
	void HandleUnlock();


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNPPartyComponent> PartyComp;

	UPROPERTY()
	TObjectPtr<class ANPCameraRig> CameraRig;

	UPROPERTY()
	TObjectPtr<class ANPPlayerCharacter> CachedCharacter = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> IMC_Character;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_MoveForward;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_MoveRight;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_LookUp;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Attack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_AttackHold;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Dash;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Skill;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Ultimate;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Swap1;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Swap2;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Swap3;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_LockOn;

	UPROPERTY()
	float CachedMouseSensitivity = 1.f;

	FRotator CachedControlRotation;

	FTimerHandle TimerHandle_Spawn;
	FTimerHandle TimerHandle_Dilation;
};
