#pragma once

#include "Kismet/GameplayStatics.h"

/** GameInstanceSubsystem Getter함수 구현 매크로 */
#define NP_DECLARE_GAMEINSTANCE_SUBSYSTEM_GETTER() \
	public: \
		static ThisClass* GetChecked(const UObject* WorldContextObject) \
		{ \
			static_assert(std::is_base_of_v<UGameInstanceSubsystem, ThisClass>, \
				"이 매크로는 UGameInstanceSubsystem를 상속한 클래스에서만 사용할 수 있습니다."); \
			check(IsValid(WorldContextObject)); \
			\
			UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject); \
			check(IsValid(GameInstance)); \
			\
			ThisClass* Subsystem = GameInstance->GetSubsystem<ThisClass>(); \
			check(Subsystem); \
			\
			return Subsystem; \
		} \
	private:

/** WorldSubsystem Getter함수 구현 매크로 */
#define NP_DECLARE_WORLD_SUBSYSTEM_GETTER() \
	public: \
		static ThisClass* GetChecked(const UObject* WorldContextObject) \
		{ \
			static_assert(std::is_base_of_v<UWorldSubsystem, ThisClass>, \
				"이 매크로는 UWorldSubsystem를 상속한 클래스에서만 사용할 수 있습니다."); \
			check(IsValid(WorldContextObject)); \
			\
			UWorld* World = WorldContextObject->GetWorld(); \
			check(IsValid(World));	\
			\
			ThisClass* Subsystem = World->GetSubsystem<ThisClass>(); \
			check(IsValid(Subsystem)); \
			\
			return Subsystem; \
		} \
	private:

/** DeveloperSettings 필수 구현 함수 매크로 */
#define NP_DECLARE_SETTINGS() \
	public: \
		static const ThisClass* GetChecked() \
		{ \
			static_assert(std::is_base_of_v<UDeveloperSettings, ThisClass>, \
				"이 매크로는 UDeveloperSettings를 상속한 클래스에서만 사용할 수 있습니다."); \
			const ThisClass* DefaultClass = GetDefault<ThisClass>(); \
			check(DefaultClass); \
			return DefaultClass; \
		} \
		virtual FName GetCategoryName() const override { return TEXT("NeonProject"); } \
	private: