// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatComponent.h"

#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraShakeBase.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "TimerManager.h"
#include "HealthComponent.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

ACharacter* UCombatComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}

void UCombatComponent::AddAttackDamageMultiplierBonus(float Delta)
{
	AttackDamageMultiplier += Delta;
}

void UCombatComponent::SetWeaponEquipped(bool bEquipped)
{
	bWeaponEquipped = bEquipped;
}

const TArray<FMacheteComboAttack>& UCombatComponent::GetActiveComboAttacks() const
{
	return bWeaponEquipped ? WeaponComboAttacks : ComboAttacks;
}

const FMacheteComboAttack& UCombatComponent::GetActiveChargeAttack() const
{
	return bWeaponEquipped ? WeaponChargeAttack : ChargeAttack;
}

UAnimMontage* UCombatComponent::GetActiveChargeStartMontage() const
{
	return bWeaponEquipped ? WeaponChargeStartMontage : ChargeStartMontage;
}

void UCombatComponent::OnAttackHoldStarted()
{
	bChargeReady = false;
	GetWorld()->GetTimerManager().SetTimer(ChargeTimerHandle, this, &UCombatComponent::OnChargeThresholdReached, ChargeThreshold, false);

	if (CurrentComboIndex == INDEX_NONE)
	{
		PlayChargeStartAnimation();
	}
}

void UCombatComponent::PlayChargeStartAnimation()
{
	UAnimMontage* Montage = GetActiveChargeStartMontage();
	if (!Montage)
	{
		return;
	}

	ACharacter* Character = GetOwnerCharacter();
	UAnimInstance* AnimInstance = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return;
	}

	bIsAttacking = true;
	CurrentComboIndex = INDEX_NONE;
	bComboWindowOpen = false;
	bNextAttackQueued = false;
	ActiveAttackMontage = Montage;

	AnimInstance->Montage_Play(Montage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UCombatComponent::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);
}

void UCombatComponent::OnChargeThresholdReached()
{
	bChargeReady = true;
	PlayChargedAttack();
}

void UCombatComponent::OnAttackReleased()
{
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimerHandle);

	if (bChargeReady)
	{
		return;
	}

	TryAttack();
}

void UCombatComponent::TryAttack()
{
	if (CurrentComboIndex == INDEX_NONE)
	{
		StartCombo();
		return;
	}

	if (bComboWindowOpen)
	{
		AdvanceCombo();
	}
	else
	{
		bNextAttackQueued = true;
	}
}

void UCombatComponent::StartCombo()
{
	if (GetActiveComboAttacks().Num() == 0)
	{
		return;
	}

	CurrentComboIndex = 0;
	PlayComboAttack(CurrentComboIndex);
}

void UCombatComponent::AdvanceCombo()
{
	const int32 NextIndex = CurrentComboIndex + 1;
	if (!GetActiveComboAttacks().IsValidIndex(NextIndex))
	{
		EndAttack();
		return;
	}

	CurrentComboIndex = NextIndex;
	PlayComboAttack(CurrentComboIndex);
}

void UCombatComponent::PlayComboAttack(int32 Index)
{
	const TArray<FMacheteComboAttack>& Attacks = GetActiveComboAttacks();
	if (!Attacks.IsValidIndex(Index))
	{
		EndAttack();
		return;
	}

	PlayAttackMontage(Attacks[Index].Montage, Attacks[Index].DamageAmount, Attacks[Index].StunDuration);
}

void UCombatComponent::PlayChargedAttack()
{
	const FMacheteComboAttack& Charge = GetActiveChargeAttack();
	CurrentComboIndex = INDEX_NONE;
	PlayAttackMontage(Charge.Montage, Charge.DamageAmount, Charge.StunDuration);
}

void UCombatComponent::PlayAttackMontage(UAnimMontage* Montage, float Damage, float StunDuration)
{
	ACharacter* Character = GetOwnerCharacter();
	UAnimInstance* AnimInstance = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (!Montage || !AnimInstance)
	{
		EndAttack();
		return;
	}

	bIsAttacking = true;
	bComboWindowOpen = false;
	bNextAttackQueued = false;
	CurrentAttackDamage = Damage;
	CurrentAttackStunDuration = StunDuration;

	ActiveAttackMontage = Montage;

	AnimInstance->Montage_Play(Montage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UCombatComponent::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);
}

void UCombatComponent::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActiveAttackMontage)
	{
		return;
	}

	EndAttack();
}

void UCombatComponent::EndAttack()
{
	ActiveAttackMontage = nullptr;
	CurrentComboIndex = INDEX_NONE;
	bComboWindowOpen = false;
	bNextAttackQueued = false;
	bIsAttacking = false;
}

void UCombatComponent::OnComboWindowOpen()
{
	bComboWindowOpen = true;

	if (bNextAttackQueued && CurrentComboIndex != INDEX_NONE)
	{
		AdvanceCombo();
	}
}

void UCombatComponent::OnComboWindowClose()
{
	bComboWindowOpen = false;
}

void UCombatComponent::BeginWeaponTrace(FName SocketName, float Radius, float DamageOverride)
{
	ACharacter* Character = GetOwnerCharacter();
	if (!Character || !Character->GetMesh())
	{
		return;
	}

	TraceSocketName = SocketName;
	TraceRadius = Radius;
	TraceDamage = (DamageOverride >= 0.0f ? DamageOverride : CurrentAttackDamage) * AttackDamageMultiplier;
	TraceStunDuration = CurrentAttackStunDuration;

	ActorsHitThisSwing.Reset();
	PreviousTraceLocation = Character->GetMesh()->GetSocketLocation(TraceSocketName);
	bTracing = true;
}

void UCombatComponent::UpdateWeaponTrace()
{
	if (!bTracing)
	{
		return;
	}

	ACharacter* Character = GetOwnerCharacter();
	if (!Character || !Character->GetMesh())
	{
		return;
	}

	const FVector CurrentLocation = Character->GetMesh()->GetSocketLocation(TraceSocketName);

	TArray<FHitResult> Hits;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	GetWorld()->SweepMultiByChannel(
		Hits,
		PreviousTraceLocation,
		CurrentLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams
	);

	bool bHitSomethingThisUpdate = false;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || ActorsHitThisSwing.Contains(HitActor))
		{
			continue;
		}

		ActorsHitThisSwing.Add(HitActor);
		UGameplayStatics::ApplyDamage(HitActor, TraceDamage, Character->GetInstigatorController(), Character, DamageTypeClass);
		OnWeaponHit.Broadcast(HitActor, Hit.ImpactPoint, Hit.ImpactNormal);

		if (TraceStunDuration > 0.0f)
		{
			if (UHealthComponent* HitHealth = HitActor->FindComponentByClass<UHealthComponent>())
			{
				HitHealth->ApplyStun(TraceStunDuration);
			}
		}

		bHitSomethingThisUpdate = true;
	}

	if (bHitSomethingThisUpdate)
	{
		PlayCameraShakeOnOwner(HitLandedCameraShakeClass);
		PlayRumbleOnOwner(HitLandedRumbleEffect);
	}

	PreviousTraceLocation = CurrentLocation;
}

void UCombatComponent::PlayCameraShakeOnOwner(TSubclassOf<UCameraShakeBase> ShakeClass) const
{
	if (!ShakeClass)
	{
		return;
	}

	ACharacter* Character = GetOwnerCharacter();
	APlayerController* PC = Character ? Cast<APlayerController>(Character->GetController()) : nullptr;
	if (PC)
	{
		PC->ClientStartCameraShake(ShakeClass);
	}
}

void UCombatComponent::PlayRumbleOnOwner(UForceFeedbackEffect* RumbleEffect) const
{
	if (!RumbleEffect)
	{
		return;
	}

	ACharacter* Character = GetOwnerCharacter();
	APlayerController* PC = Character ? Cast<APlayerController>(Character->GetController()) : nullptr;
	if (PC)
	{
		PC->ClientPlayForceFeedback(RumbleEffect, FForceFeedbackParameters());
	}
}

void UCombatComponent::EndWeaponTrace()
{
	bTracing = false;
	ActorsHitThisSwing.Reset();
}
