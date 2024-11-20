#include "KfTargetComponent.h"

#include "KfCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Knight/Core/Common.h"
#include "Knight/Core/Combat/CombatCommon.h"
#include "Knight/Core/Math/KfVectorUtils.h"

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
	UpdateCtrlCamera(DeltaTime);
}

void UKfTargetComponent::UpdateCtrlCamera(float dt) {
	if(!_target.Get()) return;

	const auto owner = Cast<AKfCharacter>(GetOwner());
	const auto targetLoc = _target->GetTargetLocation();
	const auto selfLoc = owner->GetActorLocation();

	// Set Actor Rotation
	FVector horizontalTargetDir = KfVectorUtils::XY0(targetLoc - selfLoc);
	float targetDist;

	if (!KfVectorUtils::UnitDirectionAndSize(horizontalTargetDir, horizontalTargetDir, targetDist)) {
		return;
	}

	const auto targetDir = horizontalTargetDir / targetDist;
	const auto horizontalFacing = KfVectorUtils::HorizontalUnitDirection(owner->GetActorForwardVector());

	if (horizontalFacing.Dot(targetDir) < -0.2f) {
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
		ReleaseTarget();
		return false;
	}

	_target = _GetClosestTarget(_overlaps, loc);
	const bool hasTarget = _target.IsValid();

	if (hasTarget) {
		if (const auto* ch = AKfCharacter::CastFrom(owner)) { }
		UC_MSG("Target Acquired");
	}

	SetActive(hasTarget);
	return hasTarget;
}

void UKfTargetComponent::ReleaseTarget() {
	_overlaps.Empty();
	_target = nullptr;
	SetActive(false);
}

void UKfTargetComponent::SetTargetCamera(UCameraComponent* targetCamera) {
	_targetCamera = targetCamera;
}

FVector UKfTargetComponent::GetTargetLocation() const {
	return _target.IsValid() ? _target->GetTargetLocation() : FVector::ZeroVector;
}

void UKfTargetComponent::CalcCameraLocation_SprintArmState(const FVector& selfLoc, float pitch, const FSpringArmState& springArmState, const FVector& facingDirection, FVector& outCamLocation) {
	auto facingRot = facingDirection.Rotation();
	facingRot.Pitch = pitch;
	const auto armLengthOffset = (-(facingRot.Vector())) * springArmState.targetArmLength;
	const auto camWorldOffset = facingRot.RotateVector(springArmState.socketOffset);
	outCamLocation = selfLoc + armLengthOffset + camWorldOffset;
}

// Custom Camera Behavior
void UKfTargetComponent::CalcCamera(float DeltaTime, UCameraComponent* camera, const FCameraConfig* springArmConfig, FMinimalViewInfo& outResult) {
	if (!_target.IsValid()) return;

	if (!camera) return;
	if (!springArmConfig) return;
	const auto owner = Cast<AKfCharacter>(GetOwner());
	if (!owner) return;

	const auto pc = owner->GetInstigatorController();
	if (!pc) return;

	const auto targetLoc = _target->GetTargetLocation();
	const auto selfLoc = owner->GetActorLocation();

	const auto ctrlRot = pc->GetControlRotation();
	const FSpringArmState springArmState = springArmConfig->springArmState;

#if 0
	const auto clampCtrlPitch = FMath::ClampAngle(ctrlRot.Pitch, _lockPitchRange.X, _lockPitchRange.Y);
#else
	constexpr float clampCtrlPitch(0);
#endif

	auto horizontalTargetDir = KfVectorUtils::XY0(targetLoc - selfLoc);
	float targetDist;
	if (!KfVectorUtils::UnitDirectionAndSize(horizontalTargetDir, horizontalTargetDir, targetDist)) {
		return;
	}
	const auto horizontalFacing = KfVectorUtils::HorizontalUnitDirection(owner->GetActorForwardVector());

#if 0
	if (facing.Dot(targetDir) < -0.2f) {
		ReleaseTarget();
		return;
	}
#endif

#if 0
	if (targetDist < 1000.f) return;
#endif

	// append Config Offset
	const auto camTrans = camera->GetComponentTransform();
	const auto camRot = camTrans.GetRotation().Rotator();

	FVector finalCamLoc;
	CalcCameraLocation_SprintArmState(selfLoc, clampCtrlPitch, springArmState, horizontalFacing, finalCamLoc);

	const auto camDir = targetLoc - finalCamLoc;
	const auto yawLookAtRot = camDir.Rotation();
	const FRotator targetCamRot = FRotator(clampCtrlPitch, yawLookAtRot.Yaw, ctrlRot.Roll);

#if 0
	const float adjustedLockSpeed = _lockSpeed * 1.5f; // Adjust this value as needed
	auto finalRot = FMath::RInterpTo(camRot, targetCamRot, DeltaTime, adjustedLockSpeed);
#else
	auto finalRot = targetCamRot;
#endif

	outResult.Rotation = finalRot;
	outResult.Location = finalCamLoc;

	if (springArmState.usePawnControllerRotation) {
		pc->SetControlRotation(finalRot);
	}

#if 1
	if (horizontalFacing.Dot(horizontalTargetDir) > -0.2f) {
		FRotator chRot = owner->GetActorRotation();
		chRot.Yaw = horizontalTargetDir.Rotation().Yaw;
		owner->SetActorRotation(chRot);
	}
#endif

	if (_debug) {
		DebugLockState(outResult);
	}
}

void UKfTargetComponent::DebugLockState(const FMinimalViewInfo& outResult) const {
	if (!hasTarget()) return;
	const auto owner = Cast<AKfCharacter>(GetOwner());
	const auto targetLoc = _target->GetTargetLocation();
	const auto selfLoc = owner->GetActorLocation();
	const auto* world = GetWorld();
	if (!world) return;

	const auto& camLoc = outResult.Location;
	const auto& camRot = outResult.Rotation;

	DrawDebugLine(world, selfLoc, targetLoc, FColor::Red, false, -1, -0, 2.0f); // Actor to Target
	DrawDebugLine(world, selfLoc, camLoc, FColor::Green, false, -1, 0, 2.0f); // Actor to Camera
	DrawDebugLine(world, camLoc, camLoc + camRot.Vector() * 5000.f, FColor::Red); // Camera to LookAt
	DrawDebugCamera(world, camLoc, camRot, outResult.FOV, 3.f, FColor::Magenta); // Camera
}

bool UKfTargetComponent::ToggleTargetMode() {
	if (hasTarget()) {
		ReleaseTarget();
		return false;
	}

	return ScanTarget();
}