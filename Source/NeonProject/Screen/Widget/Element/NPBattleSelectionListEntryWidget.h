// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "NPBattleSelectionListEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEONPROJECT_API UNPBattleSelectionListEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;

private:
	//TWeakObjectPtr<class 전투 구성 엑터> Cached전투구성엑터;

};
