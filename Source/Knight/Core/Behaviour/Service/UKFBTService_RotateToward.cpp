// Fill out your copyright notice in the Description page of Project Settings.

#include "UKFBTService_RotateToward.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Knight/Core/Core.h"
#include "Knight/Core/Agent/KfKnightAIController.h"
#include "Knight/Core/Character/Knight/KfKnightAIPawn.h"

UUKFBTService_RotateToward::UUKFBTService_RotateToward(FObjectInitializer const& ObjectInitializer)
	: RotateSpeed(10) {
	NodeName = "RotateToward";
}

void UUKFBTService_RotateToward::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	UC_MSG("UUKFBTService_RotateToward::OnBecomeRelevant");
}

void UUKFBTService_RotateToward::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	const auto* aiCtrl = OwnerComp.GetOwner<AKfKnightAIController>();
	if (!aiCtrl) {
		UC_LOG_ERROR_MSG("UUKFBTService_RotateToward::TickNode: failed to cast to AKfKnightAIController");
		return;
	}

	auto* knight = aiCtrl->GetPawn<AKfKnightAIPawn>();
	if (!knight) {
		UC_LOG_ERROR_MSG("UUKFBTService_RotateToward::TickNode: failed to cast to AKfKnightAIPawn");
		return;
	}

	// Get Target Actor from Blackboard
	const auto* blackboard = OwnerComp.GetBlackboardComponent();
	const auto* targetActor = Cast<AActor>(blackboard->GetValueAsObject(GetSelectedBlackboardKey()));
	if (!targetActor) {
		UC_LOG_ERROR_MSG("UUKFBTService_RotateToward::TickNode: failed to get TargetActor from Blackboard");
		return;
	}

	// Rotate toward Target Actor
	auto targetLocation = targetActor->GetActorLocation();
	auto knightLocation = knight->GetActorLocation();
	auto targetDirection = (targetLocation - knightLocation).GetSafeNormal();
	auto targetRotation = targetDirection.Rotation();
	auto knightRotation = knight->GetActorRotation();
	auto newRotation = FMath::RInterpTo(knightRotation, targetRotation, DeltaSeconds, RotateSpeed);
	knight->SetActorRotation(newRotation);
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}