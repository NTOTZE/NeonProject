#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NPDialogueTypes.generated.h"

class UNPDialogueDataAsset;

USTRUCT(BlueprintType)
struct FNPDialogueLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText DialogueText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NextIndex = INDEX_NONE;
};


USTRUCT(BlueprintType)
struct FNPDialogueRegistryRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UNPDialogueDataAsset> DialogueAsset;
};