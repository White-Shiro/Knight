#include "KfTargetComponent.h"

#include "KfCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Knight/Core/Common.h"
#include "Knight/Core/Combat/CombatCommon.h"

UKfTargetComponent::UKfTargetComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	_target = nullptr;
}

void UKfTargetComponent::BeginPlay() {
	Super::BeginPlay();
}

void UKfTargetComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateCamera(DeltaTime);
}

void UKfTargetComponent::UpdateCamera(float dt) {
	if(!_target.Get()) return;

	auto owner = Cast<AKfCharacter>(GetOwner());
	const auto targetLoc = _target->GetTargetLocation();

	// Set Actor Rotation
	const auto chTotargetDir = targetLoc - owner->GetActorLocation();
	auto chRot = FRotationMatrix::MakeFromX(chTotargetDir).Rotator();
	chRot.Pitch = 0;
	chRot.Roll = 0;
	owner->SetActorRotation(chRot);

	// Set Player Controller
	const auto pc = owner->GetInstigatorController();
	if (!pc) return;

	const auto camLoc = owner->GetCameraLocation();
	const auto camDir = targetLoc - camLoc;

	auto rot = FRotationMatrix::MakeFromX(camDir).Rotator();

	// Clamp Pitch
	rot.Pitch = FMath::ClampAngle(rot.Pitch, _lockPitchRange.X, _lockPitchRange.Y);

	// Adjust the lock speed to make the camera move slower
	float adjustedLockSpeed = _lockSpeed * 0.5f; // Adjust this value as needed

	pc->SetControlRotation(FMath::RInterpTo(pc->GetControlRotation(), rot, dt, adjustedLockSpeed));
}

static ITargetable* _GetClosestTarget(const TArray<FOverlapResult>& overlaps, const FVector& loc) {
	ITargetable* target = nullptr;
	float minDist = FLT_MAX;
	for (const auto& overlap : overlaps) {
		if (const auto actor = overlap.GetActor()) {
			if (const auto targetable = Cast<ITargetable>(actor)) {
				if (const auto dist = FVector::DistSquared(loc, actor->GetActorLocation()); dist < minDist) {
					minDist = dist;
					target = targetable;
				}
			}
		}
	}
	return target;
}

bool UKfTargetComponent::ScanTarget() {
	auto* owner  = GetOwner();
	_target = nullptr;
	const auto loc = owner->GetActorLocation();
	FCollisionQueryParams queryParam = {};
	queryParam.AddIgnoredActor(owner);
	const bool hit = GetWorld()->OverlapMultiByObjectType(_overlaps, loc, FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(_scanRadius), queryParam);

	if (!hit) {
		UC_MSG("No Target Found");
		SetActive(false);
		return false;
	}

	_target = _GetClosestTarget(_overlaps, loc);
	const bool hasTarget = _target.IsValid();

	if(hasTarget) {
		if (const auto* ch = AKfCharacter::CastFrom(owner)) { }
		UC_MSG("Target Acquired");
	}

	SetActive(hasTarget);
	return hasTarget;
}

void UKfTargetComponent::ReleaseTarget() {
	_overlaps.Empty();
	_target = nullptr;

	if (const auto* ch = AKfCharacter::CastFrom(GetOwner())) {
	}

	Deactivate();
}

void UKfTargetComponent::SetTargetCamera(UCameraComponent* targetCamera) {
	_targetCamera = targetCamera;
}

FVector UKfTargetComponent::GetTargetLocation() {
	return _target.IsValid() ? _target->GetTargetLocation() : FVector::ZeroVector;
}