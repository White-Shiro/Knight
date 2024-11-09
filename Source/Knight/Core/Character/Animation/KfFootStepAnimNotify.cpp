#include "KfFootStepAnimNotify.h"
#include "Knight/Core/Character/Knight/KfCharacterAnimInstance.h"

FFootStepEvent::FFootStepEvent() {
	footStepType = EFootStepType::None;
	bPlaySound = true;
	bPlayVFX = true;
}

UKfFootStepAnimNotify::UKfFootStepAnimNotify() {}

void UKfFootStepAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::Notify(MeshComp, Animation, EventReference);
	if (auto* kfAnim = Cast<UKfCharacterAnimInstance>(MeshComp->GetAnimInstance())) {
		kfAnim->NotifyFootStep(footStepEvent);
	}
}