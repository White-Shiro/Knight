#pragma once

#include "KfCharacterCommon.generated.h"

struct FInputActionValue;
class USpringArmComponent;
class UInputAction;
class ACharacter;

USTRUCT(BlueprintType)
struct FKfCharacterInputActionSet {
	GENERATED_BODY()

	FKfCharacterInputActionSet();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EvadeAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LockTargetAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ToggleCombatStateAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Attack1Action = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Attack2Action = nullptr;
};

USTRUCT(BlueprintType)
struct FSpringArmState {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector socketOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float targetArmLength = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float interpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool usePawnControllerRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool useCameraLag = true;

	void InterpPosition(USpringArmComponent* springArm, float deltaTime) const;
	void ApplyPosition(USpringArmComponent* springArm) const;
	void ApplyBool(USpringArmComponent* springArm) const;
};

USTRUCT(BlueprintType)
struct FCameraConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSpringArmState springArmState;
};

USTRUCT(BlueprintType)
struct FCameraRotationState {
	GENERATED_BODY()

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
	static void HandleTurnInput		 (const FInputActionValue& Value, ACharacter* character, FCameraRotationState& cameraRotationState);
	static void HandleLookInput      (const FInputActionValue& Value, ACharacter* character, FCameraRotationState& cameraRotationState, float cameraLookSpeed);
	static void HandleZoomInput      (const FInputActionValue& Value, ACharacter* character, class USpringArmComponent* cameraBoom, float zoomSpeed ,const FVector2f& clampRange);
	//static void HandleJumpInput      (const FInputActionValue& Value, ACharacter* const character);
	//static void HandleStopJumpInput  (const FInputActionValue& Value, ACharacter* const character);
	//static void HandleSprintInput    (const FInputActionValue& Value, ACharacter* const character);
	//static void HandleStopSprintInput(const FInputActionValue& Value, ACharacter* const character);
	static void UpdateCameraRotation (float deltaTime, const ACharacter* character, FCameraRotationState& cameraRotationState, float interpolateSpeed);

	static UInputAction* GetDefaultMoveAction();
	static UInputAction* GetDefaultLookAction();
	static UInputAction* GetDefaultZoomAction();
	static UInputAction* GetDefaultJumpAction();
	static UInputAction* GetDefaultLockTargetAction();

	static UInputAction* GetDefaultAttack1Action();
	static UInputAction* GetDefaultAttack2Action();
	static UInputAction* GetDefaultToggleCombatStateAction();
	static UInputAction* GetDefaultEvadeAction();
	static UInputAction* GetDefaultSprintAction();
	static FName HitBoxCollisionPresetName;
};