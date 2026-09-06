// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InstancedStruct.h"
#include "NPGameFlowHandlerBase.generated.h"


UENUM()
enum class ENPGameFlowHandlerState : uint8
{
	Idle,
	Preparing,
	Executing,
	Finished,
	CleanedUp
};

USTRUCT()
struct FNPGameFlowHandlerDataBase
{
	GENERATED_BODY()

};

DECLARE_DELEGATE_TwoParams(FNPGameFlowHandlerFinishedDelegate, const UNPGameFlowHandlerBase*, bool);

UCLASS(Abstract)
class NEONPROJECT_API UNPGameFlowHandlerBase : public UObject
{
	GENERATED_BODY()
	
public:
	void InitializeHandlerData(const TSharedPtr<const TInstancedStruct<FNPGameFlowHandlerDataBase>>& InHandlerData);
	void PrepareExecute();
	void Execute();

private:
	virtual void PrepareExecuteInternal() PURE_VIRTUAL(UNPGameFlowHandlerBase::PrepareExecuteInternal, );
	virtual void ExecuteInternal() PURE_VIRTUAL(UNPGameFlowHandlerBase::ExecuteInternal, );
	virtual void FinishInternal() PURE_VIRTUAL(UNPGameFlowHandlerBase::FinishInternal, );
	virtual void CleanupInternal() PURE_VIRTUAL(UNPGameFlowHandlerBase::CleanupInternal, );

protected:
	void Finish(bool bCompleted);
	const TInstancedStruct<FNPGameFlowHandlerDataBase>& GetHandlerData() const
	{
		check(HandlerData.IsValid());
		return *HandlerData;
	}

private:
	void Cleanup();

private:
	bool IsCleanedUp() const { return (HandlerState == ENPGameFlowHandlerState::CleanedUp); }
	bool IsFinished() const { return (HandlerState == ENPGameFlowHandlerState::Finished) || IsCleanedUp(); }

public:
	FNPGameFlowHandlerFinishedDelegate OnGameFlowHandlerFinished;

private:
	TSharedPtr<const TInstancedStruct<FNPGameFlowHandlerDataBase>> HandlerData;
	ENPGameFlowHandlerState HandlerState = ENPGameFlowHandlerState::Idle;
};
