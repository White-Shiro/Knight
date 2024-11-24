#include "KfKnightMovementComponent.h"
#include "KfKnightAIPawn.h"
#include "Knight/Core/Logger.h"

void UKfKnightMovementComponent::OnRegister() {
	Super::OnRegister();
}

void UKfKnightMovementComponent::RequestPathMove(const FVector& MoveInput) {
	if (PawnOwner) PawnOwner->AddMovementInput(MoveInput, 1.f, true);
}

void UKfKnightMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) {
	if (PawnOwner) PawnOwner->AddMovementInput(MoveVelocity, 1.f, true);
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

