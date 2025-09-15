// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPMemberInfoBase.h"
#include "NeonProject.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UNPMemberInfoBase::NativeConstruct()
{
	Super::NativeConstruct();

	Image = Cast<UImage>(GetWidgetFromName(ImageWidgetName));
	if (!IsValid(Image))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}

	HPBar = Cast<UProgressBar>(GetWidgetFromName(HPBarWidgetName));
	if (!IsValid(HPBar))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}
}

void UNPMemberInfoBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UNPMemberInfoBase::SetHPBarRatio(float Ratio)
{
	HPBar->SetPercent(Ratio);
}

void UNPMemberInfoBase::SetImage(TSoftObjectPtr<UTexture2D> SoftTexture)
{
	Image->SetBrushFromSoftTexture(SoftTexture);
}
