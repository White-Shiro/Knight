#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KfKnightAIPawn.generated.h"

UCLASS()
class AKfKnightAIPawn : public ACharacter {
	GENERATED_BODY()

public:
	explicit AKfKnightAIPawn(const FObjectInitializer& initializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	UFUNCTION()
	FORCEINLINE class UKfMeleeAttackComponent* GetMeleeAttackComponent() const { return MeleeAttackComponent; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "AI", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UPathFollowingComponent> PathFollowingComponent;

	// Behaviour Tree
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "AI", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTree;

	UPROPERTY()
	TObjectPtr<class UKfCharacterAnimInstance> _animInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	class UKfMeleeAttackComponent* MeleeAttackComponent;
};