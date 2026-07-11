// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UAnimMontage;
class APlayerCharacter;
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

	// Called by the owning character when the attack input is pressed.
	void TryAttack();

	// Called by AnimNotifyState_ComboWindow.
	void OnComboWindowOpen();
	void OnComboWindowClose();

	// Called by AnimNotifyState_WeaponTrace.
	void BeginWeaponTrace(FName SocketName, float TraceRadius, float DamageOverride = -1.0f);
	void UpdateWeaponTrace();
	void EndWeaponTrace();

	UFUNCTION(BlueprintPure, Category = "Combat")
	FORCEINLINE bool IsAttacking() const { return CurrentComboIndex != INDEX_NONE; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FMacheteComboAttack> ComboAttacks;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

private:
	void StartCombo();
	void AdvanceCombo();
	void EndCombo();
	void PlayComboAttack(int32 Index);

	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	APlayerCharacter* GetOwnerCharacter() const;

	int32 CurrentComboIndex = INDEX_NONE;
	bool bComboWindowOpen = false;
	bool bNextAttackQueued = false;

	// Weapon trace state, reset every time BeginWeaponTrace is called.
	FName TraceSocketName;
	float TraceRadius = 40.0f;
	float TraceDamage = 0.0f;
	bool bTracing = false;
	FVector PreviousTraceLocation = FVector::ZeroVector;
	TSet<AActor*> ActorsHitThisSwing;
};
