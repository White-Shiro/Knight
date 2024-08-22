#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Knight/Core/Audio/SoundRequest.h"
#include "Knight/Core/Combat/CombatCommon.h"
#include "KfMeleeAttackComponent.generated.h"

USTRUCT(BlueprintType)
struct FSwordHitEffectSet {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	TObjectPtr<UParticleSystem> hitEffect_Pawn = nullptr;

	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	TObjectPtr<UParticleSystem> hitEffect_Ground = nullptr;

	UPROPERTY(EditAnywhere, Category = "Hit Effect")
	FVector effectSize = FVector::OneVector; // Jason: very minor

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

	TObjectPtr<class UKfCharacterAnimInstance> _animInstance; // = nullptr; Jason no needed

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool _debug = false;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool _persistentLine = false;

	UPROPERTY(EditAnywhere, Category = "Melee State")
	FMeleeSequenceState _meleeSequenceState;

public:
	UKfMeleeAttackComponent();

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
	FTraceDelegate _traceDelegate;
	FTraceHandle _traceHandle;

	virtual void BeginPlay() override;
	virtual void OnAnimationInstanceInit(UKfCharacterAnimInstance* animInstance);

	UFUNCTION()
	virtual void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	UFUNCTION()
	virtual void OnMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	bool DoSwingHits(const struct UHitDetectionNotifyParam& param);
	bool TrySwordHitOnce(const FVector& start, const FVector& direction, bool isSegment = false);
	FTraceHandle TraceSwordHits(const FVector& start, const FVector& direction, float distance, float swordRad, bool isSegment = false) const;
	void OnSwordHitResult(const FHitResult& hitResult) const;
	virtual void onTraceCompleted(const FTraceHandle& handle, FTraceDatum& data);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DoMeleeAttack();
	void DoMeleeAttack_Directional(const FVector2d& movementInput);
	void DoMeleeAttack_Heavy();
	void HandleAnimHitDetection(float frameDt, const struct UHitDetectionNotifyParam& param);

	void ResetAttackSequence();
	void SetAllowCombo(bool bAllowCombo);

#if WITH_EDITOR

private:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};