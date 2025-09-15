// NPCombatTypes.h
#pragma once
#include "CoreMinimal.h"
#include "NPCombatTypes.generated.h"



///////////////////////////////////////
// 데미지 콜리전 타입

UENUM(BlueprintType)
enum class ENPDamageCollisionType : uint8
{
    MonsterHit, //NP_MonsterHit
    PlayerHit,  //NP_PlayerHit
    Count       UMETA(Hidden),
};
const FName NPDamageCollisionProfileName[(uint8)ENPDamageCollisionType::Count] = {
    TEXT("NP_MonsterHit"),
    TEXT("NP_PlayerHit")
};
const ECollisionChannel NPDamageCollisionTraceChannel[(uint8)ENPDamageCollisionType::Count] = {
    ECC_GameTraceChannel1,  //NP_MonsterHit
    ECC_GameTraceChannel2   //NP_PlayerHit
};

///////////////////////////////////////
// 캐릭터 콜리전 타입
UENUM(BlueprintType)
enum class ENPCharacterCollisionType : uint8
{
    PlayerCapsule,  //NP_PlayerCapsule
    MonsterCapsule, //NP_MonsterCapsule
    Count       UMETA(Hidden),
};
const FName NPCharacterCollisionProfileName[(uint8)ENPCharacterCollisionType::Count] = {
    TEXT("NP_PlayerCapsule"),
    TEXT("NP_MonsterCapsule")
};

///////////////////////////////////////
// 공격 어빌리티 타입 (플래그)
UENUM(BlueprintType)
enum class ENPAbilityType : uint8
{
    None        UMETA(Hidden),
    NormalAttack,
    ChargeAttack,
    Dash,
    Skill,
    Ultimate,
    DodgeSuccess,
    SwapIn,
};


///////////////////////////////////////
// 전투 관련 캐릭터 상태 (플래그)
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ENPCharacterState : uint8
{
    None = 0        UMETA(Hidden),
    Active = 1 << 0,
    Move = 1 << 1,
    Run = 1 << 2,
    Invincible = 1 << 3,

    Stagger = 1 << 6,
    Dead = 1 << 7,
};
ENUM_CLASS_FLAGS(ENPCharacterState)

const FString ENPCharacterStateString[8] = {
    TEXT("Active"),
    TEXT("Move"),
    TEXT("Run"),
    TEXT("Invincible"),
    TEXT(""),
    TEXT(""),
    TEXT("Stagger"),
    TEXT("Dead"),
};
const FString GetStateFlagsName(ENPCharacterState flag);

///////////////////////////////////////
// 데미지 판정 타입
UENUM(BlueprintType)
enum class ENPDamageMode : uint8
{
    TraceOnce,      // 한 프레임만 체크 (AnimNotify 타이밍 등)
    Persistent,     // 일정 시간 유지되는 오버랩
    Periodic        // 반복 데미지
};

///////////////////////////////////////
// 데미지 영역 모양
UENUM(BlueprintType)
enum class ENPShapeType : uint8
{
    Box,
    Sphere,
    Capsule,
};

///////////////////////////////////////
// 나이아가라 스펙
USTRUCT(BlueprintType)
struct FNPNiagaraSpec
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<class UNiagaraSystem> NiagaraSystem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform Offset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAttach = true;
};

///////////////////////////////////////
// 데미지 영역 스펙
USTRUCT(BlueprintType)
struct FNPDamageAreaSpec
{
    //meta = (,ClampMin = "0.1", UIMin = "0.1")
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Shape")
    ENPShapeType Shape = ENPShapeType::Box;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Shape", meta = (EditCondition = "Shape==ENPShapeType::Box", EditConditionHides, ClampMin = "0.1", UIMin = "0.1"))
    FVector BoxExtent = FVector(100.f, 100.f, 60.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Shape", meta = (EditCondition = "Shape==ENPShapeType::Sphere", EditConditionHides, ClampMin = "0.1", UIMin = "0.1"))
    float SphereRadius = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Shape", meta = (EditCondition = "Shape==ENPShapeType::Capsule", EditConditionHides, ClampMin = "0.1", UIMin = "0.1"))
    float CapsuleRadius = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Shape", meta = (EditCondition = "Shape==ENPShapeType::Capsule", EditConditionHides, ClampMin = "0.1", UIMin = "0.1"))
    float CapsuleHalfHeight = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Collision")
    ENPDamageCollisionType CollisionType = ENPDamageCollisionType::MonsterHit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Collision")
    ENPDamageMode Mode = ENPDamageMode::TraceOnce;

    // Persistent 모드용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Collision", meta = (EditCondition = "Mode==ENPDamageMode::Persistent", EditConditionHides, ClampMin = "0.1", UIMin = "0.1"))
    float DurationSeconds = 0.2f;

    // 데미지 반복 주기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Collision", meta = (EditCondition = "Mode==ENPDamageMode::Periodic", EditConditionHides, ClampMin = "0.05", UIMin = "0.05"))
    float DamageInterval = 0.2f;

    // 데미지 반복 횟수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Collision", meta = (EditCondition = "Mode==ENPDamageMode::Periodic", EditConditionHides, ClampMin = "1", UIMin = "1"))
    int32 DamageCount = 5;

    // 데미지 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Niagara")
    FNPNiagaraSpec NigaraSpec;

    // 이펙트 싱크를 위한 딜레이
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Niagara", meta = (ClampMin = "0", UIMin = "0"))
    float DamageDelay = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Damage", meta = (ClampMin = "0", UIMin = "0"))
    float Damage = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Timing", meta = (ClampMin = "0", UIMin = "0"))
    float PostDamageLifeSpan = 2.f;
};

///////////////////////////////////////
// 투사체 스펙
USTRUCT(BlueprintType)
struct FNPHomingProjectileSpec
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Collision", meta = (ClampMin = "0", UIMin = "0"))
    float CollisionRadius = 10.f;

    // 투사체 충돌 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Collision")
    ENPDamageCollisionType CollisionType = ENPDamageCollisionType::MonsterHit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Movement", meta = (ClampMin = "0", UIMin = "0"))
    float InitialSpeed = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Movement", meta = (ClampMin = "0", UIMin = "0"))
    float MaxSpeed = 2000.f;

    /* HomingAccel의 베이스값. 거리와 관계없이 베이스로 깔려있는 값.*/ 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Movement", meta = (ClampMin = "0", UIMin = "0"))
    float HomingAccelBase = 0.f;

    /* HomingAccelBase 값에 추가로 거리에 비례하여 더해지는 값의 최대치.\n
    * 목표에 도달하면서 HomingAccel이 Base + HomingAccelBonusMax 에 점점 가까워짐.
    HomingAccel = Base + (MaxRange - DistanceToTarget) / MaxRange * BonusMax */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Movement", meta = (ClampMin = "0", UIMin = "0"))
    float HomingAccelBonusMax = 25000.f;

    // 최대 거리 도달시 투사체 파괴 (캐릭터 타게팅 거리보다 짧게 설정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Movement", meta = (ClampMin = "0", UIMin = "0"))
    float MaxRange = 1500.f;

    // 투사체 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Niagara")
    FNPNiagaraSpec NigaraSpec;

    // 해당 시간 동안 명중하지 못할 경우 폭파
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Timing", meta = (ClampMin = "0", UIMin = "0"))
    float ExplosionTimeout = 2.f;

    // 충돌 후 적용될 LifeSpan
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Timing", meta = (ClampMin = "0", UIMin = "0"))
    float PostImpactLifeSpan = 2.f;
};
