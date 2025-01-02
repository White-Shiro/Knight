#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Knight/Core/Audio/SoundRequest.h"
#include "Knight/Core/Effect/VFXRequest.h"
#include "Knight/Core/Combat/Combat.h"
#include "KfMeleeAttackComponent.generated.h"

USTRUCT(BlueprintType)
struct FSwordHitEffectSet {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	bool bUseHitStop = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect", meta = (EditCondition = "bUseHitStop", EditConditionHides, ClampMin = "0.001"))
	float hitStopDuration = 0.005f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect", meta = (EditCondition = "bUseHitStop", EditConditionHides, ClampMin = "0.001", ClampMax = "1.0"))
	float hitStopTimeScale = 0.05f;

	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	FVFXRequest hitEffect_Pawn;

	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	FVFXRequest hitEffect_Ground;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FSoundRequest hitSoundRequest_Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FSoundRequest hitSoundRequest_Ground;
};

USTRUCT(BlueprintType)
struct FMeleeSequenceState {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bAllowCombo = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int comboSequence = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EAttackInputDirection lastAttackDirection = EAttackInputDirection::Normal;

	FORCEINLINE bool isSequenceRunning() const { return comboSequence > 0; }
};

UCLASS(ClassGroup=(Knight), meta=(BlueprintSpawnableComponent))
class KNIGHT_API UKfMeleeAttackComponent : public UActorComponent {
	GENERATED_BODY()

	UKfMeleeAttackComponent();

	UPROPERTY()
	TObjectPtr<class UKfCharacterAnimInstance> _animInstance = nullptr;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool _debug = false;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool _persistentLine = false;

	UPROPERTY(EditAnywhere, Category = "Melee State")
	FMeleeSequenceState _meleeSequenceState;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float _hitBoxRadius = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	float _hitBoxLength = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FSwordHitEffectSet _swordHitEffectSet;

	UPROPERTY(EditAnywhere, Category = "Hit Detection")
	bool _fillHitDetectionGap = true;

protected:

	struct FTraceDirectionInfo {
		FVector start;
		FVector unitDirection;
		int sequenceId = INT_MIN;
	};

	FTraceDirectionInfo prevTraceDirection;
	FTraceDelegate traceDelegate;
	FTraceHandle traceHandle;

	FTimerHandle hitStopTimerHandle;

	virtual void BeginPlay() override;
	virtual void OnAnimationInstanceInit(UKfCharacterAnimInstance* animInstance);

	UFUNCTION()
	virtual void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	virtual void OnMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	bool DoSwingHits(const struct UHitDetectionNotifyParam& param);
	bool TrySwordHitOnce(const FVector& start, const FVector& direction, bool isSegment = false);
	FTraceHandle TraceSwordHits(const FVector& start, const FVector& direction, float distance, float swordRad, bool isSegment = false) const;
	void OnSwordHitResult(const FHitResult& hitResult);
	virtual void OnTraceCompleted(const FTraceHandle& handle, FTraceDatum& data);
	virtual void ScheduleHitStop(float duration, float timeScale);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE void SetHitBoxRadius(float radius) { _hitBoxRadius = radius; }
	FORCEINLINE void SetHitBoxLength(float length) { _hitBoxLength = length; }

	UFUNCTION()
	void DoMeleeAttack();

	UFUNCTION()

	bool DoMeleeAttack_Directional_FromVector2(const FVector2f& movementInput, float& outDuration);
	UFUNCTION()
	bool DoMeleeAttack_Directional(EAttackInputDirection meleeDirection, float& outDuration);

	UFUNCTION()
	void DoMeleeAttack_Heavy();
	void HandleAnimHitDetection(float frameDt, const struct UHitDetectionNotifyParam& param);

	void ResetAttackSequence();
	void SetAllowCombo(bool bAllowCombo);

	FMeleeSequenceState GetMeleeSequenceState() const { return _meleeSequenceState; }

#if WITH_EDITOR

private:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};