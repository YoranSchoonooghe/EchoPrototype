// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UAnimMontage;
class ACharacter;
class UDamageType;

USTRUCT(BlueprintType)
struct FMacheteComboAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	float DamageAmount = 15.0f;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOPROTOTYPE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

	void OnAttackHoldStarted();
	void OnAttackReleased();

	// Called by AnimNotifyState_ComboWindow
	void OnComboWindowOpen();
	void OnComboWindowClose();

	// Called by AnimNotifyState_WeaponTrace
	void BeginWeaponTrace(FName SocketName, float TraceRadius, float DamageOverride = -1.0f);
	void UpdateWeaponTrace();
	void EndWeaponTrace();

	UFUNCTION(BlueprintPure, Category = "Combat")
	FORCEINLINE bool IsAttacking() const { return bIsAttacking; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	FORCEINLINE bool IsChargeReady() const { return bChargeReady; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AddAttackDamageMultiplierBonus(float Delta);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FMacheteComboAttack> ComboAttacks;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FMacheteComboAttack ChargeAttack;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> ChargeStartMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChargeThreshold = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackDamageMultiplier = 1.0f;

private:
	void TryAttack();
	void StartCombo();
	void AdvanceCombo();
	void PlayChargedAttack();
	void PlayChargeStartAnimation();
	void PlayComboAttack(int32 Index);
	void PlayAttackMontage(UAnimMontage* Montage, float Damage);
	void EndAttack();

	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnChargeThresholdReached();

	ACharacter* GetOwnerCharacter() const;

	int32 CurrentComboIndex = INDEX_NONE;
	bool bComboWindowOpen = false;
	bool bNextAttackQueued = false;
	bool bIsAttacking = false;
	float CurrentAttackDamage = 0.0f;

	bool bChargeReady = false;
	FTimerHandle ChargeTimerHandle;

	// Weapon trace state
	FName TraceSocketName;
	float TraceRadius = 40.0f;
	float TraceDamage = 0.0f;
	bool bTracing = false;
	FVector PreviousTraceLocation = FVector::ZeroVector;
	TSet<AActor*> ActorsHitThisSwing;
};
