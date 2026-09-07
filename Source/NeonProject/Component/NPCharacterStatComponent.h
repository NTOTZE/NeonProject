// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NPCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnResourceStatChangedDelegate, ENPResourceStatType, float /*CurrentValue*/, float /*MaxValue*/);

UENUM(BlueprintType)
enum class ENPResourceStatType : uint8
{
	Hp,
	Stamina,
	SkillCost,
	UltimateCost,

	Count	UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct FNPResourceStat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "NP|Stat")
	float MaxValue = 0.f;

	UPROPERTY(VisibleInstanceOnly, Category = "NP|Stat")
	float CurrentValue = 0.f;

	UPROPERTY(EditAnywhere, Category = "NP|Stat")
	float RegenPerSec = 0.f;

	UPROPERTY(EditAnywhere, Category = "NP|Stat", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float HitRecoveryAmount = 0.f;
};

USTRUCT(BlueprintType)
struct FNPNaturalRecoveryContext
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NaturalRecovery")
	float RecoveryMultiplier = 1.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEONPROJECT_API UNPCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNPCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FORCEINLINE const FNPResourceStat& GetResourceStat(ENPResourceStatType Type) { return ResourceStat[(uint8)Type]; }

	float ApplyDamage(float NewDamage);
	void ApplyNaturalRecovery(FNPNaturalRecoveryContext context, float DeltaSeconds);
	void RecoverUltimateCostOnHit();

	void RecoverResourceStat(ENPResourceStatType Type, float Amount);
	bool ConsumeResourceStat(ENPResourceStatType Type, float Amount);

public:
	FOnHpZeroDelegate OnHpZero;
private:
	void SetResourceStat(ENPResourceStatType Type, FNPResourceStat NewStat);
	void SetResourceCurrentValue(ENPResourceStatType Type, float NewValue);

	UPROPERTY(EditAnywhere, Category = "NP|Stat")
	FNPResourceStat ResourceStat[(uint8)ENPResourceStatType::Count];

public:
	FOnResourceStatChangedDelegate OnResourceStatChanged;
};
