// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataType/NPScreenTypes.h"
#include "NPScreenWidgetBase.generated.h"

class UNPScreenSubsystem;

UCLASS()
class NEONPROJECT_API UNPScreenWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UNPScreenWidgetBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	// 스크린 종료 시 명령 전달 필요
	bool RequestCloseScreen();

public:
	virtual const FNPScreenOptions& GetScreenOptions() const
	{ 
		static const FNPScreenOptions DefaultScreenOptions;
		return DefaultScreenOptions;
	}
	
	virtual int32 GetZOrder() const { return 100; }

protected:
	APlayerController* GetPlayerController();
	APlayerController* GetPlayerControllerChecked();
	UNPScreenSubsystem* GetScreenSubsystem();
	UNPScreenSubsystem* GetScreenSubsystemChecked();

private:
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> CachedController;

private:
	/// @brief GetScreenOpenOptions()를 통해 접근할 것. 직접 사용하지 않음
	
};
