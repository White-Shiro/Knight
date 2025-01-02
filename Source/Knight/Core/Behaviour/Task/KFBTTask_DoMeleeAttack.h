#pragma once

#include "CoreMinimal.h"
#include "KFBTTaskNodeBase.h"
#include "KFBTTask_DoMeleeAttack.generated.h"

USTRUCT(BlueprintType)
struct FOptionalFloat {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsEnabled", EditConditionHides, ClampMin = "0.0"))
	float Value = 0;
};


UCLASS()
class KNIGHT_API UKFBTTask_DoMeleeAttack : public UKFBTTaskNodeBase {
	GENERATED_BODY()
protected:

	UKFBTTask_DoMeleeAttack(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());
	UPROPERTY(Category= Node, EditAnywhere)
	EAttackInputDirection attackDirection;

	UPROPERTY(Category= Node, EditAnywhere)
	FOptionalFloat specificDuration;

	virtual EBTNodeResult::Type OnExecuteTask_Knight(UBehaviorTreeComponent& OwnerComp, AKfKnightAIPawn* KnightAIPawn, uint8* NodeMemory) override;
	virtual void OnTickTask_Knight(UBehaviorTreeComponent& OwnerComp, AKfKnightAIPawn* KnightAIPawn, uint8* NodeMemory, float DeltaSeconds) override;
};