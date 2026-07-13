// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StealthKillComponent.generated.h"

class UAnimMontage;
class AEnemyCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOPROTOTYPE_API UStealthKillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStealthKillComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "Stealth Kill")
	AEnemyCharacter* GetStealthKillTarget() const { return CurrentTarget.Get(); }

	UFUNCTION(BlueprintPure, Category = "Stealth Kill")
	FORCEINLINE bool IsPerformingKill() const { return bIsPerformingKill; }

	void TryStealthKill();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Stealth Kill")
	TObjectPtr<UAnimMontage> StealthKillMontage;

	UPROPERTY(EditAnywhere, Category = "Stealth Kill")
	float DetectionRadius = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Stealth Kill", meta = (ClampMin = "0", ClampMax = "180"))
	float MaxAngleFromBehind = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Stealth Kill")
	float SnapDistance = 80.0f;

private:
	AEnemyCharacter* FindStealthKillTarget() const;

	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	TWeakObjectPtr<AEnemyCharacter> CurrentTarget;
	bool bIsPerformingKill = false;
};
