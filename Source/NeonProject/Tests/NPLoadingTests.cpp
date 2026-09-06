#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Loading/NPLoadingSubsystem.h"
#include "Loading/NPStageSessionSubsystem.h"
#include "UObject/StrongObjectPtr.h"
#include "Loading/UI/SNPLoadingScreen.h"
#include "Engine/Texture2D.h"
#include "Widgets/SOverlay.h"
#include "Async/Async.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNPEmptyStageAssetsTest, "NeonProject.Loading.EmptyAssetRequest",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNPEmptyStageAssetsTest::RunTest(const FString& Parameters)
{
	TStrongObjectPtr<UGameInstance> Instance(NewObject<UGameInstance>());
	TStrongObjectPtr<UNPLoadingSubsystem> Loading(NewObject<UNPLoadingSubsystem>(Instance.Get()));
	int32 CompletionCount = 0;
	const TSharedPtr<FStreamableHandle> Handle = Loading->LoadAssets({},
		FStreamableDelegate::CreateLambda([&CompletionCount]() { ++CompletionCount; }));
	TestEqual(TEXT("로딩할 에셋이 없는 경우 완료 콜백이 한 번만 호출되어야 합니다."), CompletionCount, 1);
	TestFalse(TEXT("로딩할 에셋이 없는 경우 핸들을 생성하지 않아야 합니다."), Handle.IsValid());
	return true;
}

struct FNPAssetOwnershipTestState
{
	TStrongObjectPtr<UGameInstance> Instance { NewObject<UGameInstance>() };
	TStrongObjectPtr<UNPLoadingSubsystem> Loading { NewObject<UNPLoadingSubsystem>(Instance.Get()) };
	TStrongObjectPtr<UWorld> World { NewObject<UWorld>() };
	TStrongObjectPtr<UNPStageSessionSubsystem> Owner { NewObject<UNPStageSessionSubsystem>(World.Get()) };
	TSharedPtr<FStreamableHandle> Handle;
	bool bCompleted = false;
	double StartedAt = FPlatformTime::Seconds();
};

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FNPVerifyAssetOwnership,
	TSharedPtr<FNPAssetOwnershipTestState>, State, FAutomationTestBase*, Test);

bool FNPVerifyAssetOwnership::Update()
{
	if (!State->bCompleted)
	{
		if (FPlatformTime::Seconds() - State->StartedAt < 10.0)
			return false;
		Test->AddError(TEXT("에셋 로딩 완료 콜백 대기 시간이 초과됐습니다."));
		if (State->Handle)
			State->Handle->CancelHandle();
		return true;
	}
	if (!Test->TestTrue(TEXT("에셋 로딩 요청시 핸들이 반환되어야 합니다."), State->Handle.IsValid()))
		return true;

	Test->TestTrue(TEXT("요청한 텍스쳐가 로딩되어 있어야 합니다."),
		FSoftObjectPath(TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture")).ResolveObject() != nullptr);
	TWeakPtr<FStreamableHandle> Observer = State->Handle;
	FNPStageSessionData SessionData;
	SessionData.StageId = TEXT("TestStage");
	SessionData.PartyCharacterIds = { TEXT("Player0001") };
	SessionData.MonsterIds = { TEXT("Monster0001") };
	TArray<TSharedPtr<FStreamableHandle>> AssetHandles;
	AssetHandles.Add(MoveTemp(State->Handle));
	State->Owner->InitializeSession(MoveTemp(SessionData), MoveTemp(AssetHandles));
	Test->TestFalse(TEXT("핸들 전달 후 기존 소유자의 참조가 제거되어야 합니다."), State->Handle.IsValid());
	Test->TestEqual(TEXT("스테이지 ID가 세션에 전달되어야 합니다."),
		State->Owner->GetSessionData().StageId, FName(TEXT("TestStage")));
	Test->TestEqual(TEXT("파티 캐릭터 ID가 세션에 전달되어야 합니다."),
		State->Owner->GetSessionData().PartyCharacterIds.Num(), 1);
	Test->TestEqual(TEXT("몬스터 ID가 세션에 전달되어야 합니다."),
		State->Owner->GetSessionData().MonsterIds.Num(), 1);
	Test->TestTrue(TEXT("새 월드의 서브시스템에서 핸들을 보관해야 합니다."), Observer.IsValid());
	if (Observer.IsValid())
		Test->TestTrue(TEXT("전달된 핸들이 로딩된 에셋을 유지해야 합니다."), Observer.Pin()->IsActive());
	State->Owner->Deinitialize();
	Test->TestFalse(TEXT("월드 서브시스템 정리시 핸들의 마지막 참조가 제거되어야 합니다."), Observer.IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNPStageAssetOwnershipTest, "NeonProject.Loading.AssetOwnership",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNPStageAssetOwnershipTest::RunTest(const FString& Parameters)
{
	TSharedPtr<FNPAssetOwnershipTestState> State = MakeShared<FNPAssetOwnershipTestState>();
	TWeakPtr<FNPAssetOwnershipTestState> WeakState = State;
	State->Handle = State->Loading->LoadAssets(
		{ FSoftObjectPath(TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture")) },
		FStreamableDelegate::CreateLambda([WeakState]()
		{
			if (const TSharedPtr<FNPAssetOwnershipTestState> Pinned = WeakState.Pin())
				Pinned->bCompleted = true;
		}));
	ADD_LATENT_AUTOMATION_COMMAND(FNPVerifyAssetOwnership(State, this));
	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FNPVerifyLoadingFade, TFunction<bool()>, Verify);

bool FNPVerifyLoadingFade::Update()
{
	return Verify();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNPLoadingScreenFadeTest, "NeonProject.Loading.FadeWithoutActiveTimers",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNPLoadingScreenFadeTest::RunTest(const FString& Parameters)
{
	UTexture2D* Texture = LoadObject<UTexture2D>(nullptr,
		TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture"));
	if (!TestNotNull(TEXT("테스트에 사용할 텍스쳐가 유효해야 합니다."), Texture))
		return false;

	FNPLoadingScreenResources Resources;
	Resources.ArtworkTexture = Texture;
	Resources.VignetteTexture = Texture;
	Resources.ThrobberTexture = Texture;

	struct FTestState
	{
		TArray<TSharedPtr<SNPLoadingScreen>> Screens;
		int32 CompletionCount = 0;
		bool bFadeChecked = false;
		double StartedAt = FPlatformTime::Seconds();
	};
	TSharedRef<FTestState> State = MakeShared<FTestState>();
	for (int32 Index = 0; Index < 2; ++Index)
	{
		TSharedPtr<SNPLoadingScreen> Screen = SNew(SNPLoadingScreen).Resources(Resources);
		Screen->OnLoadingScreenFinished.BindLambda([WeakState = TWeakPtr<FTestState>(State)]()
		{
			if (const TSharedPtr<FTestState> Pinned = WeakState.Pin())
				++Pinned->CompletionCount;
		});
		State->Screens.Add(Screen);
	}

	TFunction<bool()> Verify = [this, State]()
	{
		if (FPlatformTime::Seconds() - State->StartedAt > 10.0)
		{
			AddError(TEXT("로딩스크린 Fade 검증 대기 시간이 초과됐습니다."));
			for (const TSharedPtr<SNPLoadingScreen>& Screen : State->Screens)
				Screen->Shutdown();
			return true;
		}
		for (const TSharedPtr<SNPLoadingScreen>& Screen : State->Screens)
		{
			if (!Screen->IsReady())
				return false;
		}

		if (!State->bFadeChecked)
		{
			// 일반 Slate Tick과 ActiveTimer 실행 없이 로딩 위젯 Tick만 진행
			for (int32 Index = 0; Index < State->Screens.Num(); ++Index)
			{
				const TSharedPtr<SNPLoadingScreen>& Screen = State->Screens[Index];
				Screen->StartLoadingScreen();
				if (Index == 1)
					Screen->BeginFinishLoadingScreen();

				Screen->Tick(FGeometry(), 0.0, 0.25f);
				TestEqual(TEXT("로딩 중 FadeIn이 진행되어야 합니다."),
					Screen->LoadingContent->GetRenderOpacity(), 0.75f);
				Screen->Tick(FGeometry(), 0.0, 0.25f);
				TestEqual(TEXT("종료 요청이 먼저 도착해도 FadeIn을 완료해야 합니다."),
					Screen->LoadingContent->GetRenderOpacity(), 1.f);

				if (Index == 0)
				{
					Screen->Tick(FGeometry(), 0.0, 1.f);
					TestEqual(TEXT("종료 요청 전에는 로딩스크린이 유지되어야 합니다."),
						Screen->LoadingContent->GetRenderOpacity(), 1.f);
					Screen->BeginFinishLoadingScreen();
					Screen->Tick(FGeometry(), 0.0, 0.f);
				}

				Screen->Tick(FGeometry(), 0.0, 0.25f);
				TestEqual(TEXT("FadeOut이 진행되어야 합니다."),
					Screen->LoadingContent->GetRenderOpacity(), 0.75f);
				Screen->Tick(FGeometry(), 0.0, 0.25f);
				TestEqual(TEXT("FadeOut 완료시 투명도가 0이어야 합니다."),
					Screen->LoadingContent->GetRenderOpacity(), 0.f);
				Screen->Tick(FGeometry(), 0.0, 1.f);
			}
			State->bFadeChecked = true;
			return false;
		}

		if (State->CompletionCount < State->Screens.Num())
			return false;
		TestEqual(TEXT("각 로딩스크린의 종료 콜백이 한 번만 호출되어야 합니다."),
			State->CompletionCount, State->Screens.Num());
		for (const TSharedPtr<SNPLoadingScreen>& Screen : State->Screens)
			Screen->Shutdown();
		return true;
	};
	ADD_LATENT_AUTOMATION_COMMAND(FNPVerifyLoadingFade(MoveTemp(Verify)));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNPLoadingDisplaySnapshotTest, "NeonProject.Loading.DisplaySnapshot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNPLoadingDisplaySnapshotTest::RunTest(const FString& Parameters)
{
	UTexture2D* Texture = LoadObject<UTexture2D>(nullptr,
		TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture"));
	if (!TestNotNull(TEXT("테스트에 사용할 텍스쳐가 유효해야 합니다."), Texture))
		return false;
	FNPLoadingScreenResources Resources;
	Resources.ArtworkTexture = Texture;
	Resources.VignetteTexture = Texture;
	Resources.ThrobberTexture = Texture;
	TSharedRef<SNPLoadingScreen> Screen = SNew(SNPLoadingScreen).Resources(Resources);

	FNPLoadingDisplayData Data;
	Data.StatusText = TEXT("에셋 준비 중");
	Data.Assets = {
		{ TEXT("플레이어 캐릭터"), ENPLoadingAssetState::Loaded },
		{ TEXT("몬스터"), ENPLoadingAssetState::Failed },
		{ TEXT("오브젝트"), ENPLoadingAssetState::Requested },
		{ TEXT("전투 데이터"), ENPLoadingAssetState::Canceled }
	};
	// 다른 스레드에서 전달한 데이터가 위젯을 직접 변경하지 않는지 검증
	SNPLoadingScreen* ScreenPtr = &Screen.Get();
	Async(EAsyncExecution::ThreadPool, [ScreenPtr, Data]()
	{
		ScreenPtr->SetLoadingDisplayData(Data);
	}).Get();
	TestEqual(TEXT("Slate Tick 전에는 표시 데이터가 변경되지 않아야 합니다."),
		Screen->DisplayData.Assets.Num(), 0);
	Data.Assets[0].DisplayName = TEXT("원본 변경");
	Screen->Tick(FGeometry(), 0.0, 0.f);
	TestEqual(TEXT("요청한 에셋 수만큼 목록이 표시되어야 합니다."),
		Screen->AssetList->GetChildren()->Num(), 4);
	TestEqual(TEXT("전달 후 원본 변경이 표시 데이터에 영향을 주지 않아야 합니다."),
		Screen->DisplayData.Assets[0].DisplayName, FString(TEXT("플레이어 캐릭터")));
	TestEqual(TEXT("실패와 취소도 처리 완료로 계산되어야 합니다."),
		Screen->LoadingProgress.Load(), 0.75f);
	TestTrue(TEXT("로딩 실패 상태가 유지되어야 합니다."),
		Screen->DisplayData.Assets[1].State == ENPLoadingAssetState::Failed);

	Data.StatusText = TEXT("레벨 이동 중");
	Data.Assets.Reset();
	Screen->SetLoadingDisplayData(Data);
	Screen->Tick(FGeometry(), 0.0, 0.f);
	TestEqual(TEXT("빈 요청은 안내 문구만 표시해야 합니다."),
		Screen->AssetList->GetChildren()->Num(), 1);
	TestEqual(TEXT("에셋 요청이 없으면 에셋 준비가 완료된 상태여야 합니다."),
		Screen->LoadingProgress.Load(), 1.f);
	Screen->Shutdown();
	return true;
}

#endif
