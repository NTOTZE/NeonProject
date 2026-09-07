// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/NPCharacterBase.h"
#include "InputAction.h"

#include "DataType/NPCombatTypes.h"
#include "Interface/NPInputCommandReceiver.h"

#include "NPPlayerCharacterBase.generated.h"


struct FNPInputCommand;

UCLASS()
class NEONPROJECT_API ANPPlayerCharacterBase : public ANPCharacterBase, public INPInputCommandReceiver
{
	GENERATED_BODY()

public:
	ANPPlayerCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual void OnMoveForwardInput(FVector ForwardVector, float ForwardInput);
	virtual void OnMoveRightInput(FVector RightVector, float RightInput);
	virtual void OnMoveStop();

	void HandleStateChange(ENPCharacterState Flags, bool Value);

public:
	UFUNCTION(BlueprintCallable)
	void NP_SeqEventTest(float BlendTime = 0.3f);

public:
	virtual bool HandleInputCommand(class AController* InstigatorController, FNPInputCommand Command) override;


public:
	class ULevelSequence* GetLevelSequence() { return LevelSeq; }
	void SetTarget(AActor* Target) { RecentTarget = Target; }
	virtual AActor* GetTarget() override { return RecentTarget.IsValid() ? RecentTarget.Get() : nullptr; }


public:		//	UI
	TSoftObjectPtr<class UTexture2D> GetCharacterSoftTexture() const { return CharacterImage; }
	void SetCharacterSoftTexture(const TSoftObjectPtr<class UTexture2D>& InCharacterImage) { CharacterImage = InCharacterImage; }
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "NP|Stats", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<class UTexture2D> CharacterImage;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "NP|Player", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 240.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "NP|Player", meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 630.f;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Player|Seq", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ULevelSequence> LevelSeq;

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> RecentTarget = nullptr;

	FVector2D MoveInput;
};
