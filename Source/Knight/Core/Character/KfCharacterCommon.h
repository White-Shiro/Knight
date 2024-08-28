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
	static void HandleMoveInput      (const FInputActionValue& Value, ACharacter* character);
	static void HandleTurnInput		 (const FInputActionValue& Value, ACharacter* character, SCameraRotationState& cameraRotationState);
	static void HandleLookInput      (const FInputActionValue& Value, ACharacter* character, SCameraRotationState& cameraRotationState, float CameraLookSpeed);
	static void HandleZoomInput      (const FInputActionValue& Value, ACharacter* character, class USpringArmComponent* cameraBoom, float zoomSpeed ,const FVector2f& clampRange);
	//static void HandleJumpInput      (const FInputActionValue& Value, ACharacter* const character);
	//static void HandleStopJumpInput  (const FInputActionValue& Value, ACharacter* const character);
	//static void HandleSprintInput    (const FInputActionValue& Value, ACharacter* const character);
	//static void HandleStopSprintInput(const FInputActionValue& Value, ACharacter* const character);
	static void UpdateCameraRotation (float deltaTime, const ACharacter* character, SCameraRotationState& cameraRotationState, float interpolateSpeed);

	static UInputAction* GetDefaultMoveAction();
	static UInputAction* GetDefaultLookAction();
	static UInputAction* GetDefaultZoomAction();
	static UInputAction* GetDefaultJumpAction();
	static UInputAction* GetDefaultLockTargetAction();

	static UInputAction* GetDefaultAttack1Action();
	static UInputAction* GetDefaultAttack2Action();
	static UInputAction* GetDefaultToggleCombatStateAction();
	static UInputAction* GetDefaultEvadeAction();
	static FName HitBoxCollisionPresetName;
};