#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Knight/Core/Character/KfCharacterCommon.h"
#include "Knight/Core/Combat/Combat.h"
#include "KfKnightCharacter.generated.h"

UCLASS()
class KNIGHT_API AKfKnightCharacter final : public ACharacter,
                                      public ITargetable,
                                      public IReactToAttack,
                                      public IReactToAnimHitDectectNotifyState,
                                      public IReactToComboWindowNotifyState {

private:
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	class UKfMeleeAttackComponent* MeleeAttackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Target, meta = (AllowPrivateAccess = "true"))
	class UKfTargetComponent* _targetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraFreeLookSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FKfCharacterInputActionSet _inputActionSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FCameraConfig _lockModeCameraBoomState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FCameraConfig _thirdPersonCameraBoomState;

	UPROPERTY()
	TObjectPtr<class UKfCharacterAnimInstance> _animInstance;

	UPROPERTY()
	TObjectPtr<class UCharacterMovementComponent> _characterMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Motion, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterTrajectoryComponent> _characterTrajectory;

	FCameraRotationState _cameraRotationState;
	FVector2d _lastMoveInput;
	FCameraConfig* _currentSpringArmConfig = nullptr;
	FHurtHistory _hurtHistory;

public:
	AKfKnightCharacter(const FObjectInitializer& initializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) override;
	FVector GetLocalInputVector() const;

	virtual FVector GetTargetLocation() override;
	FORCEINLINE FVector GetCameraLocation() const { return FollowCamera->GetComponentLocation(); }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCharacterTrajectoryComponent* GetCharacterTrajectory() const { return _characterTrajectory.Get(); }

	virtual FAttackResult ReactToAttack(const FAttackRequest& req) override;
	virtual void ReactToAnimHitDetection(float frameDeltaTime, const UHitDetectionNotifyParam& payload) override;
	virtual void ReactToComboWindowNotifyState(const bool isBegin, const bool isEnd, const bool isComboAllowed) override;
	virtual void ReactToComboWindowNotifyState_ResetComboSequence() override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

	FORCEINLINE FVector2d GetLastMovementInput() const { return _lastMoveInput; }

protected:
	void OnMoveInput(const FInputActionValue& Value);
	void OnLookInput(const FInputActionValue& Value);
	void OnJumpInput(const FInputActionValue& Value);
	void OnEvadeInput(const FInputActionValue& Value);
	void OnStopJumpInput(const FInputActionValue& Value);
	void OnSprintInput(const FInputActionValue& Value);
	void OnStopSprintInput(const FInputActionValue& Value);
	void OnToggleCombatStateInput(const FInputActionValue& Value);
	void OnLockTargetInput(const FInputActionValue& Value);
	void OnAttackInput();
	void OnHeavyAttackInput();
	void ConsumeMovementInput();
	void OnUpdateCamera(float deltaTime);
	void SetCameraBoomConfig(FCameraConfig* config);
	void SetCharacterOrientToCamera(bool shouldOrient);

public:
	FORCEINLINE static AKfKnightCharacter* CastFrom(AActor* actor) { return Cast<AKfKnightCharacter>(actor); }
};