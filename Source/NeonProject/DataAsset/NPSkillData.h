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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Type")
    ENPAbilityType AbilityType;

    // 연결된 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    UAnimMontage* Montage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    uint8 MaxCombo = 1;

    // 컷씬 재생 여부
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cutscene")
    bool bPlaySkillCutscene = false;

    // 스킬 컷씬 레벨시퀀스
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cutscene", meta = (EditCondition = "bPlaySkillCutscene==true"))
    TObjectPtr<class ULevelSequence> SkillCutscene;

    // 스킬 컷씬 종료 후 카메라 복귀 BlendTime
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cutscene", meta = (EditCondition = "bPlaySkillCutscene==true", ClampMin = "0", UIMin = "0"))
    float RestoreViewBlendTime = 0.5f;

    // 타게팅 여부
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spec")
    bool bAutoTarget = true;

    // 쿨다운 시간(초)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spec")
    float Cooldown = 1.0f;

    // 공격 소모량 (Skill은 SkillCost, Ultimate는 UltimateCost, 그 외엔 Stamina 소모량)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spec")
    float Cost = 0.f;
};
