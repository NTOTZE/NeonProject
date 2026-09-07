// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Cutscene/NPSkillCutsceneSubsystem.h"

#include "Core/Controller/NPBattlePlayerController.h"

#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneObjectBindingID.h"
#include "MovieSceneSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "MovieSceneSequenceTickInterval.h" 
#include "CineCameraActor.h"
#include "CineCameraComponent.h"

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"

UNPSkillCutsceneSubsystem::UNPSkillCutsceneSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetFinder(
		TEXT("/Game/NeonProject/Blueprint/LevelSequence/LS_Material/WBP_CutsceneOverlay"));
	if (WidgetFinder.Succeeded())
	{
		OverlayWidgetClass = WidgetFinder.Class;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OverlayMTFinder(
		TEXT("/Game/NeonProject/Blueprint/LevelSequence/LS_Material/M_CutsceneOverlay.M_CutsceneOverlay"));
	if (OverlayMTFinder.Succeeded())
	{
		OverlayUIMaterialBase = OverlayMTFinder.Object;
	}
}

void UNPSkillCutsceneSubsystem::PlaySkillCutscene(ULevelSequence* SequenceAsset, AActor* Target, float restoreViewBlendTime)
{
	if (!SequenceAsset) return;

	UWorld* World = GetWorld();
	if (!World) return;
	ALevelSequenceActor* LevelSeqActor = nullptr;
	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bAutoPlay = false;
	Settings.bInheritTickIntervalFromOwner = false;
	Settings.TickInterval.TickIntervalSeconds = 0.f;

	// 스킬 컷씬이 재생될때 PAUSE가 걸리는데, 해당 옵션이 true여야 정상적으로 재생됨.
	Settings.TickInterval.bTickWhenPaused = true;

	ULevelSequencePlayer* LevelSeqPlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(World, SequenceAsset, Settings, LevelSeqActor);
	if (!LevelSeqActor || !LevelSeqPlayer) return;

	Player = LevelSeqPlayer;
	SeqActor = LevelSeqActor;
	RestoreViewBlendTime = restoreViewBlendTime;

	SeqActor->SetBindingByTag(TargetBindingTag, { Target });
	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC) return;

	if (ANPBattlePlayerController* NPController = Cast<ANPBattlePlayerController>(PC))
	{
		NPController->InitControlRotation();
	}

	PC->SetCinematicMode(true, true, true, true, true);

	CachedViewTarget = PC->GetViewTarget();

	CreateCaptureAndOverlay(World);

	UGameplayStatics::SetGamePaused(World, true);

	Player->OnPlay.AddDynamic(this, &UNPSkillCutsceneSubsystem::OnSequencePlay);
	Player->OnCameraCut.AddDynamic(this, &UNPSkillCutsceneSubsystem::OnSequenceCameraCut);
	Player->OnFinished.AddDynamic(this, &UNPSkillCutsceneSubsystem::OnFinished);

	StartFollowing();

	Player->Play();
}

void UNPSkillCutsceneSubsystem::OnFinished()
{
	StopFollowing();

	UWorld* World = GetWorld();
	if (!World) return;

	UGameplayStatics::SetGamePaused(World, false);

	DestroyCaptureAndOverlay();

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		const FMinimalViewInfo CurrPOV = PC->PlayerCameraManager->GetCameraCacheView();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ACineCameraActor* CineCam = World->SpawnActor<ACineCameraActor>(CurrPOV.Location, CurrPOV.Rotation, Params);
		if (CineCam)
		{
			if (UCineCameraComponent* CineComp = CineCam->GetCineCameraComponent())
			{
				CineComp->SetFieldOfView(CurrPOV.FOV);
				CineComp->PostProcessSettings = CurrPOV.PostProcessSettings;
				CineComp->PostProcessBlendWeight = CurrPOV.PostProcessBlendWeight;
			}
		}
		PC->SetViewTarget(CineCam);
		PC->SetCinematicMode(false, true, true, true, true);

		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateLambda(
				[this, PC, CineCam]()
				{
					PC->SetViewTargetWithBlend(
						CachedViewTarget.Get(),
						RestoreViewBlendTime,
						VTBlend_Cubic,
						2.f,
						false);

					const float Epsilon = 0.2f;

					CineCam->SetLifeSpan(RestoreViewBlendTime + Epsilon);
					CachedViewTarget = nullptr;
				})
		);
	}

	Player = nullptr;
	SeqActor = nullptr;

	LastLoc = FVector::ZeroVector;
	LastRot = FRotator::ZeroRotator;
	LastFOV = -1.f;
}

void UNPSkillCutsceneSubsystem::CreateCaptureAndOverlay(UWorld* World)
{
	FVector2D ViewSize(1920.f, 1080.f);
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewSize);
	}

	float CamAspectRatio = ViewSize.X / FMath::Max(1.f, ViewSize.Y);
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
	{
		const FMinimalViewInfo POV = PC->PlayerCameraManager->GetCameraCacheView();
		if (POV.bConstrainAspectRatio && POV.AspectRatio > 0.f)
		{
			CamAspectRatio = POV.AspectRatio;
		}
	}

	const int32 W = FMath::Max(64, FMath::RoundToInt(ViewSize.X));
	const int32 H = FMath::Max(64, FMath::RoundToInt(W / CamAspectRatio));

	UTextureRenderTarget2D* RT = NewObject<UTextureRenderTarget2D>(this);
	RT->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
	RT->bAutoGenerateMips = false;
	RT->InitAutoFormat(W, H);
	RT->UpdateResourceImmediate(true);
	CutsceneRT = RT;

	FActorSpawnParameters SP;
	SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASceneCapture2D* CapActor = World->SpawnActor<ASceneCapture2D>(ASceneCapture2D::StaticClass(), FTransform::Identity, SP);
	if (!CapActor) return;

	CapActor->PrimaryActorTick.bCanEverTick = true;
	CapActor->PrimaryActorTick.bStartWithTickEnabled = true;
	CapActor->PrimaryActorTick.bTickEvenWhenPaused = true;

	USceneCaptureComponent2D* Cap = CapActor->GetCaptureComponent2D();
	if (!Cap) return;

	Cap->TextureTarget = RT;
	Cap->bCaptureEveryFrame = true;
	Cap->bCaptureOnMovement = false;
	Cap->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	Cap->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	Cap->ShowOnlyActors.Empty();

	Cap->PrimaryComponentTick.bCanEverTick = true;
	Cap->PrimaryComponentTick.bStartWithTickEnabled = true;
	Cap->PrimaryComponentTick.bTickEvenWhenPaused = true;

	CutsceneCapture = CapActor;

	if (!OverlayWidgetClass) return;

	UUserWidget* Overlay = CreateWidget<UUserWidget>(World, OverlayWidgetClass);
	if (!Overlay) return;

	Overlay->AddToViewport(9999);
	if (UWidget* Widget = Overlay->GetWidgetFromName(OverlayImageWidgetName))
	{
		if (UImage* Img = Cast<UImage>(Widget))
		{
			if (OverlayUIMaterialBase)
			{
				OverlayMID = UMaterialInstanceDynamic::Create(OverlayUIMaterialBase, this);
				if (OverlayMID)
				{
					OverlayMID->SetTextureParameterValue(TEXT("RenderTarget"), RT);
					OverlayMID->SetScalarParameterValue(TEXT("OverlayAlpha"), 1.0f); // 초기값
					Img->SetBrushSize(FVector2D(W, H));
					Img->SetBrushFromMaterial(OverlayMID);
				}
			}
		}
	}
	CutsceneOverlay = Overlay;
}

void UNPSkillCutsceneSubsystem::DestroyCaptureAndOverlay()
{
	if (CutsceneOverlay.IsValid())
		CutsceneOverlay->RemoveFromParent();
	
	if (CutsceneCapture.IsValid())
		CutsceneCapture->Destroy();

	CutsceneRT = nullptr;
	CutsceneOverlay = nullptr;
	CutsceneCapture = nullptr;
}

void UNPSkillCutsceneSubsystem::BuildShowOnlyFromSequence(ALevelSequenceActor* SequenceActor)
{
	if (!SequenceActor || !CutsceneCapture.IsValid()) return;

	USceneCaptureComponent2D* Cap = CutsceneCapture->GetCaptureComponent2D();
	Cap->ShowOnlyActors.Empty();

	ULevelSequence* LS = SequenceActor->GetSequence();
	ULevelSequencePlayer* LSP = SequenceActor->GetSequencePlayer();
	if (!LS || !LSP) return;

	UMovieScene* MS = LS->GetMovieScene();
	if (!MS) return;

	const FMovieSceneSequenceID RootID = MovieSceneSequenceID::Root;
	for (const FMovieSceneBinding& Binding : MS->GetBindings())
	{
		const FGuid& Guid = Binding.GetObjectGuid();

		TArrayView<TWeakObjectPtr<UObject>> Objects = LSP->FindBoundObjects(Guid, RootID);
		for (TWeakObjectPtr<UObject> WObj : Objects)
		{
			UObject* Obj = WObj.Get();
			if (!Obj) continue;

			AActor* Actor = Cast<AActor>(Obj);
			if (!Actor)
				if (UActorComponent* Comp = Cast<UActorComponent>(Obj))
					Actor = Comp->GetOwner();

			if (Actor)
				Cap->ShowOnlyActors.AddUnique(Actor);
		}
	}
}

void UNPSkillCutsceneSubsystem::SyncCaptureToCurrentCamera()
{
	if (!CutsceneCapture.IsValid()) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	const FMinimalViewInfo POV = PC->PlayerCameraManager->GetCameraCacheView();

	if (!POV.Location.Equals(LastLoc, 0.01f) || !POV.Rotation.Equals(LastRot, 0.01f))
	{
		CutsceneCapture->SetActorLocationAndRotation(POV.Location, POV.Rotation);
		LastLoc = POV.Location;
		LastRot = POV.Rotation;
	}

	if (!FMath::IsNearlyEqual(POV.FOV, LastFOV))
	{
		if (USceneCaptureComponent2D* Cap = CutsceneCapture->GetCaptureComponent2D())
		{
			Cap->FOVAngle = POV.FOV;
		}
		LastFOV = POV.FOV;
	}
}

void UNPSkillCutsceneSubsystem::OnSequencePlay()
{
	if (!SeqActor.IsValid()) return;

	BuildShowOnlyFromSequence(SeqActor.Get());
	SyncCaptureToCurrentCamera();
}

void UNPSkillCutsceneSubsystem::OnSequenceCameraCut(UCameraComponent* CameraComponent)
{
}

void UNPSkillCutsceneSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SyncCaptureToCurrentCamera();
}
