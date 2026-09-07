// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Battle/NPPartyInfoBase.h"

#include "NeonProject.h"

#include "UI/Battle/NPMemberInfoBase.h"

void UNPPartyInfoBase::NativeConstruct()
{
	Super::NativeConstruct();

	if(!MemberInfo_1)
		NP_LOG(NPLog, Error, TEXT("MemberInfo_1 is nullptr."));

	if (!MemberInfo_2)
		NP_LOG(NPLog, Error, TEXT("MemberInfo_2 is nullptr."));

	if (!MemberInfo_3)
		NP_LOG(NPLog, Error, TEXT("MemberInfo_3 is nullptr."));

	MemberInfos.Reset();
	MemberInfos.Add(MemberInfo_1);
	MemberInfos.Add(MemberInfo_2);
	MemberInfos.Add(MemberInfo_3);
}

void UNPPartyInfoBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UNPPartyInfoBase::SetMemberHpBar(int32 Idx, float current, float max)
{
	if (!MemberInfos.IsValidIndex(Idx))
		return;

	if (MemberInfos[Idx] == nullptr)
	{
		NP_LOG(NPLog, Error, TEXT("memberInfo is nullptr."));
		return;
	}

	MemberInfos[Idx]->SetHpBar(current, max);
}

void UNPPartyInfoBase::SetMemberSkillCostBar(int32 Idx, float current, float max)
{
	if (!MemberInfos.IsValidIndex(Idx))
		return;

	if (MemberInfos[Idx] == nullptr)
	{
		NP_LOG(NPLog, Error, TEXT("memberInfo is nullptr."));
		return;
	}

	MemberInfos[Idx]->SetSkillCostBar(current, max);
}

void UNPPartyInfoBase::SetMemberUltimateCostBar(int32 Idx, float current, float max)
{
	if (!MemberInfos.IsValidIndex(Idx))
		return;

	if (MemberInfos[Idx] == nullptr)
	{
		NP_LOG(NPLog, Error, TEXT("memberInfo is nullptr."));
		return;
	}

	MemberInfos[Idx]->SetUltimateCostBar(current, max);
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
