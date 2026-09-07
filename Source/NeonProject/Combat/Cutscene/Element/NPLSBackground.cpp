// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Cutscene/Element/NPLSBackground.h"
#include "NeonProject.h"

#include "Components/PostProcessComponent.h" 

ANPLSBackground::ANPLSBackground()
{
	PrimaryActorTick.bCanEverTick = true;
	// 레벨시퀀스 재생중에 퍼즈중이라 필요함
	PrimaryActorTick.bTickEvenWhenPaused = true;

	PostProcessComp = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComp"));
	SetRootComponent(PostProcessComp);

	PostProcessComp->bUnbound = true;
	PostProcessComp->Priority = 9999.f;
	PostProcessComp->BlendWeight = 1.f;
}

void ANPLSBackground::BeginPlay()
{
	Super::BeginPlay();
	
	InitOverlay();
}

void ANPLSBackground::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyParams();
}

void ANPLSBackground::InitOverlay()
{
	if (!OverlayPPMaterial) return;

	{
		auto& WB = PostProcessComp->Settings.WeightedBlendables.Array;
		for (int32 i = WB.Num() - 1; i >= 0; --i)
		{
			if (WB[i].Object == OverlayMID)
			{
				WB.RemoveAt(i);
			}
		}
	}

	OverlayMID = UMaterialInstanceDynamic::Create(OverlayPPMaterial, this);
	if (!OverlayMID) return;

	OverlayMID->SetScalarParameterValue(TEXT("BgOpacity"), 1.0f);

	if (BackgroundTex)
	{
		OverlayMID->SetTextureParameterValue(TEXT("BackgroundTex"), BackgroundTex);
	}

	OverlayMID->SetScalarParameterValue(TEXT("BgOpacity"), Opacity01);

	{
		auto& WB = PostProcessComp->Settings.WeightedBlendables.Array;
		WB.Add(FWeightedBlendable(1.0f, OverlayMID));
	}

	PostProcessComp->BlendWeight = OverlayWeight;
}

void ANPLSBackground::ApplyParams()
{
	PostProcessComp->BlendWeight = OverlayWeight;
	if (OverlayMID)
	{
		OverlayMID->SetScalarParameterValue(TEXT("BgOpacity"), Opacity01);
	}
}

#if WITH_EDITOR
void ANPLSBackground::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	InitOverlay();
}

void ANPLSBackground::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName Name = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (Name == GET_MEMBER_NAME_CHECKED(ANPLSBackground, OverlayPPMaterial) ||
		Name == GET_MEMBER_NAME_CHECKED(ANPLSBackground, BackgroundTex))
	{
		// 리소스가 바뀌면 재초기화
		InitOverlay();
	}
	else if (Name == GET_MEMBER_NAME_CHECKED(ANPLSBackground, Opacity01) ||
		Name == GET_MEMBER_NAME_CHECKED(ANPLSBackground, OverlayWeight))
	{
		ApplyParams();
	}
}
#endif
