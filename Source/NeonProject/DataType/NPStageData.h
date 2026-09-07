#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NPStageData.generated.h"

UENUM(BlueprintType)
enum class ENPStageType : uint8
{
	None,
	Hub,
	Battle,
};

USTRUCT(BlueprintType)
struct FNPHubStageData : public FTableRowBase
{
	GENERATED_BODY();

// 기본 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText StageName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Level;
};

USTRUCT(BlueprintType)
struct FNPBattleStageData : public FTableRowBase
{
	GENERATED_BODY()

// 기본 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText StageName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Difficulty = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> ThumbnailTexture;
// 전투 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<class UNPEncounterData> EncounterData;
};
