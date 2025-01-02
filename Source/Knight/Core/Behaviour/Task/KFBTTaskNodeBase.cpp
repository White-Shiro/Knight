#include "KFBTTaskNodeBase.h"

UKFBTTaskNodeBase::UKFBTTaskNodeBase(const FObjectInitializer& ObjectInitializer) {
	bCreateNodeInstance = true;
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UKFBTTaskNodeBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	if (!knightAIOwner) {
		knightAIOwner = Cast<AKfKnightAIController>(OwnerComp.GetAIOwner());
	}

	if (!knightAIPawn) {
		auto* ctrl = knightAIOwner ? knightAIOwner : OwnerComp.GetAIOwner();
		knightAIPawn = ctrl ? ctrl->GetPawn<AKfKnightAIPawn>() : nullptr;
		if (!knightAIPawn) {
			return EBTNodeResult::Failed;
		}
	}

	return OnExecuteTask_Knight(OwnerComp, knightAIPawn, NodeMemory);
}

void UKFBTTaskNodeBase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	OnTickTask_Knight(OwnerComp, knightAIPawn, NodeMemory, DeltaSeconds);
}

void UKFBTTaskNodeBase::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) {
	OnTaskFinished_Knight(OwnerComp, knightAIPawn, NodeMemory, TaskResult);
}

EBTNodeResult::Type UKFBTTaskNodeBase::OnExecuteTask_Knight(UBehaviorTreeComponent& OwnerComp, AKfKnightAIPawn* KnightAIPawn, uint8* NodeMemory) {
	return EBTNodeResult::Succeeded;
}

void UKFBTTaskNodeBase::OnTickTask_Knight(UBehaviorTreeComponent& OwnerComp, AKfKnightAIPawn* KnightAIPawn, uint8* NodeMemory, float DeltaSeconds) {}

void UKFBTTaskNodeBase::OnTaskFinished_Knight(UBehaviorTreeComponent& OwnerComp, AKfKnightAIPawn* KnightAIPawn, uint8* NodeMemory, EBTNodeResult::Type TaskResult) {}