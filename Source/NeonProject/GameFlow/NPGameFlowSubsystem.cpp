// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/NPGameFlowSubsystem.h"
#include "NeonProject.h"

#include "Kismet/GameplayStatics.h"
#include "DataType/NPStageData.h"
#include "GameData/NPGameDataSubsystem.h"

#include "GameFlow/NPGameFlowCommand.h"
#include "GameFlow/NPGameFlowSettings.h"
#include "GameFlow/NPGameFlowHandlerBase.h"

void UNPGameFlowSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UNPGameFlowSettings* Settings = UNPGameFlowSettings::GetChecked();
	for (const auto& HandlerPair : Settings->GetHandlerMap())
	{
		UScriptStruct* HandlerData = HandlerPair.Key.LoadSynchronous();
		UClass* HandlerClass = HandlerPair.Value.LoadSynchronous();

		check(HandlerData);
		check(HandlerClass);

		RuntimeHandlerMap.Add(HandlerData, HandlerClass);
	}

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this,
		&UNPGameFlowSubsystem::HandlePostLoadMapWithWorld
	);
}

void UNPGameFlowSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	Super::Deinitialize();
}

void UNPGameFlowSubsystem::HandlePostLoadMapWithWorld(UWorld* LoadedWorld)
{
	NP_LOG(NPLog, Warning, TEXT(""));

}

bool UNPGameFlowSubsystem::RequestExecuteCommand(const FNPGameFlowCommand& InCommand)
{
	if (!CanExecuteCommand(InCommand))
	{
		if (CanEnqueueCommand(InCommand))
		{
			EnqueueCommand(InCommand);
			return true;
		}
		return false;
	}

	ExecuteCommand(InCommand);
	return true;
}

bool UNPGameFlowSubsystem::CanExecuteCommand(const FNPGameFlowCommand& InCommand)
{
	if (!ensureAlwaysMsgf(InCommand.IsValid(), TEXT("[%s]의 [%s]가 유효하지 않습니다."), NP_NAMEOF(InCommand), *InCommand.GetInvalidMemberNames()))
		return false;

	if (ActiveHandler)
		return false;
	
	return true;
}

void UNPGameFlowSubsystem::ExecuteCommand(const FNPGameFlowCommand& InCommand)
{
	checkf(InCommand.IsValid(), TEXT("[%s]의 [%s]가 유효하지 않습니다."), NP_NAMEOF(InCommand), *InCommand.GetInvalidMemberNames());

	PrepareCommandOptions(InCommand.Options);
	PrepareExecuteHandler(InCommand.HandlerData);

	StartCommandExecution();
}

bool UNPGameFlowSubsystem::CanEnqueueCommand(const FNPGameFlowCommand& InCommand)
{
	if (!ensureAlwaysMsgf(InCommand.IsValid(), TEXT("[%s]의 [%s]가 유효하지 않습니다."), NP_NAMEOF(InCommand), *InCommand.GetInvalidMemberNames()))
		return false;

	if (!InCommand.Options->bCanEnqueue)
		return false;

	// 펜딩큐 처리 방식 구현 전까지 항상 return false;
	return false;	//return true;
}

void UNPGameFlowSubsystem::EnqueueCommand(const FNPGameFlowCommand& InCommand)
{
	checkf(InCommand.IsValid(), TEXT("[%s]의 [%s]가 유효하지 않습니다."), NP_NAMEOF(InCommand), *InCommand.GetInvalidMemberNames());

	PendingCommandQueue.Enqueue(InCommand);
}

void UNPGameFlowSubsystem::PrepareCommandOptions(const TSharedPtr<const FNPGameFlowCommandOptions>& InOptions)
{
	checkf(!ActiveCommandOptions, TEXT("[%s]가 이미 존재합니다. GameFlowHandler는 동시에 하나만 활성화할 수 있습니다."), NP_NAMEOF(ActiveCommandOptions));

	ActiveCommandOptions = InOptions;
}

const TSubclassOf<UNPGameFlowHandlerBase>* UNPGameFlowSubsystem::FindHandlerClass(const TSharedPtr<const TInstancedStruct<FNPGameFlowHandlerDataBase>>& InHandlerData)
{
	if (!ensureAlwaysMsgf(InHandlerData.IsValid(), TEXT("[%s]가 nullptr 입니다."), NP_NAMEOF(InHandlerData)))
		return nullptr;

	const UScriptStruct* HandlerData = InHandlerData->GetScriptStruct();
	if (!ensureAlwaysMsgf(HandlerData, TEXT("[%s]에 Struct 데이터가 존재하지 않습니다."), NP_NAMEOF(InHandlerData)))
		return nullptr;

	const TSubclassOf<UNPGameFlowHandlerBase>* HandlerClass = RuntimeHandlerMap.Find(HandlerData);
	if (!ensureAlwaysMsgf(HandlerClass, TEXT("[%s]에 대응하는 [%s]를 찾을 수 없습니다. "), *HandlerData->GetName(), NP_NAMEOF(HandlerClass)))
		return nullptr;

	return HandlerClass;
}

void UNPGameFlowSubsystem::PrepareExecuteHandler(const TSharedPtr<const TInstancedStruct<FNPGameFlowHandlerDataBase>>& InHandlerData)
{
	checkf(!ActiveHandler, TEXT("[%s]가 이미 존재합니다. GameFlowHandler는 동시에 하나만 활성화할 수 있습니다."), NP_NAMEOF(ActiveHandler));

	const TSubclassOf<UNPGameFlowHandlerBase>* HandlerClass = FindHandlerClass(InHandlerData);
	checkf(HandlerClass, TEXT("[%s]가 nullptr 입니다."), NP_NAMEOF(HandlerClass));
	

	ActiveHandler = NewObject<UNPGameFlowHandlerBase>(this, HandlerClass->Get());
	checkf(ActiveHandler, TEXT("[%s] 인스턴스 생성에 실패했습니다."), *GetNameSafe(HandlerClass->Get()));

	ActiveHandler->InitializeHandlerData(InHandlerData);
	ActiveHandler->PrepareExecute();
}

void UNPGameFlowSubsystem::StartCommandExecution()
{
	checkf(IsValid(ActiveHandler), TEXT("[%s]가 유효하지 않습니다. [%s] 호출 전에 [%s]를 호출해야 합니다."),
		NP_NAMEOF(ActiveHandler), LOG_CALLINFO, NP_NAMEOF(PrepareExecuteHandler));

	checkf(ActiveCommandOptions.IsValid(), TEXT("[%s]가 nullptr 입니다. [%s] 호출 전에 [%s]를 호출해야 합니다."),
		NP_NAMEOF(ActiveCommandOptions), LOG_CALLINFO, NP_NAMEOF(PrepareCommandOptions));

	if (!ActiveCommandOptions->bUseFade)
	{
		ExecuteActiveHandler();
		return;
	}

	// Fade완료 델리게이트에 ExecuteActiveHandler 바인딩
	//ExecuteActiveHandler 내부에서 바인딩 해제
}

void UNPGameFlowSubsystem::ExecuteActiveHandler()
{
	checkf(IsValid(ActiveHandler), TEXT("[%s]가 유효하지 않습니다. [%s] 호출 전에 [%s]를 호출해야 합니다."), 
		NP_NAMEOF(ActiveHandler), LOG_CALLINFO, NP_NAMEOF(PrepareExecuteHandler));

	ActiveHandler->OnGameFlowHandlerFinished.BindUObject(this, &UNPGameFlowSubsystem::HandleActiveHandlerFinished);
	ActiveHandler->Execute();
}

void UNPGameFlowSubsystem::HandleActiveHandlerFinished(const UNPGameFlowHandlerBase* FinishedHandler, bool bCompleted)
{
	checkf(FinishedHandler, TEXT("[%s]가 nullptr입니다."), NP_NAMEOF(FinishedHandler));
	checkf(ActiveHandler, TEXT("[%s]가 nullptr입니다."), NP_NAMEOF(ActiveHandler));
	checkf(ActiveHandler == FinishedHandler, TEXT("[%s]가 [%s]와 일치하지 않습니다."), NP_NAMEOF(FinishedHandler), NP_NAMEOF(ActiveHandler));

	ActiveHandler->OnGameFlowHandlerFinished.Unbind();
	
	//...

	ActiveHandler = nullptr;
}
