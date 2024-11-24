#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "KfKnightAIController.generated.h"

UCLASS()
class KNIGHT_API AKfKnightAIController : public AAIController {


private:
	GENERATED_BODY()

public:
	AKfKnightAIController();
	virtual void Tick(float DeltaTime) override;
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};

