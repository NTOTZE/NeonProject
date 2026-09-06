// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/NPInteractableActorBase.h"
#include "NeonProject.h"
#include "Core/Controller/NPHubPlayerController.h"
#include "Dialogue/NPDialogueSubsystem.h"
#include "DataAsset/NPDialogueDataAsset.h"
#include "Component/NPInteractionComponent.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

// Sets default values
ANPInteractableActorBase::ANPInteractableActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(SceneComp);
	TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	TriggerBox->SetCollisionProfileName(FName("NP_PlayerHit"));
	TriggerBox->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ANPInteractableActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ANPInteractableActorBase::HandleTiggerBeginOverlap);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ANPInteractableActorBase::HandleTriggerEndOverlap);
	}
}

void ANPInteractableActorBase::SetInteractionFocused(bool bNewFocused)
{
	if (bIsFocused == bNewFocused)
		return;

	bIsFocused = bNewFocused;
	OnInteractionFocusChanged.Broadcast(bIsFocused);
}

void ANPInteractableActorBase::HandleTiggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UNPInteractionComponent* InteractionComp = OtherActor->GetInstigatorController()->FindComponentByClass<UNPInteractionComponent>();
	if (!InteractionComp) return;
	InteractionComp->AddInteractableActor(this);

	NP_LOG(NPLog, Warning, TEXT(""));
}

void ANPInteractableActorBase::HandleTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn) return;
	AController* Controller = Pawn->GetController();
	if (!Controller) return;

	UNPInteractionComponent* InteractionComp = Controller->FindComponentByClass<UNPInteractionComponent>();
	if (!InteractionComp) return;
	InteractionComp->RemoveInteractableActor(this);

	NP_LOG(NPLog, Warning, TEXT(""));
}

