// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Battle/NPBattleHUDBase.h"

#include "NeonProject.h"

#include "UI/Battle/NPPlayerInfoBase.h"
#include "UI/Battle/NPPartyInfoBase.h"
#include "UI/Battle/NPMemberInfoBase.h"
#include "UI/Battle/NPWaveInfo.h"
#include "Components/Border.h"

void UNPBattleHUDBase::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (!World) return;

	if (!IsValid(PlayerInfo))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}
	
	if (!IsValid(PartyInfo))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}

	if (!IsValid(WaveInfo))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}
}

void UNPBattleHUDBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UNPBattleHUDBase::SetPlayerHpBar(float current, float max)
{
	PlayerInfo->SetHpBar(current, max);
}

void UNPBattleHUDBase::SetPlayerStaminaBar(float current, float max)
{
	PlayerInfo->SetStaminaBar(current, max);
}

void UNPBattleHUDBase::SetPlayerSkillCostBar(float current, float max)
{
	PlayerInfo->SetSkillCostBar(current, max);
}

void UNPBattleHUDBase::SetPlayerUltimateCostBar(float current, float max)
{
	PlayerInfo->SetUltimateCostBar(current, max);
}

void UNPBattleHUDBase::SetMemberHpBar(int32 Idx, float current, float max)
{
	PartyInfo->SetMemberHpBar(Idx, current, max);
}

void UNPBattleHUDBase::SetMemberSkillCostBar(int32 Idx, float current, float max)
{
	PartyInfo->SetMemberSkillCostBar(Idx, current, max);
}

void UNPBattleHUDBase::SetMemberUltimateCostBar(int32 Idx, float current, float max)
{
	PartyInfo->SetMemberUltimateCostBar(Idx, current, max);
}

void UNPBattleHUDBase::SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture)
{
	PartyInfo->SetMemberImage(Idx, SoftTexture);
}

void UNPBattleHUDBase::SetMemberOpacity(int32 Idx, float Opacity)
{
	PartyInfo->SetMemberOpacity(Idx, Opacity);
}

void UNPBattleHUDBase::SetWaveText(const FString& str)
{
	WaveInfo->SetTextBlock(str);
}
