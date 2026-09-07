// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/NPSpawnPoint.h"
#include "Components/ArrowComponent.h" 
#include "NavigationSystem.h"
// Sets default values
ANPSpawnPoint::ANPSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(SceneComp);
}

FTransform ANPSpawnPoint::GetSpawnTransform() const
{
	FVector Loc = GetActorLocation();
	const FRotator Rot = GetActorRotation();

	if (RandomRadius > 0.f)
	{
		// 랜덤 오프셋
		const float Angle = FMath::FRandRange(0.f, 2.f * PI);
		const float Radius = FMath::FRandRange(0.f, RandomRadius);
		Loc += FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.f);
	}

	return FTransform(Rot, Loc, FVector::OneVector);
}

