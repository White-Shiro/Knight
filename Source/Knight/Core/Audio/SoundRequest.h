#pragma once
#include "SoundRequest.generated.h"

USTRUCT(BlueprintType)
struct FSoundRequest {
	GENERATED_BODY()
	FSoundRequest();

	// Sound to Play
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SoundRequest", meta=(ExposeOnSpawn = true))
	TObjectPtr<USoundBase> Sound;

	// Volume Multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SoundRequest", meta=(ExposeOnSpawn = true))
	float VolumeMultiplier;

	// Pitch Multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SoundRequest", meta=(ExposeOnSpawn = true))
	float PitchMultiplier;

	// If this sound should follow its owner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="SoundRequest")
	uint32 bFollow:1;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Config, EditAnywhere, Category = "SoundRequest")
	uint32 bPreviewIgnoreAttenuation:1;
#endif

	// Socket or bone name to attach sound to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SoundRequest", meta=(EditCondition="bFollow", ExposeOnSpawn = true))
	FName AttachName;

	void Play(USceneComponent* requestor) const;
};