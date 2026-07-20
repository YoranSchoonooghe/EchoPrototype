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

	virtual UPlayerStateBase* UpdateState(APlayerCharacter* Character, float DeltaTime) override;

	virtual UPlayerStateBase* OnSprintPressed(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnSneakPressed(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnAttackPressed(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnAttackReleased(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnStealthKillPressed(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnClimbPressed(APlayerCharacter* Character) override;
};

// SPRINT STATE
UCLASS()
class ECHOPROTOTYPE_API UPlayerState_Sprint : public UPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual void EnterState(class APlayerCharacter* Character) override;

	virtual UPlayerStateBase* UpdateState(APlayerCharacter* Character, float DeltaTime) override;

	virtual UPlayerStateBase* OnSprintReleased(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnSneakPressed(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnAttackPressed(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnAttackReleased(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnClimbPressed(APlayerCharacter* Character) override;
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
	virtual UPlayerStateBase* OnStealthKillPressed(APlayerCharacter* Character) override;
};

// ATTACKING STATE
UCLASS()
class ECHOPROTOTYPE_API UPlayerState_Attacking : public UPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual UPlayerStateBase* UpdateState(APlayerCharacter* Character, float DeltaTime) override;

	virtual UPlayerStateBase* OnAttackPressed(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnAttackReleased(APlayerCharacter* Character) override;
};

UCLASS()
class ECHOPROTOTYPE_API UPlayerState_Dead : public UPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual bool CanMove() const override { return false; }
};

// STEALTH KILL STATE
UCLASS()
class ECHOPROTOTYPE_API UPlayerState_StealthKilling : public UPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual UPlayerStateBase* UpdateState(APlayerCharacter* Character, float DeltaTime) override;

	virtual bool CanMove() const override { return false; }
};

// HANGING STATE (ledge climbing)
UCLASS()
class ECHOPROTOTYPE_API UPlayerState_Hanging : public UPlayerStateBase
{
	GENERATED_BODY()

public:
	virtual UPlayerStateBase* UpdateState(APlayerCharacter* Character, float DeltaTime) override;

	virtual UPlayerStateBase* OnClimbPressed(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnSprintPressed(APlayerCharacter* Character) override;
	virtual UPlayerStateBase* OnSprintReleased(APlayerCharacter* Character) override;

	virtual bool CanMove() const override { return false; }
};