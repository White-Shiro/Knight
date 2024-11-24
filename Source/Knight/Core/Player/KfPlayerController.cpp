#include "KfPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Knight/Core/Core.h"
#include "Knight/Core/Camera/KfCameraManager.h"
#include "Knight/Core/Character/Knight/KfKnightCharacter.h"

AKfPlayerController::AKfPlayerController() {
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> Finder = TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Core/Config/Input/IMC_KnightGame.IMC_KnightGame'");
	DefaultMappingContext = Finder.Object;
	PlayerCameraManagerClass = AKfCameraManager::StaticClass();
}

static void _InitInputContext(const UInputMappingContext* ctx, const ULocalPlayer* lp) {
	if (!lp) return;
	if (!ctx) {
		UC_LOG_ERROR_MSG("Mapping Context Not Set")
		return;
	}

	if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(lp)) {
		if (Subsystem->HasMappingContext(ctx)) {
			UC_LOG_WARNING_MSG("Mapping Context has arleady set")
			return;
		}
		Subsystem->AddMappingContext(ctx, 0);
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
	_currentKnightFrame = Cast<AKfKnightCharacter>(InPawn);
}
