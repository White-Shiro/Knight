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

FKfCharacterInputActionSet::FKfCharacterInputActionSet() {
	MoveAction = KfCharacterCommon::GetDefaultMoveAction();
	LookAction = KfCharacterCommon::GetDefaultLookAction();
	ZoomAction = KfCharacterCommon::GetDefaultZoomAction();
	JumpAction = KfCharacterCommon::GetDefaultJumpAction();
	LockTargetAction = KfCharacterCommon::GetDefaultLockTargetAction();
	ToggleCombatStateAction = KfCharacterCommon::GetDefaultToggleCombatStateAction();
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

	bUseControllerRotationYaw = false;
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
}

void AKfCharacter::BeginPlay() {
	Super::BeginPlay();
	_animInstance = Cast<UKfCharacterAnimInstance>(GetMesh()->GetAnimInstance());
}

void AKfCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	OnUpdateCamera(DeltaTime);
}

void AKfCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	using This = AKfCharacter;
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &This::OnAttackInput);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &This::OnHeavyAttackInput);

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
	if (_animInstance) _animInstance->PlayHurtMontage();
	return FAttackResult(true);
}

void AKfCharacter::ReactToAnimHitDetection(float frameDt, const UHitDetectionNotifyParam& payload) {
	if(MeleeAttackComponent) MeleeAttackComponent->HandleAnimHitDetection(frameDt, payload);
}

FVector AKfCharacter::GetTargetLocation() {
	return GetActorLocation() + GetActorUpVector() * 500.f;
}

void AKfCharacter::OnMoveInput(const FInputActionValue& Value) {
	KfCharacterCommon::HandleMoveInput_CameraBase(Value, this);
}

void AKfCharacter::OnLookInput(const FInputActionValue& Value) {
	KfCharacterCommon::HandleLookInput(Value, this, _cameraRotationState, CameraFreeLookSpeed);
}

void AKfCharacter::OnJumpInput(const FInputActionValue& Value) {
}

void AKfCharacter::OnStopJumpInput(const FInputActionValue& Value) {
}

void AKfCharacter::OnSprintInput(const FInputActionValue& Value) {
}

void AKfCharacter::OnStopSprintInput(const FInputActionValue& Value) {
}

void AKfCharacter::OnToggleCombatStateInput(const FInputActionValue& Value) {

}

void AKfCharacter::OnLockTargetInput(const FInputActionValue& Value) {
	if (!_targetComponent) return;
	if (_targetComponent->IsActive()) {
		_targetComponent->ReleaseTarget();
	} else {
		_targetComponent->ScanTarget();
	}
}

void AKfCharacter::OnUpdateCamera(float deltaTime) {
}

void AKfCharacter::OnAttackInput() {
	MeleeAttackComponent->DoMeleeAttack();
}

void AKfCharacter::OnHeavyAttackInput() {
	const auto ins = GetMesh()->GetAnimInstance();
	if (auto* kAnim = Cast<UKfCharacterAnimInstance>(ins)) {
		kAnim->PlayMeleeMontage(11);
	}
}