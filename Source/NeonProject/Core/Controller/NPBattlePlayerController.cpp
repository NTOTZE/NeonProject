// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controller/NPBattlePlayerController.h"
#include "NeonProject.h"

#include "DataType/NPCombatTypes.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"

#include "DataType/NPInputCommandTypes.h"
#include "Interface/NPInputCommandReceiver.h"
#include "Core/Controller/NPCameraRig.h"
#include "Character/Player/NPBattlePlayerCharacter.h"
#include "Component/NPPartyComponent.h"
#include "Component/NPCharacterStatComponent.h"
#include "Interface/NPBattleHUDInterface.h"
#include "Combat/Cutscene/NPSkillCutsceneSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "UI/Battle/NPBattleHUDBase.h"
#include "Core/GameMode/NPBattleGameMode.h"
#include "Loading/NPStageSessionSubsystem.h"


ANPBattlePlayerController::ANPBattlePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	bAutoManageActiveCameraTarget = false;

	PartyComp = CreateDefaultSubobject<UNPPartyComponent>(TEXT("PartyComp"));

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_AttackFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_Attack.IA_Attack"));
	if (IA_AttackFinder.Succeeded())
	{
		IA_Attack = IA_AttackFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_AttackHoldFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_AttackHold.IA_AttackHold"));
	if (IA_AttackHoldFinder.Succeeded())
	{
		IA_AttackHold = IA_AttackHoldFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_SkillFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_Skill.IA_Skill"));
	if (IA_SkillFinder.Succeeded())
	{
		IA_Skill = IA_SkillFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_UltimateFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_Ultimate.IA_Ultimate"));
	if (IA_UltimateFinder.Succeeded())
	{
		IA_Ultimate = IA_UltimateFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Swap1Finder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_Swap1.IA_Swap1"));
	if (IA_Swap1Finder.Succeeded())
	{
		IA_Swap1 = IA_Swap1Finder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Swap2Finder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_Swap2.IA_Swap2"));
	if (IA_Swap2Finder.Succeeded())
	{
		IA_Swap2 = IA_Swap2Finder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Swap3Finder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_Swap3.IA_Swap3"));
	if (IA_Swap3Finder.Succeeded())
	{
		IA_Swap3 = IA_Swap3Finder.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LockOnFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_LockOn.IA_LockOn"));
	if (IA_LockOnFinder.Succeeded())
	{
		IA_LockOn = IA_LockOnFinder.Object;
	}

	CutsceneSubLevel = TSoftObjectPtr<UWorld>(FSoftObjectPath(
		TEXT("/Game/NeonProject/Map/SubLevel/SL_Cutscene.SL_Cutscene")));

	ConstructorHelpers::FClassFinder<UUserWidget> Finder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/NeonProject/Blueprint/UI/Battle/WBP_BattleHUD.WBP_BattleHUD_C'"));
	if (Finder.Succeeded())
	{
		BattleHudClass = Finder.Class;
	}


	PrimaryActorTick.bTickEvenWhenPaused = true;

	// *** 퍼즈상태일때 레벨시퀀스에서 카메라 사용하기 위해서 필요
	// 사용 안할경우 뷰가 갱신되지 않음
	bShouldPerformFullTickWhenPaused = true;
}

void ANPBattlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitBattleHUD();

	UWorld* World = GetWorld();
	if (!World) return;
	AGameModeBase* GameMode = World->GetAuthGameMode();
	if (!GameMode) return;
	
	FTransform SpawnTransform = FTransform::Identity;
	if (AActor* StartPoint = GameMode->FindPlayerStart(this))
	{
		SpawnTransform = StartPoint->GetActorTransform();
	}

	if (!CameraRig)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CameraRig = GetWorld()->SpawnActor<ANPCameraRig>(ANPCameraRig::StaticClass(), SpawnTransform, Params);
	}
	SetViewTarget(CameraRig);

	UNPStageSessionSubsystem* StageSession = GetWorld()->GetSubsystem<UNPStageSessionSubsystem>();
	check(StageSession);
	StageSession->OnSessionReady.AddUObject(this, &ThisClass::HandleStageSessionReady);
	if (StageSession->IsSessionInitialized())
		HandleStageSessionReady();

}

void ANPBattlePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}

void ANPBattlePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		
		EnhancedInputComp->BindAction(IA_Attack, ETriggerEvent::Triggered, this,
			&ANPBattlePlayerController::TryExecuteInputCommand, FNPInputCommand::MakeAttack());

		EnhancedInputComp->BindAction(IA_AttackHold, ETriggerEvent::Triggered, this,
			&ANPBattlePlayerController::TryExecuteInputCommand, FNPInputCommand::MakeAbility(ENPAbilityType::ChargeAttack));
		EnhancedInputComp->BindAction(IA_Skill, ETriggerEvent::Triggered, this, 
			&ANPBattlePlayerController::TryExecuteInputCommand, FNPInputCommand::MakeAbility(ENPAbilityType::Skill));
		EnhancedInputComp->BindAction(IA_Ultimate, ETriggerEvent::Triggered, this, 
			&ANPBattlePlayerController::TryExecuteInputCommand, FNPInputCommand::MakeAbility(ENPAbilityType::Ultimate));

		EnhancedInputComp->BindAction(IA_Swap1, ETriggerEvent::Triggered, this, 
			&ANPBattlePlayerController::TryExecuteInputCommand, FNPInputCommand::MakeSwap(0));
		EnhancedInputComp->BindAction(IA_Swap2, ETriggerEvent::Triggered, this,
			&ANPBattlePlayerController::TryExecuteInputCommand, FNPInputCommand::MakeSwap(1));
		EnhancedInputComp->BindAction(IA_Swap3, ETriggerEvent::Triggered, this,
			&ANPBattlePlayerController::TryExecuteInputCommand, FNPInputCommand::MakeSwap(2));

		EnhancedInputComp->BindAction(IA_LockOn, ETriggerEvent::Canceled, this,
			&ANPBattlePlayerController::TryExecuteInputCommand, FNPInputCommand::MakeLockOn());
		EnhancedInputComp->BindAction(IA_LockOn, ETriggerEvent::Triggered, this,
			&ANPBattlePlayerController::TryExecuteInputCommand, FNPInputCommand::MakeUnlock());
	}
}

bool ANPBattlePlayerController::ExecuteInputCommand(FNPInputCommand Command)
{
	bool bSucceeded = Super::ExecuteInputCommand(Command);
	if (bSucceeded)
		return true;

	INPInputCommandReceiver* Receiver = nullptr;

	switch (Command.Receiver)
	{
		case ENPInputCommandReceiver::Character:
		{
			if (CachedCharacter)
			{
				Receiver = Cast<INPInputCommandReceiver>(CachedCharacter);
			}

			break;
		}
		case ENPInputCommandReceiver::Party:
		{
			if (PartyComp)
			{
				Receiver = Cast<INPInputCommandReceiver>(PartyComp);
				CachedControlRotation = GetControlRotation();
			}

			break;
		}
		case ENPInputCommandReceiver::CameraRig:
		{
			if (CameraRig)
			{
				Receiver = Cast<INPInputCommandReceiver>(CameraRig);
			}

			break;
		}
	}

	if (Receiver)
	{
		bSucceeded = Receiver->HandleInputCommand(this, Command);
	}

	return bSucceeded;
}

void ANPBattlePlayerController::HandleStageSessionReady()
{
	if (bPartyInitialized)
		return;

	UNPStageSessionSubsystem* StageSession = GetWorld()->GetSubsystem<UNPStageSessionSubsystem>();
	check(StageSession);
	const FNPStageSessionData& SessionData = StageSession->GetSessionData();

	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	AActor* StartPoint = GameMode ? GameMode->FindPlayerStart(this) : nullptr;
	const FTransform SpawnTransform = StartPoint ? StartPoint->GetActorTransform() : FTransform::Identity;
	SpawnAndInitParty(SpawnTransform, 1.5f, SessionData.PartyCharacterIds);
	bPartyInitialized = true;
}

void ANPBattlePlayerController::SpawnAndInitParty(FTransform SpawnTransform, float SpawnDelay, const TArray<FName>& PartyCharacterIds)
{
	PartyComp->InitParty(this, SpawnTransform, PartyCharacterIds);

	OnPossessedPawnChanged.AddDynamic(this, &ANPBattlePlayerController::HandlePossessedPawnChanged);
	GetWorldTimerManager().SetTimer(TimerHandle_Spawn, this, &ANPBattlePlayerController::BeginSpawnCharacter, SpawnDelay, false);
}

void ANPBattlePlayerController::BeginSpawnCharacter()
{
	PartyComp->SwapToIndex(this, 0);
}

void ANPBattlePlayerController::HandleMemberStateChange(ENPCharacterState Flags, bool bValue, int32 idx)
{
	if (EnumHasAnyFlags(Flags, ENPCharacterState::Active) && bValue == false)
	{
		if (PartyComp->GetCurrentIdx() == idx && PartyComp->GetCurrent()->HasAnyState(ENPCharacterState::Dead))
		{
			if (!PartyComp->SwapNext(this))
			{
				NP_LOG(NPLog, Warning, TEXT("전멸"));
			}
		}
	}
}

AActor* ANPBattlePlayerController::GetLockOnTarget() const
{
	return CameraRig->GetLockOnTarget();
}

AActor* ANPBattlePlayerController::FindTargetAround(ECollisionChannel TraceChannel, float Radius) const
{
	return CameraRig->SearchNearTargetWithinCircle(TraceChannel, Radius);
}

AActor* ANPBattlePlayerController::FindTargetInFront(ECollisionChannel TraceChannel, float Radius, const FVector& Direction, float AngleDeg) const
{
	return CameraRig->SearchNearTargetWithinSector(TraceChannel, Radius, Direction, AngleDeg);
}

/////////////////////////////////////////////////////////////////////////
// 	Battle HUD
void ANPBattlePlayerController::InitBattleHUD()
{
	if (IsValid(BattleHudClass))
	{
		BattleHUD = Cast<UNPBattleHUDBase>(CreateWidget(GetWorld(), BattleHudClass));
	}

	if (IsValid(BattleHUD))
	{
		BattleHUD->AddToViewport();
	}
}

void ANPBattlePlayerController::SetPlayCharacterHpBar(float current, float max)
{
	if (!BattleHUD)
	{
		NP_LOG(NPLog, Error, TEXT("BattleHUD : nullptr"));
		return;
	}
	BattleHUD->SetPlayerHpBar(current, max);
}

void ANPBattlePlayerController::SetPlayCharacterStaminaBar(float current, float max)
{
	if (!BattleHUD)
	{
		NP_LOG(NPLog, Error, TEXT("BattleHUD : nullptr"));
		return;
	}
	BattleHUD->SetPlayerStaminaBar(current, max);
}

void ANPBattlePlayerController::SetPlayCharacterSkillCostBar(float current, float max)
{
	if (!BattleHUD)
	{
		NP_LOG(NPLog, Error, TEXT("BattleHUD : nullptr"));
		return;
	}
	BattleHUD->SetPlayerSkillCostBar(current, max);
}

void ANPBattlePlayerController::SetPlayCharacterUltimateCostBar(float current, float max)
{
	if (!BattleHUD)
	{
		NP_LOG(NPLog, Error, TEXT("BattleHUD : nullptr"));
		return;
	}
	BattleHUD->SetPlayerUltimateCostBar(current, max);
}

void ANPBattlePlayerController::SetMemberHpBar(int32 Idx, float current, float max)
{
	if (!BattleHUD)
	{
		NP_LOG(NPLog, Error, TEXT("BattleHUD : nullptr"));
		return;
	}
	BattleHUD->SetMemberHpBar(Idx, current, max);
}

void ANPBattlePlayerController::SetMemberSkillCostBar(int32 Idx, float current, float max)
{
	if (!BattleHUD)
	{
		NP_LOG(NPLog, Error, TEXT("BattleHUD : nullptr"));
		return;
	}
	BattleHUD->SetMemberSkillCostBar(Idx, current, max);
}

void ANPBattlePlayerController::SetMemberUltimateCostBar(int32 Idx, float current, float max)
{
	if (!BattleHUD)
	{
		NP_LOG(NPLog, Error, TEXT("BattleHUD : nullptr"));
		return;
	}
	BattleHUD->SetMemberUltimateCostBar(Idx, current, max);
}

void ANPBattlePlayerController::SetMemberImage(int32 Idx, TSoftObjectPtr<class UTexture2D> SoftTexture)
{
	if (!BattleHUD)
	{
		NP_LOG(NPLog, Error, TEXT("BattleHUD : nullptr"));
		return;
	}
	BattleHUD->SetMemberImage(Idx, SoftTexture);
}

void ANPBattlePlayerController::SetMemberOpacity(int32 Idx, float Opacity)
{
	if (!BattleHUD)
	{
		NP_LOG(NPLog, Error, TEXT("BattleHUD : nullptr"));
		return;
	}
	BattleHUD->SetMemberOpacity(Idx, Opacity);
}

void ANPBattlePlayerController::SetWaveText(const FString& str)
{
	if (!BattleHUD)
	{
		NP_LOG(NPLog, Error, TEXT("BattleHUD : nullptr"));
		return;
	}
	BattleHUD->SetWaveText(str);
}

void ANPBattlePlayerController::ApplyTimeDilation(float Dilation, float Duration)
{
	UWorld* World = GetWorld();
	if (!World) return;

	const float AppliedDilation = FMath::Clamp(Dilation, 0.01f, 1.0f);
	const float AppliedDuration = FMath::Clamp(Duration, 0.1f, 10.f) * AppliedDilation;

	UGameplayStatics::SetGlobalTimeDilation(World, AppliedDilation);

	World->GetTimerManager().SetTimer(TimerHandle_Dilation,
		[World]()
		{
			if (!World) return;
			UGameplayStatics::SetGlobalTimeDilation(World, 1.f);
		}
	, AppliedDuration, false);
}

void ANPBattlePlayerController::InitControlRotation()
{
	SetControlRotation(FRotator(DefaultControlRotation.Pitch, CachedCharacter->GetActorRotation().Yaw, 0.f));
}

void ANPBattlePlayerController::SetViewTargetToRig()
{
	SetViewTargetWithBlend(CameraRig, 0.5f, VTBlend_Cubic, 2.f, true);
}

void ANPBattlePlayerController::HandlePossessedPawnChanged(APawn* PrevPawn, APawn* NextPawn)
{
	if (!NextPawn) return;

	ANPBattlePlayerCharacter* NextCharacter = Cast<ANPBattlePlayerCharacter>(NextPawn);
	if (!NextCharacter) return;

	NP_LOG(NPLog, Warning, TEXT(""));
	CachedCharacter = NextCharacter;

	CameraRig->SetFollowTarget(NextCharacter);

	SetControlRotation(CachedControlRotation);

	const FNPResourceStat& HpStat = CachedCharacter->GetStatComponent()->GetResourceStat(ENPResourceStatType::Hp);
	const FNPResourceStat& StaminaStat = CachedCharacter->GetStatComponent()->GetResourceStat(ENPResourceStatType::Stamina);
	const FNPResourceStat& SkillStat = CachedCharacter->GetStatComponent()->GetResourceStat(ENPResourceStatType::SkillCost);
	const FNPResourceStat& UltimateStat = CachedCharacter->GetStatComponent()->GetResourceStat(ENPResourceStatType::UltimateCost);

	SetPlayCharacterHpBar(HpStat.CurrentValue, HpStat.MaxValue);
	SetPlayCharacterStaminaBar(StaminaStat.CurrentValue, StaminaStat.MaxValue);
	SetPlayCharacterSkillCostBar(SkillStat.CurrentValue, SkillStat.MaxValue);
	SetPlayCharacterUltimateCostBar(SkillStat.CurrentValue, UltimateStat.MaxValue);
	
	SetMemberOpacity(0, 0.5f);
	SetMemberOpacity(1, 0.5f);
	SetMemberOpacity(2, 0.5f);
	SetMemberOpacity(PartyComp->GetCurrentIdx(), 1.f);
}


