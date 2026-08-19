// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DodgeComponent.generated.h"

class UAnimMontage;
class ACharacter;

UENUM(BlueprintType)
enum class EDodgeDirection : uint8
{
	Forward,
	ForwardRight,
	Right,
	BackwardRight,
	Backward,
	BackwardLeft,
	Left,
	ForwardLeft
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOPROTOTYPE_API UDodgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDodgeComponent();

	void TryDodge(const FVector2D& InputDirection2D);

	UFUNCTION(BlueprintPure, Category = "Dodge")
	FORCEINLINE bool IsDodging() const { return bIsDodging; }

	UFUNCTION(BlueprintPure, Category = "Dodge")
	FORCEINLINE bool IsInvulnerable() const { return bIsInvulnerable; }

	void BeginInvulnerability();
	void EndInvulnerability();

protected:
	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> ForwardDodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> BackwardDodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> LeftDodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> RightDodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> ForwardLeftDodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> ForwardRightDodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> BackwardLeftDodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Dodge")
	TObjectPtr<UAnimMontage> BackwardRightDodgeMontage;

private:
	static EDodgeDirection ComputeDodgeDirection(const FVector2D& InputDirection2D);
	UAnimMontage* GetMontageForDirection(EDodgeDirection Direction) const;
	UAnimMontage* GetDirectMontageForDirection(EDodgeDirection Direction) const;

	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	ACharacter* GetOwnerCharacter() const;

	bool bIsDodging = false;
	bool bIsInvulnerable = false;
};
