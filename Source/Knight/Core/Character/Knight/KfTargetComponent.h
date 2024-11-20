#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Knight/Core/Character/KfCharacterCommon.h"
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

	UPROPERTY(EditAnywhere)
	bool _debug = false;

public:
	UKfTargetComponent();

protected:
	virtual void BeginPlay() override;
	void DebugLockState(const FMinimalViewInfo& outResult) const;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void SetTargetCamera(UCameraComponent* targetCamera);
	void UpdateCtrlCamera(float dt);
	void CalcCamera(float DeltaTime, UCameraComponent* camera, const struct FCameraConfig* springArmConfig, FMinimalViewInfo& outResult);
	bool ScanTarget();
	void ReleaseTarget();
	bool ToggleTargetMode();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool hasTarget() const { return _target.IsValid(); }

	UFUNCTION(BlueprintCallable)
	FVector GetTargetLocation() const;
	static void CalcCameraLocation_SprintArmState(const FVector& selfLoc, float pitch, const FSpringArmState& springArmState, const FVector& facingDirection, FVector& outCamLocation);
};