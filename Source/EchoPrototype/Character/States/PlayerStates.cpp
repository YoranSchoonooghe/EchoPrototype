#include "PlayerStates.h"
#include "../PlayerCharacter.h"
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