// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/Cutscene/Element/NPLSProxyMesh.h"

// Sets default values
ANPLSProxyMesh::ANPLSProxyMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

	SetRootComponent(SceneComp);
	SkeletalMeshComp->SetupAttachment(SceneComp);
	SkeletalMeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	SkeletalMeshComp->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponMesh->SetupAttachment(SkeletalMeshComp, TEXT("WeaponSocket"));
	WeaponMesh->SetGenerateOverlapEvents(false);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

	PrimaryActorTick.bTickEvenWhenPaused = true;
	SkeletalMeshComp->PrimaryComponentTick.bTickEvenWhenPaused = true;
}

// Called when the game starts or when spawned
void ANPLSProxyMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPLSProxyMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

