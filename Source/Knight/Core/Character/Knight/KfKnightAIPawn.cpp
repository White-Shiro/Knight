#include "KfKnightAIPawn.h"
#include "Knight/Core/Agent/KfKnightAIController.h"

AKfKnightAIPawn::AKfKnightAIPawn(const FObjectInitializer& initializer) {
	PrimaryActorTick.bCanEverTick = true;
	//AIControllerClass = AKfKnightAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}