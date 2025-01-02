#include "KfGameMath.h"

FRotator KfGameMath::FixedRotate(const FRotator& current, const FRotator& target, float deltaSeconds, float rate) {
	FRotator newFacing;
	float deltaRate = rate * deltaSeconds;
	newFacing.Pitch = FMath::FixedTurn(current.Pitch, target.Pitch, deltaRate);
	newFacing.Yaw = FMath::FixedTurn(current.Yaw, target.Yaw, deltaRate);
	newFacing.Roll = FMath::FixedTurn(current.Roll, target.Roll, deltaRate);
	return newFacing;
}

FRotator KfGameMath::FixedRotate_Yaw(const FRotator& current, const FRotator& target, float deltaSeconds, float rate) {
	FRotator newFacing;
	newFacing.Pitch = current.Pitch;
	newFacing.Yaw = FMath::FixedTurn(current.Yaw, target.Yaw, rate * deltaSeconds);
	newFacing.Roll = current.Roll;
	return newFacing;
}