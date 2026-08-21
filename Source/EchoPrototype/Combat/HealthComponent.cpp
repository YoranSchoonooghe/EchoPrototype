// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraShakeBase.h"
#include "GameFramework/PlayerController.h"
#include "DodgeComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::Heal(float Amount)
{
	if (bIsDead || Amount <= 0.0f)
	{
		return;
	}

	const float NewHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	if (FMath::IsNearlyEqual(NewHealth, CurrentHealth))
	{
		return;
	}

	CurrentHealth = NewHealth;
	OnHealthChanged.Broadcast(GetHealthPercent());
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

void UHealthComponent::AddMaxHealthBonus(float Delta)
{
	MaxHealth += Delta;
	CurrentHealth = FMath::Min(CurrentHealth + Delta, MaxHealth);
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead || IsInvulnerable())
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(GetHealthPercent());

	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		Die(DamageCauser);
	}
	else
	{
		PlayHitReactAnimation(ComputeHitDirection(DamageCauser));
		PlayCameraShakeOnOwner(DamageCameraShakeClass);
		OnDamage.Broadcast(DamageCauser);
	}
}

void UHealthComponent::PlayCameraShakeOnOwner(TSubclassOf<UCameraShakeBase> ShakeClass) const
{
	if (!ShakeClass)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	APlayerController* PC = Character ? Cast<APlayerController>(Character->GetController()) : nullptr;
	if (PC)
	{
		PC->ClientStartCameraShake(ShakeClass);
	}
}

bool UHealthComponent::IsInvulnerable() const
{
	AActor* Owner = GetOwner();
	if (UDodgeComponent* Dodge = Owner ? Owner->FindComponentByClass<UDodgeComponent>() : nullptr)
	{
		return Dodge->IsInvulnerable();
	}

	return false;
}

void UHealthComponent::Kill(AActor* Instigator)
{
	if (bIsDead)
	{
		return;
	}

	CurrentHealth = 0.0f;
	bIsDead = true;

	Die(Instigator);
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

static UAnimSequence* PickAnimFromDirectionalPool(EHitDirection Direction,
	const TArray<TObjectPtr<UAnimSequence>>& FrontPool, const TArray<TObjectPtr<UAnimSequence>>& BackPool,
	const TArray<TObjectPtr<UAnimSequence>>& LeftPool, const TArray<TObjectPtr<UAnimSequence>>& RightPool)
{
	const TArray<TObjectPtr<UAnimSequence>>* Pool = &FrontPool;
	switch (Direction)
	{
	case EHitDirection::Back:
		Pool = &BackPool;
		break;
	case EHitDirection::Left:
		Pool = &LeftPool;
		break;
	case EHitDirection::Right:
		Pool = &RightPool;
		break;
	default:
		break;
	}

	if (Pool->Num() == 0)
	{
		Pool = &FrontPool;
	}

	if (Pool->Num() == 0)
	{
		return nullptr;
	}

	return (*Pool)[FMath::RandRange(0, Pool->Num() - 1)];
}

void UHealthComponent::PlayDeathAnimation(EHitDirection Direction)
{
	UAnimSequence* SelectedAnim = PickAnimFromDirectionalPool(Direction, FrontDeathAnims, BackDeathAnims, LeftDeathAnims, RightDeathAnims);

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

void UHealthComponent::PlayHitReactAnimation(EHitDirection Direction)
{
	UAnimSequence* SelectedAnim = PickAnimFromDirectionalPool(Direction, FrontHitAnims, BackHitAnims, LeftHitAnims, RightHitAnims);

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	UAnimInstance* AnimInstance = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (SelectedAnim && AnimInstance)
	{
		AnimInstance->PlaySlotAnimationAsDynamicMontage(SelectedAnim, HitReactSlotName, 0.1f, 0.1f, 1.0f, 1);
	}
}

void UHealthComponent::Die(AActor* DamageCauser)
{
	PlayDeathAnimation(ComputeHitDirection(DamageCauser));
	PlayCameraShakeOnOwner(DeathCameraShakeClass);

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		Character->GetCharacterMovement()->DisableMovement();
	}

	OnDeath.Broadcast();
}
