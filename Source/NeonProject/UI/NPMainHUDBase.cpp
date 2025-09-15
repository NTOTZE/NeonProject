// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPMainHUDBase.h"

#include "NeonProject.h"

#include "UI/NPPlayerInfoBase.h"
#include "UI/NPPartyInfoBase.h"
#include "UI/NPMemberInfoBase.h"
#include "UI/NPWaveInfo.h"
#include "Components/Border.h"

void UNPMainHUDBase::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (!World) return;

	PlayerInfo = Cast<UNPPlayerInfoBase>(GetWidgetFromName(PlayerInfoWidgetName));
	if (!IsValid(PlayerInfo))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}
	
	BlackScreen = Cast<UBorder>(GetWidgetFromName(BlackScreenWidgetName));
	if (!IsValid(BlackScreen))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}
	else
	{
		BlackScreen->SetRenderOpacity(1.f);
		FTimerHandle Handle;
		World->GetTimerManager().SetTimer(
			Handle,
			FTimerDelegate::CreateUObject(this, &UNPMainHUDBase::EnableBlackScreen, false, 1.f),
			4.f,
			false
		);
	}

	PartyInfo = Cast<UNPPartyInfoBase>(GetWidgetFromName(PartyInfoWidgetName));
	if (!IsValid(PartyInfo))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}

	WaveInfo = Cast<UNPWaveInfo>(GetWidgetFromName(WaveInfoWidgetName));
	if (!IsValid(WaveInfo))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}
}

void UNPMainHUDBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UNPMainHUDBase::SetPlayerHPBarRatio(float Ratio)
{
	PlayerInfo->SetHPBarRatio(Ratio);
}

void UNPMainHUDBase::SetMemberHPBarRatio(int32 Idx, float Ratio)
{
	PartyInfo->SetMemberHPBarRatio(Idx, Ratio);
}

void UNPMainHUDBase::SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture)
{
	PartyInfo->SetMemberImage(Idx, SoftTexture);
}

void UNPMainHUDBase::SetMemberOpacity(int32 Idx, float Opacity)
{
	PartyInfo->SetMemberOpacity(Idx, Opacity);
}

void UNPMainHUDBase::SetWaveText(const FString& str)
{
	WaveInfo->SetTextBlock(str);
}

void UNPMainHUDBase::EnableBlackScreen(bool bEnable, float duration)
{
	UWorld* World = GetWorld();
	if (!World) return;

	BlackScreenEnabled = bEnable;
	FadeDuration = duration;

	if (duration <= 0.f)
	{
		if (BlackScreenEnabled)
			BlackScreen->SetRenderOpacity(1.f);
		else
			BlackScreen->SetRenderOpacity(0.f);

		return;
	}
	World->GetTimerManager().ClearTimer(BlackScreenTimer);

	World->GetTimerManager().SetTimer(BlackScreenTimer,
		this,
	&UNPMainHUDBase::BlackScreenTimerCallback
	, 0.02f, true, 0.f);
}

void UNPMainHUDBase::BlackScreenTimerCallback()
{
	const float Delta = 0.02f / FadeDuration;
	const float Current = BlackScreen->GetRenderOpacity();
	float NewValue;
	if (BlackScreenEnabled)
	{
		NewValue = FMath::Min(Current + Delta, 1.f);
		BlackScreen->SetRenderOpacity(NewValue);
		if (NewValue >= 1.f)
		{
			UWorld* World = GetWorld();
			if (!World) return;

			World->GetTimerManager().ClearTimer(BlackScreenTimer);
		}
	}
	else
	{
		NewValue = FMath::Max(Current - Delta, 0.f);
		BlackScreen->SetRenderOpacity(NewValue);
		if (NewValue <= 0.f)
		{
			UWorld* World = GetWorld();
			if (!World) return;

			World->GetTimerManager().ClearTimer(BlackScreenTimer);
		}
	}
}
