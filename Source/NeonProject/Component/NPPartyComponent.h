// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType/NPCombatTypes.h"
#include "NPPartyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEONPROJECT_API UNPPartyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UNPPartyComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Party")
    void InitParty(APlayerController* PC, const FTransform& Transform);

    UFUNCTION(BlueprintCallable, Category = "Party")
    bool SwapNext(APlayerController* PC);

    UFUNCTION(BlueprintCallable, Category = "Party")
    bool SwapPrev(APlayerController* PC);

    UFUNCTION(BlueprintCallable, Category = "Party")
    bool SwapToIndex(APlayerController* PC, int32 NewIndex);

    class ANPPlayerCharacter* GetCurrent() const { return PartyMembers.IsValidIndex(CurrentIndex) ? PartyMembers[CurrentIndex] : nullptr; }
    int32 GetCurrentIdx() { return CurrentIndex; }

    // 파티 멤버 수
    UFUNCTION(BlueprintPure, Category = "Party")
    int32 Num() const { return PartyMembers.Num(); }

    const TArray<class ANPPlayerCharacter*>& GetAllMembers() const { return PartyMembers; }

private:
    bool IsValidIndex(int32 Index) const { return PartyMembers.IsValidIndex(Index); }
    void ClearParty();
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party")
    TArray<TSubclassOf<class ANPPlayerCharacter>> PartyClasses;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party")
    FVector SwapLocationOffset = FVector(0.f, 200.f, 0.f);

private:
    /** 스폰된 파티 멤버들 */
    UPROPERTY()
    TArray<ANPPlayerCharacter*> PartyMembers;

    /** 현재 조종 중인 인덱스 (없으면 -1) */
    int32 CurrentIndex = INDEX_NONE;
};