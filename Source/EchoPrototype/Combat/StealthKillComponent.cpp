// Fill out your copyright notice in the Description page of Project Settings.

#include "StealthKillComponent.h"

#include "HealthComponent.h"
#include "../Enemies/EnemyCharacter.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Engine/OverlapResult.h"

UStealthKillComponent::UStealthKillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStealthKillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStealthKillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsPerformingKill)
	{
		return;
	}

	AEnemyCharacter* NewTarget = FindStealthKillTarget();
	AEnemyCharacter* OldTarget = CurrentTarget.Get();

	if (NewTarget != OldTarget)
	{
		if (OldTarget)
		{
			OldTarget->ShowStealthKillPrompt(false);
		}

		if (NewTarget)
		{
			NewTarget->ShowStealthKillPrompt(true);
		}

		CurrentTarget = NewTarget;
	}
}

AEnemyCharacter* UStealthKillComponent::FindStealthKillTarget() const
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return nullptr;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		OwnerCharacter->GetActorLocation(),
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(DetectionRadius),
		QueryParams
	);

	const float MinDotFromBehind = FMath::Cos(FMath::DegreesToRadians(MaxAngleFromBehind));

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Overlap.GetActor());
		if (!Enemy || Enemy->GetAlertState() != EAlertState::Neutral)
		{
			continue;
		}

		UHealthComponent* EnemyHealth = Enemy->FindComponentByClass<UHealthComponent>();
		if (!EnemyHealth || EnemyHealth->IsDead())
		{
			continue;
		}

		const FVector ToPlayer = (OwnerCharacter->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal2D();
		if (ToPlayer.IsNearlyZero())
		{
			continue;
		}

		const float DotFromBehind = FVector::DotProduct(-Enemy->GetActorForwardVector(), ToPlayer);
		if (DotFromBehind < MinDotFromBehind)
		{
			continue;
		}

		return Enemy;
	}

	return nullptr;
}

void UStealthKillComponent::TryStealthKill()
{
	AEnemyCharacter* Target = CurrentTarget.Get();
	if (!Target || bIsPerformingKill)
	{
		return;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	Target->ShowStealthKillPrompt(false);

	const FRotator TargetRotation = Target->GetActorRotation();
	const FVector SnapLocation = Target->GetActorLocation() - (Target->GetActorForwardVector() * SnapDistance);
	OwnerCharacter->SetActorLocationAndRotation(SnapLocation, TargetRotation);

	bIsPerformingKill = true;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh() ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
	if (StealthKillMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(StealthKillMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &UStealthKillComponent::HandleMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, StealthKillMontage);
	}
	else
	{
		bIsPerformingKill = false;
	}

	if (UHealthComponent* TargetHealth = Target->FindComponentByClass<UHealthComponent>())
	{
		TargetHealth->Kill(OwnerCharacter);
	}

	CurrentTarget = nullptr;
}

void UStealthKillComponent::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsPerformingKill = false;
}
