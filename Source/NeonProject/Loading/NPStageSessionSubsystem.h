#pragma once

#include "CoreMinimal.h"
#include "DataType/NPStageData.h"
#include "Subsystems/WorldSubsystem.h"
#include "Utility/NPMacros.h"
#include "NPStageSessionSubsystem.generated.h"

struct FStreamableHandle;

DECLARE_MULTICAST_DELEGATE(FNPStageSessionReadyDelegate);

USTRUCT(BlueprintType)
struct FNPStageSessionData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ENPStageType StageType = ENPStageType::None;

	UPROPERTY(BlueprintReadOnly)
	FName StageId;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> PartyCharacterIds;

	UPROPERTY(BlueprintReadOnly)
	TArray<FSoftObjectPath> AdditionalAssets;
};

// 스테이지 진입 데이터와 로드 완료 에셋 핸들을 월드 수명 동안 보관
UCLASS()
class NEONPROJECT_API UNPStageSessionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	NP_DECLARE_WORLD_SUBSYSTEM_GETTER()

public:
	void InitializeSession(FNPStageSessionData&& InSessionData, TArray<TSharedPtr<FStreamableHandle>>&& InAssetHandles);
	const FNPStageSessionData& GetSessionData() const { return SessionData; }
	bool IsSessionInitialized() const { return bSessionInitialized; }
	FNPStageSessionReadyDelegate OnSessionReady;

	virtual void Deinitialize() override;

private:
	FNPStageSessionData SessionData;
	TArray<TSharedPtr<FStreamableHandle>> AssetHandles;
	bool bSessionInitialized = false;
};
