// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Utility/NPMacros.h"
#include "NPSkillCutsceneSubsystem.generated.h"

class ULevelSequence;
class ALevelSequenceActor;
class ULevelSequencePlayer;

class UUserWidget;
class ASceneCapture2D;
class UTextureRenderTarget2D;
class UCameraComponent;

UCLASS()
class NEONPROJECT_API UNPSkillCutsceneSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

    NP_DECLARE_WORLD_SUBSYSTEM_GETTER()

public:
    UNPSkillCutsceneSubsystem();

    UFUNCTION(BlueprintCallable, Category = "NP|Cutscene")
    void PlaySkillCutscene(ULevelSequence* SequenceAsset, AActor* Target, float restoreViewBlendTime = 0.f);

private:
    UFUNCTION()
    void OnFinished();

    // 씬캡쳐2D 함수
    void CreateCaptureAndOverlay(UWorld* World);
    void DestroyCaptureAndOverlay();
    void BuildShowOnlyFromSequence(ALevelSequenceActor* SequenceActor);
    void SyncCaptureToCurrentCamera();

    void StartFollowing() { bFollowing = true; }
    void StopFollowing() { bFollowing = false; }

    UFUNCTION()
    void OnSequencePlay();
    UFUNCTION()
    void OnSequenceCameraCut(UCameraComponent* CameraComponent);

    // Tick 설정
    virtual bool IsTickable() const override { return bFollowing; }
    virtual bool IsTickableWhenPaused() const override { return true; }
    virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(UNPSkillCutsceneSubsystem, STATGROUP_Tickables);
    }

private:
    // 레벨 시퀀스 구성 요소
    TWeakObjectPtr<ULevelSequencePlayer> Player;
    TWeakObjectPtr<ALevelSequenceActor>  SeqActor;
    TWeakObjectPtr<AActor> CachedViewTarget;
    float RestoreViewBlendTime;
    FName TargetBindingTag = TEXT("PlayerCharacter");

    // 씬캡쳐2D 구성요소
    UPROPERTY() TSubclassOf<UUserWidget> OverlayWidgetClass;

    FName OverlayImageWidgetName = TEXT("OverlayImage");

    TWeakObjectPtr<ASceneCapture2D> CutsceneCapture;
    TWeakObjectPtr<UTextureRenderTarget2D> CutsceneRT;
    TWeakObjectPtr<UUserWidget> CutsceneOverlay;

    UPROPERTY() TObjectPtr<UMaterialInterface> OverlayUIMaterialBase = nullptr;
    UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> OverlayMID = nullptr;

    FVector  LastLoc = FVector::ZeroVector;
    FRotator LastRot = FRotator::ZeroRotator;
    float    LastFOV = -1.f;

    // 틱 설정
    bool bFollowing = false;
};
