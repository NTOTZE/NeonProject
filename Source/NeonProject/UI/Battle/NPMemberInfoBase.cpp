// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Battle/NPMemberInfoBase.h"
#include "NeonProject.h"
#include "UI/Battle/NPProgressBarWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UNPMemberInfoBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (Image == nullptr)
		NP_LOG(NPLog, Error, TEXT("Image is nullptr."));

	if (HpBar == nullptr)
		NP_LOG(NPLog, Error, TEXT("HpBar is nullptr."));

	//if (SkillCostBar == nullptr)
	//	NP_LOG(NPLog, Error, TEXT("SkillCostBaris nullptr."));

	if (UltimateCostBar == nullptr)
		NP_LOG(NPLog, Error, TEXT("UltimateCostBar is nullptr."));
}

void UNPMemberInfoBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UNPMemberInfoBase::SetHpBar(float current, float max)
{
	if (max <= 0.f)
	{
		NP_LOG(NPLog, Error, TEXT("max 값이 0보다 작거나 같습니다. 값을 1.f로 대체합니다."));
		max = 1.f;
	}

	const float Ratio = current / max;
	HpBar->SetPercent(Ratio);
}

void UNPMemberInfoBase::SetSkillCostBar(float current, float max)
{
	if (max <= 0.f)
	{
		NP_LOG(NPLog, Error, TEXT("max 값이 0보다 작거나 같습니다. 값을 1.f로 대체합니다."));
		max = 1.f;
	}

	const float Ratio = current / max;
	//SkillCostBar->SetPercent(Ratio);
}

void UNPMemberInfoBase::SetUltimateCostBar(float current, float max)
{
	if (max <= 0.f)
	{
		NP_LOG(NPLog, Error, TEXT("max 값이 0보다 작거나 같습니다. 값을 1.f로 대체합니다."));
		max = 1.f;
	}

	const float Ratio = current / max;
	UltimateCostBar->SetPercent(Ratio);
}

void UNPMemberInfoBase::SetImage(TSoftObjectPtr<UTexture2D> SoftTexture)
{
	Image->SetBrushFromSoftTexture(SoftTexture);
}
