#pragma once

#include "Units/RigUnit.h"
#include "RigUnit_CustomNode.generated.h"

USTRUCT(meta = (DisplayName = "My Custom Node", Category = "Custom"))
struct KNIGHT_API FRigUnit_CustomNode : public FRigUnit {
	GENERATED_BODY()

	FRigUnit_CustomNode()
		: A(0.f), B(0.f), Result(0.f) {}

	RIGVM_METHOD()
	virtual void Execute() override;

	UPROPERTY(meta = (Input))
	float A;

	UPROPERTY(meta = (Input))
	float B;

	UPROPERTY(meta = (Output))
	float Result;

};