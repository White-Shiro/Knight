#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CombatCommon.h"
#include "HitDetectionNotifyState.generated.h"

struct UHitDetectionNotifyParam {
	USkeletalMeshComponent* meshComp = nullptr;
	UAnimSequenceBase* animation = nullptr;
	float totalDuration = 0;
	int sequenceId = -1;
};

UCLASS()
class KNIGHT_API UHitDetectionNotifyState : public UAnimNotifyState {
	GENERATED_BODY()

public:
	UHitDetectionNotifyState();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
		const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

protected:

	TWeakInterfacePtr<IReactToAnimHitDectectNotifyState> _target = nullptr;
	int _sequenceId = -1;
};