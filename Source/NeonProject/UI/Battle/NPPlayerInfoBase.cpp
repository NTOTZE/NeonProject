// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Battle/NPPlayerInfoBase.h"
#include "NeonProject.h"
#include "UI/Battle/NPProgressBarWidget.h"
#include "Components/TextBlock.h"

void UNPPlayerInfoBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (HpBar == nullptr)
		NP_LOG(NPLog, Error, TEXT("HPBar is nullptr."));

	if (HpText == nullptr)
		NP_LOG(NPLog, Error, TEXT("HpText is nullptr."));

	if (SkillCostBar == nullptr)
		NP_LOG(NPLog, Error, TEXT("SkillCostBaris nullptr."));

	if (UltimateCostBar == nullptr)
		NP_LOG(NPLog, Error, TEXT("UltimateCostBar is nullptr."));

	SetHpText(0.f, 0.f);
}

void UNPPlayerInfoBase::SetHpBar(float current, float max)
{
	if (HpBar == nullptr)
	{
		NP_LOG(NPLog, Error, TEXT("HpBar is nullptr."));
		return;
	}

	if (max <= 0.f)
	{
		NP_LOG(NPLog, Error, TEXT("Max HP가 0보다 작거나 같습니다. 값을 1.f로 대체합니다."));
		max = 1.f;
	}

	SetHpText(current, max);
	const float ratio = FMath::Clamp(current / max, 0.f, 1.f);
	HpBar->SetPercent(ratio);
}

void UNPPlayerInfoBase::SetStaminaBar(float current, float max)
{
}

void UNPPlayerInfoBase::SetSkillCostBar(float current, float max)
{
	if (SkillCostBar == nullptr)
	{
		NP_LOG(NPLog, Error, TEXT("HpBar is nullptr."));
		return;
	}

	if (max <= 0.f)
	{
		NP_LOG(NPLog, Error, TEXT("Max HP가 0보다 작거나 같습니다. 값을 1.f로 대체합니다."));
		max = 1.f;
	}

	const float ratio = FMath::Clamp(current / max, 0.f, 1.f);
	SkillCostBar->SetPercent(ratio);
}

void UNPPlayerInfoBase::SetUltimateCostBar(float current, float max)
{
	if (UltimateCostBar == nullptr)
	{
		NP_LOG(NPLog, Error, TEXT("HpBar is nullptr."));
		return;
	}

	if (max <= 0.f)
	{
		NP_LOG(NPLog, Error, TEXT("Max HP가 0보다 작거나 같습니다. 값을 1.f로 대체합니다."));
		max = 1.f;
	}

	const float ratio = FMath::Clamp(current / max, 0.f, 1.f);
	UltimateCostBar->SetPercent(ratio);
}

void UNPPlayerInfoBase::SetHpText(float current, float max)
{
	FString str = FString::Printf(TEXT("%d / %d"), (int)current, (int)max);
	HpText->SetText(FText::FromString(str));
}
