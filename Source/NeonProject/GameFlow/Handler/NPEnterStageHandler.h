// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFlow/NPGameFlowHandlerBase.h"
#include "NPEnterStageHandler.generated.h"

enum class ENPStageType : uint8;

USTRUCT(BlueprintType)
struct FNPEnterStageHandlerData : public FNPGameFlowHandlerDataBase
{
	GENERATED_BODY()

public:
	FNPEnterStageHandlerData() = default;

	static TSharedPtr<const TInstancedStruct<FNPGameFlowHandlerDataBase>> Make(ENPStageType stageType, const FName& stageId)
	{
		TSharedPtr<TInstancedStruct<FNPGameFlowHandlerDataBase>> HandlerDataPtr
			= MakeShared<TInstancedStruct<FNPGameFlowHandlerDataBase>>();
		HandlerDataPtr->InitializeAs<FNPEnterStageHandlerData>();
		
		FNPEnterStageHandlerData& HandlerData = HandlerDataPtr->GetMutable<FNPEnterStageHandlerData>();
		HandlerData.StageType = stageType;
		HandlerData.StageId = stageId;

		return HandlerDataPtr;
	}

public:
	ENPStageType StageType;
	FName StageId;
};

UCLASS()
class NEONPROJECT_API UNPEnterStageHandler : public UNPGameFlowHandlerBase
{
	GENERATED_BODY()
	
private:
	virtual void PrepareExecuteInternal() override;
	virtual void ExecuteInternal() override;
	virtual void FinishInternal() override;
	virtual void CleanupInternal() override;

	void HandlePostLoadMap(UWorld* LoadedWorld);

private:
	TSoftObjectPtr<UWorld> StageLevel;
	FDelegateHandle PostLoadMapHandle;
};
