#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

class FNPInputBlockProcessor : public IInputProcessor
{
public:
	static void Enable(TSharedPtr<FNPInputBlockProcessor>& Processor)
	{
		if (Processor.IsValid())
			return;

		if (!ensure(FSlateApplication::IsInitialized()))
			return;

		Processor = MakeShared<FNPInputBlockProcessor>();
		FSlateApplication::Get().RegisterInputPreProcessor(Processor, 0);
	}

	static void Disable(TSharedPtr<FNPInputBlockProcessor>& Processor)
	{
		if (!Processor.IsValid())
			return;

		if (FSlateApplication::IsInitialized())
			FSlateApplication::Get().UnregisterInputPreProcessor(Processor);

		Processor.Reset();
	}

public:
	virtual void Tick(
		const float DeltaTime,
		FSlateApplication& SlateApp,
		TSharedRef<ICursor> Cursor) override
	{
	}

	// 키보드 + 게임패드 버튼
	virtual bool HandleKeyDownEvent(
		FSlateApplication& SlateApp,
		const FKeyEvent& InKeyEvent) override
	{
		return true;
	}

	virtual bool HandleKeyUpEvent(
		FSlateApplication& SlateApp,
		const FKeyEvent& InKeyEvent) override
	{
		return true;
	}

	// 게임패드 스틱 / 트리거
	virtual bool HandleAnalogInputEvent(
		FSlateApplication& SlateApp,
		const FAnalogInputEvent& InAnalogInputEvent) override
	{
		return true;
	}
};