#include "KfVectorUtils.h"
bool KfVectorUtils::UnitDirectionAndSize(const FVector& vector, FVector& outDir, float& outSize) {
	outSize = vector.Size();
	if (FMath::IsNearlyZero(outSize)) {
		outDir = FVector::ZeroVector;
		return false;
	}

	outDir = vector / outSize;
	return true;
}

FVector KfVectorUtils::SafeDivide(const FVector& vector, float divisor) {
	if (FMath::IsNearlyZero(divisor)) {
		return FVector::ZeroVector;
	}

	return vector / divisor;
}