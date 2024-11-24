#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KfPlayerController.generated.h"

UCLASS()
class KNIGHT_API AKfPlayerController : public APlayerController {
private:
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	TWeakObjectPtr<class AKfKnightCharacter> _currentKnightFrame;

public:
	AKfPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
};