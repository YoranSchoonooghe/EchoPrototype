#pragma once

#include "CoreMinimal.h"
#include "PlayerStateBase.h"
#include "PlayerStates.generated.h"


// IDLE / WALK STATE
UCLASS()
class ECHOPROTOTYPE_API UPlayerState_IdleWalk : public UPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void EnterState(class APlayerCharacter* Character) override;

	virtual UPlayerStateBase* OnSprintPressed(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnSneakPressed(APlayerCharacter* Character) override;
};

// SPRINT STATE
UCLASS()
class ECHOPROTOTYPE_API UPlayerState_Sprint : public UPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void EnterState(class APlayerCharacter* Character) override;

	virtual UPlayerStateBase* OnSprintReleased(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnSneakPressed(APlayerCharacter* Character) override;

private:
	 float SprintSpeed = 800.0f;
};

// SNEAK STATE
UCLASS()
class ECHOPROTOTYPE_API UPlayerState_Sneak : public UPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void EnterState(class APlayerCharacter* Character) override;
	virtual void ExitState(class APlayerCharacter* Character) override;

	virtual UPlayerStateBase* OnSneakReleased(APlayerCharacter* Character) override;

private:
	float SneakSpeed = 250.0f;
};