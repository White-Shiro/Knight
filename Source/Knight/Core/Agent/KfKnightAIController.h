#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "KfKnightAIController.generated.h"

UCLASS()
class KNIGHT_API AKfKnightAIController : public AAIController {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKfKnightAIController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};