#pragma once

// Jason: minor thing, I would avoid prefix "Wx" because of WxWidgets
struct WxGamePlayUtil {
	static void PlayHitNormalEffect(UParticleSystem* pPfx, const FVector& size, const FHitResult& hitResult, const UObject* ctxObject);
};