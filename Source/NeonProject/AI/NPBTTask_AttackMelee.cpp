// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NPBTTask_AttackMelee.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UNPBTTask_AttackMelee::UNPBTTask_AttackMelee()
{
	NodeName = TEXT("NP Attack Melee");
	bNotifyTick = true;
}


EBTNodeResult::Type UNPBTTask_AttackMelee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;
	
	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(AIController->GetWorld(), 0);
	if (!PlayerCharacter) return EBTNodeResult::Failed;

	AIController->StopMovement();
	const FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(AIPawn->GetActorLocation(), PlayerCharacter->GetActorLocation());
	AIPawn->SetActorRotation(FRotator(0.f, LookRot.Yaw, 0.f));

	if (ACharacter* AICharacter = Cast<ACharacter>(AIPawn))
	{
		if (AttackMontage)
		{
			AIController->StopMovement();
			AICharacter->PlayAnimMontage(AttackMontage);

			FOnMontageEnded MontageEndDelegate;
			TWeakObjectPtr<UNPBTTask_AttackMelee> WeakTask(this);
			TWeakObjectPtr<UBehaviorTreeComponent> WeakOwnerComp(&OwnerComp);
			MontageEndDelegate.BindLambda([WeakTask, WeakOwnerComp](UAnimMontage* Montage, bool bInterrupted)
				{
					if (WeakTask.IsValid() && WeakOwnerComp.IsValid())
					{
						WeakTask.Get()->FinishLatentTask(*WeakOwnerComp.Get(), EBTNodeResult::Succeeded);
					}
				}
			);
			AICharacter->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, AttackMontage);

			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

void UNPBTTask_AttackMelee::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return;

	if (ACharacter* Character = Cast<ACharacter>(AIPawn))
	{
		FOnMontageEnded ClearDelegate;
		Character->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(ClearDelegate, AttackMontage);
	}
}
