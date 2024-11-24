#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KfKnightMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KNIGHT_API UKfKnightMovementComponent : public UCharacterMovementComponent {
public:
	virtual void OnRegister() override;
	virtual void RequestPathMove(const FVector& MoveInput) override;
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;

private:
	GENERATED_BODY()
public:
	UKfKnightMovementComponent();

protected:
	TObjectPtr<class AKfKnightAIPawn> knightOwner;
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};