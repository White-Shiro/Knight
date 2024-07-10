#include "HitDetectionNotifyState.h"
#include "CombatCommon.h"

static constexpr int MIN_SEQUENCE_ID = -100000;
static constexpr int MAX_SEQUENCE_ID = 100000;

UHitDetectionNotifyState::UHitDetectionNotifyState() {
	_sequenceId = FMath::RandRange(MIN_SEQUENCE_ID, MAX_SEQUENCE_ID);
}

void UHitDetectionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
                                           UAnimSequenceBase* Animation,
                                           float TotalDuration,
                                           const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	_target = Cast<IReactToAnimHitDectectNotifyState>(MeshComp->GetOwner());
	_sequenceId++;
	if (_sequenceId > MAX_SEQUENCE_ID) {
		_sequenceId = MIN_SEQUENCE_ID;
	};
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