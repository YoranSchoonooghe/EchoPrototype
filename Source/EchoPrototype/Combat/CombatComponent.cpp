// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatComponent.h"

#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "TimerManager.h"

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
	if (!ChargeStartMontage)
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

	AnimInstance->Montage_Play(ChargeStartMontage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UCombatComponent::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ChargeStartMontage);
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
	if (ComboAttacks.Num() == 0)
	{
		return;
	}

	CurrentComboIndex = 0;
	PlayComboAttack(CurrentComboIndex);
}

void UCombatComponent::AdvanceCombo()
{
	const int32 NextIndex = CurrentComboIndex + 1;
	if (!ComboAttacks.IsValidIndex(NextIndex))
	{
		EndAttack();
		return;
	}

	CurrentComboIndex = NextIndex;
	PlayComboAttack(CurrentComboIndex);
}

void UCombatComponent::PlayComboAttack(int32 Index)
{
	if (!ComboAttacks.IsValidIndex(Index))
	{
		EndAttack();
		return;
	}

	PlayAttackMontage(ComboAttacks[Index].Montage, ComboAttacks[Index].DamageAmount);
}

void UCombatComponent::PlayChargedAttack()
{
	CurrentComboIndex = INDEX_NONE;
	PlayAttackMontage(ChargeAttack.Montage, ChargeAttack.DamageAmount);
}

void UCombatComponent::PlayAttackMontage(UAnimMontage* Montage, float Damage)
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

	AnimInstance->Montage_Play(Montage);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UCombatComponent::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);
}

void UCombatComponent::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		return;
	}

	EndAttack();
}

void UCombatComponent::EndAttack()
{
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
	TraceDamage = DamageOverride >= 0.0f ? DamageOverride : CurrentAttackDamage;

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

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || ActorsHitThisSwing.Contains(HitActor))
		{
			continue;
		}

		ActorsHitThisSwing.Add(HitActor);
		UGameplayStatics::ApplyDamage(HitActor, TraceDamage, Character->GetInstigatorController(), Character, DamageTypeClass);
	}

	PreviousTraceLocation = CurrentLocation;
}

void UCombatComponent::EndWeaponTrace()
{
	bTracing = false;
	ActorsHitThisSwing.Reset();
}
