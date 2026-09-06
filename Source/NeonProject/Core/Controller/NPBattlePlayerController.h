// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Controller/NPPlayerControllerBase.h"
#include "DataType/NPCombatTypes.h"
#include "Interface/NPTargetingInterface.h"
#include "Interface/NPBattleHUDInterface.h"
#include "NPBattlePlayerController.generated.h"

/**
 * 
 */

struct FInputActionValue;
enum class ETriggerEvent : uint8;

UCLASS()
class NEONPROJECT_API ANPBattlePlayerController : public ANPPlayerControllerBase, public INPTargetingInterface, public INPBattleHUDInterface
{
	GENERATED_BODY()
	
public:
	ANPBattlePlayerController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void SetupInputComponent() override;
	virtual bool ExecuteInputCommand(FNPInputCommand Command) override;

public:	// Party
	void SpawnAndInitParty(FTransform SpawnTransform, float SpawnDelay);
	void BeginSpawnCharacter();
	void HandleMemberStateChange(ENPCharacterState Flags, bool bValue, int32 idx);

/////////////////////////////////////////////////
// Targeting Interface
public:	
	virtual AActor* GetLockOnTarget() const override;
	virtual AActor* FindTargetAround(ECollisionChannel TraceChannel, float Radius) const override;
	virtual AActor* FindTargetInFront(ECollisionChannel TraceChannel, float Radius, const FVector& Direction, float AngleDeg) const override;


/////////////////////////////////////////////////
// 	Battle HUD
private: // Battle HUD Init
	void InitBattleHUD();

public:	// INPBattleHUDInterface
	virtual void SetPlayCharacterHpBar(float current, float max) override;
	virtual void SetPlayCharacterStaminaBar(float current, float max) override;
	virtual void SetPlayCharacterSkillCostBar(float current, float max) override;
	virtual void SetPlayCharacterUltimateCostBar(float current, float max) override;
	virtual void SetMemberHpBar(int32 Idx, float current, float max) override;
	virtual void SetMemberSkillCostBar(int32 Idx, float current, float max) override;
	virtual void SetMemberUltimateCostBar(int32 Idx, float current, float max) override;
	virtual void SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture) override;
	virtual void SetMemberOpacity(int32 Idx, float Opacity) override;
	virtual void SetWaveText(const FString& str) override;

/////////////////////////////////////////////////
public:
	void ApplyTimeDilation(float Dilation, float Duration);
	void InitControlRotation();
	void SetViewTargetToRig();

private:	// Input
	UFUNCTION() 
	void HandlePossessedPawnChanged(APawn* PrevPawn, APawn* NextPawn);


public:
	UFUNCTION(BlueprintCallable) class ANPCameraRig* GetCameraRigActor() { return CameraRig; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UNPPartyComponent> PartyComp;

	UPROPERTY()
	TObjectPtr<class ANPCameraRig> CameraRig;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> CutsceneSubLevel;

	UPROPERTY()
	TObjectPtr<class ANPBattlePlayerCharacter> CachedCharacter = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_Attack;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_AttackHold;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_Skill;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_Ultimate;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_Swap1;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_Swap2;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_Swap3;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Input")
	TObjectPtr<UInputAction> IA_LockOn;

	UPROPERTY()
	TSubclassOf<class UUserWidget>	BattleHudClass;

	UPROPERTY()
	TObjectPtr<class UNPBattleHUDBase> BattleHUD;

	FTimerHandle TimerHandle_Spawn;
	FTimerHandle TimerHandle_Dilation;

	friend class ANPPlayerCharacterBase;
};
