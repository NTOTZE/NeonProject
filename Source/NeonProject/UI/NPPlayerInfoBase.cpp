// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPPlayerInfoBase.h"
#include "NeonProject.h"
#include "Components/ProgressBar.h"

void UNPPlayerInfoBase::NativeConstruct()
{
	Super::NativeConstruct();

	HPBar = Cast<UProgressBar>(GetWidgetFromName(HPBarWidgetName));
	if (!IsValid(HPBar))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}
}

void UNPPlayerInfoBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


}
void UNPPlayerInfoBase::SetHPBarRatio(float Ratio)
{
	HPBar->SetPercent(Ratio);
}