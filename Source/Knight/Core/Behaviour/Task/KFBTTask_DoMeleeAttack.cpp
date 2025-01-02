// Fill out your copyright notice in the Description page of Project Settings.

#include "KFBTTask_DoMeleeAttack.h"
#include "Knight/Core/Logger.h"
#include "Knight/Core/Character/Knight/KfMeleeAttackComponent.h"

UKFBTTask_DoMeleeAttack::UKFBTTask_DoMeleeAttack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NodeName = "Melee Attack";
	bTickIntervals = true;
	attackDirection = EAttackInputDirection::Normal;
}

EBTNodeResult::Type UKFBTTask_DoMeleeAttack::OnExecuteTask_Knight(UBehaviorTreeComponent& OwnerComp, AKfKnightAIPawn* KnightAIPawn, uint8* NodeMemory) {
	if (!KnightAIPawn) {
		UC_LOG_ERROR_MSG("KnightAIPawn is not found");
		return EBTNodeResult::Failed;
	}

	auto* melee = KnightAIPawn->GetMeleeAttackComponent();
	if (!melee) {
		UC_LOG_ERROR_MSG("Melee attack component in %s is not found", *KnightAIPawn->GetName());
		return EBTNodeResult::Failed;
	}

	float duration = 0;
	if (!melee->DoMeleeAttack_Directional(attackDirection, duration)) return EBTNodeResult::Failed;

	if (specificDuration.bIsEnabled) {
		duration = specificDuration.Value;
	}

	SetNextTickTime(NodeMemory, duration);
	return EBTNodeResult::InProgress;
}

void UKFBTTask_DoMeleeAttack::OnTickTask_Knight(UBehaviorTreeComponent& OwnerComp, AKfKnightAIPawn* KnightAIPawn, uint8* NodeMemory, float DeltaSeconds) {
	// Using the SetNextTickTime in ExecuteTask we are certain we are only getting ticked when the wait is finished
	ensure(GetSpecialNodeMemory<FBTTaskMemory>(NodeMemory)->NextTickRemainingTime <= 0.f);

	// continue execution from this node
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}