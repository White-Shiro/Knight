#include "KfKnightAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Knight/Core/Character/Knight/KfKnightAIPawn.h"

AKfKnightAIController::AKfKnightAIController() {
	PrimaryActorTick.bCanEverTick = true;
	bStartAILogicOnPossess = true;
	bStopAILogicOnUnposses = true;
}

void AKfKnightAIController::BeginPlay() {
	Super::BeginPlay();
}

void AKfKnightAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	if (bStartAILogicOnPossess) {
		if (const auto* knightAi = Cast<AKfKnightAIPawn>(InPawn)) {
			if (auto* tree = knightAi->GetBehaviorTree()) {
				RunBehaviorTree(tree);
			}
		}
	}
}

void AKfKnightAIController::OnUnPossess() {
	Super::OnUnPossess();
}

void AKfKnightAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	const auto* p = GetPawn();
	if (!p) return;
	auto* bb = GetBlackboardComponent();
	if (!bb) return;

	static const FName TargetLocKey(TEXT("TargetLoc"));
#if 0

	const auto origin = p->GetActorLocation();
	FNavLocation result;
	auto* w = GetWorld();
	if (const auto* sys = UNavigationSystemV1::GetCurrent(w)) {
		if (sys->GetRandomPointInNavigableRadius(origin, 10000.f, result)) {
			bb->SetValueAsVector(TargetLocKey, result.Location);
		}
	}
#else

	static const FName targetKey(TEXT("TargetActor"));

	const auto pc = GetWorld()->GetFirstPlayerController();
	AActor* target = nullptr;
	FVector targetLoc = FVector::ZeroVector;
	if (pc) {
		if (auto* pawn = pc->GetPawn()) {
			target = pawn;
			targetLoc = target->GetActorLocation();
		}
	}

	bb->SetValueAsVector(TargetLocKey, targetLoc);
	bb->SetValueAsObject(targetKey, target);
#endif

}