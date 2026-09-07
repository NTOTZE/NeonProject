// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NPBattleHUDInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNPBattleHUDInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NEONPROJECT_API INPBattleHUDInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetPlayCharacterHpBar(float current, float max) = 0;
	virtual void SetPlayCharacterStaminaBar(float current, float max) = 0;
	virtual void SetPlayCharacterSkillCostBar(float current, float max) = 0;
	virtual void SetPlayCharacterUltimateCostBar(float current, float max) = 0;

	virtual void SetMemberHpBar(int32 Idx, float current, float max) = 0;
	virtual void SetMemberSkillCostBar(int32 Idx, float current, float max) = 0;
	virtual void SetMemberUltimateCostBar(int32 Idx, float current, float max) = 0;

	virtual void SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture) = 0;
	virtual void SetMemberOpacity(int32 Idx, float Opacity) = 0;

	virtual void SetWaveText(const FString& str) = 0;
};
