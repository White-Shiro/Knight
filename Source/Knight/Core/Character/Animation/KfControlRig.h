#pragma once
#include "ControlRig.h"
#include "KfControlRig.generated.h"

UCLASS(Blueprintable) // without this the class won't show up in the editor
class KNIGHT_API UKfControlRig : public UControlRig {
	GENERATED_BODY()

protected:
	//virtual bool Execute(const FName& InEventName) override; // Tick() equivalent
	//virtual void Initialize(bool bRequestInit) override; // BeginPlay() equivalent, initialize variables here

};
