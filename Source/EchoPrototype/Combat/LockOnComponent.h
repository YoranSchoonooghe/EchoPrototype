// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"

class ACharacter;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockOnTargetChanged, AActor*, NewTarget);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOPROTOTYPE_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULockOnComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Lock On")
	void ToggleLockOn();

	UFUNCTION(BlueprintCallable, Category = "Lock On")
	void ClearLockOn();

	UFUNCTION(BlueprintPure, Category = "Lock On")
	FORCEINLINE bool IsLockedOn() const { return LockedTarget.IsValid(); }

	UFUNCTION(BlueprintPure, Category = "Lock On")
	AActor* GetLockedTarget() const { return LockedTarget.Get(); }

	UPROPERTY(BlueprintAssignable, Category = "Lock On")
	FOnLockOnTargetChanged OnLockOnTargetChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Lock On")
	float LockOnRange = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Lock On", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float LockOnViewConeAngle = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Lock On")
	float TurnSpeed = 540.0f;

private:
	void FindAndSetTarget();
	void SetTarget(AActor* NewTarget);
	bool IsTargetStillValid(AActor* Target) const;

	ACharacter* GetOwnerCharacter() const;
	APlayerController* GetOwnerPlayerController() const;

	TWeakObjectPtr<AActor> LockedTarget;
};
