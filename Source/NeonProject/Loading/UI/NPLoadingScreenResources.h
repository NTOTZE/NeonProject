#pragma once

#include "CoreMinimal.h"
#include "NPLoadingScreenResources.generated.h"


USTRUCT(BlueprintType)
struct FNPLoadingScreenResources
{
	GENERATED_BODY();

public:
	/// @brief 로딩 스크린에 표시될 쓰로버 텍스쳐
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> ThrobberTexture;

	/// @brief 로딩 스크린에 비네트 효과 표현을 위한 텍스쳐
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> VignetteTexture;

	/// @brief 로딩 스크린 아트웍 텍스쳐
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> ArtworkTexture;

public:
	/// @brief 모든 멤버에 대해 check(!TSoftObjectPtr::IsNull()) 수행
	void CheckPath() const
	{
		check(!ThrobberTexture.IsNull());
		check(!VignetteTexture.IsNull());
		check(!ArtworkTexture.IsNull());
	}

	/// @brief 모든 멤버에 대해 check(TSoftObjectPtr::IsValid()) 수행
	void CheckLoaded() const
	{
		check(ThrobberTexture.IsValid());
		check(VignetteTexture.IsValid());
		check(ArtworkTexture.IsValid());
	}

	TArray<FSoftObjectPath> GetAssetPaths() const
	{
		CheckPath();

		TArray<FSoftObjectPath> AssetPaths;
		AssetPaths.Reserve(3);

		AssetPaths.Add(ThrobberTexture.ToSoftObjectPath());
		AssetPaths.Add(VignetteTexture.ToSoftObjectPath());
		AssetPaths.Add(ArtworkTexture.ToSoftObjectPath());

		return AssetPaths;
	}
};