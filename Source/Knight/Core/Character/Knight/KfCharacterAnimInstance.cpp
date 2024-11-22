#include "KfCharacterAnimInstance.h"
#include "KfCharacter.h"
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

	if (_knightCh) {
		characterTrajectory = _knightCh->GetCharacterTrajectory();
	}
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
	SetWalkBlendSpaceDirection2D(_lastMoveInput);

	UC_LOG_MSG_CONDITIONAL(logDirAndSpeed, "DIR: %s | SPD: %f", *aMoveDirection.ToString(), aMoveAnimPlaySpeed);
}

void UKfCharacterAnimInstance::SyncWalkPlaySpeed(const FVector& worldVel, const FVector& localInputV, float meshScale) {
	constexpr float animWalkMotionVelMag = 313.655f / 1.0f; // Root Motion Distance / Animation Length in Sec
	constexpr float animBackWalkMotionVelMag = 131.86f / 1.8f;

	const float animVelMag = localInputV.X > -0.1f ? animWalkMotionVelMag : animBackWalkMotionVelMag;
	const float velocityMag = worldVel.Length();
	const float playSpeed = velocityMag / (animVelMag * meshScale);
	aMoveAnimPlaySpeed = velocityMag > 0 ? playSpeed : 1;
}

void UKfCharacterAnimInstance::SetWalkBlendSpaceDirection2D(const FVector2d& localInputV) {
	FVector2f calInputV2f(localInputV);

	if (calInputV2f != FVector2f::ZeroVector) {
		calInputV2f.Normalize();
		calInputV2f *= WALK_BLENDSPACE_AXIS_SCALE;
	}

	aMoveDirection.X = calInputV2f.X; // Right
	aMoveDirection.Y = calInputV2f.Y; // Fwd
}

void UKfCharacterAnimInstance::SetWalkBlendSpaceDirection1D(const FVector& worldVel, const float maxSpeed) {
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

void UKfCharacterAnimInstance::SetMovementInput(const FVector2D& inputV) {
	_lastMoveInput = inputV;
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
		case EAttackInputDirection::Up:     montage = animSet.attackMontage_NA_Normal; break;
		case EAttackInputDirection::Left:   montage = animSet.attackMontage_NA_Left;   break;
		case EAttackInputDirection::Right:  montage = animSet.attackMontage_NA_Right;  break;
		case EAttackInputDirection::Down:   montage = animSet.attackMontage_NA_Down;   break;
	}

	float length = 0.f;
	if (montage) length = Montage_Play(montage);
	return length;
}

void UKfCharacterAnimInstance::PlayHurtMontage() {
	auto* mtg = attackAnimationSet.hurtMontage;
	if (!mtg) return;

	Montage_Play(mtg);

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

void UKfCharacterAnimInstance::NotifyFootStep(const FFootStepEvent& footStep) {
	aCurrentForwardFoot = footStep.footStepType;

	auto* mesh = GetSkelMeshComponent();
	if (!mesh) return;

	if (footStep.bPlaySound) footStepSoundRequest.Play(mesh);
	if (!footStep.bPlayVFX) return;

	static FName footStep_L_SocketName(TEXT("footStep_L"));
	static FName footStep_R_SocketName(TEXT("footStep_R"));

	// Play VFX At foot location
	switch (footStep.footStepType) {
		case EFootStepType::None: break;
		case EFootStepType::Left: footStepVFXRequest.EmitParticleAt(mesh->GetSocketLocation(footStep_L_SocketName), FRotator::ZeroRotator, mesh); break;
		case EFootStepType::Right: footStepVFXRequest.EmitParticleAt(mesh->GetSocketLocation(footStep_R_SocketName), FRotator::ZeroRotator, mesh); break;
		case EFootStepType::Both: {
			footStepVFXRequest.EmitParticleAt(mesh->GetSocketLocation(footStep_R_SocketName), FRotator::ZeroRotator, mesh);
			footStepVFXRequest.EmitParticleAt(mesh->GetSocketLocation(footStep_L_SocketName), FRotator::ZeroRotator, mesh);
		} break;
		default: break;
	}
}