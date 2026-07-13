#include "PlayerStates.h"
#include "../PlayerCharacter.h"
#include "../../Combat/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// IDLE / WALK STATE
void UPlayerState_IdleWalk::EnterState(APlayerCharacter* Character)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = 500.0f;
}

UPlayerStateBase* UPlayerState_IdleWalk::OnSprintPressed(APlayerCharacter* Character)
{
	return NewObject<UPlayerState_Sprint>(Character);
}

UPlayerStateBase* UPlayerState_IdleWalk::OnSneakPressed(APlayerCharacter* Character)
{
	return NewObject<UPlayerState_Sneak>(Character);
}

UPlayerStateBase* UPlayerState_IdleWalk::OnAttackPressed(APlayerCharacter* Character)
{
	if (UCombatComponent* Combat = Character->GetCombatComponent())
	{
		Combat->OnAttackHoldStarted();

		if (Combat->IsAttacking())
		{
			return NewObject<UPlayerState_Attacking>(Character);
		}
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_IdleWalk::OnAttackReleased(APlayerCharacter* Character)
{
	if (UCombatComponent* Combat = Character->GetCombatComponent())
	{
		Combat->OnAttackReleased();

		if (Combat->IsAttacking())
		{
			return NewObject<UPlayerState_Attacking>(Character);
		}
	}

	return nullptr;
}

// SPRINT STATE
void UPlayerState_Sprint::EnterState(APlayerCharacter* Character)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

UPlayerStateBase* UPlayerState_Sprint::OnSprintReleased(APlayerCharacter* Character)
{
	return NewObject<UPlayerState_IdleWalk>(Character);
}

UPlayerStateBase* UPlayerState_Sprint::OnSneakPressed(APlayerCharacter* Character)
{
	return nullptr;
}

UPlayerStateBase* UPlayerState_Sprint::OnAttackPressed(APlayerCharacter* Character)
{
	if (UCombatComponent* Combat = Character->GetCombatComponent())
	{
		Combat->OnAttackHoldStarted();

		if (Combat->IsAttacking())
		{
			return NewObject<UPlayerState_Attacking>(Character);
		}
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_Sprint::OnAttackReleased(APlayerCharacter* Character)
{
	if (UCombatComponent* Combat = Character->GetCombatComponent())
	{
		Combat->OnAttackReleased();

		if (Combat->IsAttacking())
		{
			return NewObject<UPlayerState_Attacking>(Character);
		}
	}

	return nullptr;
}

// SNEAK STATE
void UPlayerState_Sneak::EnterState(APlayerCharacter* Character)
{
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = SneakSpeed;
	Character->Crouch();
}

void UPlayerState_Sneak::ExitState(APlayerCharacter* Character)
{
	Character->UnCrouch();
}

UPlayerStateBase* UPlayerState_Sneak::OnSneakReleased(APlayerCharacter* Character)
{
	return NewObject<UPlayerState_IdleWalk>(Character);
}

// ATTACKING STATE
UPlayerStateBase* UPlayerState_Attacking::UpdateState(APlayerCharacter* Character, float DeltaTime)
{
	UCombatComponent* Combat = Character->GetCombatComponent();
	if (!Combat || !Combat->IsAttacking())
	{
		return NewObject<UPlayerState_IdleWalk>(Character);
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_Attacking::OnAttackPressed(APlayerCharacter* Character)
{
	if (UCombatComponent* Combat = Character->GetCombatComponent())
	{
		Combat->OnAttackHoldStarted();
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_Attacking::OnAttackReleased(APlayerCharacter* Character)
{
	if (UCombatComponent* Combat = Character->GetCombatComponent())
	{
		Combat->OnAttackReleased();
	}

	return nullptr;
}