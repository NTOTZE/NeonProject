// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPPartyInfoBase.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPPartyInfoBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetMemberHPBarRatio(int32 Idx, float Ratio);
	void SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture);
	void SetMemberOpacity(int32 Idx, float Opacity);

private:
	TObjectPtr<class UNPMemberInfoBase> MemberInfo_1;
	FName MemberInfo_1_WidgetName = TEXT("MemberInfo_1");

	TObjectPtr<class UNPMemberInfoBase> MemberInfo_2;
	FName MemberInfo_2_WidgetName = TEXT("MemberInfo_2");

	TObjectPtr<class UNPMemberInfoBase> MemberInfo_3;
	FName MemberInfo_3_WidgetName = TEXT("MemberInfo_3");
};
