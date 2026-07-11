// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatComponent.h"

#include "../Character/PlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

APlayerCharacter* UCombatComponent::GetOwnerCharacter() const
{
	return Cast<APlayerCharacter>(GetOwner());
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
		EndCombo();
		return;
	}

	CurrentComboIndex = NextIndex;
	PlayComboAttack(CurrentComboIndex);
}

void UCombatComponent::PlayComboAttack(int32 Index)
{
	APlayerCharacter* Character = GetOwnerCharacter();
	if (!Character || !ComboAttacks.IsValidIndex(Index))
	{
		EndCombo();
		return;
	}

	UAnimMontage* Montage = ComboAttacks[Index].Montage;
	UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (!Montage || !AnimInstance)
	{
		EndCombo();
		return;
	}

	bComboWindowOpen = false;
	bNextAttackQueued = false;

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

	EndCombo();
}

void UCombatComponent::EndCombo()
{
	CurrentComboIndex = INDEX_NONE;
	bComboWindowOpen = false;
	bNextAttackQueued = false;
}

void UCombatComponent::OnComboWindowOpen()
{
	bComboWindowOpen = true;

	if (bNextAttackQueued)
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
	APlayerCharacter* Character = GetOwnerCharacter();
	if (!Character || !Character->GetMesh())
	{
		return;
	}

	TraceSocketName = SocketName;
	TraceRadius = Radius;
	TraceDamage = DamageOverride >= 0.0f ? DamageOverride
		: (ComboAttacks.IsValidIndex(CurrentComboIndex) ? ComboAttacks[CurrentComboIndex].DamageAmount : 0.0f);

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

	APlayerCharacter* Character = GetOwnerCharacter();
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
