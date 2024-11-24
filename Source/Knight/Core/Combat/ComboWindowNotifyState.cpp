#include "ComboWindowNotifyState.h"
#include "Combat.h"

UComboWindowNotifyState::UComboWindowNotifyState() {
	isBeginAllowed = true;
	isEndAllowed = false;
	resetComboSequenceOnEnd = true;
}

void UComboWindowNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (auto* const target = Cast<IReactToComboWindowNotifyState>(MeshComp->GetOwner())) {
		target->ReactToComboWindowNotifyState(true, false, isBeginAllowed);
	}
}

void UComboWindowNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (auto* const target = Cast<IReactToComboWindowNotifyState>(MeshComp->GetOwner())) {
		target->ReactToComboWindowNotifyState(false, true, isEndAllowed);
		if (resetComboSequenceOnEnd) {
			target->ReactToComboWindowNotifyState_ResetComboSequence();
		}
	}
}