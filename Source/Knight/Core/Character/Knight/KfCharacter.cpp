#include "KfCharacter.h"

#include "EnhancedInputComponent.h"
#include "KfCharacterAnimInstance.h"
#include "KfMeleeAttackComponent.h"
#include "KfTargetComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Knight/Core/Common.h"

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
}

AKfCharacter::AKfCharacter(FObjectInitializer const& initializer) {
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::Disabled;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	GetArrowComponent()->SetArrowSize(5.f);

	auto* capsule = GetCapsuleComponent();
	capsule->InitCapsuleSize(200.0f, 900.0f);
	static FName msCollisionPresetName(TEXT("Vehicle"));
	capsule->SetCollisionProfileName(msCollisionPresetName);

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	auto* charMove = GetCharacterMovement();
	charMove->bOrientRotationToMovement = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(USpringArmComponent::StaticClass()->GetFName(), false);
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 3000.0f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 800.f);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagMaxDistance = 1000.f;
	CameraBoom->CameraLagSpeed = 10.f;

	CameraFreeLookSpeed = 1.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(UCameraComponent::StaticClass()->GetFName(), false);
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	MeleeAttackComponent = CreateDefaultSubobject<UKfMeleeAttackComponent>(UKfMeleeAttackComponent::StaticClass()->GetFName(), false);
	_targetComponent = CreateDefaultSubobject<UKfTargetComponent>(UKfTargetComponent::StaticClass()->GetFName(), false);

	_thridPersonCameraBoomState.socketOffset = FVector(0, 700, 550);
	_thridPersonCameraBoomState.targetArmLength = 2000.f;
	_thridPersonCameraBoomState.interpSpeed = 10.f;

	_lockModeCameraBoomState.socketOffset = FVector(0, 0, 750);
	_lockModeCameraBoomState.targetArmLength = 3000.f;
	_lockModeCameraBoomState.interpSpeed = 10.f;

	_lastMoveInput = FVector2D::ZeroVector;
}

void AKfCharacter::BeginPlay() {
	Super::BeginPlay();
	_animInstance = Cast<UKfCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	SetCameraBoomState(_thridPersonCameraBoomState);
}

void AKfCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	ConsumeMovementInput();
	OnUpdateCamera(DeltaTime);
}

void AKfCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	using This = AKfCharacter;
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

void AKfCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) {
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

FVector AKfCharacter::GetLocalInputVector() const {
	const auto worldVel = GetVelocity();
	const auto localVel = ActorToWorld().InverseTransformVectorNoScale(worldVel);
	return localVel;
}

FAttackResult AKfCharacter::ReactToAttack(const FAttackReqeust& req) {
	const double currentTime = GetWorld()->GetTimeSeconds();
	if (!_hurtHistory.IsHurtable(currentTime)) {
		return FAttackResult(false);
	}

	if (_animInstance) _animInstance->PlayHurtMontage();
	_hurtHistory.SetHurtTime(currentTime);
	return FAttackResult(true);
}

void AKfCharacter::ReactToAnimHitDetection(float frameDt, const UHitDetectionNotifyParam& payload) {
	if (MeleeAttackComponent) MeleeAttackComponent->HandleAnimHitDetection(frameDt, payload);
}

void AKfCharacter::ReactToComboWindowNotifyState(bool isBegin, bool isEnd, bool isComboAllowed) {
	if (MeleeAttackComponent) MeleeAttackComponent->SetAllowCombo(isComboAllowed);
}

void AKfCharacter::ReactToComboWindowNotifyState_ResetComboSequence() {
	if (MeleeAttackComponent) MeleeAttackComponent->ResetAttackSequence();
}

FVector AKfCharacter::GetTargetLocation() {
	return GetActorLocation() + GetActorUpVector() * 500.f;
}

void AKfCharacter::OnMoveInput(const FInputActionValue& Value) {
	_lastMoveInput += Value.Get<FVector2D>();
	//KfCharacterCommon::HandleMoveInput_CameraBase(Value, this);
}

void AKfCharacter::OnLookInput(const FInputActionValue& Value) {
	KfCharacterCommon::HandleLookInput(Value, this, _cameraRotationState, CameraFreeLookSpeed);
}

void AKfCharacter::OnJumpInput(const FInputActionValue& Value) {
	_animInstance->PlayEvadeMontage(EEvadeDirection::Forward);
	MeleeAttackComponent->ResetAttackSequence();
}

void AKfCharacter::OnEvadeInput(const FInputActionValue& Value) {
	_animInstance->PlayEvadeMontage(EEvadeDirection::Backward);
	MeleeAttackComponent->ResetAttackSequence();
}

void AKfCharacter::OnStopJumpInput(const FInputActionValue& Value) {
}

void AKfCharacter::OnSprintInput(const FInputActionValue& Value) {
}

void AKfCharacter::OnStopSprintInput(const FInputActionValue& Value) {
}

void AKfCharacter::OnToggleCombatStateInput(const FInputActionValue& Value) {

}

static void _UpdateSpringArm(USpringArmComponent* springArm, const FSpringArmState& state) {
	springArm->SocketOffset = state.socketOffset;
	springArm->TargetArmLength = state.targetArmLength;
}

void AKfCharacter::OnLockTargetInput(const FInputActionValue& Value) {
	if (!_targetComponent) return;
	if (_targetComponent->IsActive()) {
		_targetComponent->ReleaseTarget();
		SetCameraBoomState(_thridPersonCameraBoomState);
		return;
	}

	if (_targetComponent->ScanTarget()) {
		SetCameraBoomState(_lockModeCameraBoomState);
		return;
	}
}

void AKfCharacter::OnUpdateCamera(float deltaTime) {
	const float interpSpeed = _targetCameraBoomState.interpSpeed;
	const auto lerpingOffset = FMath::Lerp(CameraBoom->SocketOffset, _targetCameraBoomState.socketOffset, deltaTime * interpSpeed);
	const auto lerpingLength = FMath::Lerp(CameraBoom->TargetArmLength, _targetCameraBoomState.targetArmLength, deltaTime * interpSpeed);

	// Smooth Damp
	//CameraBoom->SocketOffset = FMath::Damp(CameraBoom->SocketOffset, _targetCameraBoomState.socketOffset, _targetCameraBoomState.socketOffset, 0.1f, 1000.f, deltaTime);
	CameraBoom->SocketOffset = lerpingOffset;
	CameraBoom->TargetArmLength = lerpingLength;
}

void AKfCharacter::SetCameraBoomState(const FSpringArmState& state) {
	_targetCameraBoomState = state;
}

void AKfCharacter::OnAttackInput() {
	//if (MeleeAttackComponent) MeleeAttackComponent->DoMeleeAttack();
	if (MeleeAttackComponent) MeleeAttackComponent->DoMeleeAttack_Directional(_lastMoveInput);
}

void AKfCharacter::OnHeavyAttackInput() {
	if (MeleeAttackComponent) MeleeAttackComponent->DoMeleeAttack_Heavy();
}

void AKfCharacter::ConsumeMovementInput() {
	_animInstance->SetMovementInput(_lastMoveInput);
	_lastMoveInput = FVector2D::ZeroVector;
}