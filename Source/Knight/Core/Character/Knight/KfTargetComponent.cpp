#include "KfTargetComponent.h"

#include "KfCharacter.h"
#include "Knight/Core/Common.h"
#include "Knight/Core/Combat/CombatInterface.h"


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

	// Set Player Controller
	const auto pc = owner->GetInstigatorController();
	if (!pc) return;

	const auto camLoc = owner->GetCameraLocation();
	const auto targetLoc = _target->GetTargetLocation();

	const auto dir = targetLoc - camLoc;

	auto rot = FRotationMatrix::MakeFromX(dir).Rotator();

	// Clamp Pitch
	rot.Pitch = FMath::ClampAngle(rot.Pitch, _lockPitchRange.X, _lockPitchRange.Y);
	pc->SetControlRotation(FMath::RInterpTo(pc->GetControlRotation(), rot, dt, _lockSpeed));
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

void UKfTargetComponent::ScanTarget() {
	auto owner = GetOwner();
	_target = nullptr;
	const auto loc = owner->GetActorLocation();
	FCollisionQueryParams queryParam = {};
	queryParam.AddIgnoredActor(owner);
	bool hit = GetWorld()->OverlapMultiByObjectType(_overlaps, loc, FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(_scanRadius), queryParam);
	SetActive(hit);
	if (!hit) {
		UC_MSG("No Target Found");
		return;
	}
	_target = _GetClosestTarget(_overlaps, loc);

	if(_target.Get()) {
		UC_MSG("Target Acquired");
	}
}

void UKfTargetComponent::ReleaseTarget() {
	_overlaps.Empty();
	_target = nullptr;
	Deactivate();
}

FVector UKfTargetComponent::GetTargetLocation() {
	return _target.IsValid() ? _target->GetTargetLocation() : FVector::ZeroVector;
}


