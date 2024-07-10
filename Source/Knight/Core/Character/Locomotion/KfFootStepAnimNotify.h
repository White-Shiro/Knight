#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "KfFootStepAnimNotify.generated.h"

UENUM()
enum class EFootStepType {
	None,
	Left,
	Right,
	Both,
};

UCLASS()
class KNIGHT_API UKfFootStepAnimNotify : public UAnimNotify {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	EFootStepType footStepType = EFootStepType::None;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};