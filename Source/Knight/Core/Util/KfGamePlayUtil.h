#pragma once

struct KfGamePlayUtil {
	static void PlayHitNormalEffect(UParticleSystem* pPfx, const FVector& size, const FHitResult& hitResult, const UObject* ctxObject);
};