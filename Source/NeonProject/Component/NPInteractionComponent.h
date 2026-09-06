// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NPInteractionComponent.generated.h"

class ANPInteractableActorBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractableActorsChanged, const TArray<TWeakObjectPtr<ANPInteractableActorBase>>&);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEONPROJECT_API UNPInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNPInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void AddInteractableActor(ANPInteractableActorBase* interactableActor);
	void RemoveInteractableActor(ANPInteractableActorBase* interactableActor);
	void ShiftFocus(int32 offset);

	int32 NumInteractableActor() { return InteractableActors.Num(); }
	bool ExecuteFocusedInteraction(APlayerController* RequestingController);

private:
	bool ExecuteInteractionByIndex(int32 index, APlayerController* RequestingController);
	bool InteractionByActor(ANPInteractableActorBase* interactableActor, APlayerController* RequestingController);
	void SetFocusIndex(int32 newIndex);

public:
	FOnInteractableActorsChanged OnInteractableActorsChanged;

private:
	TArray<TWeakObjectPtr<ANPInteractableActorBase>> InteractableActors;
	int32 FocusIdx = -1;
};
