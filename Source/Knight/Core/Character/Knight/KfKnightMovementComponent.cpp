#include "KfKnightMovementComponent.h"

void UKfKnightMovementComponent::OnRegister() {
	Super::OnRegister();
}

void UKfKnightMovementComponent::RequestPathMove(const FVector& MoveInput) {
	RequestDirectMove(MoveInput, false);
}

void UKfKnightMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) {
	if (!IsValid(PawnOwner)) return;

	if (bOrientRotationToMovement) {
		const auto currentFacing = PawnOwner->GetActorForwardVector().GetSafeNormal2D();
		const auto moveDir = MoveVelocity.GetSafeNormal2D();
		FRotator newFacing = FRotator::ZeroRotator;
		const auto deltaRot = GetDeltaRotation(GetWorld()->GetDeltaSeconds());
		newFacing.Yaw = FMath::FixedTurn(currentFacing.Rotation().Yaw, moveDir.Rotation().Yaw, deltaRot.Yaw);
		PawnOwner->SetActorRotation(newFacing);
	}

	PawnOwner->AddMovementInput(MoveVelocity, 1.f, true);
}

UKfKnightMovementComponent::UKfKnightMovementComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UKfKnightMovementComponent::BeginPlay() {
	Super::BeginPlay();
}

void UKfKnightMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

