// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/NPInteractionComponent.h"
#include "NeonProject.h"
#include "Interaction/NPInteractableActorBase.h"
#include "DataType/NPScreenTypes.h"
#include "Screen/NPScreenSubsystem.h"

// Sets default values for this component's properties
UNPInteractionComponent::UNPInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UNPInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UNPInteractionComponent::AddInteractableActor(ANPInteractableActorBase* interactableActor)
{
	InteractableActors.AddUnique(interactableActor);
	SetFocusIndex(FocusIdx);

	OnInteractableActorsChanged.Broadcast(InteractableActors);
}

void UNPInteractionComponent::RemoveInteractableActor(ANPInteractableActorBase* interactableActor)
{
	InteractableActors.RemoveSingle(interactableActor);
	interactableActor->SetInteractionFocused(false);

	//현재 FocusIndex보다 낮은 Index의 Actor가 목록에서 빠지면
	//FocusIdx를 1 줄이는 기능 추가해야함
	SetFocusIndex(FocusIdx);

	OnInteractableActorsChanged.Broadcast(InteractableActors);
}

void UNPInteractionComponent::ShiftFocus(int32 offset)
{
	SetFocusIndex(FocusIdx + offset);
}

bool UNPInteractionComponent::ExecuteFocusedInteraction(APlayerController* RequestingController)
{
	return ExecuteInteractionByIndex(FocusIdx, RequestingController);
}

bool UNPInteractionComponent::ExecuteInteractionByIndex(int32 index, APlayerController* RequestingController)
{
	if (!InteractableActors.IsValidIndex(index))
		return false;

	NP_LOG(NPLog, Warning, TEXT("FocusIndex : %d"), FocusIdx);

	return InteractionByActor(InteractableActors[index].Get(), RequestingController);
}

bool UNPInteractionComponent::InteractionByActor(ANPInteractableActorBase* interactableActor, APlayerController* RequestingController)
{
	if(!interactableActor)
		return false;
	if (!RequestingController)
		return false;

	NP_LOG(NPLog, Warning, TEXT("Interaction : %s, RequestingController : %s"), 
		*interactableActor->GetInteractionDisplayText().ToString(), 
		*RequestingController->GetClass()->GetName());


	switch (interactableActor->GetInteractionType())
	{
	case ENPInteractionType::BattleEntry:
	{
		UGameInstance* GameInstance = RequestingController->GetGameInstance();
		UNPScreenSubsystem* ScreenSubsystem = IsValid(GameInstance)
			? GameInstance->GetSubsystem<UNPScreenSubsystem>()
			: nullptr;
		if (!ScreenSubsystem)
		{
			NP_LOG(NPLog, Warning, TEXT("전투 선택 화면을 열 ScreenSubsystem이 설정되어 있지 않습니다."));
			return false;
		}

		return ScreenSubsystem->OpenScreen(ENPScreenType::BattleSelection);
	}
	default:
		return false;
	}
}

void UNPInteractionComponent::SetFocusIndex(int32 newIndex)
{
	const int32 endIndex = InteractableActors.Num() - 1;
	if (endIndex < 0)
	{
		newIndex = -1;
	}
	else
	{
		newIndex = FMath::Clamp(newIndex, 0, endIndex);
		if (!InteractableActors.IsValidIndex(newIndex))
			newIndex = -1;
	}
	
	for (int32 i = 0; i <= endIndex; ++i)
	{
		if(!InteractableActors.IsValidIndex(i))
			continue;

		const bool bFocus = (i == newIndex);
		InteractableActors[i].Get()->SetInteractionFocused(bFocus);
	}

	FocusIdx = newIndex;
}
