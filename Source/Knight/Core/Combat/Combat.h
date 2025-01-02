#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Combat.generated.h"

UENUM()
enum class EEvadeDirection {
	None,
	Forward,
	Backward,
	Left,
	Right
};

UENUM()
enum class EAttackInputDirection {
	Normal,
	Up,
	Down,
	Left,
	Right,
};

USTRUCT(BlueprintType)
struct FHitStop {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bIsEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsEnabled", EditConditionHides, ClampMin = "0.01"))
	float hitStopDuration = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsEnabled", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
	float hitStopTimeScale = 0.05f;
};

USTRUCT(BlueprintType)
struct FHitFeedBackDefinition {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FHitStop hitStopOnHit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FHitStop hitStopOnBlock;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> cameraShakeClass;
};

USTRUCT(BlueprintType)
struct FCombatEffectRequest {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FHitFeedBackDefinition hitFeedback;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVFXRequest vfxRequest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSoundRequest soundRequest;
};

struct FAttackRequest {
	float damage = 0;
};

struct FHurtHistory {
	const float HURT_INTERVAL = 0.1f;
	float lastHurtTime = 0.0f;
	FORCEINLINE void SetHurtTime(const double time) { lastHurtTime = time; }
	FORCEINLINE bool IsHurtable(const double currentTime) const { return (currentTime - lastHurtTime) > HURT_INTERVAL; }
};

struct FAttackResult {
	explicit FAttackResult(bool success_) : success(success_) {

	}

	bool success = false;
};

UINTERFACE()
class UReactToAttack : public UInterface {
	GENERATED_BODY()
};

class KNIGHT_API IReactToAttack {
	GENERATED_BODY()

public:
	virtual FAttackResult ReactToAttack(const FAttackRequest& req) = 0;
};

UINTERFACE()
class UReactToAnimHitDectectNotifyState : public UInterface {
	GENERATED_BODY()
};

class KNIGHT_API IReactToAnimHitDectectNotifyState {
	GENERATED_BODY()

public:
	virtual void ReactToAnimHitDetection(float frameDt, const struct UHitDetectionNotifyParam& payload) = 0;
};

UINTERFACE()
class UReactToComboWindowNotifyState : public UInterface {
	GENERATED_BODY()
};

class IReactToComboWindowNotifyState {
	GENERATED_BODY()

public:
	virtual void ReactToComboWindowNotifyState(bool isBegin, bool isEnd, bool isAllowCombo) = 0;
	virtual void ReactToComboWindowNotifyState_ResetComboSequence() = 0;
};


UINTERFACE()
class UTargetable : public UInterface {
	GENERATED_BODY()
};

class ITargetable {
	GENERATED_BODY()

public:
	virtual FVector GetTargetLocation() = 0;
};

class CombatUtils {
	static constexpr float DIRECTIONAL_INPUT_DEADZONE = 0.2f;
public:
	FORCEINLINE static EAttackInputDirection GetAttackInputDirection(const FVector2f& direction) {
		if (direction.X > DIRECTIONAL_INPUT_DEADZONE) return EAttackInputDirection::Right;
		if (direction.X < -DIRECTIONAL_INPUT_DEADZONE) return EAttackInputDirection::Left;
		if (direction.Y > DIRECTIONAL_INPUT_DEADZONE) return EAttackInputDirection::Up;
		if (direction.Y < -DIRECTIONAL_INPUT_DEADZONE) return EAttackInputDirection::Down;
		return EAttackInputDirection::Normal;
	}

	FORCEINLINE static EEvadeDirection GetEvadeDirection(const FVector2f& direction) {
		if (direction.X > DIRECTIONAL_INPUT_DEADZONE) return EEvadeDirection::Right;
		if (direction.X < -DIRECTIONAL_INPUT_DEADZONE) return EEvadeDirection::Left;
		if (direction.Y > DIRECTIONAL_INPUT_DEADZONE) return EEvadeDirection::Forward;
		if (direction.Y < -DIRECTIONAL_INPUT_DEADZONE) return EEvadeDirection::Backward;
		return EEvadeDirection::None;
	}
};