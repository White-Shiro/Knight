#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Knight/Core/Combat/Combat.h"
#include "KfKnightAIPawn.generated.h"

UCLASS()
class AKfKnightAIPawn : public ACharacter,
                        public ITargetable,
                        public IReactToAttack,
                        public IReactToAnimHitDectectNotifyState,
                        public IReactToComboWindowNotifyState {
public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) override;

	virtual FAttackResult ReactToAttack(const FAttackRequest& req) override;
	virtual void ReactToAnimHitDetection(float frameDeltaTime, const UHitDetectionNotifyParam& payload) override;
	virtual void ReactToComboWindowNotifyState(bool isBegin, bool isEnd, bool isComboAllowed) override;
	virtual void ReactToComboWindowNotifyState_ResetComboSequence() override;
	virtual FVector GetTargetLocation() override;

private:
	GENERATED_BODY()

public:
	explicit AKfKnightAIPawn(const FObjectInitializer& initializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	UFUNCTION()
	FORCEINLINE class UKfMeleeAttackComponent* GetMeleeAttackComponent() const { return MeleeAttackComponent; }
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTreeAsset; }

private:
	FHurtHistory _hurtHistory;
	FVector2d _lastMoveInput;
	// Behaviour Tree
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY()
	TObjectPtr<class UKfCharacterAnimInstance> _animInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	class UKfMeleeAttackComponent* MeleeAttackComponent;

protected:
	void ConsumeMovementInput();
};