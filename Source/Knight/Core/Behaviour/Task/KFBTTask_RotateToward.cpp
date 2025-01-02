// Fill out your copyright notice in the Description page of Project Settings.

#include "KFBTTask_RotateToward.h"
#include "Knight/Core/Core.h"

UKFBTTask_RotateToward::UKFBTTask_RotateToward(FObjectInitializer const& ObjectInitializer) {
	NodeName = "RotateToward";
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UKFBTTask_RotateToward::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	UC_MSG("UUKFBTTask_RotateToward::ExecuteTask");
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UKFBTTask_RotateToward::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	UC_MSG("UUKFBTTask_RotateToward::Aborted");
	return EBTNodeResult::Aborted;
}

void UKFBTTask_RotateToward::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	UC_MSG("UUKFBTTask_RotateToward::Tick");
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}