#include "VFXRequest.h"
#include "Kismet/GameplayStatics.h"

FVFXRequest::FVFXRequest() {
	effectSize = FVector::One();
}

void FVFXRequest::EmitAt(const FVector& location, USceneComponent* requestor) const {
	if (!effect || !requestor) return;
	const auto* world = requestor->GetWorld();
	if (!world) return;

#if WITH_EDITORONLY_DATA
	if (world && world->WorldType == EWorldType::EditorPreview) {
		UGameplayStatics::SpawnEmitterAtLocation(world, effect, location, FRotator::ZeroRotator,  effectSize, true, EPSCPoolMethod::AutoRelease, true);
		return;
	}
#endif

	UGameplayStatics::SpawnEmitterAtLocation(world, effect, location, FRotator::ZeroRotator,  effectSize, true, EPSCPoolMethod::AutoRelease, true);
}