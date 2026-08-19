#include "PlayerStates.h"
#include "../PlayerCharacter.h"
#include "../../Combat/CombatComponent.h"
#include "../../Combat/StealthKillComponent.h"
#include "../../Combat/DodgeComponent.h"
#include "../../Movement/ClimbingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// IDLE / WALK STATE
void UPlayerState_IdleWalk::EnterState(APlayerCharacter* Character)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = Character->GetWalkSpeed();
}

UPlayerStateBase* UPlayerState_IdleWalk::UpdateState(APlayerCharacter* Character, float DeltaTime)
{
	UClimbingComponent* Climbing = Character->GetClimbingComponent();
	if (Climbing && Climbing->IsHanging())
	{
		return NewObject<UPlayerState_Hanging>(Character);
	}

	return nullptr;
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

UPlayerStateBase* UPlayerState_IdleWalk::OnStealthKillPressed(APlayerCharacter* Character)
{
	UStealthKillComponent* StealthKill = Character->GetStealthKillComponent();
	if (StealthKill && StealthKill->GetStealthKillTarget())
	{
		StealthKill->TryStealthKill();
		return NewObject<UPlayerState_StealthKilling>(Character);
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_IdleWalk::OnClimbPressed(APlayerCharacter* Character)
{
	if (UClimbingComponent* Climbing = Character->GetClimbingComponent())
	{
		Climbing->OnClimbActionPressed();

		if (Climbing->IsHanging())
		{
			return NewObject<UPlayerState_Hanging>(Character);
		}
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_IdleWalk::OnDodgePressed(APlayerCharacter* Character)
{
	if (UDodgeComponent* Dodge = Character->GetDodgeComponent())
	{
		Dodge->TryDodge(Character->GetLastMovementInput());

		if (Dodge->IsDodging())
		{
			return NewObject<UPlayerState_Dodging>(Character);
		}
	}

	return nullptr;
}

// SPRINT STATE
void UPlayerState_Sprint::EnterState(APlayerCharacter* Character)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = Character->GetSprintSpeed();
}

UPlayerStateBase* UPlayerState_Sprint::UpdateState(APlayerCharacter* Character, float DeltaTime)
{
	UClimbingComponent* Climbing = Character->GetClimbingComponent();
	if (Climbing && Climbing->IsHanging())
	{
		return NewObject<UPlayerState_Hanging>(Character);
	}

	return nullptr;
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

UPlayerStateBase* UPlayerState_Sprint::OnClimbPressed(APlayerCharacter* Character)
{
	if (UClimbingComponent* Climbing = Character->GetClimbingComponent())
	{
		Climbing->OnClimbActionPressed();

		if (Climbing->IsHanging())
		{
			return NewObject<UPlayerState_Hanging>(Character);
		}
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_Sprint::OnDodgePressed(APlayerCharacter* Character)
{
	if (UDodgeComponent* Dodge = Character->GetDodgeComponent())
	{
		Dodge->TryDodge(Character->GetLastMovementInput());

		if (Dodge->IsDodging())
		{
			return NewObject<UPlayerState_Dodging>(Character);
		}
	}

	return nullptr;
}

// SNEAK STATE
void UPlayerState_Sneak::EnterState(APlayerCharacter* Character)
{
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = Character->GetSneakSpeed();
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

UPlayerStateBase* UPlayerState_Sneak::OnStealthKillPressed(APlayerCharacter* Character)
{
	UStealthKillComponent* StealthKill = Character->GetStealthKillComponent();
	if (StealthKill && StealthKill->GetStealthKillTarget())
	{
		StealthKill->TryStealthKill();
		return NewObject<UPlayerState_StealthKilling>(Character);
	}

	return nullptr;
}

// ATTACKING STATE
UPlayerStateBase* UPlayerState_Attacking::UpdateState(APlayerCharacter* Character, float DeltaTime)
{
	UCombatComponent* Combat = Character->GetCombatComponent();
	if (!Combat || !Combat->IsAttacking())
	{
		// A dodge pressed mid-attack is buffered (see OnDodgePressed below) rather than dropped -
		// fire it now that the attack has actually finished.
		if (Character->ConsumeBufferedDodgeIfFresh())
		{
			if (UDodgeComponent* Dodge = Character->GetDodgeComponent())
			{
				Dodge->TryDodge(Character->GetLastMovementInput());
				if (Dodge->IsDodging())
				{
					return NewObject<UPlayerState_Dodging>(Character);
				}
			}
		}

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

UPlayerStateBase* UPlayerState_Attacking::OnDodgePressed(APlayerCharacter* Character)
{
	// Can't cancel straight into a dodge mid-swing - buffer it so it fires the instant the attack
	// (or combo) actually ends, instead of the press being dropped on the floor.
	Character->BufferDodge();
	return nullptr;
}

// DODGING STATE
UPlayerStateBase* UPlayerState_Dodging::UpdateState(APlayerCharacter* Character, float DeltaTime)
{
	UDodgeComponent* Dodge = Character->GetDodgeComponent();
	if (!Dodge || !Dodge->IsDodging())
	{
		// A buffered attack (see OnAttackPressed below) fires the instant the roll finishes.
		if (Character->ConsumeBufferedAttackIfFresh())
		{
			if (UCombatComponent* Combat = Character->GetCombatComponent())
			{
				Combat->OnAttackHoldStarted();
				Combat->OnAttackReleased();

				if (Combat->IsAttacking())
				{
					return NewObject<UPlayerState_Attacking>(Character);
				}
			}
		}

		return NewObject<UPlayerState_IdleWalk>(Character);
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_Dodging::OnAttackPressed(APlayerCharacter* Character)
{
	// Can't attack out of a roll early - buffer it so it fires the instant the roll ends, instead
	// of the press being dropped on the floor.
	Character->BufferAttack();
	return nullptr;
}

// STEALTH KILL STATE
UPlayerStateBase* UPlayerState_StealthKilling::UpdateState(APlayerCharacter* Character, float DeltaTime)
{
	UStealthKillComponent* StealthKill = Character->GetStealthKillComponent();
	if (!StealthKill || !StealthKill->IsPerformingKill())
	{
		return NewObject<UPlayerState_IdleWalk>(Character);
	}

	return nullptr;
}

// HANGING STATE (ledge climbing)
UPlayerStateBase* UPlayerState_Hanging::UpdateState(APlayerCharacter* Character, float DeltaTime)
{
	UClimbingComponent* Climbing = Character->GetClimbingComponent();
	if (!Climbing || !Climbing->IsHanging())
	{
		return NewObject<UPlayerState_IdleWalk>(Character);
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_Hanging::OnClimbPressed(APlayerCharacter* Character)
{
	if (UClimbingComponent* Climbing = Character->GetClimbingComponent())
	{
		Climbing->OnClimbActionPressed();
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_Hanging::OnSprintPressed(APlayerCharacter* Character)
{
	if (UClimbingComponent* Climbing = Character->GetClimbingComponent())
	{
		Climbing->SetJumpModifierHeld(true);
	}

	return nullptr;
}

UPlayerStateBase* UPlayerState_Hanging::OnSprintReleased(APlayerCharacter* Character)
{
	if (UClimbingComponent* Climbing = Character->GetClimbingComponent())
	{
		Climbing->SetJumpModifierHeld(false);
	}

	return nullptr;
}