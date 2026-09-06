#pragma once

#include "CoreMinimal.h"
#include "NPScreenTypes.generated.h"

UENUM(BlueprintType)
enum class ENPFadeAnimationType : uint8
{
	FadeIn,
	FadeOut,
};

UENUM(BlueprintType)
enum class ENPScreenType : uint8
{
	None,

	// Common
	Settings,

	// Hub
	Dialogue,
	Inventory,
	BattleSelection,

	// Battle
	PartySelect,
	BattleResult,

	Count	       UMETA(Hidden),
};

/** 스크린을 열 때 적용할 옵션들 */
USTRUCT(BlueprintType)
struct FNPScreenOptions
{
	GENERATED_BODY()

	/// @brief 스크린 열 때 적용할 옵션들
	/// 
	/// 모든 옵션의 기본값은 TRUE 입니다
	FNPScreenOptions() = default;

	/// @brief 스크린 열 때 적용할 옵션들
	/// @param pauseGame : 게임 일시정지 여부
	/// @param showMouseCursor : 마우스커서 노출 여부
	/// @param closeOnEscape : ESC로 창 닫기 가능 여부
	FNPScreenOptions(bool pauseGame, bool showMouseCursor, bool closeOnEscape)
		: bPauseGame(pauseGame)
		, bShowMouseCursor(showMouseCursor)
		, bCloseOnEscape(closeOnEscape)
	{
	}
	/// @brief 스크린 열 때 적용할 옵션들
	/// @param bValue : 값이 모든 옵션에 일괄 적용됩니다
	FNPScreenOptions(bool bValue)
		: bPauseGame(bValue)
		, bShowMouseCursor(bValue)
		, bCloseOnEscape(bValue)
	{
	}

	FNPScreenOptions& SetCloseOptions()
	{
		this->bPauseGame = false;
		this->bShowMouseCursor = false;
		this->bCloseOnEscape = false;
		return *this;
	}

	static FNPScreenOptions MakeCloseOptions()
	{
		FNPScreenOptions CloseOptions;
		return CloseOptions.SetCloseOptions();
	}
	

	/** 스크린이 열려 있는 동안 게임 일시정지 여부 */
	bool bPauseGame = true;
	/** 스크린이 열려 있는 동안 마우스커서 노출 여부 */
	bool bShowMouseCursor = true;
	/** 키보드 'ESC'키 입력으로 스크린 닫기 가능 여부 */
	bool bCloseOnEscape = true;
};