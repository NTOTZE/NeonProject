// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameMode/NPGameMode.h"
#include "NeonProject.h"

#include "Blueprint/UserWidget.h"
#include "UI/NPMainHUDBase.h"
#include "Character/NPPlayerCharacter.h"
#include "Combat/NPEncounterManager.h"
#include "Kismet/GameplayStatics.h"

ANPGameMode::ANPGameMode()
{
	ConstructorHelpers::FClassFinder<UUserWidget> Finder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/NeonProject/Blueprint/UI/WBP_MainHUD.WBP_MainHUD_C'"));
	if (Finder.Succeeded())
	{
		MainHudClass = Finder.Class;
	}
}

void ANPGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(MainHudClass))
	{
		MainHUD = Cast<UNPMainHUDBase>(CreateWidget(GetWorld(), MainHudClass));
	}

	if (IsValid(MainHUD))
	{
		MainHUD->AddToViewport();
	}

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPEncounterManager::StaticClass(), Found);
	if (Found.IsValidIndex(0))
	{
		if(ANPEncounterManager* manager = Cast<ANPEncounterManager>(Found[0]))
		EncounterManager = manager;
		EncounterManager->OnWaveStarted.AddDynamic(this, &ANPGameMode::OnWaveStarted);
		EncounterManager->OnWaveCleared.AddDynamic(this, &ANPGameMode::OnWaveCleared);
		EncounterManager->OnEncounterCleared.AddDynamic(this, &ANPGameMode::OnEncounterCleared);
	}
}

void ANPGameMode::SetPlayCharacterHPBarRatio(float Ratio)
{
	MainHUD->SetPlayerHPBarRatio(Ratio);
}

void ANPGameMode::SetMemberHPBarRatio(int32 Idx, float Ratio)
{
	MainHUD->SetMemberHPBarRatio(Idx, Ratio);
}

void ANPGameMode::SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture)
{
	MainHUD->SetMemberImage(Idx, SoftTexture);
}

void ANPGameMode::SetMemberOpacity(int32 Idx, float Opacity)
{
	MainHUD->SetMemberOpacity(Idx, Opacity);
}

void ANPGameMode::ApplyTimeDilation(float Dilation, float Duration)
{
	UWorld* World = GetWorld();
	if (!World) return;

	const float AppliedDilation = FMath::Clamp(Dilation, 0.01f, 1.0f);
	const float AppliedDuration = FMath::Clamp(Duration, 0.1f, 10.f) * AppliedDilation;

	UGameplayStatics::SetGlobalTimeDilation(World, AppliedDilation);

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle,
		[World]()
		{
			if (!World) return;
			UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
		}
	, AppliedDuration, false);
}


void ANPGameMode::OnWaveStarted(int32 CurrentWave, int32 TotalWave)
{
	FString str = FString::Printf(TEXT("WAVE START %d / %d"), CurrentWave, TotalWave);
	MainHUD->SetWaveText(str);
}

void ANPGameMode::OnWaveCleared(int32 CurrentWave, int32 TotalWave)
{
	NP_LOG(NPLog, Warning, TEXT("WAVE CLEAR %d / %d"), CurrentWave, TotalWave);
	
	if (CurrentWave >= TotalWave) return;
		
	ApplyTimeDilation(0.1f, 2.f);
}

void ANPGameMode::OnEncounterCleared()
{
	FString str = FString::Printf(TEXT("CLEAR"));
	MainHUD->SetWaveText(str);
	ApplyTimeDilation(0.1f, 3.f);

	UWorld* World = GetWorld();
	if (!World) return;
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle,
		[this]()
		{
			this->MainHUD->EnableBlackScreen(true, 2.f);
		}
	, 3.f, false);
}
