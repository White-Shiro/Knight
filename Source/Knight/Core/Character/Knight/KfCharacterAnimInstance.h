#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KfCharacterAnimInstance.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FAttackAnimationSet {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Animation Set")
	UAnimMontage* attackMontage_NA_1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Animation Set")
	UAnimMontage* attackMontage_NA_2 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Animation Set")
	UAnimMontage* attackMontage_NA_3 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Animation Set")
	UAnimMontage* attackMontage_HA_1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hurt Animation Set")
	UAnimMontage* hurtMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Walk Motion")
	UBlendSpace* walkBlendSpace = nullptr;
};

UCLASS()
class KNIGHT_API UKfCharacterAnimInstance : public UAnimInstance {
	GENERATED_BODY()


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Variables")
	FVector2f aMoveDirection = FVector2f::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim Variables")
	float aMoveAnimPlaySpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim Variables")
	FAttackAnimationSet attackAnimationSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool logDirAndSpeed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Walk")
	UBlendSpace* walkBlendSpace = nullptr;

	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	void syncWalkPlaySpeed(const FVector& worldVel, const FVector& localInputV, float meshScale);
	void setWalkBlendspaceDirection2D(const FVector& localInputV);
	void setWalkBlendspaceDirection1D(const FVector& worldVel, const float maxSpeed);

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	float PlayMeleeMontage(int32 montageIndex);

	void PlayHurtMontage();

protected:
	void CacheWalkBlendSpaceInfo();
private:

	static constexpr float WALK_BLENDSPACE_AXIS_SCALE = 100.f;

	TObjectPtr<class AKfCharacter> _knightCh = nullptr;

	enum class EWalkDirection {
		Forward,
		Backward,
		Left,
		Right
	};

	TMap<EWalkDirection, TObjectPtr<UAnimSequence>> _walkBlendSpaceMap;
	static EWalkDirection GetWalkDirection(const FVector2d& inputV);

};