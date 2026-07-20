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
	AActor* HitActor = nullptr;
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

	UFUNCTION(BlueprintPure, Category = "Climbing")
	AActor* GetGrabbedLedgeActor() const { return bIsHanging ? CachedLedgeActor.Get() : nullptr; }

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
	float MaxWallNormalZ = 0.6f;

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

	UPROPERTY(EditAnywhere, Category = "Climbing|Detection")
	float LedgeValidityCheckRadius = 12.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Position")
	float ForwardOffset = 35.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Position")
	float HeightOffset = -63.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Position")
	float SidewaysOffset = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Position")
	float HangBlendDuration = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float ShimmyArcOffset = 29.0f;

	// Normal shimmy step
	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float ShimmyStepDistance = 120.0f;

	// if no ledge exists at ShimmyStepDistance, the shimmy keeps scanning outward up to this distance
	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float MaxShimmyDistance = 280.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float ShimmyVerticalTolerance = 50.0f;

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
	float JumpFallbackBlendDuration = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	float MoveCooldown = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	TObjectPtr<UAnimMontage> MoveUpMontage;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	TObjectPtr<UAnimMontage> MoveDownMontage;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	TObjectPtr<UAnimMontage> MoveLeftMontage;

	UPROPERTY(EditAnywhere, Category = "Climbing|Movement")
	TObjectPtr<UAnimMontage> MoveRightMontage;

	UPROPERTY(EditAnywhere, Category = "Climbing|ClimbUp")
	TObjectPtr<UAnimMontage> ClimbUpMontage;

	UPROPERTY(EditAnywhere, Category = "Climbing|ClimbUp")
	float ClimbUpForwardOffset = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Climbing|ClimbUp")
	float ClimbUpFallbackDuration = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Climbing|ClimbUp")
	FName ClimbUpRequiredTag = "Up";

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
	void ApplyHangTransform(const FLedgeTraceResult& LedgeResult, float BlendDurationOverride = -1.0f, bool bApplyShimmyArc = false);
	void TryShimmyStep(const FVector& Direction, float StepDistance);
	void TryJumpToLedge(const FVector& Direction, const FVector2D& RawInput);
	UAnimMontage* SelectMoveMontage(const FVector2D& RawInput) const;
	float PlayMoveMontage(UAnimMontage* Montage);
	void HandleMoveMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	bool TryClimbUp();
	void FinishClimbUp();
	void HandleClimbUpMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	bool IsCachedLedgeStillValid() const;

	void StartMoveCooldown(float Duration);
	void ClearMoveCooldown();

	void RestoreMoveCollision();

	void StartBlendTo(const FVector& TargetLocation, const FRotator& TargetRotation, float Duration,
		const FVector& ArcDirection = FVector::ZeroVector, float ArcAmount = 0.0f);
	void UpdateBlend(float DeltaTime);

	ACharacter* GetOwnerCharacter() const;

	bool bIsHanging = false;
	bool bIsClimbingUp = false;
	bool bHasLegsContact = false;
	bool bJumpModifierHeld = false;
	bool bMoveOnCooldown = false;
	bool bSavedUseControllerRotationYaw = false;
	float LastDetachTime = -10.0f;

	FVector PendingClimbUpStandLocation = FVector::ZeroVector;
	FTimerHandle ClimbUpTimerHandle;

	bool bClimbUpRootMotionDriven = false;

	uint8 SavedCapsuleCollisionType = 0;

	bool bMoveCollisionDisabled = false;
	uint8 SavedMoveCollisionType = 0;
	FTimerHandle MoveCollisionTimerHandle;

	FVector2D LastShimmyInput = FVector2D::ZeroVector;
	float LastShimmyInputTime = -10.0f;

	uint8 SavedRootMotionMode = 0;

	FVector CachedLedgeEdge = FVector::ZeroVector;
	FVector CachedWallNormal = FVector::ForwardVector;
	TWeakObjectPtr<AActor> CachedLedgeActor;

	bool bIsBlending = false;
	float BlendElapsed = 0.0f;
	float ActiveBlendDuration = 0.2f;
	FVector BlendStartLocation = FVector::ZeroVector;
	FVector BlendTargetLocation = FVector::ZeroVector;
	FQuat BlendStartRotation = FQuat::Identity;
	FQuat BlendTargetRotation = FQuat::Identity;
	FVector BlendArcDirection = FVector::ZeroVector;
	float BlendArcAmount = 0.0f;

	FTimerHandle MoveCooldownTimerHandle;
};
