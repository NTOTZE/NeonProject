// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InstancedStruct.h"
#include "Utility/NPMacros.h"
#include "NPGameFlowSubsystem.generated.h"

struct FNPGameFlowCommand;
struct FNPGameFlowCommandOptions;
struct FNPGameFlowHandlerDataBase;
class UNPGameFlowHandlerBase;

UCLASS()
class NEONPROJECT_API UNPGameFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	NP_DECLARE_GAMEINSTANCE_SUBSYSTEM_GETTER()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	void HandlePostLoadMapWithWorld(UWorld* LoadedWorld);

public:
	bool RequestExecuteCommand(const FNPGameFlowCommand& InCommand);

private:
	bool CanExecuteCommand(const FNPGameFlowCommand& InCommand);
	void ExecuteCommand(const FNPGameFlowCommand& InCommand);

	bool CanEnqueueCommand(const FNPGameFlowCommand& InCommand);
	void EnqueueCommand(const FNPGameFlowCommand& InCommand);

	void PrepareCommandOptions(const TSharedPtr<const FNPGameFlowCommandOptions>& InOptions);
	const TSubclassOf<UNPGameFlowHandlerBase>* FindHandlerClass(const TSharedPtr<const TInstancedStruct<FNPGameFlowHandlerDataBase>>& InHandlerData);

	void PrepareExecuteHandler(const TSharedPtr<const TInstancedStruct<FNPGameFlowHandlerDataBase>>& InHandlerData);
	void StartCommandExecution();
	void ExecuteActiveHandler();

	void HandleActiveHandlerFinished(const UNPGameFlowHandlerBase* FinishedHandler, bool bCompleted);

private:
	UPROPERTY(Transient)
	TObjectPtr<UNPGameFlowHandlerBase> ActiveHandler;

	TSharedPtr<const FNPGameFlowCommandOptions> ActiveCommandOptions;
	TQueue<const FNPGameFlowCommand> PendingCommandQueue;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UScriptStruct>, TSubclassOf<UNPGameFlowHandlerBase>> RuntimeHandlerMap;
};
