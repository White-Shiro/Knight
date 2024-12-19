// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight/Core/Core.h"
#include "UKFBTTask_RotateToward.h"

UUKFBTTask_RotateToward::UUKFBTTask_RotateToward(FObjectInitializer const& ObjectInitializer) {
	NodeName = "RotateToward";
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UUKFBTTask_RotateToward::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	UC_MSG("UUKFBTTask_RotateToward::ExecuteTask");
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UUKFBTTask_RotateToward::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	UC_MSG("UUKFBTTask_RotateToward::Aborted");
	return EBTNodeResult::Aborted;
}

void UUKFBTTask_RotateToward::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	UC_MSG("UUKFBTTask_RotateToward::Tick");
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}