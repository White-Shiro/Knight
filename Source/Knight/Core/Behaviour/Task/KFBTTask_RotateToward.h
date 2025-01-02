#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "KFBTTask_RotateToward.generated.h"

UCLASS()
class KNIGHT_API UKFBTTask_RotateToward : public UBTTaskNode {
	GENERATED_BODY()
	UKFBTTask_RotateToward(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};