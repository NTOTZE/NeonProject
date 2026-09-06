#pragma once

#include "CoreMinimal.h"
#include "NeonProject.h"
#include "InstancedStruct.h"
#include "NPGameFlowCommand.generated.h"

USTRUCT()
struct FNPGameFlowCommandOptions
{
	GENERATED_BODY()

public:
	static TSharedPtr<FNPGameFlowCommandOptions> Make(bool canEnqueu = true, bool useFade = true, float fadeDuration = 1.f)
	{
		TSharedPtr<FNPGameFlowCommandOptions> OptionsPtr =
			MakeShared<FNPGameFlowCommandOptions>();
		OptionsPtr->bCanEnqueue = canEnqueu;
		OptionsPtr->bUseFade = useFade;
		OptionsPtr->FadeDuration = fadeDuration;
		return OptionsPtr;
	}

public:
	bool bCanEnqueue = true;
	bool bUseFade = true;
	float FadeDuration = 1.f;
};

USTRUCT()
struct FNPGameFlowCommand
{
	GENERATED_BODY()

public:
	static FNPGameFlowCommand Make(
		const TSharedPtr<const FNPGameFlowCommandOptions>& InOptions
		, const TSharedPtr<const TInstancedStruct<struct FNPGameFlowHandlerDataBase>>& InHandlerData)
	{
		FNPGameFlowCommand Command;
		Command.Options = InOptions;
		Command.HandlerData = InHandlerData;
		
		return Command;
	}

	bool IsValid() const
	{
		return Options.IsValid() && HandlerData.IsValid();
	}

	FString GetInvalidMemberNames() const
	{
		if (!Options.IsValid() && !HandlerData.IsValid())
		{
			return FString::Printf(TEXT("%s, %s"), NP_NAMEOF(Options), NP_NAMEOF(HandlerData));
		}

		if (!Options.IsValid())
		{
			return NP_NAMEOF(Options);
		}

		if (!HandlerData.IsValid())
		{
			return NP_NAMEOF(HandlerData);
		}

		return TEXT("None");
	}

public:
	TSharedPtr<const FNPGameFlowCommandOptions> Options;
	TSharedPtr<const TInstancedStruct<struct FNPGameFlowHandlerDataBase>> HandlerData;
};
