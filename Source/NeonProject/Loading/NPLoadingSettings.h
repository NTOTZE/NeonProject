// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Utility/NPMacros.h"
#include "Loading/UI/NPLoadingScreenResources.h"
#include "UI/Start/NPStartWidget.h"
#include "NPLoadingSettings.generated.h"

class UTexture2D;

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Loading Settings"))
class NEONPROJECT_API UNPLoadingSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	NP_DECLARE_SETTINGS()

public:
	TSubclassOf<UNPStartWidget> GetStartWidgetClass() const
	{
		return StartWidgetClass.LoadSynchronous();
	}

	FNPLoadingScreenResources GetLoadingScreenResources(const FName& Id) const
	{
		LoadingScreenResources.CheckPath();

		const TSoftObjectPtr<UTexture2D>* Artwork = ArtworkMap.Find(Id);
		if (!ensure(Artwork) || !ensure(!Artwork->IsNull()))
			return LoadingScreenResources;

		FNPLoadingScreenResources NewResources = LoadingScreenResources;
		NewResources.ArtworkTexture = *Artwork;
		return NewResources;
	}

private:
	/// @brief 시작 화면에서 사용할 위젯 클래스입니다.
	UPROPERTY(EditAnywhere, Config, Category = "Loading Screen")
	TSoftClassPtr<UNPStartWidget> StartWidgetClass;

	/// @brief 로딩 스크린을 구성하는 리소스. 
	UPROPERTY(EditAnywhere, Config, Category = "Loading Screen")
	FNPLoadingScreenResources LoadingScreenResources;

	/// @brief ID별로 사용할 로딩 화면 Artwork Texture입니다
	/// 로딩 스크린 생성시 사용된 ID에 해당하는 Texture가 ArtworkMap에 존재하지 않을경우,
	/// 기본값인 LoadingScreenResources의 ArtworkTexture을 사용합니다.
	UPROPERTY(EditAnywhere, Config, Category = "Loading Screen")
	TMap<FName, TSoftObjectPtr<UTexture2D>> ArtworkMap;
};
