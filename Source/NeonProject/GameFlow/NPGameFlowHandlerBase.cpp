// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/NPGameFlowHandlerBase.h"
#include "NeonProject.h"

void UNPGameFlowHandlerBase::InitializeHandlerData(const TSharedPtr<const TInstancedStruct<FNPGameFlowHandlerDataBase>>& InHandlerData)
{
	checkf(HandlerState == ENPGameFlowHandlerState::Idle,
		TEXT("HandlerData는 Idle 상태에서만 초기화할 수 있습니다. 현재 상태: [%s]"),
		*UEnum::GetValueAsString(HandlerState));

	checkf(!HandlerData.IsValid(), TEXT("[%s]가 이미 초기화되어 있습니다. HandlerData는 한 번만 초기화할 수 있습니다."), NP_NAMEOF(HandlerData));
	checkf(InHandlerData.IsValid(), TEXT("[%s]가 nullptr입니다."), NP_NAMEOF(InHandlerData));
	checkf(InHandlerData->IsValid(), TEXT("[%s]에 유효한 Struct 데이터가 존재하지 않습니다."), NP_NAMEOF(InHandlerData));

	HandlerData = InHandlerData;
}

void UNPGameFlowHandlerBase::PrepareExecute()
{
	checkf(HandlerState == ENPGameFlowHandlerState::Idle,
		TEXT("[%s]는 Idle 상태에서만 호출할 수 있습니다. 현재 상태: [%s]"),
		LOG_CALLINFO,
		*UEnum::GetValueAsString(HandlerState));

	checkf(HandlerData.IsValid(), TEXT("[%s]가 nullptr입니다. [%s] 호출 전에 [%s]를 호출해야 합니다."),
		NP_NAMEOF(HandlerData), LOG_CALLINFO, NP_NAMEOF(InitializeHandlerData));

	checkf(HandlerData->IsValid(), TEXT("[%s]에 유효한 Struct 데이터가 존재하지 않습니다."), NP_NAMEOF(HandlerData));

	HandlerState = ENPGameFlowHandlerState::Preparing;

	PrepareExecuteInternal();
}

void UNPGameFlowHandlerBase::Execute()
{
	checkf(HandlerState == ENPGameFlowHandlerState::Preparing,
		TEXT("[%s]는 Preparing 상태에서만 호출할 수 있습니다. 현재 상태: [%s]"),
		LOG_CALLINFO,
		*UEnum::GetValueAsString(HandlerState));

	checkf(HandlerData.IsValid(), TEXT("[%s]가 nullptr입니다. [%s] 호출 전에 [%s]를 호출해야 합니다."),
		NP_NAMEOF(HandlerData), LOG_CALLINFO, NP_NAMEOF(InitializeHandlerData));

	checkf(HandlerData->IsValid(), TEXT("[%s]에 유효한 Struct 데이터가 존재하지 않습니다."), NP_NAMEOF(HandlerData));

	HandlerState = ENPGameFlowHandlerState::Executing;

	ExecuteInternal();
}

void UNPGameFlowHandlerBase::Finish(bool bCompleted)
{
	if (IsFinished())
	{
		return;
	}

	HandlerState = ENPGameFlowHandlerState::Finished;
	FinishInternal();

	OnGameFlowHandlerFinished.ExecuteIfBound(this, bCompleted);

	Cleanup();
}

void UNPGameFlowHandlerBase::Cleanup()
{
	if (IsCleanedUp())
		return;

	HandlerState = ENPGameFlowHandlerState::CleanedUp;
	CleanupInternal();

	HandlerData.Reset();
	OnGameFlowHandlerFinished.Unbind();
}
