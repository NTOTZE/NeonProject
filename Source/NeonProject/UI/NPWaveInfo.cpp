// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPWaveInfo.h"
#include "NeonProject.h"
#include "Components/TextBlock.h"

void UNPWaveInfo::NativeConstruct()
{
	Super::NativeConstruct();
	Text = Cast<UTextBlock>(GetWidgetFromName(TextWidgetName));
	if (!IsValid(Text))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}
	else
	{
		OpacityValue = 0.f;
		Text->SetRenderOpacity(0.f);
		Text->SetText(CachedText);
	}
}

void UNPWaveInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (OpacityValue > 0.f)
	{
		OpacityValue -= InDeltaTime;
		Text->SetRenderOpacity(FMath::Clamp(OpacityValue, 0.f, 1.f));
	}
}

void UNPWaveInfo::SetTextBlock(const FString& str)
{
	OpacityValue = 2.f;

	CachedText = FText::FromString(str);
	Text->SetText(CachedText);
}
