#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Knight/Core/Character/KfCharacterCommon.h"
#include "Knight/Core/Combat/CombatInterface.h"
#include "KfCharacter.generated.h"

USTRUCT(BlueprintType)
struct FKfCharacterInputActionSet {
	GENERATED_BODY()

	FKfCharacterInputActionSet();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction = nullptr;

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
};

UCLASS()
class KNIGHT_API AKfCharacter final : public ACharacter, public IReactToAttack,
                                      public IReactToAnimHitDectectNotifyState, public ITargetable {


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

	SCameraRotationState _cameraRotationState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FKfCharacterInputActionSet _inputActionSet;

	TObjectPtr<class UKfCharacterAnimInstance> _animInstance = nullptr;

public:
	explicit AKfCharacter(FObjectInitializer const& initializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) override;
	FVector GetLocalInputVector() const;

	virtual FAttackResult ReactToAttack(const FAttackReqeust& req) override;
	virtual void ReactToAnimHitDetection(float frameDt, const UHitDetectionNotifyParam& payload) override;
	virtual FVector GetTargetLocation() override;
	FORCEINLINE FVector GetCameraLocation() const { return FollowCamera->GetComponentLocation(); }

protected:
	void OnMoveInput(const FInputActionValue& Value);
	void OnLookInput(const FInputActionValue& Value);
	void OnJumpInput(const FInputActionValue& Value);
	void OnStopJumpInput(const FInputActionValue& Value);
	void OnSprintInput(const FInputActionValue& Value);
	void OnStopSprintInput(const FInputActionValue& Value);
	void OnToggleCombatStateInput(const FInputActionValue& Value);
	void OnLockTargetInput(const FInputActionValue& Value);
	void OnAttackInput();
	void OnHeavyAttackInput();
	void OnUpdateCamera(float deltaTime);
};
