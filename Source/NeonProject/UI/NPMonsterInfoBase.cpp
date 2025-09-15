// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPMonsterInfoBase.h"

#include "NeonProject.h"
#include "Components/ProgressBar.h"

void UNPMonsterInfoBase::NativeConstruct()
{
	Super::NativeConstruct();

	HPBar = Cast<UProgressBar>(GetWidgetFromName(HPBarWidgetName));
	if (!IsValid(HPBar))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}
	else
	{
		SetHPBarRatio(HPRatio);
	}
}

void UNPMonsterInfoBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


}
void UNPMonsterInfoBase::SetHPBarRatio(float Ratio)
{
	HPRatio = Ratio;
	if (!IsValid(HPBar)) return;

	HPBar->SetPercent(Ratio);
}