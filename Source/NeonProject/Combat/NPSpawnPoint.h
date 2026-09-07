// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPSpawnPoint.generated.h"

UCLASS()
class NEONPROJECT_API ANPSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ANPSpawnPoint();

	UFUNCTION(BlueprintCallable, Category = "NP|Spawn")
	FTransform GetSpawnTransform() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NP|Spawn")
	TObjectPtr<class USceneComponent> SceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NP|Spawn")
	TObjectPtr<class UArrowComponent> ArrowComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Spawn")
	float RandomRadius = 0.f;
};
