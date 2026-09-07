// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPCharacterActivityArea.generated.h"

UCLASS(Abstract)
class NEONPROJECT_API ANPCharacterActivityArea : public AActor
{
	GENERATED_BODY()

public:
	ANPCharacterActivityArea();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	/// @brief 자식 블루프린트에서 시각용 Static Mesh를 연결할 루트 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea")
	TObjectPtr<class USceneComponent> SceneComp;

	/// @brief 에디터에서 활동 구역을 확인하기 위한 Cube Static Mesh입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea")
	TObjectPtr<class UStaticMeshComponent> AreaMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea")
	TObjectPtr<class UBoxComponent> FloorCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea")
	TObjectPtr<class UBoxComponent> LeftWallCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea")
	TObjectPtr<class UBoxComponent> RightWallCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea")
	TObjectPtr<class UBoxComponent> FrontWallCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea")
	TObjectPtr<class UBoxComponent> BackWallCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea")
	TObjectPtr<class UBoxComponent> CeilingCollision;

	/// @brief 활동 구역의 전체 크기입니다. 액터 스케일은 (1, 1, 1)로 유지합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea", meta = (ClampMin = "1.0", AllowPrivateAccess = "true"))
	FVector AreaSize = FVector(1000.f, 1000.f, 500.f);

	/// @brief 바닥과 벽 콜리전의 고정 두께입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea", meta = (ClampMin = "1.0", AllowPrivateAccess = "true"))
	float CollisionThickness = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea", meta = (AllowPrivateAccess = "true"))
	bool bUseFloorCollision = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|CharacterActivityArea", meta = (AllowPrivateAccess = "true"))
	bool bUseCeilingCollision = false;

private:
	void UpdateCollisionLayout();
	void SetCollisionEnabled(class UBoxComponent* CollisionComponent, bool bEnabled) const;
};
