#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "KfFootStepAnimNotify.generated.h"

UENUM(BlueprintType)
enum class EFootStepType : uint8 {
	None,
	Left,
	Right,
	Both,
};

USTRUCT()
struct FFootStepEvent {
	GENERATED_BODY()
	FFootStepEvent();
	FFootStepEvent(EFootStepType footStepType, bool bPlaySound, bool bPlayVFX);

	UPROPERTY(EditAnywhere)
	EFootStepType footStepType;

	UPROPERTY(EditAnywhere)
	uint8 bPlaySound : 1;

	UPROPERTY(EditAnywhere)
	uint8 bPlayVFX : 1;
};

UCLASS()
class KNIGHT_API UKfFootStepAnimNotify : public UAnimNotify {
	GENERATED_BODY()
	UKfFootStepAnimNotify();

public:
	UPROPERTY(EditAnywhere)
	FFootStepEvent footStepEvent;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};