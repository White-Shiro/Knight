#pragma once

struct WxGamePlayUtil {
	static void PlayHitNormalEffect(UParticleSystem* pPfx, const FVector& size, const FHitResult& hitResult, const UObject* ctxObject);
};