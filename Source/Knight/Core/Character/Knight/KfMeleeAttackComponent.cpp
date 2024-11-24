#include "KfMeleeAttackComponent.h"
#include "KfCharacterAnimInstance.h"
#include "Knight/Core/Core.h"
#include "Knight/Core/Combat/Combat.h"
#include "Knight/Core/Combat/HitDetectionNotifyState.h"

UKfMeleeAttackComponent::UKfMeleeAttackComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UKfMeleeAttackComponent::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) {
	UC_MSG("OnMontageNotifyBegin");
}

void UKfMeleeAttackComponent::OnMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) {
	UC_MSG("OnMontageNotifyEnd");
}

void UKfMeleeAttackComponent::BeginPlay() {
	Super::BeginPlay();

	SetAllowCombo(true);

	_traceDelegate.BindUObject(this, &UKfMeleeAttackComponent::onTraceCompleted);

	if (const auto* skeletalMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>()) {
		_animInstance = Cast<UKfCharacterAnimInstance>(skeletalMesh->GetAnimInstance());
		OnAnimationInstanceInit(_animInstance);
	}
}

void UKfMeleeAttackComponent::OnAnimationInstanceInit(UKfCharacterAnimInstance* animInstance) {
	if (!animInstance) return;

	animInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UKfMeleeAttackComponent::OnMontageNotifyBegin);
	animInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UKfMeleeAttackComponent::OnMontageNotifyEnd);
}

void UKfMeleeAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UKfMeleeAttackComponent::DoMeleeAttack() {
	if (!_animInstance) {
		UC_LOG_ERROR("Do Melee Attack Failed: No Anim Instance Found!")
		return;
	}

	if (!_meleeSequenceState.bAllowCombo) return;

	constexpr int MAX_NORMAL_ATTACK_INDEX = 3;
	constexpr int MIN_NORMAL_ATTACK_INDEX = 1;

	int currentAttackIdx = _meleeSequenceState.comboSequence;

	currentAttackIdx = (++currentAttackIdx) % (MAX_NORMAL_ATTACK_INDEX + 1);

	if (currentAttackIdx < MIN_NORMAL_ATTACK_INDEX) {
		currentAttackIdx = MIN_NORMAL_ATTACK_INDEX;
	}

	_meleeSequenceState.comboSequence = currentAttackIdx;
	_animInstance->PlayMeleeMontage(currentAttackIdx);
}

static bool _isDirectionRepeated(const FMeleeSequenceState& state, const EAttackInputDirection requestDirection) {
	return state.isSequenceRunning() && state.lastAttackDirection == requestDirection;
}

void UKfMeleeAttackComponent::DoMeleeAttack_Directional(const FVector2d& movementInput) {
	if (!_animInstance) {
		UC_LOG_ERROR("Do Melee Attack Failed: No Anim Instance Found!")
		return;
	}

	if (!_meleeSequenceState.bAllowCombo) return;
	auto eDirection = CombatUtils::GetAttackInputDirection(movementInput);

	if(_isDirectionRepeated(_meleeSequenceState, eDirection)) {
		if (_debug) {
			UC_MSG("Repeated Attack Direction %d", static_cast<int>(eDirection));
		}
		return;
	}

	_animInstance->PlayMeleeMontage_Directional(eDirection);
	_meleeSequenceState.comboSequence++;
	_meleeSequenceState.lastAttackDirection = eDirection;

	if (_debug) {
		if (const auto e = StaticEnum<EAttackInputDirection>()) {
			const FString eString = e->GetNameStringByValue(static_cast<int64>(eDirection));

			FString directionString;

			switch (eDirection) {
				case EAttackInputDirection::Normal: directionString = "o"; break;
				case EAttackInputDirection::Up:		directionString = "^"; break;
				case EAttackInputDirection::Down:	directionString = "v"; break;
				case EAttackInputDirection::Left:	directionString = "<"; break;
				case EAttackInputDirection::Right:	directionString = ">"; break;
			}

			UC_MSG("Attack Direction %s %s" , *directionString, *eString)
		}
	}
}

void UKfMeleeAttackComponent::DoMeleeAttack_Heavy() {
	if (!_meleeSequenceState.bAllowCombo) return;
	constexpr int MIN_HEAVY_ATTACK_INDEX = 11;
	// constexpr int MAX_HEAVY_ATTACK_INDEX = 11;
	_animInstance->PlayMeleeMontage(MIN_HEAVY_ATTACK_INDEX);
}

void UKfMeleeAttackComponent::ResetAttackSequence() {
	int last = _meleeSequenceState.comboSequence;
	_meleeSequenceState.comboSequence = 0;
	_meleeSequenceState.lastAttackDirection = EAttackInputDirection::Normal;
	SetAllowCombo(true);
	if (_debug) UC_MSG("Reset Attack Sequence from %d to 0", last);
}

void UKfMeleeAttackComponent::SetAllowCombo(bool bAllowCombo) {
	_meleeSequenceState.bAllowCombo = bAllowCombo;
}

FTraceHandle UKfMeleeAttackComponent::TraceSwordHits(const FVector& start, const FVector& direction, float distance, float swordRad, bool isSegment) const {
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(GetOwner());
	traceParams.bTraceComplex = true;
	traceParams.bReturnPhysicalMaterial = true;

	const auto halfHeight = distance * 0.5f;
	const auto traceEnd = start + (direction * distance);
	const auto sweepHitBox = FCollisionShape::MakeSphere(swordRad);

	// For Capsule
	// const auto traceEnd = start + direction;
	// const auto sweepHitBox = FCollisionShape::MakeCapsule(swordRad, halfHeight);

	const FQuat swordQuat = FQuat::FindBetweenNormals(FVector::UpVector, direction);

	const auto world = GetWorld();
	//const bool hit = world->SweepSingleByChannel(outHitResult, start, traceEnd, swordQuat, ECollisionChannel::ECC_Pawn, sweepHitBox, traceParams);
	const auto h = world->AsyncSweepByChannel(EAsyncTraceType::Multi, start, traceEnd, swordQuat, ECollisionChannel::ECC_Pawn, sweepHitBox, traceParams, FCollisionResponseParams::DefaultResponseParam, &_traceDelegate, 0);

	if (_debug) {
		const auto center = start + (direction * halfHeight);
		const auto color = isSegment ? FColor::Blue : FColor::Yellow;
		DrawDebugCapsule(world, center, halfHeight, swordRad, swordQuat, color, _persistentLine, 0.1f, 0, 1.f);
	}

	return h;
}

static FAttackRequest CreateAttackRequest(const FHitResult& hitResult) {
	const FAttackRequest request = {};
	return request;
}

void UKfMeleeAttackComponent::OnSwordHitResult(const FHitResult& hitResult) const {
	auto* attackable = Cast<IReactToAttack>(hitResult.GetActor());
	if (!attackable) return;

	const auto req = CreateAttackRequest(hitResult);
	const auto result = attackable->ReactToAttack(req);

	if (!result.success) return;

	if (hitResult.Component.IsValid()) {
		if (const auto targetSceneComp = hitResult.Component.Get()) {
			_swordHitEffectSet.hitEffect_Pawn.EmitHitImpact(hitResult, targetSceneComp);
			_swordHitEffectSet.hitSoundRequest_Pawn.Play(targetSceneComp);
		}
	}

	if (_debug) {
		DrawDebugSphere(GetWorld(), hitResult.ImpactPoint, 100.f, 4, FColor::Red, _persistentLine, 1.f, 0, 1.f);
	}
}

void UKfMeleeAttackComponent::onTraceCompleted(const FTraceHandle& handle, FTraceDatum& data) {
	if (data.OutHits.Num() <= 0) return;
	for (const auto& hit : data.OutHits) {
		OnSwordHitResult(hit);
	}
}

bool UKfMeleeAttackComponent::TrySwordHitOnce(const FVector& start, const FVector& direction, bool isSegment) {
	const auto h = TraceSwordHits(start, direction, _hitBoxLength, _hitBoxRadius, isSegment);
	if (h.IsValid()) {
		_traceHandle = h;
	}
	return h.IsValid();
}

bool UKfMeleeAttackComponent::DoSwingHits(const UHitDetectionNotifyParam& param) {
	static const FName WEAPON_SOCKET_RIGHT = "hand_r_slot";

	if (!param.meshComp->DoesSocketExist(WEAPON_SOCKET_RIGHT)) {
		UC_LOG_MSG("Weapon Socket %s Not Found! ", ToCStr(WEAPON_SOCKET_RIGHT.ToString()));
		return true;
	}
	const auto t = param.meshComp->GetSocketTransform(WEAPON_SOCKET_RIGHT);

	const auto targetStart = t.GetLocation();
	const auto targetSwordDir = t.TransformVectorNoScale(-FVector::UpVector);

	const FTraceDirectionInfo current = {
		.start = targetStart,
		.unitDirection = targetSwordDir,
		.sequenceId = param.sequenceId,
	};

	bool hit = false;
	if (_fillHitDetectionGap && param.sequenceId == prevTraceDirection.sequenceId) {
		const auto prev = prevTraceDirection ;

		const float archAngleRadians = FMath::Acos(FVector::DotProduct(prev.unitDirection, current.unitDirection));
		const float archLength = archAngleRadians * _hitBoxLength;

		constexpr int MAX_SEGMENT_COUNT = 12;
		const float hitboxDiameter = _hitBoxRadius * 2;
		int segmentCount = FMath::CeilToInt(archLength / hitboxDiameter);
		segmentCount = FMath::Clamp(segmentCount, 1, MAX_SEGMENT_COUNT);

		// For each segment
		for (int i = 0; i < segmentCount; ++i) {
			// Calculate the interpolation factor
			float alpha = static_cast<float>(i) / segmentCount;

			// Interpolate the position and rotation
			const FVector startPos = FMath::Lerp(prev.start, current.start, alpha);
			const FVector direction = FMath::Lerp(prev.unitDirection, current.unitDirection, alpha);
			hit |= TrySwordHitOnce(startPos, direction, i < segmentCount - 1);
		}
	} else {
		hit = TrySwordHitOnce(targetStart, targetSwordDir);
	}

	prevTraceDirection = current;
	return hit;
}

void UKfMeleeAttackComponent::HandleAnimHitDetection(float frameDt, const UHitDetectionNotifyParam& param) {
	DoSwingHits(param);
}

#if WITH_EDITOR
void UKfMeleeAttackComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);
	// Check if the _drawHitBox flag is true

	UC_LOG_MSG("Drawing")

	// Draw a capsule gizmo at the actor's location
	DrawDebugCapsule(
		GetWorld(),
		GetOwner()->GetActorLocation(),
		_hitBoxLength / 2.0f,
		_hitBoxRadius,
		GetOwner()->GetActorQuat(),
		FColor::Red,
		false,
		// persistent (lasts forever)
		0.1f,
		// lifetime (0 means one frame)
		-20,
		// depth priority
		5.f // thickness
	);
}

#endif