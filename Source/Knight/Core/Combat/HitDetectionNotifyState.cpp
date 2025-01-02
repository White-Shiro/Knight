#include "HitDetectionNotifyState.h"
#include "Combat.h"

static constexpr int MAX_SEQUENCE_ID = 200000;

UHitDetectionNotifyState::UHitDetectionNotifyState() {
	_sequenceId = FMath::RandRange(0, MAX_SEQUENCE_ID);
}

void UHitDetectionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
                                           UAnimSequenceBase* Animation,
                                           float TotalDuration,
                                           const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	_target = MeshComp->GetOwner<IReactToAnimHitDectectNotifyState>();
	_sequenceId++;
	_sequenceId = _sequenceId % MAX_SEQUENCE_ID;
}

void UHitDetectionNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp,
                                          UAnimSequenceBase* Animation,
                                          float FrameDeltaTime,
                                          const FAnimNotifyEventReference& EventReference) {
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (_target.Get()) {
		const UHitDetectionNotifyParam payLoad = {MeshComp, Animation, FrameDeltaTime, _sequenceId};
		_target->ReactToAnimHitDetection(FrameDeltaTime, payLoad);
	}
}

void UHitDetectionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp,
                                         UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference) {
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	_target = nullptr;
}