// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UDamageType;
class UAnimSequence;
class AController;
class UCameraShakeBase;
class UForceFeedbackEffect;

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

//Bloodspatter effect
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, HealthPercent);

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

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Kill(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddMaxHealthBonus(float Delta);

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDamageSignature OnDamage;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const { return MaxHealth > 0.0f ? (CurrentHealth / MaxHealth) : 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount);

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

	UPROPERTY(EditAnywhere, Category = "Hit Reaction")
	FName HitReactSlotName = "FullBody";

	UPROPERTY(EditAnywhere, Category = "Hit Reaction")
	TArray<TObjectPtr<UAnimSequence>> FrontHitAnims;

	UPROPERTY(EditAnywhere, Category = "Hit Reaction")
	TArray<TObjectPtr<UAnimSequence>> BackHitAnims;

	UPROPERTY(EditAnywhere, Category = "Hit Reaction")
	TArray<TObjectPtr<UAnimSequence>> LeftHitAnims;

	UPROPERTY(EditAnywhere, Category = "Hit Reaction")
	TArray<TObjectPtr<UAnimSequence>> RightHitAnims;

	UPROPERTY(EditDefaultsOnly, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> DamageCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Camera Shake")
	TSubclassOf<UCameraShakeBase> DeathCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Controller Rumble")
	TObjectPtr<UForceFeedbackEffect> DamageRumbleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Controller Rumble")
	TObjectPtr<UForceFeedbackEffect> DeathRumbleEffect;

private:
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void Die(AActor* DamageCauser);
	EHitDirection ComputeHitDirection(AActor* DamageCauser) const;
	void PlayDeathAnimation(EHitDirection Direction);
	void PlayHitReactAnimation(EHitDirection Direction);
	bool IsInvulnerable() const;
	void PlayCameraShakeOnOwner(TSubclassOf<UCameraShakeBase> ShakeClass) const;
	void PlayRumbleOnOwner(UForceFeedbackEffect* RumbleEffect) const;

	float CurrentHealth = 0.0f;
	bool bIsDead = false;
};
