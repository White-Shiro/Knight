#include "KfCharacterAnimInstance.h"
#include "KfCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Knight/Core/Character/Locomotion/KfFootStepAnimNotify.h"
#include "Knight/Core/Common.h"

void UKfCharacterAnimInstance::NativeBeginPlay() {
	Super::NativeBeginPlay();
}

void UKfCharacterAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	aMoveAnimPlaySpeed = 1.0f;
	aMoveDirection = FVector2f::ZeroVector;
	_lastMoveInput = FVector2d::ZeroVector;
	_knightCh = Cast<AKfCharacter>(TryGetPawnOwner());
	//CacheWalkBlendSpaceInfo();
}

void UKfCharacterAnimInstance::SyncWalkPlaySpeed(const FVector& worldVel, const FVector& localInputV, const float meshScale) {
	constexpr float animWalkMotionVelMag = 313.655f / 1.0f; // Root Motion Distance / Animation Length
	constexpr float animBackWalkMotionVelMag = 131.86f / 1.8f;

	const float animVelMag = localInputV.X > -0.1f ? animWalkMotionVelMag : animBackWalkMotionVelMag;
	const float velocityMag = worldVel.Length();

	// Jason: be careful div by zero
	float div = animVelMag * meshScale;
	const float playSpeed = FMath::IsNearlyZero(animVelMag) ? 0 : velocityMag / (animVelMag * meshScale);
	// Jason: but I do have MyMath::SafeDiv(a,b)

	aMoveAnimPlaySpeed = velocityMag > 0 ? playSpeed : 1;
}

void UKfCharacterAnimInstance::SetWalkBlendspaceDirection2D(const FVector2d& localInputV) {
	FVector2f calInputV2f(localInputV);

	if (calInputV2f != FVector2f::ZeroVector) {
		calInputV2f.Normalize();
		calInputV2f *= WALK_BLENDSPACE_AXIS_SCALE;
	}

	aMoveDirection.X = calInputV2f.X; // Right
	aMoveDirection.Y = calInputV2f.Y; // Fwd
}

void UKfCharacterAnimInstance::SetWalkBlendspaceDirection1D(const FVector& worldVel, const float maxSpeed) {
	// Jason: you sure compare float by "==" or better use FMath::IsNearlyZero
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

	// const auto worldVel = _knightCh->GetVelocity();
	// const auto localInputV = _knightCh->GetLocalInputVector();
	// const float meshScale =  _knightCh->GetMesh()->GetComponentScale().X;
	// SetWalkBlendspaceDirection1D(worldVel, _knightCh->GetMovementComponent()->GetMaxSpeed());
	// SyncWalkPlaySpeed(worldVel, localInputV, meshScale);

	aIsWalking = _lastMoveInput != FVector2d::ZeroVector;

	if (aIsWalking) {
		// let state cached the lastMovingDirection for Blending
		SetWalkBlendspaceDirection2D(_lastMoveInput);
	}

	UC_LOG_MSG_CONDITIONAL(logDirAndSpeed, "DIR: %s | SPD: %f", *aMoveDirection.ToString(), aMoveAnimPlaySpeed);
}

void UKfCharacterAnimInstance::SetMovementInput(const FVector2D& inputV) {
	_lastMoveInput = inputV;
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

float UKfCharacterAnimInstance::PlayMeleeMontage_Directional(const EAttackInputDirection atkDir) {
	const auto animSet = attackAnimationSet;
	UAnimMontage* montage = nullptr;

	switch (atkDir) {
		case EAttackInputDirection::Normal: montage = animSet.attackMontage_NA_Normal; break;
		case EAttackInputDirection::Up: montage = animSet.attackMontage_NA_Normal; break;
		case EAttackInputDirection::Left: montage = animSet.attackMontage_NA_Left; break;
		case EAttackInputDirection::Right: montage = animSet.attackMontage_NA_Right; break;
		case EAttackInputDirection::Down: montage = animSet.attackMontage_NA_Down; break;
	}

	float length = 0.f;
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

void UKfCharacterAnimInstance::PlayEvadeMontage(const EEvadeDirection evadeDir) {
	UAnimMontage* mtg = nullptr;

	switch (evadeDir) {
		case EEvadeDirection::Forward:
		case EEvadeDirection::Left:
		case EEvadeDirection::Right:
			mtg = attackAnimationSet.evadeMontage;
			break;
		case EEvadeDirection::Backward:
			mtg = attackAnimationSet.evade_Back_Montage;
			break;
	}
	if (!mtg) return;

	Montage_Play(mtg, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
}

void UKfCharacterAnimInstance::NotifyFootStep(const EFootStepType foot_step) const {
	footStepSoundRequest.Play(GetSkelMeshComponent());
}

