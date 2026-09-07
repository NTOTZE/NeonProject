// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataType/NPCombatTypes.h"
#include "Interface/NPInputCommandReceiver.h"
#include "Component/NPCharacterStatComponent.h"
#include "NPPartyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEONPROJECT_API UNPPartyComponent : public UActorComponent, public INPInputCommandReceiver
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
	void InitParty(APlayerController* PC, const FTransform& Transform, const TArray<FName>& PartyCharacterIds);

    UFUNCTION(BlueprintCallable, Category = "Party")
    bool SwapNext(APlayerController* PC);

    UFUNCTION(BlueprintCallable, Category = "Party")
    bool SwapPrev(APlayerController* PC);

    UFUNCTION(BlueprintCallable, Category = "Party")
    bool SwapToIndex(AController* PC, int32 NewIndex);

    virtual bool HandleInputCommand(class AController* InstigatorController, FNPInputCommand Command) override;

    class ANPBattlePlayerCharacter* GetCurrent() const { return PartyMembers.IsValidIndex(CurrentIndex) ? PartyMembers[CurrentIndex] : nullptr; }
    int32 GetCurrentIdx() { return CurrentIndex; }

    // 파티 멤버 수
    UFUNCTION(BlueprintPure, Category = "Party")
    int32 Num() const { return PartyMembers.Num(); }

    const TArray<class ANPBattlePlayerCharacter*>& GetAllMembers() const { return PartyMembers; }

private:
    bool IsValidIndex(int32 Index) const { return PartyMembers.IsValidIndex(Index); }
    void ClearParty();

////////////////////////////////////////////
// 리소스 스탯 관련
    void InitResourceStatHUD();
    void BindResourceStatChanged();
    void NaturalRecoveryTimerCallback();
    void HandleResourceStatChanged(ENPResourceStatType Type, float current, float max, int32 idx);

///////////////////////////////////////////
// 스테이트 관련 (전멸 판정)
    void HandleMemberStateChange(ENPCharacterState Flags, bool bValue, int32 idx);

///////////////////////////////////////////

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Party")
    FVector SwapLocationOffset = FVector(0.f, 200.f, 0.f);

private:
    /** 스폰된 파티 멤버들 */
    UPROPERTY()
    TArray<ANPBattlePlayerCharacter*> PartyMembers;

    UPROPERTY(Transient)
	TArray<class UNPCharacterStatComponent*> StatComponents;

    /** 현재 조종 중인 인덱스 (없으면 -1) */
    int32 CurrentIndex = INDEX_NONE;

    /** 리소스스탯 자연회복 관련*/
    FTimerHandle TimerHandle_NaturalRecovery;
    float NaturalRecoveryTickInterval = 0.1f;
};
