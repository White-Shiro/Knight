#include "WxGamePlayUtil.h"
#include "Kismet/GameplayStatics.h"

void WxGamePlayUtil::PlayHitNormalEffect(UParticleSystem* pPfx, const FVector& size, const FHitResult& hitResult,
                                         const UObject* ctxObject) {
	if (!pPfx) return;
	const auto loc = hitResult.ImpactPoint;
	const auto rot = FRotationMatrix::MakeFromX(hitResult.ImpactNormal).Rotator();
	auto comp = UGameplayStatics::SpawnEmitterAtLocation(ctxObject, pPfx, loc, rot,
	                                                     size, true,
	                                                     EPSCPoolMethod::AutoRelease, true);
}
