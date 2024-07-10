#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Knight/Core/Character/KfCharacterCommon.h"
#include "Knight/Core/Combat/CombatCommon.h"
#include "KfCharacter.generated.h"

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
	FVector socketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float targetArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float interpSpeed = 10.0f;
};

UCLASS()
class KNIGHT_API AKfCharacter final : public ACharacter,
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraFreeLookSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FKfCharacterInputActionSet _inputActionSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FSpringArmState _lockModeCameraBoomState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FSpringArmState _thridPersonCameraBoomState;

	TObjectPtr<class UKfCharacterAnimInstance> _animInstance = nullptr;
	SCameraRotationState _cameraRotationState;
	FVector2d _lastMoveInput;
	FSpringArmState _targetCameraBoomState;
	FHurtHistory _hurtHistory;

public:
	explicit AKfCharacter(FObjectInitializer const& initializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) override;
	FVector GetLocalInputVector() const;

	virtual FVector GetTargetLocation() override;
	FORCEINLINE FVector GetCameraLocation() const { return FollowCamera->GetComponentLocation(); }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual FAttackResult ReactToAttack(const FAttackReqeust& req) override;
	virtual void ReactToAnimHitDetection(float frameDt, const UHitDetectionNotifyParam& payload) override;
	virtual void ReactToComboWindowNotifyState(const bool isBegin, const bool isEnd, const bool isComboAllowed) override;
	virtual void ReactToComboWindowNotifyState_ResetComboSequence() override;

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
	void SetCameraBoomState(const FSpringArmState& state);

public:
	FORCEINLINE static AKfCharacter* CastFrom(AActor* actor) { return Cast<AKfCharacter>(actor); }
};
