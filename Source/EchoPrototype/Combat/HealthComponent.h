// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UDamageType;
class UAnimSequence;
class AController;

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Front,
	Back,
	Left,
	Right
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageSignature, AActor*, Aggressor);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOPROTOTYPE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE bool IsDead() const { return bIsDead; }

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDamageSignature OnDamage;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Death")
	FName DeathSlotName = "FullBody";

	UPROPERTY(EditAnywhere, Category = "Death")
	TArray<TObjectPtr<UAnimSequence>> FrontDeathAnims;

	UPROPERTY(EditAnywhere, Category = "Death")
	TArray<TObjectPtr<UAnimSequence>> BackDeathAnims;

	UPROPERTY(EditAnywhere, Category = "Death")
	TArray<TObjectPtr<UAnimSequence>> LeftDeathAnims;

	UPROPERTY(EditAnywhere, Category = "Death")
	TArray<TObjectPtr<UAnimSequence>> RightDeathAnims;

private:
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void Die(AActor* DamageCauser);
	EHitDirection ComputeHitDirection(AActor* DamageCauser) const;
	void PlayDeathAnimation(EHitDirection Direction);

	float CurrentHealth = 0.0f;
	bool bIsDead = false;
};
