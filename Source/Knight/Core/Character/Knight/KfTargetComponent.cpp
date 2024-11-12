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

	const auto owner = Cast<AKfCharacter>(GetOwner());
	const auto targetLoc = _target->GetTargetLocation();
	const auto selfLoc = owner->GetActorLocation();

	// Set Actor Rotation
	auto targetVector = targetLoc - selfLoc;
	targetVector.Z = 0;
	const auto targetDist = targetVector.Size();
	if (FMath::IsNearlyZero(targetDist)) return;
	const auto targetDir = targetVector / targetDist;
	auto facing = owner->GetActorForwardVector();
	facing.Z = 0;
	facing.Normalize();

	if (facing.Dot(targetDir) < -0.2f) {
		ReleaseTarget();
		return;
	}

	if (targetDist < 1000.f) return;
	// Set Player Controller
	const auto pc = owner->GetInstigatorController();
	if (!pc) return;

	const auto camLoc = owner->GetCameraLocation();
	const auto ctrlRot = pc->GetControlRotation();
	const auto camDir = targetLoc - camLoc;

	const auto yawLookAtRot = FRotationMatrix::MakeFromX(camDir).Rotator();
	const auto clampPitch = FMath::ClampAngle(ctrlRot.Pitch, _lockPitchRange.X, _lockPitchRange.Y);
	const FRotator targetCtrlRot = FRotator(clampPitch, yawLookAtRot.Yaw, ctrlRot.Roll);
	const float adjustedLockSpeed = _lockSpeed * 0.5f; // Adjust this value as needed

	pc->SetControlRotation(FMath::RInterpTo(ctrlRot, targetCtrlRot, dt, adjustedLockSpeed));
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