// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataType/NPStageData.h"
#include "Screen/Widget/NPScreenWidgetBase.h"
#include "NPBattleSelectionWidget.generated.h"

UCLASS(BlueprintType)
class NEONPROJECT_API UNPBattleSelectionItem : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnClicked, UObject*);

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	FName BattleStageId;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	FText StageName;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	int32 Difficulty = 0;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	TSoftObjectPtr<UTexture2D> ThumbnailTexture;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	bool bIsSelected = false;

	FOnClicked OnClicked;
};

UCLASS(BlueprintType)
class NEONPROJECT_API UNPBattleSelectionCharacterItem : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnClicked, UObject*);
	DECLARE_MULTICAST_DELEGATE(FOnSelectionStateChanged);

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	FName CharacterId;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	FText CharacterName;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	TSoftObjectPtr<UTexture2D> ThumbnailTexture;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	bool bIsSelected = false;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	bool bIsSelectionLocked = false;

	UPROPERTY(BlueprintReadOnly, Category = "NP|Battle")
	int32 SelectionOrder = 0;

	FOnClicked OnClicked;
	FOnSelectionStateChanged OnSelectionStateChanged;
};

UCLASS()
class NEONPROJECT_API UNPBattleSelectionWidget : public UNPScreenWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void OnBattleItemClicked(UObject* BattleItem);
	void OnCharacterItemClicked(UObject* CharacterItem);
	void SetSelectedBattleItem(UNPBattleSelectionItem* BattleItem);
	void UpdateCharacterList();
	void UpdateCharacterSelectionState();
	void UpdateEntryButtonState();
	TArray<FName> GetPartyCharacterIds() const;
	void StartFade(float TargetOpacity, bool bCloseWhenFinished);
	void BeginCloseScreen();
	void RequestEnterBattle(const FName& BattleStageId);

	UFUNCTION()
	void OnEntryButtonClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "NP|Battle")
	void BP_OnBattleStageSelected(UNPBattleSelectionItem* BattleItem);

	UFUNCTION(BlueprintImplementableEvent, Category = "NP|Battle")
	void BP_OnCharacterSelectionChanged(const TArray<FName>& CharacterIds);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UListView> BattleList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UListView> CharacterList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> EntryButton;

	UPROPERTY(Transient)
	TObjectPtr<UNPBattleSelectionItem> SelectedBattleItem;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UNPBattleSelectionItem>> BattleItems;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UNPBattleSelectionCharacterItem>> CharacterItems;

	UPROPERTY(Transient)
	TArray<FName> SelectedCharacterSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NP|Battle", meta = (ClampMin = "1", AllowPrivateAccess = "true"))
	int32 MaxPartyMemberCount = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NP|Battle", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float FadeDuration = 0.2f;

	float FadeElapsedTime = 0.f;
	float FadeStartOpacity = 1.f;
	float FadeTargetOpacity = 1.f;
	bool bFadeActive = false;
	bool bCloseAfterFade = false;
};
