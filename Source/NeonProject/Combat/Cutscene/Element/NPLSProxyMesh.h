// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPLSProxyMesh.generated.h"

UCLASS()
class NEONPROJECT_API ANPLSProxyMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPLSProxyMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	class USkeletalMeshComponent* GetMesh() { return SkeletalMeshComp; }

private:
	UPROPERTY(EditAnywhere, Category = "NP", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USceneComponent> SceneComp;

	UPROPERTY(EditAnywhere, Category = "NP", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> SkeletalMeshComp;

	UPROPERTY(EditAnywhere, Category = "NP", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;
};
