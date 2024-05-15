#include "KfMeleeAttackComponent.h"
#include "KfCharacterAnimInstance.h"
#include "Knight/Core/Common.h"
#include "Knight/Core/Combat/CombatInterface.h"
#include "Knight/Core/Combat/HitDetectionNotifyState.h"
#include "Knight/Core/Util/WxGamePlayUtil.h"

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

	constexpr int MAX_NORMAL_ATTACK_INDEX = 3;
	constexpr int MIN_NORMAL_ATTACK_INDEX = 1;
	_attackIndex = (++_attackIndex) % (MAX_NORMAL_ATTACK_INDEX + 1);

	if (_attackIndex < MIN_NORMAL_ATTACK_INDEX) {
		_attackIndex = MIN_NORMAL_ATTACK_INDEX;
	}

	_animInstance->PlayMeleeMontage(_attackIndex);
}

bool UKfMeleeAttackComponent::TraceSwordHits(const FVector& start, const FVector& direction, float distance, float swordRad, FHitResult& outHitResult, bool isSegment) const {
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
	const bool hit = world->SweepSingleByChannel(outHitResult, start, traceEnd, swordQuat, ECollisionChannel::ECC_Pawn, sweepHitBox, traceParams);

	if (_drawHitBox) {
		const auto center = start + (direction * halfHeight);
		const auto color = hit ? FColor::Red : isSegment ? FColor::Blue : FColor::Yellow;
		DrawDebugCapsule(world, center, halfHeight, swordRad, swordQuat, color, _persistentLine, 0.1f, 0, 1.f);

		if (hit) {
			//UC_MSG("Hit %s", *outHitResult.GetActor()->GetName());
			DrawDebugSphere(GetWorld(), outHitResult.ImpactPoint, 100.f, 4, FColor::Red, _persistentLine, 1.f, 0, 1.f);
		}
	}

	return hit;
}

static FAttackReqeust CreateAttackRequest(const FHitResult& hitResult) {
	const FAttackReqeust request = {};
	return request;
}

void UKfMeleeAttackComponent::OnSwordHitResult(const FHitResult& hitResult) const {
	const auto* world = GetWorld();
	UParticleSystem* pPfx = _swordHitEffectSet.hitEffect_Ground.Get();

	if (auto* attackable = Cast<IReactToAttack>(hitResult.GetActor())) {
		const auto req = CreateAttackRequest(hitResult);
		const auto result = attackable->ReactToAttack(req);

		if (result.success) {
			pPfx = _swordHitEffectSet.hitEffect_Pawn.Get();
		}
	}

	if (pPfx) {
		WxGamePlayUtil::PlayHitNormalEffect(_swordHitEffectSet.hitEffect_Ground.Get(), _swordHitEffectSet.effectSize, hitResult, world);
	}
}

bool UKfMeleeAttackComponent::TrySwordHitOnce(const FVector& start, const FVector& direction, bool isSegment) const {
	FHitResult hitResult = FHitResult();
	const bool hit = TraceSwordHits(start, direction, _hitBoxLength, _hitBoxRadius, hitResult, isSegment);
	if (hit) {
		OnSwordHitResult(hitResult);
	}

	return hit;
}

bool UKfMeleeAttackComponent::DoSwingHits(const UHitDetectionNotifyParam& param) {
	static const FName WEAPON_SOCKET_RIGHT = "hand_r_slot";

	if (!param.meshComp->DoesSocketExist(WEAPON_SOCKET_RIGHT)) {
		UC_LOG_ERROR("Weapon Socket %s Not Found!", *WEAPON_SOCKET_RIGHT.ToString());
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
			FVector startPos = FMath::Lerp(prev.start, current.start, alpha);
			FVector direction = FMath::Lerp(prev.unitDirection, current.unitDirection, alpha);
			hit |= TrySwordHitOnce(startPos, direction, i < segmentCount - 1);
		}
	} else {
		hit = TrySwordHitOnce(targetStart, targetSwordDir);
	}

	prevTraceDirection = current;
	return hit;
}

void UKfMeleeAttackComponent::HandleAnimHitDetection(float frameDt, const UHitDetectionNotifyParam& param) {
	if (DoSwingHits(param)) return;
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