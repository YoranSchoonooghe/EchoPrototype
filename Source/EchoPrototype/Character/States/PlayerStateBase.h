#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerStateBase.generated.h"

class APlayerCharacter;

UCLASS(Abstract)
class ECHOPROTOTYPE_API UPlayerStateBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void EnterState(APlayerCharacter* Character) {}
	virtual void ExitState(APlayerCharacter* Character) {}

	virtual UPlayerStateBase* UpdateState(APlayerCharacter* Character, float DeltaTime) { return nullptr; }

	virtual UPlayerStateBase* OnSprintPressed(APlayerCharacter* Character) { return nullptr; }
	virtual UPlayerStateBase* OnSprintReleased(APlayerCharacter* Character) { return nullptr; }
	virtual UPlayerStateBase* OnSneakPressed(APlayerCharacter* Character) { return nullptr; }
	virtual UPlayerStateBase* OnSneakReleased(APlayerCharacter* Character) { return nullptr; }

	virtual UPlayerStateBase* OnAttackPressed(APlayerCharacter* Character) { return nullptr; }
	virtual UPlayerStateBase* OnAttackReleased(APlayerCharacter* Character) { return nullptr; }

	virtual UPlayerStateBase* OnStealthKillPressed(APlayerCharacter* Character) { return nullptr; }

	virtual UPlayerStateBase* OnClimbPressed(APlayerCharacter* Character) { return nullptr; }

	virtual bool CanMove() const { return true; }
};