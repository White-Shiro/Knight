#include "KfKnightAIController.h"

AKfKnightAIController::AKfKnightAIController() {
	PrimaryActorTick.bCanEverTick = true;
}

void AKfKnightAIController::BeginPlay() {
	Super::BeginPlay();
}

void AKfKnightAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}