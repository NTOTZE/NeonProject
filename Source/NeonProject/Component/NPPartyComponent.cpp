// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NPPartyComponent.h"
#include "NeonProject.h"
#include "Character/Player/NPBattlePlayerCharacter.h"
#include "DataType/NPInputCommandTypes.h"
#include "Component/NPCharacterStatComponent.h"
#include "Interface/NPBattleHUDInterface.h"
#include "DataType/NPCharacterData.h"
#include "GameData/NPGameDataSubsystem.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h" 
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameModeBase.h"


UNPPartyComponent::UNPPartyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UNPPartyComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UNPPartyComponent::InitParty(APlayerController* PC, const FTransform& Transform, const TArray<FName>& PartyCharacterIds)
{
    if (!PC || PartyCharacterIds.IsEmpty())
    {
        NP_LOG(NPLog, Warning, TEXT("파티 캐릭터 ID가 설정되어 있지 않습니다."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    ClearParty();

    for (const FName& CharacterId : PartyCharacterIds)
    {
		const FNPCharacterData* CharacterData = UNPGameDataSubsystem::GetPlayerData(this, CharacterId);
        UClass* CharacterClass = CharacterData ? CharacterData->CharacterClass.Get() : nullptr;
        if (!CharacterClass || !CharacterClass->IsChildOf(ANPBattlePlayerCharacter::StaticClass()))
        {
            NP_LOG(NPLog, Warning, TEXT("파티 캐릭터 [%s]의 BattlePlayerCharacter 클래스가 유효하지 않습니다."), *CharacterId.ToString());
            continue;
        }

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ANPBattlePlayerCharacter* NewChar = World->SpawnActorDeferred<ANPBattlePlayerCharacter>(CharacterClass, Transform, /*Owner=*/PC);
        if (!NewChar)
            continue;

		NewChar->SetCharacterSoftTexture(CharacterData->ThumbnailTexture);
        NewChar->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        NewChar->SetActorTickEnabled(false);
        NewChar->SetActorHiddenInGame(true);
        NewChar->FinishSpawning(Transform);

        PartyMembers.Add(NewChar);

        StatComponents.Add(NewChar->GetStatComponent());
    }

    if (PartyMembers.Num() == 0)
    {
        NP_LOG(NPLog, Warning, TEXT("파티 멤버 없음"));
        return;
    }

    InitResourceStatHUD();
    BindResourceStatChanged();

    CurrentIndex = INDEX_NONE;
    World->GetTimerManager().SetTimer(
        TimerHandle_NaturalRecovery,
        this,
        &UNPPartyComponent::NaturalRecoveryTimerCallback,
        NaturalRecoveryTickInterval,
        true);
}

bool UNPPartyComponent::SwapNext(APlayerController* PC)
{
    if (PartyMembers.Num() <= 1) return false;

    for (int32 i = 1; i < PartyMembers.Num(); ++i)
    {
        const int32 NewIndex = (CurrentIndex + i) % PartyMembers.Num();
        if (SwapToIndex(PC, NewIndex))
            return true;
    }

    return false;
}

bool UNPPartyComponent::SwapPrev(APlayerController* PC)
{
    if (PartyMembers.Num() <= 1) return false;

    for (int32 i = PartyMembers.Num() - 1; i > 0; --i)
    {
        const int32 NewIndex = (CurrentIndex + i) % PartyMembers.Num();
        if (SwapToIndex(PC, NewIndex))
            return true;
    }

    return false;
}

bool UNPPartyComponent::SwapToIndex(AController* PC, int32 NewIndex)
{
    if (!PC || !IsValidIndex(NewIndex) || NewIndex == CurrentIndex)
        return false;
    
    if (!IsValidIndex(CurrentIndex))
    {   //초기상태라면
        CurrentIndex = NewIndex;
        PartyMembers[CurrentIndex]->SwapIn(PartyMembers[CurrentIndex]->GetActorTransform());
        PC->Possess(PartyMembers[CurrentIndex]);
        return true;
    }
    
    ANPBattlePlayerCharacter* OldChar = PartyMembers[CurrentIndex];
    ANPBattlePlayerCharacter* NewChar = PartyMembers[NewIndex];
    
    if (!IsValid(OldChar) || !IsValid(NewChar))
        return false;

    if (NewChar->HasAnyState(ENPCharacterState::Dead))
        return false;

    const FTransform OldTransform = OldChar->GetActorTransform();
    FTransform NewTransform = OldTransform;
    {
        const FQuat ControlRotation = PC->GetControlRotation().Quaternion();
        const FVector Offset = ControlRotation.RotateVector(SwapLocationOffset);

        NewTransform.SetRotation(ControlRotation);
        NewTransform.SetLocation(OldTransform.GetLocation() + Offset);
    }
    
    OldChar->SwapOut();
    NewChar->SwapIn(NewTransform);
    
    CurrentIndex = NewIndex;
    PC->Possess(NewChar);
    
    return true;
}

bool UNPPartyComponent::HandleInputCommand(AController* InstigatorController, FNPInputCommand Command)
{
    bool bSucceeded = false;
    switch (Command.CommandType)
    {
    case ENPInputCommandType::Swap:
    {
        SwapToIndex(InstigatorController, Command.SwapIndex);
        bSucceeded = true;
        break;
    }
    }
    return bSucceeded;
}

void UNPPartyComponent::ClearParty()
{
    for (ANPBattlePlayerCharacter* Char : PartyMembers)
    {
        if (IsValid(Char))
        {
            Char->Destroy();
        }
    }
    PartyMembers.Empty();
    StatComponents.Empty();
    CurrentIndex = INDEX_NONE;
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TimerHandle_NaturalRecovery);
    }
}

void UNPPartyComponent::InitResourceStatHUD()
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    INPBattleHUDInterface* BattleHUDInterface = Cast<INPBattleHUDInterface>(GetOwner());
    if (!BattleHUDInterface)
    {
        NP_LOG(NPLog, Error, TEXT("MainHUDInterface : nullptr"));
		return;
	}

	BattleHUDInterface->SetMemberCount(PartyMembers.Num());

    for (int32 i = 0; i < PartyMembers.Num(); ++i)
    {
        if (!PartyMembers[i]) continue;

        PartyMembers[i]->OnStateChange.AddUObject(this, &UNPPartyComponent::HandleMemberStateChange, i);

        if (BattleHUDInterface)
        {
            const FNPResourceStat& HpStat = PartyMembers[i]->GetStatComponent()->GetResourceStat(ENPResourceStatType::Hp);
            BattleHUDInterface->SetMemberHpBar(i, HpStat.CurrentValue, HpStat.MaxValue);

            const FNPResourceStat& SkillCostStat = PartyMembers[i]->GetStatComponent()->GetResourceStat(ENPResourceStatType::SkillCost);
            BattleHUDInterface->SetMemberSkillCostBar(i, SkillCostStat.CurrentValue, SkillCostStat.MaxValue);

            const FNPResourceStat& UltimateCostStat = PartyMembers[i]->GetStatComponent()->GetResourceStat(ENPResourceStatType::UltimateCost);
            BattleHUDInterface->SetMemberUltimateCostBar(i, UltimateCostStat.CurrentValue, UltimateCostStat.MaxValue);

            BattleHUDInterface->SetMemberImage(i, PartyMembers[i]->GetCharacterSoftTexture());
            BattleHUDInterface->SetMemberOpacity(i, 0.5f);
        }
    }

}

void UNPPartyComponent::BindResourceStatChanged()
{
    for (int i = 0; i < PartyMembers.Num(); ++i)
    {
        StatComponents[i]->OnResourceStatChanged.AddUObject(this, &UNPPartyComponent::HandleResourceStatChanged, i);
    }
}

void UNPPartyComponent::NaturalRecoveryTimerCallback()
{
    for (int32 Index = 0; Index < StatComponents.Num(); ++Index)
    {
        if (!PartyMembers.IsValidIndex(Index) || !PartyMembers[Index] ||
            PartyMembers[Index]->HasAnyState(ENPCharacterState::Dead))
            continue;

        UNPCharacterStatComponent* StatComp = StatComponents[Index];
        if (!StatComp)
            continue;

        FNPNaturalRecoveryContext Context;
        Context.RecoveryMultiplier = 1.f;
        StatComp->ApplyNaturalRecovery(Context, NaturalRecoveryTickInterval);
    }
}

void UNPPartyComponent::HandleResourceStatChanged(ENPResourceStatType Type, float current, float max, int32 idx)
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    INPBattleHUDInterface* BattleHUDInterface = Cast<INPBattleHUDInterface>(GetOwner());
    if (!BattleHUDInterface)
    {
        NP_LOG(NPLog, Error, TEXT("MainHUDInterface : nullptr"));
        return;
    }

    if (!PartyMembers.IsValidIndex(idx))
        return;

    switch (Type)
    {
    case ENPResourceStatType::Hp:
    {
        BattleHUDInterface->SetMemberHpBar(idx, current, max);
        if (GetCurrentIdx() == idx)
        {
            BattleHUDInterface->SetPlayCharacterHpBar(current, max);
        }

        break;
    }
    case ENPResourceStatType::Stamina:
    {
        if (GetCurrentIdx() == idx)
        {
            BattleHUDInterface->SetPlayCharacterStaminaBar(current, max);
        }

        break;
    }
    case ENPResourceStatType::SkillCost:
    {
        BattleHUDInterface->SetMemberSkillCostBar(idx, current, max);
        if (GetCurrentIdx() == idx)
        {
            BattleHUDInterface->SetPlayCharacterSkillCostBar(current, max);
        }

        break;
    }
    case ENPResourceStatType::UltimateCost:
    {
        BattleHUDInterface->SetMemberUltimateCostBar(idx, current, max);
        if (GetCurrentIdx() == idx)
        {
            BattleHUDInterface->SetPlayCharacterUltimateCostBar(current, max);
        }

        break;
    }
    default:
        break;
    }
}

void UNPPartyComponent::HandleMemberStateChange(ENPCharacterState Flags, bool bValue, int32 idx)
{
    if (!EnumHasAnyFlags(Flags, ENPCharacterState::Dead) || !bValue)
        return;

    UWorld* World = GetWorld();
    if (!World)
        return;

    World->GetTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateUObject(this, &ThisClass::HandleMemberDeadDeferred, idx));
}

void UNPPartyComponent::HandleMemberDeadDeferred(int32 Index)
{
    if (!PartyMembers.IsValidIndex(Index) || !PartyMembers[Index] ||
        !PartyMembers[Index]->HasAnyState(ENPCharacterState::Dead))
        return;

    OnPartyMemberDead.Broadcast(Index);

    if (GetCurrentIdx() == Index)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetCurrent()->GetController()))
        {
            SwapNext(PC);
        }
    }
}
