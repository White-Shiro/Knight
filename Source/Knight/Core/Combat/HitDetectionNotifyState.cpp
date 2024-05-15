#include "HitDetectionNotifyState.h"

#include "CombatInterface.h"
#include "Knight/Core/Character/Knight/KfCharacter.h"

UHitDetectionNotifyState::UHitDetectionNotifyState() {
	constexpr int MIN_SEQUENCE_ID = -100000;
	constexpr int MAX_SEQUENCE_ID = 100000;
	_sequenceId = FMath::RandRange(MIN_SEQUENCE_ID, MAX_SEQUENCE_ID);
}

void UHitDetectionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	_target = Cast<IReactToAnimHitDectectNotifyState>(MeshComp->GetOwner());
	_sequenceId++;
}

void UHitDetectionNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if(_target.Get()) {
		const UHitDetectionNotifyParam payLoad = {MeshComp, Animation, FrameDeltaTime, _sequenceId};
		_target->ReactToAnimHitDetection(FrameDeltaTime, payLoad);
	}
}

void UHitDetectionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference) {
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	_target = nullptr;
}