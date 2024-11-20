#include "KfKnightAIPawn.h"

#include "KfCharacterAnimInstance.h"
#include "Knight/Core/Agent/KfKnightAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/CapsuleComponent.h"

AKfKnightAIPawn::AKfKnightAIPawn(const FObjectInitializer& initializer) : Super(initializer){
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AKfKnightAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	PathFollowingComponent = CreateDefaultSubobject<UPathFollowingComponent>(UPathFollowingComponent::StaticClass()->GetFName(), true);
	BehaviorTree = CreateDefaultSubobject<UBehaviorTreeComponent>(UBehaviorTreeComponent::StaticClass()->GetFName(), true);

	auto* capsule = GetCapsuleComponent();
	capsule->InitCapsuleSize(200.0f, 900.0f);
	static FName msCollisionPresetName(TEXT("Vehicle"));
	capsule->SetCollisionProfileName(msCollisionPresetName);
}

void AKfKnightAIPawn::BeginPlay() {
	Super::BeginPlay();
	_animInstance = Cast<UKfCharacterAnimInstance>(GetMesh()->GetAnimInstance());
}
