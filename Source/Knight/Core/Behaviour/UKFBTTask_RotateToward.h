// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UKFBTTask_RotateToward.generated.h"

/**
 *
 */
UCLASS()
class KNIGHT_API UUKFBTTask_RotateToward : public UBTTaskNode {
	UUKFBTTask_RotateToward(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	GENERATED_BODY()
};