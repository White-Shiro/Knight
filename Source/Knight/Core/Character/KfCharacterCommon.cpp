#include "KfCharacterCommon.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Knight/Core/Core.h"

#define IA_FIND_PATH(AssetName) TEXT("/Script/EnhancedInput.InputAction'/Game/Core/Config/Input/" AssetName "." AssetName "'");

FKfCharacterInputActionSet::FKfCharacterInputActionSet() {
	MoveAction = KfCharacterCommon::GetDefaultMoveAction();
	LookAction = KfCharacterCommon::GetDefaultLookAction();
	ZoomAction = KfCharacterCommon::GetDefaultZoomAction();
	JumpAction = KfCharacterCommon::GetDefaultJumpAction();
	EvadeAction = KfCharacterCommon::GetDefaultEvadeAction();
	LockTargetAction = KfCharacterCommon::GetDefaultLockTargetAction();
	ToggleCombatStateAction = KfCharacterCommon::GetDefaultToggleCombatStateAction();
	Attack1Action = KfCharacterCommon::GetDefaultAttack1Action();
	Attack2Action = KfCharacterCommon::GetDefaultAttack2Action();
	SprintAction = KfCharacterCommon::GetDefaultSprintAction();
}

void FSpringArmState::InterpPosition(USpringArmComponent* springArm, float deltaTime) const {
	const float alpha = FMath::Clamp(interpSpeed * deltaTime, 0.f, 1.f);
	const auto interpOffset = FMath::Lerp(springArm->SocketOffset, socketOffset, alpha);
	const auto interpLength = FMath::Lerp(springArm->TargetArmLength, targetArmLength, alpha);

	// Smooth Damp
	//CameraBoom->SocketOffset = FMath::Damp(CameraBoom->SocketOffset, _targetCameraBoomState.socketOffset, _targetCameraBoomState.socketOffset, 0.1f, 1000.f, deltaTime);
	springArm->SocketOffset = interpOffset;
	springArm->TargetArmLength = interpLength;
}

void FSpringArmState::ApplyPosition(USpringArmComponent* springArm) const {
	if (!springArm) return;
	springArm->SocketOffset = socketOffset;
	springArm->TargetArmLength = targetArmLength;
}

void FSpringArmState::ApplyBool(USpringArmComponent* springArm) const {
	if (!springArm) return;
	springArm->bUsePawnControlRotation = usePawnControllerRotation;
	springArm->bEnableCameraLag = useCameraLag;
}

static void _AddMovementFromRotation(ACharacter* character, const FVector2D& movementInput, const FRotator& rotation) {
	if (!character) return;
	const FRotator yawRotation(0, rotation.Yaw, 0);
	const FQuat rotationQuat(yawRotation);

	//const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector forwardDelta = rotationQuat.GetForwardVector() * movementInput.Y;

	//const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	const FVector rightDelta = rotationQuat.GetRightVector() * movementInput.X;

	const FVector worldDelta = forwardDelta + rightDelta;

	character->AddMovementInput(worldDelta);
}

void KfCharacterCommon::HandleMoveInput(const FInputActionValue& Value, ACharacter* character) {
	if (!character) return;
	if (!character->Controller) return;

	const FVector2D movementVector = Value.Get<FVector2D>();
	const FRotator rotation = character->GetActorRotation();
	_AddMovementFromRotation(character, movementVector, rotation);
}

void KfCharacterCommon::HandleMoveInput_CameraBase(const FInputActionValue& Value, ACharacter* character) {\
	if (!character) return;
	const auto ctrl = character->GetInstigatorController();
	if (!ctrl) return;

	const FVector2D movementVector = Value.Get<FVector2D>();
	const FRotator rotation = ctrl->GetDesiredRotation();
	_AddMovementFromRotation(character, movementVector, rotation);
}

void KfCharacterCommon::HandleTurnInput(const FInputActionValue& Value, ACharacter* character,
                                        FCameraRotationState& cameraRotationState) {
	if (!character) return;
	if(!character->Controller) return;

	const float TurnAxisScalar = Value.Get<float>();

	const auto* move = character->GetCharacterMovement();
	const auto rotRate = move->RotationRate;
	auto currentRot = character->GetActorRotation();
	const float deltaTime = character->GetWorld()->DeltaTimeSeconds;
	const auto deltaYaw = TurnAxisScalar * (rotRate.Yaw) * deltaTime;
	currentRot.Yaw += deltaYaw;
	character->SetActorRotation(currentRot);

	cameraRotationState.targetFacingRotation.Yaw += deltaYaw;
}

void KfCharacterCommon::HandleLookInput(const FInputActionValue& Value, ACharacter* character,
	FCameraRotationState& cameraRotationState, float cameraLookSpeed) {
	if (!character) return;
	const auto* pc = (character->GetInstigatorController());
	if (!pc) return;

	const float scaler = cameraLookSpeed;
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	cameraRotationState.lookOffsetRotation.Yaw += LookAxisVector.X * scaler;
	const bool bInvertY = true;
	const float yValue = bInvertY ? -LookAxisVector.Y : LookAxisVector.Y;
	cameraRotationState.lookOffsetRotation.Pitch += yValue * scaler;

	character->AddControllerYawInput(LookAxisVector.X * scaler);
	character->AddControllerPitchInput(yValue * scaler);
}

void KfCharacterCommon::HandleZoomInput(const FInputActionValue& Value, ACharacter* character,
	USpringArmComponent* cameraBoom, float zoomSpeed, const FVector2f& clampRange) {
	if(!cameraBoom) {
		UC_LOG_ERROR_MSG("cameraBoom is nullptr");
		return;
	}

	const float zoomValue = Value.Get<float>();
	const float newLength = cameraBoom->TargetArmLength- zoomValue * zoomSpeed;
	cameraBoom->TargetArmLength = FMath::Clamp(newLength, clampRange.X, clampRange.Y);
}

void KfCharacterCommon::UpdateCameraRotation(float deltaTime, const ACharacter* character, FCameraRotationState& cameraRotationState,
	float interpolateSpeed) {

	cameraRotationState.currentFacing =
		FMath::RInterpTo(cameraRotationState.currentFacing, cameraRotationState.targetFacingRotation, deltaTime, interpolateSpeed);

	const FRotator finalRot(cameraRotationState.currentFacing + cameraRotationState.lookOffsetRotation);

	if (auto* pc = character->GetInstigatorController()) {
		pc->SetControlRotation(finalRot);
	}
}

UInputAction* KfCharacterCommon::GetDefaultMoveAction() {
	static ConstructorHelpers::FObjectFinder<UInputAction> Finder = IA_FIND_PATH("IA_Move");
	return Finder.Object;
}

UInputAction* KfCharacterCommon::GetDefaultLookAction() {
	static ConstructorHelpers::FObjectFinder<UInputAction> Finder = IA_FIND_PATH("IA_Look");
	return Finder.Object;
}

UInputAction* KfCharacterCommon::GetDefaultZoomAction() {
	static ConstructorHelpers::FObjectFinder<UInputAction> Finder = IA_FIND_PATH("IA_Zoom");
	return Finder.Object;
}

UInputAction* KfCharacterCommon::GetDefaultJumpAction() {
	static ConstructorHelpers::FObjectFinder<UInputAction> Finder = IA_FIND_PATH("IA_Jump");
	return Finder.Object;
}

UInputAction* KfCharacterCommon::GetDefaultLockTargetAction() {
	static ConstructorHelpers::FObjectFinder<UInputAction> Finder = IA_FIND_PATH("IA_Lock");
	return Finder.Object;
}

UInputAction* KfCharacterCommon::GetDefaultAttack1Action() {
	static ConstructorHelpers::FObjectFinder<UInputAction> Finder = IA_FIND_PATH("IA_Attack_1");
	return Finder.Object;
}

UInputAction* KfCharacterCommon::GetDefaultAttack2Action() {
	static ConstructorHelpers::FObjectFinder<UInputAction> Finder = IA_FIND_PATH("IA_Attack_2");
	return Finder.Object;
}

UInputAction* KfCharacterCommon::GetDefaultSprintAction() {
	static ConstructorHelpers::FObjectFinder<UInputAction> Finder = IA_FIND_PATH("IA_Sprint");
	return Finder.Object;
}

UInputAction* KfCharacterCommon::GetDefaultToggleCombatStateAction() {
	static ConstructorHelpers::FObjectFinder<UInputAction> Finder = IA_FIND_PATH("IA_ToggleCombat");
	return Finder.Object;
}

UInputAction* KfCharacterCommon::GetDefaultEvadeAction() {
	static ConstructorHelpers::FObjectFinder<UInputAction> Finder = IA_FIND_PATH("IA_Evade");
	return Finder.Object;
}

FName KfCharacterCommon::HitBoxCollisionPresetName(TEXT("HitBox"));