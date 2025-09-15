// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controller/NPPlayerController.h"
#include "NeonProject.h"

#include "DataType/NPCombatTypes.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "Engine/OverlapResult.h"
#include "Core/Controller/NPCameraRig.h"
#include "Character/NPPlayerCharacter.h"
#include "Component/NPPartyComponent.h"
#include "Core/PlayerSettings.h"
#include "Core/GameMode/NPGameMode.h"
#include "Kismet/GameplayStatics.h"

ANPPlayerController::ANPPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	bAutoManageActiveCameraTarget = false;

	PartyComp = CreateDefaultSubobject<UNPPartyComponent>(TEXT("PartyComp"));

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_CharacterFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IMC_CharacterMovement.IMC_CharacterMovement"));
	if (IMC_CharacterFinder.Succeeded())
	{
		IMC_Character = IMC_CharacterFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveForwardFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_MoveForward.IA_MoveForward"));
	if (IA_MoveForwardFinder.Succeeded())
	{
		IA_MoveForward = IA_MoveForwardFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveRightFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_MoveRight.IA_MoveRight"));
	if (IA_MoveRightFinder.Succeeded())
	{
		IA_MoveRight = IA_MoveRightFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LookUpFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_LookUp.IA_LookUp"));
	if (IA_LookUpFinder.Succeeded())
	{
		IA_LookUp = IA_LookUpFinder.Object;
	}

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

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_DashFinder
	(TEXT("/Game/NeonProject/Input/CharacterMovement/IA_Dash.IA_Dash"));
	if (IA_DashFinder.Succeeded())
	{
		IA_Dash = IA_DashFinder.Object;
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
}

void ANPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World) return;
	AGameModeBase* GameMode = World->GetAuthGameMode();
	if (!GameMode) return;
	
	FTransform SpawnTransform = FTransform::Identity;
	if (AActor* StartPoint = GameMode->FindPlayerStart(this))
	{
		SpawnTransform = StartPoint->GetActorTransform();
	}

	CachedMouseSensitivity = GetDefault<UPlayerSettings>()->MouseSensitivity;

	if (!CameraRig)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CameraRig = GetWorld()->SpawnActor<ANPCameraRig>(ANPCameraRig::StaticClass(), SpawnTransform, Params);
	}
	SetViewTarget(CameraRig);

	SpawnAndInitParty(SpawnTransform, 1.5f);


	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(IMC_Character, 0);
		}
	}
}

void ANPPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}

void ANPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComp->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleMoveForward);
		EnhancedInputComp->BindAction(IA_MoveForward, ETriggerEvent::Completed, this, &ANPPlayerController::HandleMoveForward);
		EnhancedInputComp->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleMoveRight);
		EnhancedInputComp->BindAction(IA_MoveRight, ETriggerEvent::Completed, this, &ANPPlayerController::HandleMoveRight);
		EnhancedInputComp->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleLookUp);
		
		EnhancedInputComp->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleAttack);
		EnhancedInputComp->BindAction(IA_AttackHold, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleCombatAction, ENPAbilityType::ChargeAttack);
		
		EnhancedInputComp->BindAction(IA_Dash, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleDash);
		EnhancedInputComp->BindAction(IA_Skill, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleCombatAction, ENPAbilityType::Skill);
		EnhancedInputComp->BindAction(IA_Ultimate, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleCombatAction, ENPAbilityType::Ultimate);

		EnhancedInputComp->BindAction(IA_Swap1, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleSwap, 0);
		EnhancedInputComp->BindAction(IA_Swap2, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleSwap, 1);
		EnhancedInputComp->BindAction(IA_Swap3, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleSwap, 2);

		EnhancedInputComp->BindAction(IA_LockOn, ETriggerEvent::Canceled, this, &ANPPlayerController::HandleLockOn);
		EnhancedInputComp->BindAction(IA_LockOn, ETriggerEvent::Triggered, this, &ANPPlayerController::HandleUnlock);
	}
}

void ANPPlayerController::SpawnAndInitParty(FTransform SpawnTransform, float SpawnDelay)
{
	PartyComp->InitParty(this, SpawnTransform);

	const TArray<ANPPlayerCharacter*> Members = PartyComp->GetAllMembers();

	ANPGameMode* NPGameMode = Cast<ANPGameMode>(GetWorld()->GetAuthGameMode());

	for (int32 i = 0; i < Members.Num(); ++i)
	{
		if (!Members[i]) continue;

		Members[i]->OnStateChange.AddUObject(this, &ANPPlayerController::HandleMemberStateChange, i);
		Members[i]->OnHPChange.AddUObject(this, &ANPPlayerController::HandleMemberHPChange, i);
		if (NPGameMode)
		{
			const float ratio = Members[i]->GetCurrentHP() / Members[i]->GetMaxHP();
			NPGameMode->SetMemberHPBarRatio(i, ratio);
			NPGameMode->SetMemberImage(i, Members[i]->GetCharacterSoftTexture());
			NPGameMode->SetMemberOpacity(i, 0.5f);
		}
	}

	OnPossessedPawnChanged.AddDynamic(this, &ANPPlayerController::HandlePossessedPawnChanged);
	GetWorldTimerManager().SetTimer(TimerHandle_Spawn, this, &ANPPlayerController::BeginSpawnCharacter, SpawnDelay, false);
}

void ANPPlayerController::BeginSpawnCharacter()
{
	PartyComp->SwapToIndex(this, 0);
}

void ANPPlayerController::HandleMemberStateChange(ENPCharacterState Flags, bool bValue, int32 idx)
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

void ANPPlayerController::HandleMemberHPChange(float OldHP, float NewHP, int32 Idx)
{
	UWorld* World = GetWorld();
	if (!World) return;

	ANPGameMode* NPGameMode = Cast<ANPGameMode>(World->GetAuthGameMode());
	if (!NPGameMode) return;

	const TArray<ANPPlayerCharacter*> Members = PartyComp->GetAllMembers();
	if (!Members.IsValidIndex(Idx)) return;

	const float MaxHP = Members[Idx]->GetMaxHP();
	const float Ratio = NewHP / MaxHP;

	NPGameMode->SetMemberHPBarRatio(Idx, Ratio);
	if (PartyComp->GetCurrentIdx() == Idx)
		NPGameMode->SetPlayCharacterHPBarRatio(Ratio);
}

AActor* ANPPlayerController::GetLockOnTarget() const
{
	return CameraRig->GetLockOnTarget();
}

AActor* ANPPlayerController::SearchNearTargetWithinCircle(ECollisionChannel TraceChannel, float Radius)
{
	if (!CachedCharacter) return nullptr;

	TArray<FOverlapResult> Overlaps;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	const FVector CharacterLocation = CachedCharacter->GetActorLocation();

	const bool bHit = World->OverlapMultiByChannel(
		Overlaps,
		CharacterLocation,
		FQuat(),
		TraceChannel,
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams::DefaultQueryParam
	);

	TWeakObjectPtr<AActor> ResultActor = nullptr;

	for (const FOverlapResult& Overlap : Overlaps)
	{

		AActor* OverlapActor = Overlap.GetActor();
		if (!OverlapActor || OverlapActor == CachedCharacter) continue;

		if (!ResultActor.Get())
		{
			ResultActor = OverlapActor;
			continue;
		}

		// 최소거리 엑터 찾기
		const float ResultDist = FVector::Dist2D(ResultActor->GetActorLocation(), CharacterLocation);
		const float OverlapDist = FVector::Dist2D(OverlapActor->GetActorLocation(), CharacterLocation);
		if (OverlapDist < ResultDist)
		{
			ResultActor = OverlapActor;
		}
	}

	return ResultActor.Get();

	return nullptr;
}

AActor* ANPPlayerController::SearchNearTargetWithinSector(ECollisionChannel TraceChannel, float Radius, const FVector& Direction, float AngleDeg)
{
	if (!CachedCharacter) return nullptr;

	TArray<FOverlapResult> Overlaps;

	UWorld* World = CachedCharacter->GetWorld();
	if (!World) return nullptr;

	const FVector CharacterLocation = CachedCharacter->GetActorLocation();

	const bool bHit = World->OverlapMultiByChannel(
		Overlaps,
		CharacterLocation,
		FQuat(),
		TraceChannel,
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams::DefaultQueryParam
	);

	TWeakObjectPtr<AActor> ResultActor = nullptr;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* OverlapActor = Overlap.GetActor();
		if (!OverlapActor || OverlapActor == CachedCharacter) continue;

		// 부채꼴 판단
		const FVector TargetLoc = OverlapActor->GetActorLocation();
		const FVector OwnerLoc = CachedCharacter->GetActorLocation();

		FVector ToTarget = TargetLoc - OwnerLoc;
		ToTarget.Z = 0.f;
		ToTarget = ToTarget.GetSafeNormal2D();

		FVector Forward = Direction;
		Forward.Z = 0.f;
		Forward = Forward.GetSafeNormal2D();

		const float SectorCos = FMath::Cos(FMath::DegreesToRadians(AngleDeg * 0.5f));
		const float TargetCos = FVector::DotProduct(Forward, ToTarget);

		if (TargetCos < SectorCos)
			continue;

		if (!ResultActor.Get())
		{
			ResultActor = OverlapActor;
			continue;
		}

		// 최소거리 엑터 찾기
		const float ResultDist = FVector::Dist2D(ResultActor->GetActorLocation(), CharacterLocation);
		const float OverlapDist = FVector::Dist2D(OverlapActor->GetActorLocation(), CharacterLocation);
		if (OverlapDist < ResultDist)
		{
			ResultActor = OverlapActor;
		}
	}

	return ResultActor.Get();
}

void ANPPlayerController::ApplyTimeDilation(float Dilation, float Duration)
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

void ANPPlayerController::HandlePossessedPawnChanged(APawn* PrevPawn, APawn* NextPawn)
{
	if (!NextPawn) return;

	ANPPlayerCharacter* NextCharacter = Cast<ANPPlayerCharacter>(NextPawn);
	if (!NextCharacter) return;

	NP_LOG(NPLog, Warning, TEXT(""));
	CachedCharacter = NextCharacter;

	CameraRig->SetFollowTarget(NextPawn);

	SetControlRotation(CachedControlRotation);

	ANPGameMode* NPGameMode = Cast<ANPGameMode>(GetWorld()->GetAuthGameMode());
	if (NPGameMode)
	{
		const float ratio = CachedCharacter->GetCurrentHP() / CachedCharacter->GetMaxHP();
		NPGameMode->SetPlayCharacterHPBarRatio(ratio);

		NPGameMode->SetMemberOpacity(0, 0.5f);
		NPGameMode->SetMemberOpacity(1, 0.5f);
		NPGameMode->SetMemberOpacity(2, 0.5f);
		NPGameMode->SetMemberOpacity(PartyComp->GetCurrentIdx(), 1.f);
	}
}

void ANPPlayerController::HandleMoveForward(const FInputActionValue& Value)
{
	if (!CachedCharacter) return;

	const FVector2D VectorValue = Value.Get<FVector2D>();
	const float ForwardInput = VectorValue.X - VectorValue.Y;
	
	const float ControlYaw = GetControlRotation().Yaw;
	const FVector ForwardVector = FRotator(0.f, ControlYaw, 0.f).RotateVector(FVector(1.f, 0.f, 0.f));

	CachedCharacter->OnMoveForwardInput(ForwardVector, ForwardInput);
}

void ANPPlayerController::HandleMoveRight(const FInputActionValue& Value)
{
	if (!CachedCharacter) return;

	const FVector2D VectorValue = Value.Get<FVector2D>();
	const float RightInput = VectorValue.X - VectorValue.Y;

	const float ControlYaw = GetControlRotation().Yaw;
	const FVector RightVector = FRotator(0.f, ControlYaw, 0.f).RotateVector(FVector(0.f, 1.f, 0.f));
	CachedCharacter->OnMoveRightInput(RightVector, RightInput);
}

void ANPPlayerController::HandleLookUp(const FInputActionValue& Value)
{
	if (!CachedCharacter) return;

	//TODO : 락온 중 동작 안함
	// if(GetLockOnTarget()) return;
	
	const FVector2D VectorValue = Value.Get<FVector2D>();

	AddYawInput(VectorValue.X * CachedMouseSensitivity);
	AddPitchInput(-VectorValue.Y * CachedMouseSensitivity);

}

void ANPPlayerController::HandleAttack()
{
	if (!CachedCharacter) return;

	CachedCharacter->OnAttackInput();
}

void ANPPlayerController::HandleDash()
{
	if (!CachedCharacter) return;

	CachedCharacter->OnDashInput();
}

void ANPPlayerController::HandleCombatAction(ENPAbilityType AbilityType)
{
	if (!CachedCharacter) return;
	CachedCharacter->TryExecuteAbility(AbilityType);
}

void ANPPlayerController::HandleSwap(int SwapNumber)
{
	if (!CachedCharacter) return;
	if (SwapNumber < 0 || SwapNumber > 2) return;

	CachedControlRotation = GetControlRotation();
	PartyComp->SwapToIndex(this, SwapNumber);
}

void ANPPlayerController::HandleLockOn()
{
	if (!CachedCharacter) return;

	NP_LOG(NPLog, Warning, TEXT(""));
	
	AActor* LockOnTarget = SearchNearTargetWithinSector(
		NPDamageCollisionTraceChannel[(uint8)ENPDamageCollisionType::MonsterHit]
		, 3500.f
		, GetControlRotation().RotateVector(FVector(1.f,0.f,0.f))
		, 180);

	if (!LockOnTarget) return;
	
	CameraRig->SetLockOnTarget(LockOnTarget);

}

void ANPPlayerController::HandleUnlock()
{
	NP_LOG(NPLog, Warning, TEXT(""));
	CameraRig->SetLockOnTarget(nullptr);

}