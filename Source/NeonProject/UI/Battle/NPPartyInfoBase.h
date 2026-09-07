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

	void SetMemberHpBar(int32 Idx, float current, float max);
	void SetMemberSkillCostBar(int32 Idx, float current, float max);
	void SetMemberUltimateCostBar(int32 Idx, float current, float max);


	void SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture);
	void SetMemberOpacity(int32 Idx, float Opacity);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPMemberInfoBase> MemberInfo_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPMemberInfoBase> MemberInfo_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UNPMemberInfoBase> MemberInfo_3;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class UNPMemberInfoBase>> MemberInfos;
};
