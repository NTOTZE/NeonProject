// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/NPANS_TimeDilation.h"

#include "Kismet/GameplayStatics.h"

void UNPANS_TimeDilation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	UWorld* World = MeshComp->GetWorld();
	if (!World) return;

	const float Applied = FMath::Clamp(TimeDilation, 0.01f, 1.0f);
	UGameplayStatics::SetGlobalTimeDilation(World, Applied);

	AActor* Actor = MeshComp->GetOwner();
	if (!Actor) return;
	Actor->CustomTimeDilation = 1.f / Applied;
}

void UNPANS_TimeDilation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	UWorld* World = MeshComp->GetWorld();
	if (!World) return;

	UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);

	AActor* Actor = MeshComp->GetOwner();
	if (!Actor) return;
	Actor->CustomTimeDilation = 1.f;
}
