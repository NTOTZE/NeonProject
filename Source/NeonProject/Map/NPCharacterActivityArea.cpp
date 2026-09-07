// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/NPCharacterActivityArea.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "UObject/ConstructorHelpers.h"

ANPCharacterActivityArea::ANPCharacterActivityArea()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	AreaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AreaMesh"));
	AreaMesh->SetupAttachment(SceneComp);
	AreaMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaMesh->SetHiddenInGame(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		AreaMesh->SetStaticMesh(CubeMesh.Object);
	}

	FloorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FloorCollision"));
	FloorCollision->SetupAttachment(SceneComp);

	LeftWallCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWallCollision"));
	LeftWallCollision->SetupAttachment(SceneComp);

	RightWallCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWallCollision"));
	RightWallCollision->SetupAttachment(SceneComp);

	FrontWallCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FrontWallCollision"));
	FrontWallCollision->SetupAttachment(SceneComp);

	BackWallCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BackWallCollision"));
	BackWallCollision->SetupAttachment(SceneComp);

	CeilingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CeilingCollision"));
	CeilingCollision->SetupAttachment(SceneComp);

	for (UBoxComponent* CollisionComponent : { FloorCollision, LeftWallCollision, RightWallCollision, FrontWallCollision, BackWallCollision, CeilingCollision })
	{
		CollisionComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		CollisionComponent->SetGenerateOverlapEvents(false);
	}

	UpdateCollisionLayout();
}

void ANPCharacterActivityArea::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateCollisionLayout();
}

void ANPCharacterActivityArea::UpdateCollisionLayout()
{
	const FVector HalfAreaSize(
		FMath::Max(AreaSize.X * 0.5f, 0.5f),
		FMath::Max(AreaSize.Y * 0.5f, 0.5f),
		FMath::Max(AreaSize.Z * 0.5f, 0.5f));
	const float HalfThickness = FMath::Min(CollisionThickness * 0.5f, HalfAreaSize.GetMin());
	AreaMesh->SetRelativeScale3D(AreaSize / 100.f);

	FloorCollision->SetBoxExtent(FVector(HalfAreaSize.X, HalfAreaSize.Y, HalfThickness), false);
	FloorCollision->SetRelativeLocation(FVector(0.f, 0.f, -HalfAreaSize.Z + HalfThickness));
	SetCollisionEnabled(FloorCollision, bUseFloorCollision);

	LeftWallCollision->SetBoxExtent(FVector(HalfThickness, HalfAreaSize.Y, HalfAreaSize.Z), false);
	LeftWallCollision->SetRelativeLocation(FVector(-HalfAreaSize.X + HalfThickness, 0.f, 0.f));

	RightWallCollision->SetBoxExtent(FVector(HalfThickness, HalfAreaSize.Y, HalfAreaSize.Z), false);
	RightWallCollision->SetRelativeLocation(FVector(HalfAreaSize.X - HalfThickness, 0.f, 0.f));

	FrontWallCollision->SetBoxExtent(FVector(HalfAreaSize.X, HalfThickness, HalfAreaSize.Z), false);
	FrontWallCollision->SetRelativeLocation(FVector(0.f, -HalfAreaSize.Y + HalfThickness, 0.f));

	BackWallCollision->SetBoxExtent(FVector(HalfAreaSize.X, HalfThickness, HalfAreaSize.Z), false);
	BackWallCollision->SetRelativeLocation(FVector(0.f, HalfAreaSize.Y - HalfThickness, 0.f));

	CeilingCollision->SetBoxExtent(FVector(HalfAreaSize.X, HalfAreaSize.Y, HalfThickness), false);
	CeilingCollision->SetRelativeLocation(FVector(0.f, 0.f, HalfAreaSize.Z - HalfThickness));
	SetCollisionEnabled(CeilingCollision, bUseCeilingCollision);
}

void ANPCharacterActivityArea::SetCollisionEnabled(UBoxComponent* CollisionComponent, bool bEnabled) const
{
	CollisionComponent->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}
