#include "KfVectorUtils.h"
bool KfVectorUtils::UnitDirectionAndSize(const FVector& vector, FVector& outUnitDirection, float& outSize) {
	outSize = vector.Size();
	if (FMath::IsNearlyZero(outSize)) {
		outUnitDirection = FVector::ZeroVector;
		return false;
	}

	outUnitDirection = vector / outSize;
	return true;
}

FVector KfVectorUtils::SafeDivide(const FVector& vector, float divisor) {
	if (FMath::IsNearlyZero(divisor)) {
		return FVector::ZeroVector;
	}

	return vector / divisor;
}