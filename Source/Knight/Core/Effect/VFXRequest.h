#pragma once
#include "CoreMinimal.h"
#include "VFXRequest.generated.h"

USTRUCT(BlueprintType)
struct FVFXRequest {
	GENERATED_BODY()
	FVFXRequest() noexcept;
	explicit FVFXRequest(UParticleSystem* particle, const FVector& size = FVector::One()) noexcept;
	explicit FVFXRequest(class UNiagaraSystem* niagara, const FVector& size = FVector::One()) noexcept;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta=(ExposeOnSpawn = true))
	TObjectPtr<UParticleSystem> particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta=(ExposeOnSpawn = true))
	TObjectPtr<class UNiagaraSystem> niagara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", meta=(ExposeOnSpawn = true))
	FVector effectSize = FVector::One();

	void EmitParticleAt(const FVector& location, const FRotator& rotation, const USceneComponent* requester) const;
	void EmitNiagaraAt(const FVector& location, const FRotator& rotation, const USceneComponent* requester) const;
	void EmitAt(const FVector& location, const FRotator& rotation, const USceneComponent* requester) const;
	void EmitNormal(const FVector& location, const FVector& normal, const USceneComponent* requester) const;
	auto EmitHitImpact(const FHitResult& hitResult, const USceneComponent* requester) const -> void;
};