#include "KfCharacterAnimInstance.h"
#include "KfCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Knight/Core/Common.h"

void UKfCharacterAnimInstance::NativeBeginPlay() {
	Super::NativeBeginPlay();
	aMoveDirection = FVector2f::ZeroVector;
	CacheWalkBlendSpaceInfo();
}

void UKfCharacterAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	aMoveAnimPlaySpeed = 1.0f;
	_knightCh = Cast<AKfCharacter>(TryGetPawnOwner());
}

void UKfCharacterAnimInstance::syncWalkPlaySpeed(const FVector& worldVel, const FVector& localInputV, const float meshScale) {
	constexpr float animWalkMotionVelMag = 313.655f / 1.0f; // Root Motion Distance / Animation Length
	constexpr float animBackWalkMotionVelMag = 131.86f / 1.8f;

	const float animVelMag = localInputV.X > -0.1f ? animWalkMotionVelMag : animBackWalkMotionVelMag;
	const float velocityMag = worldVel.Length();
	const float playSpeed = velocityMag / (animVelMag * meshScale);
	aMoveAnimPlaySpeed = velocityMag > 0 ? playSpeed : 1;
}

void UKfCharacterAnimInstance::setWalkBlendspaceDirection2D(const FVector& localInputV) {
	FVector2f calInputV2f(localInputV.X, localInputV.Y);

	if (calInputV2f != FVector2f::ZeroVector) {
		calInputV2f.Normalize();
		calInputV2f *= WALK_BLENDSPACE_AXIS_SCALE;
	}

	aMoveDirection.X = calInputV2f.Y; // Right
	aMoveDirection.Y = calInputV2f.X; // Fwd
}

void UKfCharacterAnimInstance::setWalkBlendspaceDirection1D(const FVector& worldVel, const float maxSpeed) {
	if (worldVel == FVector::ZeroVector || maxSpeed == 0) {
		aMoveDirection = FVector2f::ZeroVector;
		return;
	}

	const float mag = worldVel.Length();

	auto ratio = mag / maxSpeed;
	ratio *= WALK_BLENDSPACE_AXIS_SCALE;

	aMoveDirection.X = 0.f;
	aMoveDirection.Y = ratio;
}

void UKfCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!_knightCh) return;

	const auto worldVel = _knightCh->GetVelocity();
	const auto localInputV = _knightCh->GetLocalInputVector();\
	const float meshScale =  _knightCh->GetMesh()->GetComponentScale().X;

	//setWalkBlendspaceDirection2D(localInputV);
	setWalkBlendspaceDirection1D(worldVel, _knightCh->GetMovementComponent()->GetMaxSpeed());
	syncWalkPlaySpeed(worldVel, localInputV, meshScale);

	UC_LOG_MSG_CONDITIONAL(logDirAndSpeed, "DIR: %s | SPD: %f", *aMoveDirection.ToString(), aMoveAnimPlaySpeed);
}

float UKfCharacterAnimInstance::PlayMeleeMontage(int32 montageIndex) {
	UAnimMontage* montage = nullptr;

	const auto animSet = attackAnimationSet;

	switch (montageIndex) {
		case 1: montage = animSet.attackMontage_NA_1; break;
		case 2: montage = animSet.attackMontage_NA_2; break;
		case 3: montage = animSet.attackMontage_NA_3; break;

		// HA
		case 11: montage = animSet.attackMontage_HA_1; break;
		case 12: break;
		default: break;
	}

	float length = 0.0f;

	if (montage) length = Montage_Play(montage);
	return length;
}

void UKfCharacterAnimInstance::PlayHurtMontage() {
	auto mtg = attackAnimationSet.hurtMontage;
	if (!mtg) return;

	Montage_Play(mtg, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);

	if (FMath::RandBool()) {
		static FName HURT_BIG_SECTION_NAME(TEXT("Big"));
		Montage_JumpToSection(HURT_BIG_SECTION_NAME, mtg);
	}
}

void UKfCharacterAnimInstance::CacheWalkBlendSpaceInfo() {
	if (!walkBlendSpace) {
		return;
	}

	const auto& samples = walkBlendSpace->GetBlendSamples();

	for (const auto& sample : samples) {
		auto v = sample.SampleValue;

		if (v == FVector::ZeroVector) continue;
		v.Normalize();
		auto dir = GetWalkDirection({v.X, v.Y});
		auto anim = sample.Animation;

		if (!_walkBlendSpaceMap.Contains(dir)) {
			_walkBlendSpaceMap.Add(dir, anim);
			//FTransform t;
			//anim->GetBoneTransform(t)
		} else {
			UC_MSG("Duplicate Walk Direction: %s", *anim.GetName());
		}
	}

	UC_MSG("CacheWalkBlendSpaceInfo")
}

UKfCharacterAnimInstance::EWalkDirection UKfCharacterAnimInstance::GetWalkDirection(const FVector2d& inputV) {
	if (inputV.X > 0.0f) return EWalkDirection::Forward;
	if (inputV.X < 0.0f) return EWalkDirection::Backward;
	if (inputV.Y > 0.0f) return EWalkDirection::Right;
	if (inputV.Y < 0.0f) return EWalkDirection::Left;
	return EWalkDirection::Forward;
}