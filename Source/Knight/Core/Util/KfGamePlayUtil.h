#pragma once

struct KfGamePlayUtil {
	static void PlayHitNormalEffect(UParticleSystem* pPfx, const FVector& size, const FHitResult& hitResult, const UObject* ctxObject);
	static void PlayEffectAt(UParticleSystem* pPfx, const FVector& size, const FVector& location, const FRotator& rotation, const UObject* ctxObject);
};