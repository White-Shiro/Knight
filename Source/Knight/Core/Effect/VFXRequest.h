#pragma once
#include "VFXRequest.generated.h"

USTRUCT(BlueprintType)
struct FVFXRequest {
	GENERATED_BODY()
	FVFXRequest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta=(ExposeOnSpawn = true))
	TObjectPtr<UParticleSystem> effect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta=(ExposeOnSpawn = true))
	FVector effectSize = FVector::One();

	void EmitAt(const FVector& location, USceneComponent* requestor) const;
};