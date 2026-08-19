// Fill out your copyright notice in the Description page of Project Settings.

#include "LockOnComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "../Enemies/EnemyCharacter.h"
#include "HealthComponent.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
}

ACharacter* ULockOnComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}

APlayerController* ULockOnComponent::GetOwnerPlayerController() const
{
	ACharacter* Character = GetOwnerCharacter();
	return Character ? Cast<APlayerController>(Character->GetController()) : nullptr;
}

void ULockOnComponent::ToggleLockOn()
{
	if (LockedTarget.IsValid())
	{
		ClearLockOn();
	}
	else
	{
		FindAndSetTarget();
	}
}

void ULockOnComponent::ClearLockOn()
{
	SetTarget(nullptr);
}

void ULockOnComponent::SetTarget(AActor* NewTarget)
{
	if (LockedTarget.Get() == NewTarget)
	{
		return;
	}

	LockedTarget = NewTarget;
	SetComponentTickEnabled(NewTarget != nullptr);

	OnLockOnTargetChanged.Broadcast(NewTarget);
}

bool ULockOnComponent::IsTargetStillValid(AActor* Target) const
{
	if (!IsValid(Target))
	{
		return false;
	}

	if (UHealthComponent* TargetHealth = Target->FindComponentByClass<UHealthComponent>())
	{
		if (TargetHealth->IsDead())
		{
			return false;
		}
	}

	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		return false;
	}

	return FVector::Dist(Character->GetActorLocation(), Target->GetActorLocation()) <= LockOnRange;
}

void ULockOnComponent::FindAndSetTarget()
{
	ACharacter* Character = GetOwnerCharacter();
	APlayerController* PC = GetOwnerPlayerController();
	if (!Character || !PC)
	{
		return;
	}

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector CamForward = CamRot.Vector();

	TArray<AActor*> Candidates;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyCharacter::StaticClass(), Candidates);

	AActor* BestCandidate = nullptr;
	float BestScore = TNumericLimits<float>::Max();

	for (AActor* Candidate : Candidates)
	{
		if (!IsTargetStillValid(Candidate))
		{
			continue;
		}

		const FVector ToCandidate = Candidate->GetActorLocation() - CamLoc;
		const float Distance = ToCandidate.Size();
		if (Distance <= KINDA_SMALL_NUMBER)
		{
			continue;
		}

		const float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CamForward, ToCandidate / Distance)));
		if (AngleDegrees > LockOnViewConeAngle)
		{
			continue;
		}

		const float Score = Distance * (1.0f + AngleDegrees / FMath::Max(LockOnViewConeAngle, 1.0f));
		if (Score < BestScore)
		{
			BestScore = Score;
			BestCandidate = Candidate;
		}
	}

	SetTarget(BestCandidate);
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Target = LockedTarget.Get();
	if (!IsTargetStillValid(Target))
	{
		SetTarget(nullptr);
		return;
	}

	APlayerController* PC = GetOwnerPlayerController();
	ACharacter* Character = GetOwnerCharacter();
	if (!PC || !Character)
	{
		return;
	}

	const FVector ToTarget = Target->GetActorLocation() - Character->GetActorLocation();
	const FRotator DesiredRotation = ToTarget.Rotation();
	const FRotator NewRotation = FMath::RInterpConstantTo(PC->GetControlRotation(), DesiredRotation, DeltaTime, TurnSpeed);

	PC->SetControlRotation(NewRotation);
}
