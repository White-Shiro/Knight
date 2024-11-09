#include "SoundRequest.h"
#include "Kismet/GameplayStatics.h"

FSoundRequest::FSoundRequest() {
	VolumeMultiplier = 1.f;
	PitchMultiplier = 1.f;
}

void FSoundRequest::Play(USceneComponent* requestor) const {
	if (!Sound || !requestor) return;

	if (!Sound->IsOneShot()) {
		UE_LOG(LogAudio, Warning, TEXT("FSoundRequest Play: tried to play a sound asset which is not a one-shot: '%s'. Spawning suppressed."), *GetNameSafe(Sound));
		return;
	}

	const auto* world = requestor->GetWorld();
	if (!world) return;

#if WITH_EDITORONLY_DATA
	if (bPreviewIgnoreAttenuation && world && world->WorldType == EWorldType::EditorPreview) {
		UGameplayStatics::PlaySound2D(world, Sound, VolumeMultiplier, PitchMultiplier);
		return;
	}
#endif

	if (bFollow && requestor) {
		UGameplayStatics::SpawnSoundAttached(Sound, requestor, AttachName, FVector(ForceInit), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier);
	} else {
		UGameplayStatics::PlaySoundAtLocation(world, Sound, requestor->GetComponentLocation(), VolumeMultiplier, PitchMultiplier);
	}
}