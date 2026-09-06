#pragma once

#include "CoreMinimal.h"

enum class ENPLoadingAssetState : uint8
{
	Requested,
	Loaded,
	Failed,
	Canceled,
};

// 로딩 Slate 스레드에 전달할 표시 데이터이며 UObject와 핸들 참조는 포함하지 않음
struct FNPLoadingAssetDisplay
{
	FString DisplayName;
	ENPLoadingAssetState State = ENPLoadingAssetState::Requested;
};

struct FNPLoadingDisplayData
{
	FString StatusText;
	TArray<FNPLoadingAssetDisplay> Assets;

	int32 GetCompletedCount() const
	{
		int32 CompletedCount = 0;
		for (const FNPLoadingAssetDisplay& Asset : Assets)
		{
			if (Asset.State != ENPLoadingAssetState::Requested)
				++CompletedCount;
		}
		return CompletedCount;
	}

	float GetProgress() const
	{
		return Assets.IsEmpty() ? 1.f : static_cast<float>(GetCompletedCount()) / Assets.Num();
	}
};
