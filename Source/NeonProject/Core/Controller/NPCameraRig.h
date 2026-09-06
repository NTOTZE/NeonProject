// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/NPInputCommandReceiver.h"
#include "NPCameraRig.generated.h"

UCLASS()
class NEONPROJECT_API ANPCameraRig : public AActor, public INPInputCommandReceiver
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

	virtual bool HandleInputCommand(class AController* InstigatorController, FNPInputCommand Command) override;

	void SetFollowTarget(ACharacter* NewTarget);
	void SetLockOnTarget(AActor* NewLockOnTarget);
	AActor* GetLockOnTarget() { return LockOnTarget.IsValid() ? LockOnTarget.Get() : nullptr; }

public:
	AActor* SearchNearTargetWithinCircle(ECollisionChannel TraceChannel, float Radius);
	AActor* SearchNearTargetWithinSector(ECollisionChannel TraceChannel, float Radius, const FVector& Direction, float AngleDeg);


public:
	UFUNCTION(BlueprintCallable) class UCameraComponent* GetCameraComponent() { return CameraComp; }
	
	UFUNCTION(BlueprintCallable) class USpringArmComponent* GetSpringArmComponent() { return SpringArmComp; }

private:
	UPROPERTY()
	TObjectPtr<class USceneComponent> SceneComp;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArmComp;

	UPROPERTY(EditDefaultsOnly, Category = "NP|Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> CameraComp;

	UPROPERTY()
	TWeakObjectPtr<ACharacter> FollowTarget;

	UPROPERTY()
	TWeakObjectPtr<AActor> LockOnTarget;
};
