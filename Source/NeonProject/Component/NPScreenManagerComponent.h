// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType/NPEventCommandTypes.h"
#include "DataType/NPScreenTypes.h"
#include "NPScreenManagerComponent.generated.h"

enum class ENPScreenType : uint8;
struct FNPScreenOptions;
class UNPScreenWidgetBase;
class UNPScreenFadeWidgetBase;
class UNPBattleSelectionWidget;
class UNPLoadingScreenWidgetBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEONPROJECT_API UNPScreenManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNPScreenManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void OpenScreen(ENPScreenType screenType);
	void CloseScreen(UNPScreenWidgetBase* screenWidget);

	void OpenScreenAfterFade(ENPScreenType screenType, float fadeDuration = 1.f, float fadeStartDelay = 0.f);

	/// <summary>
	/// Fade Animation이 종료될 때 전달한 eventCommand가 실행됩니다.
	/// 
	/// </summary>
	/// <param name="eventCommand">	: 이벤트 커맨드</param>
	/// <param name="fadeAnimationType"> : FadeIn or FadeOut</param>
	/// <param name="playbackSpeed"> : 0보다 작은값을 입력 시 애니메이션은 시작 즉시 종료됩니다.</param>
	/// <param name="fadeStartDelay"> : Fade Animation이 첫 프레임에서 fadeStartDelay만큼 대기 후 재생됩니다.</param>
	/// <returns>Fade Animation 재생이 실패하거나 eventCommand를 실행할 수 없는 경우 false가 출력됩니다.</returns>
	bool ExcuteCommandAfterFade(const FNPEventCommand& eventCommand, ENPFadeAnimationType fadeAnimationType, float fadeDuration = 1.f, float fadeStartDelay = 0.f);

private:
	void ApplyScreenOpenOptions(const FNPScreenOptions& openRequest);

private:	// Loading Screen 
	bool ShowAndPlayLoadingScreen();
	void OnLoadingScreenFinished();

private: // Fade Screen
	bool ShowAndPlayFadeScreen(ENPFadeAnimationType fadeAnimationType, float fadeDuration, float fadeStartDelay);
	void OnFadeScreenFinished(ENPFadeAnimationType CompletedFadeType);

private:
	APlayerController* GetPlayerController();
	APlayerController* GetPlayerControllerChecked();

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> CachedController;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NP")
	TMap<ENPScreenType, TSubclassOf<UNPScreenWidgetBase>> ScreenWidgetClasses;


	// 페이드 스크린
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NP")
	TSubclassOf<UNPScreenFadeWidgetBase> ScreenFadeWidgetClass;
	
	UPROPERTY(Transient)
	TObjectPtr<UNPScreenFadeWidgetBase> ScreenFadeWidget;

	// 로딩 스크린
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NP")
	TSubclassOf<UNPLoadingScreenWidgetBase> LoadingScreenWidgetClass;

	UPROPERTY()	FNPEventCommand PendingEventCommand;


public:
	float GameStartFadeInDuration = 1.f;
	float GameStartFadeInDelay = 0.f;
};
