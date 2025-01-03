﻿#include "KfKnightCharacter.h"
#include "EnhancedInputComponent.h"
#include "KfCharacterAnimInstance.h"
#include "KfMeleeAttackComponent.h"
#include "KfTargetComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CharacterTrajectoryComponent.h"
#include "MotionWarpingComponent.h"

static const bool AKfCharacter_USE_MOVEMENT_COMPONENT = false;

AKfKnightCharacter::AKfKnightCharacter(const FObjectInitializer& initializer) {
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorld;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	GetArrowComponent()->SetArrowSize(5.f);

	auto* capsule = GetCapsuleComponent();
	capsule->InitCapsuleSize(200.0f, 900.0f);
	static FName msCollisionPresetName(TEXT("Vehicle"));
	capsule->SetCollisionProfileName(msCollisionPresetName);

	_characterMovement = GetCharacterMovement();

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(USpringArmComponent::StaticClass()->GetFName());
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 3000.0f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 800.f);

	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagMaxDistance = 1000.f;
	CameraBoom->CameraLagSpeed = 10.f;
	CameraFreeLookSpeed = 1.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(UCameraComponent::StaticClass()->GetFName());
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Rotate the arm instead of the camera
	CameraBoom->bUsePawnControlRotation = true;
	FollowCamera->bUsePawnControlRotation = false;

	// Prevent character from instanly rotating to controller rotation.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	_characterMovement->bOrientRotationToMovement = false;
	_characterMovement->bUseControllerDesiredRotation = true; // will slowly rotate character to desired rotation, using RotationRate
	_characterMovement->RotationRate = FRotator(0.0f, 225.0f, 0.0f);

	_characterTrajectory = CreateDefaultSubobject<UCharacterTrajectoryComponent>(UCharacterTrajectoryComponent::StaticClass()->GetFName());

	MeleeAttackComponent = CreateDefaultSubobject<UKfMeleeAttackComponent>(UKfMeleeAttackComponent::StaticClass()->GetFName());
	MeleeAttackComponent->SetHitBoxRadius(130.f);
	MeleeAttackComponent->SetHitBoxLength(1500.f);

	_targetComponent = CreateDefaultSubobject<UKfTargetComponent>(UKfTargetComponent::StaticClass()->GetFName());

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(UMotionWarpingComponent::StaticClass()->GetFName());
	_thirdPersonCameraBoomState.springArmState.socketOffset = FVector(0, 700, 550);
	_thirdPersonCameraBoomState.springArmState.targetArmLength = 2000.f;
	_thirdPersonCameraBoomState.springArmState.interpSpeed = 10.f;

	_lockModeCameraBoomState.springArmState.socketOffset = FVector(0, 450, 450);
	_lockModeCameraBoomState.springArmState.targetArmLength = 2000.f;
	_lockModeCameraBoomState.springArmState.interpSpeed = 10.f;

	_lastMoveInput = FVector2f::ZeroVector;
}

void AKfKnightCharacter::BeginPlay() {
	Super::BeginPlay();
	_animInstance = Cast<UKfCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	SetCameraBoomConfig(&_thirdPersonCameraBoomState);
}

void AKfKnightCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	ConsumeMovementInput();
}

void AKfKnightCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	using This = AKfKnightCharacter;
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* enInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!enInput) return;

	const auto& ia = _inputActionSet;

	if (ia.MoveAction) {
		enInput->BindAction(ia.MoveAction, ETriggerEvent::Triggered, this, &This::OnMoveInput);
	}

	if (ia.LookAction) {
		enInput->BindAction(ia.LookAction, ETriggerEvent::Triggered, this, &This::OnLookInput);
	}

	if (ia.JumpAction) {
		enInput->BindAction(ia.JumpAction, ETriggerEvent::Triggered, this, &This::OnJumpInput);
		enInput->BindAction(ia.JumpAction, ETriggerEvent::Completed, this, &This::OnStopJumpInput);
	}

	if (ia.EvadeAction) {
		enInput->BindAction(ia.EvadeAction, ETriggerEvent::Triggered, this, &This::OnEvadeInput);
	}

	if (ia.SprintAction) {
		enInput->BindAction(ia.SprintAction, ETriggerEvent::Triggered, this, &This::OnSprintInput);
		enInput->BindAction(ia.SprintAction, ETriggerEvent::Completed, this, &This::OnStopSprintInput);
	}

	if (ia.ToggleCombatStateAction) {
		enInput->BindAction(ia.ToggleCombatStateAction, ETriggerEvent::Triggered, this, &This::OnToggleCombatStateInput);
	}

	if (ia.LockTargetAction) {
		enInput->BindAction(ia.LockTargetAction, ETriggerEvent::Started, this, &This::OnLockTargetInput);
	}

	if (ia.Attack1Action) {
		enInput->BindAction(ia.Attack1Action, ETriggerEvent::Triggered, this, &This::OnAttackInput);
	}

	if (ia.Attack2Action) {
		enInput->BindAction(ia.Attack2Action, ETriggerEvent::Triggered, this, &This::OnHeavyAttackInput);
	}
}

void AKfKnightCharacter::AddMovementInput(const FVector WorldDirection, float ScaleValue, bool bForce) {
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

FVector AKfKnightCharacter::GetLocalInputVector() const {
	const auto worldVel = GetVelocity();
	const auto localVel = ActorToWorld().InverseTransformVectorNoScale(worldVel);
	return localVel;
}

FAttackResult AKfKnightCharacter::ReactToAttack(const FAttackRequest& req) {
	const double currentTime = GetWorld()->GetTimeSeconds();
	if (!_hurtHistory.IsHurtable(currentTime)) {
		return FAttackResult(false);
	}

	if (_animInstance) _animInstance->PlayHurtMontage();
	_hurtHistory.SetHurtTime(currentTime);
	return FAttackResult(true);
}

void AKfKnightCharacter::ReactToAnimHitDetection(float frameDeltaTime, const UHitDetectionNotifyParam& payload) {
	if (MeleeAttackComponent) MeleeAttackComponent->HandleAnimHitDetection(frameDeltaTime, payload);
}

void AKfKnightCharacter::ReactToComboWindowNotifyState(bool isBegin, bool isEnd, bool isComboAllowed) {
	SetCharacterOrientToCamera(isComboAllowed);
	if (MeleeAttackComponent) MeleeAttackComponent->SetAllowCombo(isComboAllowed);
}

void AKfKnightCharacter::ReactToComboWindowNotifyState_ResetComboSequence() {
	SetCharacterOrientToCamera(true);
	if (MeleeAttackComponent) MeleeAttackComponent->ResetAttackSequence();
}

FVector AKfKnightCharacter::GetTargetLocation() {
	return GetActorLocation() + GetActorUpVector() * 500.f;
}

void AKfKnightCharacter::OnMoveInput(const FInputActionValue& Value) {
	const auto v2d =  Value.Get<FVector2D>();
	_lastMoveInput += FVector2f(v2d.X, v2d.Y);

	if (AKfCharacter_USE_MOVEMENT_COMPONENT) {
		KfCharacterCommon::HandleMoveInput_CameraBase(Value, this);
	}
}

void AKfKnightCharacter::OnLookInput(const FInputActionValue& Value) {
	KfCharacterCommon::HandleLookInput(Value, this, _cameraRotationState, CameraFreeLookSpeed);
}

void AKfKnightCharacter::OnJumpInput(const FInputActionValue& Value) {
	_animInstance->PlayEvadeMontage(EEvadeDirection::Forward);
	MeleeAttackComponent->ResetAttackSequence();
}

void AKfKnightCharacter::OnEvadeInput(const FInputActionValue& Value) {
	_animInstance->PlayEvadeMontage(EEvadeDirection::Backward);
	MeleeAttackComponent->ResetAttackSequence();
}

void AKfKnightCharacter::OnStopJumpInput(const FInputActionValue& Value) {}

void AKfKnightCharacter::OnSprintInput(const FInputActionValue& Value) { _animInstance->aIsSprinting = true; }

void AKfKnightCharacter::OnStopSprintInput(const FInputActionValue& Value) { _animInstance->aIsSprinting = false; }

void AKfKnightCharacter::OnToggleCombatStateInput(const FInputActionValue& Value) {}

void AKfKnightCharacter::OnLockTargetInput(const FInputActionValue& Value) {
	if (!_targetComponent) return;
	const bool hasTarget = _targetComponent->ToggleTargetMode();
	SetCameraBoomConfig(hasTarget ? &_lockModeCameraBoomState : &_thirdPersonCameraBoomState);
}

void AKfKnightCharacter::OnUpdateCamera(float deltaTime) {
	if (_currentSpringArmConfig) _currentSpringArmConfig->springArmState.InterpPosition(CameraBoom, deltaTime);
}

void AKfKnightCharacter::SetCameraBoomConfig(FCameraConfig* config) {
	_currentSpringArmConfig = config;
	if (config) config->springArmState.ApplyBool(CameraBoom);
}

void AKfKnightCharacter::SetCharacterOrientToCamera(bool shouldOrient) {
	if (_characterMovement) {
		_characterMovement->bUseControllerDesiredRotation = shouldOrient;
	}
}

void AKfKnightCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) {
	OnUpdateCamera(DeltaTime);
#if 0
	if (_targetComponent && _targetComponent->hasTarget()) {
		_targetComponent->CalcCamera(DeltaTime, FollowCamera, _currentSpringArmConfig, OutResult);
	}
#endif

	Super::CalcCamera(DeltaTime, OutResult);
}

void AKfKnightCharacter::OnAttackInput() {
	/*if (MotionWarpingComponent && _targetComponent) {
		const FName attackWrapTargetName = "target";
		if (_targetComponent->hasTarget()) {
			MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(attackWrapTargetName, _targetComponent->GetTargetLocation());
		} else {
			MotionWarpingComponent->RemoveWarpTarget(attackWrapTargetName);
		}
	}*/

	float duration;
	if (MeleeAttackComponent) MeleeAttackComponent->DoMeleeAttack_Directional_FromVector2(_lastMoveInput, duration);
}

void AKfKnightCharacter::OnHeavyAttackInput() {
	if (MeleeAttackComponent) MeleeAttackComponent->DoMeleeAttack_Heavy();
}

void AKfKnightCharacter::ConsumeMovementInput() {
	_animInstance->SetMovementInput(_lastMoveInput);
	_lastMoveInput = FVector2f::ZeroVector;
}