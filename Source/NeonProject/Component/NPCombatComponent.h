// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/NPSkillData.h"
#include "NPCombatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnSkillPlayedDelegate, ENPAbilityType /*SkillType*/, uint8 /*Combo*/, bool /*bExtra*/);

USTRUCT()
struct FNPAbilityRuntime
{
    GENERATED_BODY()

    UPROPERTY()
    const UNPSkillData* Data = nullptr;

    float LastUsedTime = -FLT_MAX;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEONPROJECT_API UNPCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNPCombatComponent();

    void InitializeCombat(class ANPPlayerCharacter* InOwnerCharacter);

    // Ability 등록 / 찾기
    void RegisterAbility(const UNPSkillData* Skill);
    const UNPSkillData* FindAbility(ENPAbilityType AbilityType) const;

    // 스킬 사용
public:
    bool TryExecuteAbility(ENPAbilityType AbilityType, bool bExtra = false);
private:
    bool ExecuteAbility(ENPAbilityType AbilityType, const FRotator& Rotation, bool bExtra = false);
    bool PlaySkill(ENPAbilityType AbilityType, const FRotator& Rotation, bool bExtra = false);

    // 콤보 관련
public:
    void HandleAttackInput();
    void HandleDashInput();
    void HandleMoveInput();

    // 노티파이에서 호출
    void OnComboWindowBegin();
    void OnComboWindowEnd();  
    void OnMoveWindowBegin(); 
    void OnMoveWindowEnd();   

    // 쿨다운
private:
    bool IsOnCooldown(const UNPSkillData* Skill) const;
    float GetRemainingCooldown(const UNPSkillData* Skill) const;

    // 무적판정
public:
    void OnAttackIgnored(AController* EventInstigator);
    void OnDodgeSuccess();

private:
    const FRotator GetTargetRotation();

    UFUNCTION()
    void OnSkillEnded(UAnimMontage* Montage, bool bInterrupted);

    void SetCurrentSkill(ENPAbilityType SkillType, bool bIsExtra = false);
    void SetNextSkill(ENPAbilityType SkillType);
    void SetBufferedSkill(ENPAbilityType SkillType);

private:
    UPROPERTY(EditDefaultsOnly, Category = "NP|Ability", meta = (AllowPrivateAccess = "true"))
    TMap<ENPAbilityType, TObjectPtr<class UNPSkillData>> SkillDataMap;

    UPROPERTY(EditAnywhere, Category = "NP|Targeting", meta = (AllowPrivateAccess = "true"))
    float AutoTargetingRange = 1500.f;

    UPROPERTY()
    ANPPlayerCharacter* OwnerCharacter = nullptr;

    UPROPERTY()
    TMap<ENPAbilityType, FNPAbilityRuntime> Abilities;

//// 콤보 관련 
    ENPAbilityType CurrentSkill = ENPAbilityType::None;
    ENPAbilityType NextSkill = ENPAbilityType::NormalAttack;
    ENPAbilityType BufferedSkill = ENPAbilityType::None;
    bool bCurrentSkillIsExtra = false;
    int32 CurrentSectionIndex = 0;
    bool bComboWindowOpen = false;
    bool bMoveWindowOpen = false;
    const TArray<FName> ComboSections = { FName("Combo_1")
                                        , FName("Combo_2")
                                        , FName("Combo_3")
                                        , FName("Combo_4")
                                        , FName("Combo_5") };

    const TArray<FName> ExtraComboSections = { FName("Combo_1_Extra")
                                             , FName("Combo_2_Extra")
                                             , FName("Combo_3_Extra")
                                             , FName("Combo_4_Extra")
                                             , FName("Combo_5_Extra") };
//// 타게팅
    UPROPERTY()
    TObjectPtr<AActor> LockedOnTarget = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NP|Collision", meta = (AllowPrivateAccess = "true"))
    TEnumAsByte<ECollisionChannel> MonsterTraceChannel = ECollisionChannel::ECC_GameTraceChannel1;

public:
    FOnSkillPlayedDelegate  OnSkillPlayed;
    FTimerHandle TimerHandle_Dilation;
};

