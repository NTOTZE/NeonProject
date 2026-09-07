// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/NPGameFlowSubsystem.h"
#include "NeonProject.h"
#include "Screen/NPScreenSubsystem.h"
#include "Containers/Ticker.h"

#include "Kismet/GameplayStatics.h"
#include "DataType/NPStageData.h"
#include "GameData/NPGameDataSubsystem.h"

#include "GameFlow/Command/NPGameFlowCommand.h"
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

}

void UNPGameFlowSubsystem::Deinitialize()
{
	UNPScreenSubsystem::GetChecked(this)->OnFadeFinished.Unbind();
	if (ActiveHandler)
	{
		ActiveHandler->OnGameFlowHandlerFinished.Unbind();
		ActiveHandler->Abort();
	}
	ActiveHandler = nullptr;
	ActiveCommandOptions.Reset();
	bFinalizingCommand = false;
	Super::Deinitialize();
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

	UNPScreenSubsystem* Screen = UNPScreenSubsystem::GetChecked(this);
	Screen->OnFadeFinished.BindUObject(this, &ThisClass::HandleCommandFadeOutFinished);
	const float Duration = ActiveCommandOptions->bUseFade ? ActiveCommandOptions->FadeDuration : 0.f;
	if (!Screen->PlayFadeOut(Duration, 0.f))
	{
		Screen->OnFadeFinished.Unbind();
		ActiveHandler->OnGameFlowHandlerFinished.BindUObject(this, &ThisClass::HandleActiveHandlerFinished);
		ActiveHandler->Abort();
	}
}

void UNPGameFlowSubsystem::HandleCommandFadeOutFinished(ENPFadeAnimationType FadeType)
{
	ExecuteActiveHandler();
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
	if (!bCompleted)
		UE_LOG(LogTemp, Warning, TEXT("GameFlowHandler [%s] 실행에 실패했습니다."), *GetNameSafe(FinishedHandler));
	BeginCommandFinalization();
}

void UNPGameFlowSubsystem::BeginCommandFinalization()
{
	bFinalizingCommand = true;
	UNPScreenSubsystem* Screen = UNPScreenSubsystem::GetChecked(this);
	Screen->OnFadeFinished.BindUObject(this, &ThisClass::HandleCommandFadeInFinished);
	const float Duration = ActiveCommandOptions->bUseFade ? ActiveCommandOptions->FadeDuration : 0.f;
	if (!Screen->PlayFadeIn(Duration, 0.f))
	{
		Screen->OnFadeFinished.Unbind();
		// FadeIn 재생에 실패한 경우 즉시 검은 화면 제거
		Screen->PlayFadeIn(0.f, 0.f);
		HandleCommandFadeInFinished(ENPFadeAnimationType::FadeIn);
	}
}

void UNPGameFlowSubsystem::HandleCommandFadeInFinished(ENPFadeAnimationType FadeType)
{
	// 0초 Fade는 Finish 호출 중 완료될 수 있으므로, Cleanup 종료 후 커맨드 정리
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this, [this](float)
	{
		if (bFinalizingCommand)
			OnCommandFinished();
		return false;
	}));
}

void UNPGameFlowSubsystem::OnCommandFinished()
{
	ActiveHandler = nullptr;
	ActiveCommandOptions.Reset();
	bFinalizingCommand = false;
}
