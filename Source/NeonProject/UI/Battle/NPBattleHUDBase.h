// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPBattleHUDBase.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPBattleHUDBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetPlayerHpBar(float current, float max);
	void SetPlayerStaminaBar(float current, float max);
	void SetPlayerSkillCostBar(float current, float max);
	void SetPlayerUltimateCostBar(float current, float max);

	void SetMemberHpBar(int32 Idx, float current, float max);
	void SetMemberSkillCostBar(int32 Idx, float current, float max);
	void SetMemberUltimateCostBar(int32 Idx, float current, float max);

	void SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture);
	void SetMemberOpacity(int32 Idx, float Opacity);
	void SetWaveText(const FString& str);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPPlayerInfoBase> PlayerInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPPartyInfoBase> PartyInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPWaveInfo> WaveInfo;

	FTimerHandle BlackScreenTimer;
	float BlackScreenEnabled;
	float FadeDuration;
};
