// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPPartyInfoBase.h"

#include "NeonProject.h"

#include "UI/NPMemberInfoBase.h"

void UNPPartyInfoBase::NativeConstruct()
{
	Super::NativeConstruct();

	MemberInfo_1 = Cast<UNPMemberInfoBase>(GetWidgetFromName(MemberInfo_1_WidgetName));
	if (!IsValid(MemberInfo_1))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}

	MemberInfo_2 = Cast<UNPMemberInfoBase>(GetWidgetFromName(MemberInfo_2_WidgetName));
	if (!IsValid(MemberInfo_2))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}

	MemberInfo_3 = Cast<UNPMemberInfoBase>(GetWidgetFromName(MemberInfo_3_WidgetName));
	if (!IsValid(MemberInfo_3))
	{
		NP_LOG(NPLog, Error, TEXT("Widget Invalid"));
	}
}

void UNPPartyInfoBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UNPPartyInfoBase::SetMemberHPBarRatio(int32 Idx, float Ratio)
{
	switch (Idx)
	{
	case 0:
		MemberInfo_1->SetHPBarRatio(Ratio);
		break;
	case 1:
		MemberInfo_2->SetHPBarRatio(Ratio);
		break;
	case 2:
		MemberInfo_3->SetHPBarRatio(Ratio);
		break;
	default:
	{
		NP_LOG(NPLog, Error, TEXT("Idx 2 초과"));
		break;
	}
	}
}

void UNPPartyInfoBase::SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture)
{
	switch (Idx)
	{
	case 0:
		MemberInfo_1->SetImage(SoftTexture);
		break;
	case 1:
		MemberInfo_2->SetImage(SoftTexture);
		break;
	case 2:
		MemberInfo_3->SetImage(SoftTexture);
		break;
	default:
	{
		NP_LOG(NPLog, Error, TEXT("Idx 2 초과"));
		break;
	}
	}
}

void UNPPartyInfoBase::SetMemberOpacity(int32 Idx, float Opacity)
{
	switch (Idx)
	{
	case 0:
		MemberInfo_1->SetRenderOpacity(Opacity);
		break;
	case 1:
		MemberInfo_2->SetRenderOpacity(Opacity);
		break;
	case 2:
		MemberInfo_3->SetRenderOpacity(Opacity);
		break;
	default:
	{
		NP_LOG(NPLog, Error, TEXT("Idx 2 초과"));
		break;
	}
	}
}
