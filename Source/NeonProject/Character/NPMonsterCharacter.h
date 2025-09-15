// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/NPCharacterBase.h"
#include "NPMonsterCharacter.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API ANPMonsterCharacter : public ANPCharacterBase
{
	GENERATED_BODY()

public:
	ANPMonsterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void LookAtTarget(bool b);
	virtual void Die() override;
	virtual AActor* GetTarget() override;

private:
	void OnHPChanged(float OldHP, float NewHP);

private:
	UPROPERTY(EditAnywhere, Category = "NP|UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> WidgetComp;

    UPROPERTY(EditAnywhere, Category="NP|AI", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

    UPROPERTY(EditAnywhere, Category = "NP|AI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UBlackboardData> BlackboardDataAsset;

	UPROPERTY()
	TObjectPtr<class UNPMonsterInfoBase> MonsterUI;

	UPROPERTY(EditAnywhere, Category = "NP|Death", meta = (AllowPrivateAccess = "true"))
	float DeathLifeSpan = 5.f;
private:
	bool bLookAtTarget = false;

	friend class ANPAIController;
};
