// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPCameraRig.generated.h"

UCLASS()
class NEONPROJECT_API ANPCameraRig : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPCameraRig();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetFollowTarget(AActor* NewTarget);
	void SetLockOnTarget(AActor* NewLockOnTarget);
	AActor* GetLockOnTarget() { return LockOnTarget.IsValid() ? LockOnTarget.Get() : nullptr; }

private:
	UPROPERTY()
	TObjectPtr<class USceneComponent> SceneComp;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArmComp;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> CameraComp;

	UPROPERTY()
	TWeakObjectPtr<AActor> FollowTarget;

	UPROPERTY()
	TWeakObjectPtr<AActor> LockOnTarget;
};
