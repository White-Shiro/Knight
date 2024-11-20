#pragma once

#include "CoreMinimal.h"
#include "KfKnightAIPawn.generated.h"

UCLASS()
class AKfKnightAIPawn : public APawn {
	GENERATED_BODY()
	public:
	explicit AKfKnightAIPawn(const FObjectInitializer& initializer = FObjectInitializer::Get());
};
