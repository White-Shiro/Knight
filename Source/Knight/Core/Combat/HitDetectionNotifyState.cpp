#include "HitDetectionNotifyState.h"
#include "CombatCommon.h"

static constexpr int MIN_SEQUENCE_ID = -100000; // Jason: why using negative ?
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

	 // Jason: not sure why doing this way, 
	 // if you want a id always within a scope, using 'id = id % N' make it more easy to read
	 // or you have special reason ? please let me know
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
		// Jason: personally, I don't like init member be {x,x,x,x...}
		// because when you change the struct members, you will have a mistake here
		// I do always assign one by one, or make a handy constructor instead
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