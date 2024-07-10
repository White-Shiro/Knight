#include "KfFootStepAnimNotify.h"
#include "Knight/Core/Character/Knight/KfCharacterAnimInstance.h"

void UKfFootStepAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	Super::Notify(MeshComp, Animation, EventReference);

	if (auto kfAnim = Cast<UKfCharacterAnimInstance>(MeshComp->GetAnimInstance())) {
		kfAnim->NotifyFootStep(footStepType);
	}
}