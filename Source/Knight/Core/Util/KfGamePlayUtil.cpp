#include "KfGamePlayUtil.h"
#include "Kismet/GameplayStatics.h"

void KfGamePlayUtil::PlayHitNormalEffect(UParticleSystem* pPfx, const FVector& size, const FHitResult& hitResult,
                                         const UObject* ctxObject) {
	if (!pPfx) return;
	const auto loc = hitResult.ImpactPoint;
	const auto rot = FRotationMatrix::MakeFromX(hitResult.ImpactNormal).Rotator();
	auto comp = UGameplayStatics::SpawnEmitterAtLocation(ctxObject, pPfx, loc, rot,
	                                                     size, true,
	                                                     EPSCPoolMethod::AutoRelease, true);
}

void KfGamePlayUtil::PlayEffectAt(UParticleSystem* pPfx, const FVector& size, const FVector& location, const FRotator& rotation, const UObject* ctxObject) {
	if (!pPfx) return;
	auto comp = UGameplayStatics::SpawnEmitterAtLocation(ctxObject, pPfx, location, rotation, size, true, EPSCPoolMethod::AutoRelease, true);
}