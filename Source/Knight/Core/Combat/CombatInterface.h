#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

struct FAttackReqeust {
	float damage;
};

struct FAttackResult {
	explicit FAttackResult(bool success_) : success(success_) {

	}

	bool success;
};

UINTERFACE()
class UReactToAttack : public UInterface {
	GENERATED_BODY()
};

class KNIGHT_API IReactToAttack {
	GENERATED_BODY()

public:
	virtual FAttackResult ReactToAttack(const FAttackReqeust& req) = 0;
};

UINTERFACE()
class UReactToAnimHitDectectNotifyState : public UInterface {
	GENERATED_BODY()
};

class KNIGHT_API IReactToAnimHitDectectNotifyState {
	GENERATED_BODY()

public:
	virtual void ReactToAnimHitDetection(float frameDt, const struct UHitDetectionNotifyParam& payload) = 0;
};


UINTERFACE()
class UTargetable : public UInterface {
	GENERATED_BODY()
};

class ITargetable {
	GENERATED_BODY()

public:
	virtual FVector GetTargetLocation() = 0;
};
