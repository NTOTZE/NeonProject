// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPLSBackground.generated.h"

UCLASS()
class NEONPROJECT_API ANPLSBackground : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPLSBackground();


// 함수
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "NP|Cinematic")
	void InitOverlay();

private:
	void ApplyParams();

#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

// 프로퍼티
private:
	UPROPERTY(VisibleAnywhere, Category = "NP|Cinematic")
	TObjectPtr<class UPostProcessComponent> PostProcessComp;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> OverlayMID;

	// === 에디터에서 지정할 자원들 ===
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Cinematic", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> OverlayPPMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NP|Cinematic", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTexture> BackgroundTex;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "NP|Cinematic", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float Opacity01 = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "NP|Cinematic", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float OverlayWeight = 1.0f;
};
