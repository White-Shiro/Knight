#pragma once

struct FInputActionValue;
class UInputAction;
class ACharacter;

struct SCameraRotationState {
	FRotator currentFacing;
	FRotator targetFacingRotation;
	FRotator lookOffsetRotation;

	FORCEINLINE void Reset(float facingYaw) {
		currentFacing = FRotator(0, facingYaw, 0);
		targetFacingRotation = currentFacing;
		lookOffsetRotation = FRotator::ZeroRotator;
	}
};

class KfCharacterCommon {
public:
	static void HandleMoveInput_CameraBase      (const FInputActionValue& Value, ACharacter* const character);
	static void HandleMoveInput      (const FInputActionValue& Value, ACharacter* const character);
	static void HandleTurnInput		 (const FInputActionValue& Value, ACharacter* const character, SCameraRotationState& cameraRotationState);
	static void HandleLookInput      (const FInputActionValue& Value, ACharacter* const character, SCameraRotationState& cameraRotationState, float CameraLookSpeed);
	static void HandleZoomInput      (const FInputActionValue& Value, ACharacter* const character, class USpringArmComponent* const cameraBoom, float zoomSpeed ,const FVector2f& clampRange);
	//static void HandleJumpInput      (const FInputActionValue& Value, ACharacter* const character);
	//static void HandleStopJumpInput  (const FInputActionValue& Value, ACharacter* const character);
	//static void HandleSprintInput    (const FInputActionValue& Value, ACharacter* const character);
	//static void HandleStopSprintInput(const FInputActionValue& Value, ACharacter* const character);
	static void UpdateCameraRotation (float dt, ACharacter* const character, SCameraRotationState& cameraRotationState, float interpolateSpeed);

	static UInputAction* GetDefaultMoveAction();
	static UInputAction* GetDefaultLookAction();
	static UInputAction* GetDefaultZoomAction();
	static UInputAction* GetDefaultJumpAction();
	static UInputAction* GetDefaultLockTargetAction();

	static UInputAction* GetDefaultAttackAction();
	static UInputAction* GetDefaultToggleCombatStateAction();
	static FName HitBoxCollisionPresetName();
};