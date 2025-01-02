#include "KfKnightAIPawn.h"
#include "KfCharacterAnimInstance.h"
#include "KfKnightMovementComponent.h"
#include "KfMeleeAttackComponent.h"
#include "Knight/Core/Agent/KfKnightAIController.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Knight/Core/Logger.h"
#include "Knight/Core/Utils/KfGameMath.h"

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

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if ((knightMovement = Cast<UKfKnightMovementComponent>(GetCharacterMovement()))) {
		knightMovement->bOrientRotationToMovement = false;
		knightMovement->bUseControllerDesiredRotation = true;
		knightMovement->RotationRate = FRotator(0.0f, 225.0f, 0.0f);
	}

	MeleeAttackComponent = CreateDefaultSubobject<UKfMeleeAttackComponent>(UKfMeleeAttackComponent::StaticClass()->GetFName());
	MeleeAttackComponent->SetHitBoxRadius(130.f);
	MeleeAttackComponent->SetHitBoxLength(1500.f);
}

void AKfKnightAIPawn::BeginPlay() {
	Super::BeginPlay();
	_animInstance = Cast<UKfCharacterAnimInstance>(GetMesh()->GetAnimInstance());
}

void AKfKnightAIPawn::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	ConsumeMovementInput();
	LookAtTarget(DeltaSeconds);
}

void AKfKnightAIPawn::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) {
	const auto localDir = ActorToWorld().InverseTransformVectorNoScale(WorldDirection).GetSafeNormal2D();
	const FVector2f localDir2d(localDir.Y, localDir.X);
	_lastMoveInput += localDir2d;
}

FAttackResult AKfKnightAIPawn::ReactToAttack(const FAttackRequest& req) {
	const double currentTime = GetWorld()->GetTimeSeconds();
	if (!_hurtHistory.IsHurtable(currentTime)) {
		return FAttackResult(false);
	}

	_hurtHistory.SetHurtTime(currentTime);
	if (_animInstance) _animInstance->PlayHurtMontage();
	return FAttackResult(true);
}

void AKfKnightAIPawn::ReactToAnimHitDetection(float frameDeltaTime, const UHitDetectionNotifyParam& payload) {
	if (MeleeAttackComponent) MeleeAttackComponent->HandleAnimHitDetection(frameDeltaTime, payload);
}

void AKfKnightAIPawn::ReactToComboWindowNotifyState(bool isBegin, bool isEnd, bool isComboAllowed) {
	if (MeleeAttackComponent) MeleeAttackComponent->SetAllowCombo(isComboAllowed);
}

void AKfKnightAIPawn::ReactToComboWindowNotifyState_ResetComboSequence() {
	if (MeleeAttackComponent) MeleeAttackComponent->ResetAttackSequence();
}

FVector AKfKnightAIPawn::GetTargetLocation() {
	return GetActorLocation() + GetActorUpVector() * 500.f;
}

void AKfKnightAIPawn::ConsumeMovementInput() {
	if (_animInstance) _animInstance->SetMovementInput(_lastMoveInput);
	_lastMoveInput = FVector2f::ZeroVector;
}

void AKfKnightAIPawn::LookAtTarget(float deltaTime) {
	if (!MeleeAttackComponent->GetMeleeSequenceState().bAllowCombo) return;
	auto* aiController = GetController<AAIController>();
	if (!IsValid(aiController)) return;

	auto* tgt = aiController->GetFocusActor();
	if (IsValid(tgt)) {
		lastFocusActor = tgt;
	}
	
	if (!lastFocusActor) return;
	const auto targetDir = (lastFocusActor->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	const auto newRot = KfGameMath::FixedRotate_Yaw(GetActorRotation(), targetDir.Rotation(), deltaTime, knightMovement->RotationRate.Yaw);
	SetActorRotation(newRot);
}