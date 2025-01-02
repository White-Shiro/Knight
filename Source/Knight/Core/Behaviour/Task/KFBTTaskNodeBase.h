#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Knight/Core/Agent/KfKnightAIController.h"
#include "Knight/Core/Character/Knight/KfKnightAIPawn.h"
#include "KFBTTaskNodeBase.generated.h"

UCLASS()
class KNIGHT_API UKFBTTaskNodeBase : public UBTTaskNode {
	GENERATED_BODY()
protected:
	UKFBTTaskNodeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/** Cached AIController owner of BehaviorTreeComponent. */
	UPROPERTY(Transient)
	TObjectPtr<AKfKnightAIController> knightAIOwner;

	/** Cached actor owner of BehaviorTreeComponent. */
	UPROPERTY(Transient)
	TObjectPtr<AKfKnightAIPawn> knightAIPawn;

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	virtual void OnTickTask_Knight(UBehaviorTreeComponent& OwnerComp, AKfKnightAIPawn* KnightAIPawn, uint8* NodeMemory, float DeltaSeconds);
	virtual EBTNodeResult::Type OnExecuteTask_Knight(UBehaviorTreeComponent& OwnerComp, AKfKnightAIPawn* KnightAIPawn, uint8* NodeMemory);
	virtual void OnTaskFinished_Knight(UBehaviorTreeComponent& OwnerComp, AKfKnightAIPawn* KnightAIPawn, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
};
