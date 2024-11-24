#include "KfKnightAIPawn.h"
#include "KfCharacterAnimInstance.h"
#include "KfKnightMovementComponent.h"
#include "Knight/Core/Agent/KfKnightAIController.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Knight/Core/Logger.h"

AKfKnightAIPawn::AKfKnightAIPawn(const FObjectInitializer& initializer) : Super(initializer.SetDefaultSubobjectClass(CharacterMovementComponentName, UKfKnightMovementComponent::StaticClass())) {
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AKfKnightAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetArrowComponent()->SetArrowSize(5.f);
	auto* capsule = GetCapsuleComponent();

	constexpr float HALF_HEIGHT = 900.0f;
	capsule->InitCapsuleSize(200.0f, HALF_HEIGHT);

	if (USkeletalMeshComponent* mesh = GetMesh()) {
		mesh->SetRelativeLocation(FVector(0.f, 0.f, -HALF_HEIGHT));
	}

	static FName msCollisionPresetName(TEXT("Vehicle"));
	capsule->SetCollisionProfileName(msCollisionPresetName);
}

void AKfKnightAIPawn::BeginPlay() {
	Super::BeginPlay();
	_animInstance = Cast<UKfCharacterAnimInstance>(GetMesh()->GetAnimInstance());
}

void AKfKnightAIPawn::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	ConsumeMovementInput();
}

void AKfKnightAIPawn::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) {
	const auto localDir = ActorToWorld().InverseTransformVectorNoScale(WorldDirection).GetSafeNormal2D();
	const FVector2d localDir2d(localDir.Y, localDir.X);
	_lastMoveInput += localDir2d;
}

void AKfKnightAIPawn::ConsumeMovementInput() {
	if (_animInstance) _animInstance->SetMovementInput(_lastMoveInput);
	_lastMoveInput = FVector2D::ZeroVector;
}