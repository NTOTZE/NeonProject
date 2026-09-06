// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utility/NPMacros.h"
#include "NPLoadingSubsystem.generated.h"

struct FStreamableHandle;
class SNPLoadingScreen;

UCLASS()
class NEONPROJECT_API UNPLoadingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	NP_DECLARE_GAMEINSTANCE_SUBSYSTEM_GETTER()

public:
	// 매개변수로 Stage 데이터와 파티멤버, 몬스터 등 로딩에 필요한 정보를 받아야함
	void PrepareStageLoading();
	void BeginStageTransition(const TSoftObjectPtr<UWorld>& StageLevel);


private:
	void PrepareLoadingScreen(const FName& ArtworkId);
	void BeginLoadingScreen();
	void HandleLoadingScreenFinished();


private:
	TSharedPtr<SNPLoadingScreen> LoadingScreen;

	FDelegateHandle MoviePlaybackFinishedHandle;
};
