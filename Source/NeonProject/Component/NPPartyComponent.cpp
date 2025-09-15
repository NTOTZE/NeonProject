// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NPPartyComponent.h"
#include "NeonProject.h"
#include "Character/NPPlayerCharacter.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h" 
#include "Components/CapsuleComponent.h"


UNPPartyComponent::UNPPartyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UNPPartyComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UNPPartyComponent::InitParty(APlayerController* PC, const FTransform& Transform)
{
    if (!PC || PartyClasses.Num() == 0)
    {
        NP_LOG(NPLog, Warning, TEXT("파티 캐릭터 클래스 없음"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    ClearParty();

    for (const TSubclassOf<ANPPlayerCharacter>& Cls : PartyClasses)
    {
        if (!*Cls) continue;

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ANPPlayerCharacter* NewChar = World->SpawnActorDeferred<ANPPlayerCharacter>(Cls, Transform, /*Owner=*/PC);
        if (!NewChar)
            continue;

        NewChar->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        NewChar->SetActorTickEnabled(false);
        NewChar->SetActorHiddenInGame(true);
        NewChar->FinishSpawning(Transform);

        PartyMembers.Add(NewChar);
    }

    if (PartyMembers.Num() == 0)
    {
        NP_LOG(NPLog, Warning, TEXT("파티 멤버 없음"));
        return;
    }

    CurrentIndex = INDEX_NONE;
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

bool UNPPartyComponent::SwapToIndex(APlayerController* PC, int32 NewIndex)
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
    
    ANPPlayerCharacter* OldChar = PartyMembers[CurrentIndex];
    ANPPlayerCharacter* NewChar = PartyMembers[NewIndex];
    
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

void UNPPartyComponent::ClearParty()
{
    for (ANPPlayerCharacter* Char : PartyMembers)
    {
        if (IsValid(Char))
        {
            Char->Destroy();
        }
    }
    PartyMembers.Empty();
    CurrentIndex = INDEX_NONE;
}
