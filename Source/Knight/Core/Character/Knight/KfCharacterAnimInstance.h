﻿#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Knight/Core/Audio/SoundRequest.h"
#include "Knight/Core/Effect/VFXRequest.h"
#include "Knight/Core/Animation/KfFootStepAnimNotify.h"
#include "Knight/Core/Combat/Combat.h"
#include "KfCharacterAnimInstance.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FAttackAnimationSet {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Motion")
	UAnimMontage* attackMontage_NA_1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Motion")
	UAnimMontage* attackMontage_NA_2 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Motion")
	UAnimMontage* attackMontage_NA_3 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Motion")
	UAnimMontage* attackMontage_HA_1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Motion")
	UAnimMontage* attackMontage_NA_Normal = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Motion")
	UAnimMontage* attackMontage_NA_Left = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Motion")
	UAnimMontage* attackMontage_NA_Right = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Motion")
	UAnimMontage* attackMontage_NA_Down = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hurt Motion")
	UAnimMontage* hurtMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evade Motion")
	UAnimMontage* evadeMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evade Motion")
	UAnimMontage* evade_Back_Montage = nullptr;
};

UCLASS()
class KNIGHT_API UKfCharacterAnimInstance : public UAnimInstance {
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Variables")
	FVector2f aMoveDirection = FVector2f::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Variables")
	float aMoveAnimPlaySpeed = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Variables")
	bool aIsWalking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Variables")
	bool aIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Variables")
	EFootStepType aCurrentForwardFoot = EFootStepType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim Variables")
	FAttackAnimationSet attackAnimationSet;

	UPROPERTY(BlueprintReadOnly, Category = "Anim Variables")
	TObjectPtr<class UCharacterTrajectoryComponent> characterTrajectory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool logDirAndSpeed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Walk")
	UBlendSpace* walkBlendSpace = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Walk")
	FSoundRequest footStepSoundRequest;

	UPROPERTY(EditAnywhere, Blueprintable, Category = "Walk")
	FVFXRequest footStepVFXRequest;

	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void SyncWalkPlaySpeed(const FVector& worldVel, const FVector& localInputV, float meshScale);
	void SetWalkBlendSpaceDirection2D(const FVector2f& localInputV);
	void SetWalkBlendSpaceDirection1D(const FVector& worldVel, const float maxSpeed);
	void SetMovementInput(const FVector2f& inputV);

	float PlayMeleeMontage(int32 montageIndex);
	float PlayMeleeMontage_Directional(const EAttackInputDirection atkDir);
	void PlayHurtMontage();
	void PlayEvadeMontage(const EEvadeDirection evadeDir = EEvadeDirection::Backward);
	void NotifyFootStep(const FFootStepEvent& footStep);

private:
	static constexpr float WALK_BLENDSPACE_AXIS_SCALE = 100.f;

	TObjectPtr<class AKfKnightCharacter> _knightCh;
	FVector2f _lastMoveInput;

	enum class EWalkDirection {
		Forward,
		Backward,
		Left,
		Right
	};

	static EWalkDirection GetWalkDirection(const FVector2d& inputV);
};