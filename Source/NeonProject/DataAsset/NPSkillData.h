// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataType/NPCombatTypes.h"
#include "NPSkillData.generated.h"


UCLASS()
class NEONPROJECT_API UNPSkillData : public UDataAsset
{
	GENERATED_BODY()
public:
    // 어빌리티 타입
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ENPAbilityType AbilityType;

    // 연결된 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UAnimMontage* Montage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    uint8 MaxCombo = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bAutoTarget = true;

    // 쿨다운 시간(초)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Cooldown = 1.0f;

    // MP(마나) 소모량
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float StaminaCost = 0.f;
};
