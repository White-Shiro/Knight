#include "VFXRequest.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

FVFXRequest::FVFXRequest() noexcept {
	effectSize = FVector::One();
}

FVFXRequest::FVFXRequest(UParticleSystem* particle, const FVector& size) noexcept {
	this->particle = particle;
	effectSize = size;
}

FVFXRequest::FVFXRequest(UNiagaraSystem* niagara, const FVector& size) noexcept {
	this->niagara = niagara;
	effectSize = size;
}

void FVFXRequest::EmitParticleAt(const FVector& location, const FRotator& rotation, const USceneComponent* requester) const {
	if (!particle || !requester) return;
	const auto* world = requester->GetWorld();
	if (!world) return;

#if WITH_EDITORONLY_DATA
	if (world && world->WorldType == EWorldType::EditorPreview) {
		UGameplayStatics::SpawnEmitterAtLocation(world, particle, location, rotation,  effectSize, true, EPSCPoolMethod::AutoRelease, true);
		return;
	}
#endif
	UGameplayStatics::SpawnEmitterAtLocation(world, particle, location, rotation,  effectSize, true, EPSCPoolMethod::AutoRelease, true);
}

void FVFXRequest::EmitNiagaraAt(const FVector& location, const FRotator& rotation, const USceneComponent* requester) const {
	if (!niagara || !requester) return;
	const auto* world = requester->GetWorld();
	if (!world) return;

#if WITH_EDITORONLY_DATA
	if (world && world->WorldType == EWorldType::EditorPreview) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(world, niagara, location, rotation, effectSize, true, true, ENCPoolMethod::AutoRelease, true);
		return;
	}
#endif
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(world, niagara, location, rotation, effectSize, true, true, ENCPoolMethod::AutoRelease, true);
}

void FVFXRequest::EmitAt(const FVector& location, const FRotator& rotation, const USceneComponent* requester) const {
	EmitParticleAt(location, rotation, requester);
	EmitNiagaraAt(location, rotation, requester);
}

void FVFXRequest::EmitNormal(const FVector& location, const FVector& normal, const USceneComponent* requester) const {
	if (!requester) return;
	const auto rot = FRotationMatrix::MakeFromX(normal).Rotator();
	EmitAt(location, rot, requester);
}

void FVFXRequest::EmitHitImpact(const FHitResult& hitResult, const USceneComponent* requester) const {
	if (!requester) return;
	EmitNormal(hitResult.ImpactPoint, hitResult.ImpactNormal, requester);
}