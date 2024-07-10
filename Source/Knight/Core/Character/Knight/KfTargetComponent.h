#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KfTargetComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KNIGHT_API UKfTargetComponent : public UActorComponent {
	GENERATED_BODY()
	TWeakInterfacePtr<class ITargetable> _target;

	UPROPERTY(EditAnywhere)
	float _scanRadius = 1000.0f;

	UPROPERTY(EditAnywhere)
	float _lockSpeed = 10.0f;

	UPROPERTY(EditAnywhere)
	FVector2f _lockPitchRange = FVector2f(-60.0f, 60.0f);

	TArray<FOverlapResult> _overlaps;
	TObjectPtr<class UCameraComponent> _targetCamera;

public:
	UKfTargetComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateCamera(float dt);
	bool ScanTarget();
	void ReleaseTarget();
	void SetTargetCamera(UCameraComponent* targetCamera);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool hasTarget() const { return _target.IsValid(); }

	UFUNCTION(BlueprintCallable)
	FVector GetTargetLocation();
};