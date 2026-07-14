// Fill out your copyright notice in the Description page of Project Settings.
//used this tutorial for this: https://www.youtube.com/watch?v=hbPBCa4z2ko

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClimbingComponent.generated.h"

class ACharacter;
class UAnimMontage;

struct FLedgeTraceResult
{
	bool bValid = false;
	FVector LedgeEdgeLocation = FVector::ZeroVector;
	FVector WallNormal = FVector::ForwardVector;
	bool bLegsContact = false;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOPROTOTYPE_API UClimbingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UClimbingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnClimbActionPressed();

	void JumpFromLedge();
	void CancelHanging();

	//movement input (WASD)
	void HandleShimmyInput(const FVector2D& MoveInput);
	void SetJumpModifierHeld(bool bHeld);

	UFUNCTION(BlueprintPure, Category = "Climbing")
	FORCEINLINE bool IsHanging() const { return bIsHanging; }

	UFUNCTION(BlueprintPure, Category = "Climbing")
	FORCEINLINE bool HasLegsContact() const { return bHasLegsContact; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Climbing|Grab")
	bool bAutoGrabWhileAirborne = true;

	UPROPERTY(EditAnywhere, Category = "Climbing|Grab")
	float RegrabCooldown = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Grab")
	float JumpOffUpVelocity = 650.0f;

	// Sideways velocity when jumping off while holding left/right
	UPROPERTY(EditAnywhere, Category = "Climbing|Grab")
	float JumpOffSideVelocity = 400.0f;

	// Away-from-wall velocity when jumping off while holding back
	UPROPERTY(EditAnywhere, Category = "Climbing|Grab")
	float JumpOffBackVelocity = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float SearchHeight = 80.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float ForwardTraceDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float Step1CapsuleRadius = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float Step1CapsuleHalfHeight = 20.0f;


	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float Step2CapsuleRadius = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float Step2CapsuleHalfHeight = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float LedgeSearchUpMargin = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float LedgeSearchDownMargin = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float Step3SphereRadius = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float MaxLedgeSlopeAngle = 45.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection", meta = (ClampMin = "0", ClampMax = "1"))
	float MaxWallNormalZ = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection", meta = (ClampMin = "0.1", ClampMax = "1"))
	float HangSpaceCheckScale = 0.85f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float Step4CapsuleRadius = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float Step4CapsuleHalfHeight = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float LegsContactTraceDropHeight = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float LegsContactTraceDistance = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Position")
	float ForwardOffset = 35.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Position")
	float HeightOffset = -110.0f;

	// How long the character takes to smoothly blend into a new hang position
	UPROPERTY(EditAnywhere, Category = "Climbing|Position")
	float HangBlendDuration = 0.2f;

	// Normal shimmy step
	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float ShimmyStepDistance = 60.0f;

	// if no ledge exists at ShimmyStepDistance, the shimmy keeps scanning outward up to this distance 
	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float MaxShimmyDistance = 140.0f;

	// minimum Z difference between the current ledge and a new one for an up/down move to count
	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float MinVerticalLedgeSeparation = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float MaxVerticalReach = 280.0f;

	// How far a lateral jump can reach across a gap
	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float MaxLateralJumpDistance = 220.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float ScanStepSize = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float JumpRepositionDelay = 0.3f;


	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float MoveCooldown = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	TObjectPtr<UAnimMontage> JumpUpMontage;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	TObjectPtr<UAnimMontage> JumpDownMontage;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	TObjectPtr<UAnimMontage> JumpLeftMontage;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	TObjectPtr<UAnimMontage> JumpRightMontage;

	UPROPERTY(EditAnywhere, Category = "Climbing|Debug")
	bool bDrawDebugShapes = false;

	UPROPERTY(EditAnywhere, Category = "Climbing|Debug")
	bool bPrintDebugMessages = true;

private:
	bool DetectLedge(const FVector& Origin, const FRotator& SearchRotation, FLedgeTraceResult& OutResult, bool bPrintFailures = true) const;
	bool ScanForLedge(const FVector& Direction, float MinDistance, float MaxDistance, FLedgeTraceResult& OutResult) const;
	bool IsVerticalMoveValid(const FVector& Direction, const FLedgeTraceResult& LedgeResult) const;
	void TryStartHang(bool bPrintFailures = true);
	void StopHanging();
	void ApplyHangTransform(const FLedgeTraceResult& LedgeResult);
	void TryShimmyStep(const FVector& Direction, float StepDistance);
	void TryJumpToLedge(const FVector& Direction, const FVector2D& RawInput);
	void ApplyPendingJumpReposition();
	UAnimMontage* SelectJumpMontage(const FVector2D& RawInput) const;

	void StartMoveCooldown(float Duration);
	void ClearMoveCooldown();

	void StartBlendTo(const FVector& TargetLocation, const FRotator& TargetRotation);
	void UpdateBlend(float DeltaTime);

	ACharacter* GetOwnerCharacter() const;

	bool bIsHanging = false;
	bool bHasLegsContact = false;
	bool bJumpModifierHeld = false;
	bool bMoveOnCooldown = false;
	bool bSavedUseControllerRotationYaw = false;
	float LastDetachTime = -10.0f;

	FVector2D LastShimmyInput = FVector2D::ZeroVector;
	float LastShimmyInputTime = -10.0f;

	FVector CachedLedgeEdge = FVector::ZeroVector;
	FVector CachedWallNormal = FVector::ForwardVector;

	FVector PendingJumpLedgeEdge = FVector::ZeroVector;
	FVector PendingJumpWallNormal = FVector::ForwardVector;
	bool bPendingJumpLegsContact = false;

	bool bIsBlending = false;
	float BlendElapsed = 0.0f;
	FVector BlendStartLocation = FVector::ZeroVector;
	FVector BlendTargetLocation = FVector::ZeroVector;
	FQuat BlendStartRotation = FQuat::Identity;
	FQuat BlendTargetRotation = FQuat::Identity;

	FTimerHandle MoveCooldownTimerHandle;
	FTimerHandle JumpRepositionTimerHandle;
};
