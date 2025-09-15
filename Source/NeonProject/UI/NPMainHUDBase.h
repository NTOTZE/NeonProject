// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPMainHUDBase.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPMainHUDBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetPlayerHPBarRatio(float Ratio);
	void SetMemberHPBarRatio(int32 Idx, float Ratio);
	void SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture);
	void SetMemberOpacity(int32 Idx, float Opacity);
	void SetWaveText(const FString& str);
	void EnableBlackScreen(bool bEnable, float duration);

private:
	void BlackScreenTimerCallback();

private:
	TObjectPtr<class UNPPlayerInfoBase> PlayerInfo;
	FName PlayerInfoWidgetName = TEXT("PlayerInfo");

	TObjectPtr<class UNPPartyInfoBase> PartyInfo;
	FName PartyInfoWidgetName = TEXT("PartyInfo");

	TObjectPtr<class UNPWaveInfo> WaveInfo;
	FName WaveInfoWidgetName = TEXT("WaveInfo");

	TObjectPtr<class UBorder> BlackScreen;
	FName BlackScreenWidgetName = TEXT("BlackScreen");

	FTimerHandle BlackScreenTimer;
	float BlackScreenEnabled;
	float FadeDuration;
};
