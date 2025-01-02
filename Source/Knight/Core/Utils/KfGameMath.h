#pragma once

#include "CoreMinimal.h"

class KfGameMath {
public:
	static FRotator FixedRotate(const FRotator& current, const FRotator& target, float deltaSeconds, float rate);
	static FRotator FixedRotate_Yaw(const FRotator& current, const FRotator& target, float deltaSeconds, float rate);
};
