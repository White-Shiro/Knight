#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "UKFBTService_RotateToward.generated.h"

UCLASS()
class KNIGHT_API UUKFBTService_RotateToward : public UBTService_BlackboardBase {
	GENERATED_BODY()

	/** Success condition precision in degrees */
	UPROPERTY(Category = Node, EditAnywhere, meta = (ClampMin = "0.0"))
	float RotateSpeed;

public:
	UUKFBTService_RotateToward(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
