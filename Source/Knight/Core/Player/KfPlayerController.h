#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KfPlayerController.generated.h"

UCLASS()
class KNIGHT_API AKfPlayerController : public APlayerController {
private:
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext = nullptr;

	// Jason: please add UPROPERTY() for any TObjectPtr<> or UObject*
	// GC will help you set null, if you don't want GC, you can try TWeakObjectPtr<> instead
	// UPROPERTY()
	TObjectPtr<class AKfCharacter> CurrentKnightFrame = nullptr;

public:
	AKfPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
};