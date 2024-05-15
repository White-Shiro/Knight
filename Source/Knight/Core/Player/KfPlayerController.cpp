#include "KfPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Knight/Core/Common.h"
#include "Knight/Core/Character/Knight/KfCharacter.h"
#include "Knight/Core/Util/ObjectFinder.h"

AKfPlayerController::AKfPlayerController() {
	DefaultMappingContext = ObjectFinder::LoadAsset<UInputMappingContext>(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Core/Config/Input/IMC_KnightGame.IMC_KnightGame'"));
}

static void _InitInputContext(const UInputMappingContext* ctx, const ULocalPlayer* lp) {
	if (!lp) return;
	if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(lp)) {
		if (ctx && !Subsystem->HasMappingContext(ctx)) {
			Subsystem->AddMappingContext(ctx, 0);
		} else {
			UC_LOG_ERROR_MSG("Mapping Context Not Set")
		}
	}
}

void AKfPlayerController::BeginPlay() {
	Super::BeginPlay();

	if (IsLocalController()) {
		_InitInputContext(DefaultMappingContext, GetLocalPlayer());
		bShowMouseCursor = false;
	}
}

void AKfPlayerController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	CurrentKnightFrame = Cast<AKfCharacter>(InPawn);
	if (CurrentKnightFrame) {
		UC_LOG_MSG("Pilot Valid");
	} else {
		UC_LOG_ERROR_MSG("Who is this?");
	}
}
