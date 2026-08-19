// Fill out your copyright notice in the Description page of Project Settings.

#include "DodgeComponent.h"

#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"

UDodgeComponent::UDodgeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ACharacter* UDodgeComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}

EDodgeDirection UDodgeComponent::ComputeDodgeDirection(const FVector2D& InputDirection2D)
{
	if (InputDirection2D.IsNearlyZero())
	{
		return EDodgeDirection::Backward;
	}

	const float AngleDegrees = FMath::RadiansToDegrees(FMath::Atan2(InputDirection2D.X, InputDirection2D.Y));
	const float Normalized = FMath::Fmod(AngleDegrees + 360.0f, 360.0f);
	const int32 Octant = FMath::RoundToInt(Normalized / 45.0f) % 8;

	switch (Octant)
	{
	case 0: return EDodgeDirection::Forward;
	case 1: return EDodgeDirection::ForwardRight;
	case 2: return EDodgeDirection::Right;
	case 3: return EDodgeDirection::BackwardRight;
	case 4: return EDodgeDirection::Backward;
	case 5: return EDodgeDirection::BackwardLeft;
	case 6: return EDodgeDirection::Left;
	case 7: return EDodgeDirection::ForwardLeft;
	default: return EDodgeDirection::Forward;
	}
}

UAnimMontage* UDodgeComponent::GetDirectMontageForDirection(EDodgeDirection Direction) const
{
	switch (Direction)
	{
	case EDodgeDirection::Forward:
		return ForwardDodgeMontage;
	case EDodgeDirection::Backward:
		return BackwardDodgeMontage;
	case EDodgeDirection::Left:
		return LeftDodgeMontage;
	case EDodgeDirection::Right:
		return RightDodgeMontage;
	case EDodgeDirection::ForwardLeft:
		return ForwardLeftDodgeMontage;
	case EDodgeDirection::ForwardRight:
		return ForwardRightDodgeMontage;
	case EDodgeDirection::BackwardLeft:
		return BackwardLeftDodgeMontage;
	case EDodgeDirection::BackwardRight:
		return BackwardRightDodgeMontage;
	}

	return nullptr;
}

UAnimMontage* UDodgeComponent::GetMontageForDirection(EDodgeDirection Direction) const
{
	if (UAnimMontage* Montage = GetDirectMontageForDirection(Direction))
	{
		return Montage;
	}
	switch (Direction)
	{
	case EDodgeDirection::ForwardLeft:
	case EDodgeDirection::ForwardRight:
		return ForwardDodgeMontage;
	case EDodgeDirection::BackwardLeft:
	case EDodgeDirection::BackwardRight:
		return BackwardDodgeMontage;
	default:
		return nullptr;
	}
}

void UDodgeComponent::TryDodge(const FVector2D& InputDirection2D)
{
	if (bIsDodging)
	{
		return;
	}

	ACharacter* Character = GetOwnerCharacter();
	UAnimInstance* AnimInstance = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return;
	}

	UAnimMontage* Montage = GetMontageForDirection(ComputeDodgeDirection(InputDirection2D));
	if (!Montage)
	{
		return;
	}

	bIsDodging = true;
	bIsInvulnerable = false;

	AnimInstance->Montage_Play(Montage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UDodgeComponent::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);
}

void UDodgeComponent::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsDodging = false;
	bIsInvulnerable = false;
}

void UDodgeComponent::BeginInvulnerability()
{
	bIsInvulnerable = true;
}

void UDodgeComponent::EndInvulnerability()
{
	bIsInvulnerable = false;
}
