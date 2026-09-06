// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Battle/NPProgressBarWidget.h"
#include "NeonProject.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UNPProgressBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IMG_ProgressBar == nullptr)
	{
		NP_LOG(NPLog, Error, TEXT("IMG_ProgressBar가 nullptr 입니다."));
		return;
	}

	ProgressMID = IMG_ProgressBar->GetDynamicMaterial();
	if (ProgressMID == nullptr)
	{
		NP_LOG(NPLog, Error, TEXT("ProgressMID가 nullptr 입니다."));
		return;
	}

	SetPercent(InitialPercent);
}

void UNPProgressBarWidget::SetPercent(float value)
{
	CurrentPercent = FMath::Clamp(value, 0.f, 1.f);

	if (ProgressMID == nullptr)
	{
		NP_LOG(NPLog, Error, TEXT("ProgressMID가 nullptr 입니다."));
		return;
	}

	ProgressMID->SetScalarParameterValue(ProgressParameterName, CurrentPercent);
}
