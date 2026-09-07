#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NPCharacterData.generated.h"

USTRUCT(BlueprintType)
struct FNPCharacterData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<class ANPCharacterBase> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<class UTexture2D> ThumbnailTexture;
};
