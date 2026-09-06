// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataType/NPInteractionTypes.h"
#include "NPInteractableActorBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionFocusChanged, bool);


UCLASS()
class NEONPROJECT_API ANPInteractableActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPInteractableActorBase();

	const FText& GetInteractionDisplayText() { return InteractionDisplayText; }
	ENPInteractionType GetInteractionType() const { return InteractionType; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void HandleTiggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void HandleTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void SetInteractionFocused(bool bNewFocused);
	bool GetInteractionFocused() { return bIsFocused; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Interaction")
	TObjectPtr<USceneComponent> SceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NP|Interaction")
	TObjectPtr<class UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Interaction")
	ENPInteractionType InteractionType = ENPInteractionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Interaction")
	FText InteractionDisplayText = FText::FromString(TEXT("상호작용"));
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DialogueId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Interaction")
	bool bInteractionEnabled = true;

	bool bIsFocused = false;

public:
	FOnInteractionFocusChanged OnInteractionFocusChanged;
};
