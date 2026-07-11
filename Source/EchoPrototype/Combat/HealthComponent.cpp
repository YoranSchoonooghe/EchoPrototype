// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || IsDead())
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

	if (IsDead())
	{
		const FString DeathMsg = FString::Printf(TEXT("%s died"), *GetNameSafe(DamagedActor));

		UE_LOG(LogTemp, Log, TEXT("%s"), *DeathMsg);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red, DeathMsg);
		}
	}
}
