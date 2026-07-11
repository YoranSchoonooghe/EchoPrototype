// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bIsDead = false;

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	const FString DamageMsg = FString::Printf(TEXT("%s took %.1f damage from %s, health: %.1f / %.1f"),
		*GetNameSafe(DamagedActor), Damage, *GetNameSafe(DamageCauser), CurrentHealth, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("%s"), *DamageMsg);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor::Yellow, DamageMsg);
	}

	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;

		const FString DeathMsg = FString::Printf(TEXT("%s died"), *GetNameSafe(DamagedActor));

		UE_LOG(LogTemp, Log, TEXT("%s"), *DeathMsg);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red, DeathMsg);
		}

		Die(DamageCauser);
	}
}

EHitDirection UHealthComponent::ComputeHitDirection(AActor* DamageCauser) const
{
	AActor* Owner = GetOwner();
	if (!Owner || !DamageCauser || DamageCauser == Owner)
	{
		return EHitDirection::Front;
	}

	const FVector ToDamageCauser = (DamageCauser->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal2D();
	if (ToDamageCauser.IsNearlyZero())
	{
		return EHitDirection::Front;
	}

	const float ForwardDot = FVector::DotProduct(Owner->GetActorForwardVector(), ToDamageCauser);
	const float RightDot = FVector::DotProduct(Owner->GetActorRightVector(), ToDamageCauser);

	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		return ForwardDot >= 0.0f ? EHitDirection::Front : EHitDirection::Back;
	}

	return RightDot >= 0.0f ? EHitDirection::Right : EHitDirection::Left;
}

void UHealthComponent::PlayDeathAnimation(EHitDirection Direction)
{
	const TArray<TObjectPtr<UAnimSequence>>* Pool = &FrontDeathAnims;
	switch (Direction)
	{
	case EHitDirection::Back:
		Pool = &BackDeathAnims;
		break;
	case EHitDirection::Left:
		Pool = &LeftDeathAnims;
		break;
	case EHitDirection::Right:
		Pool = &RightDeathAnims;
		break;
	default:
		break;
	}

	if (Pool->Num() == 0)
	{
		Pool = &FrontDeathAnims;
	}

	if (Pool->Num() == 0)
	{
		return;
	}

	UAnimSequence* SelectedAnim = (*Pool)[FMath::RandRange(0, Pool->Num() - 1)];

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	UAnimInstance* AnimInstance = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (SelectedAnim && AnimInstance)
	{
		if (UAnimMontage* DeathMontage = AnimInstance->PlaySlotAnimationAsDynamicMontage(SelectedAnim, DeathSlotName, 0.25f, 0.25f, 1.0f, 1))
		{
			// Freeze on the last frame of the death pose instead of blending back to locomotion.
			DeathMontage->bEnableAutoBlendOut = false;
		}

		if (AActor* Owner = GetOwner())
		{
			Owner->SetLifeSpan(SelectedAnim->GetPlayLength());
		}
	}
}

void UHealthComponent::Die(AActor* DamageCauser)
{
	PlayDeathAnimation(ComputeHitDirection(DamageCauser));

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		Character->GetCharacterMovement()->DisableMovement();
	}

	OnDeath.Broadcast();
}
