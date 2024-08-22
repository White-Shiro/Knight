// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ComboWindowNotifyState.generated.h"

/**
 *
 */
UCLASS()
class KNIGHT_API UComboWindowNotifyState : public UAnimNotifyState {
	GENERATED_BODY()

	UComboWindowNotifyState();

	UPROPERTY(EditAnywhere) bool isBeginAllowed;
	UPROPERTY(EditAnywhere) bool isEndAllowed;
	UPROPERTY(EditAnywhere) bool resetComboSequenceOnEnd;

	// Jason: I will do code format this way, but it just my personal preference 
	virtual void NotifyBegin(	USkeletalMeshComponent* MeshComp,
								UAnimSequenceBase* Animation,
								float TotalDuration,
								const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(	USkeletalMeshComponent* MeshComp,
							UAnimSequenceBase* Animation,
							const FAnimNotifyEventReference& EventReference) override;
};