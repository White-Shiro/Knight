#pragma once

class KfVectorUtils {
public:

	/// return true if vector is not zero
	static bool UnitDirectionAndSize(const FVector& vector, FVector& outDir, float& outSize);
	static FVector SafeDivide(const FVector& vector, float divisor);
	static FVector XY0(const FVector& vector) { return FVector(vector.X, vector.Y, 0); }
	static FVector X0Z(const FVector& vector) { return FVector(vector.X, 0, vector.Z); }
	static FVector HorizontalUnitDirection(const FVector& vector) { return FVector(vector.X, vector.Y, 0).GetSafeNormal(); }
};
