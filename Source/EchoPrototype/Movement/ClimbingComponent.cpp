// Fill out your copyright notice in the Description page of Project Settings.

#include "ClimbingComponent.h"

#include "../CollisionChannels.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "CollisionShape.h"

namespace
{
	void PrintClimbDebug(const FString& Message, const FColor& Color = FColor::Yellow)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, Color, Message);
		}
		UE_LOG(LogTemp, Log, TEXT("[Climbing] %s"), *Message);
	}
}

UClimbingComponent::UClimbingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UClimbingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsBlending)
	{
		UpdateBlend(DeltaTime);
	}

	if (bAutoGrabWhileAirborne && !bIsHanging)
	{
		ACharacter* Character = GetOwnerCharacter();
		const bool bAirborne = Character && Character->GetCharacterMovement() && Character->GetCharacterMovement()->IsFalling();
		const bool bRegrabAllowed = GetWorld()->GetTimeSeconds() - LastDetachTime >= RegrabCooldown;

		if (bAirborne && bRegrabAllowed)
		{
			TryStartHang(false);
		}
	}
}

void UClimbingComponent::StartBlendTo(const FVector& TargetLocation, const FRotator& TargetRotation, float Duration,
	const FVector& ArcDirection, float ArcAmount)
{
	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		return;
	}

	BlendStartLocation = Character->GetActorLocation();
	BlendStartRotation = Character->GetActorQuat();
	BlendTargetLocation = TargetLocation;
	BlendTargetRotation = TargetRotation.Quaternion();
	BlendArcDirection = ArcDirection;
	BlendArcAmount = ArcAmount;
	BlendElapsed = 0.0f;
	ActiveBlendDuration = Duration;
	bIsBlending = true;
}

void UClimbingComponent::UpdateBlend(float DeltaTime)
{
	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		bIsBlending = false;
		return;
	}

	BlendElapsed += DeltaTime;
	const float Alpha = ActiveBlendDuration > 0.0f ? FMath::Clamp(BlendElapsed / ActiveBlendDuration, 0.0f, 1.0f) : 1.0f;
	const float EasedAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f);

	FVector NewLocation = FMath::Lerp(BlendStartLocation, BlendTargetLocation, EasedAlpha);

	if (!BlendArcDirection.IsNearlyZero() && !FMath::IsNearlyZero(BlendArcAmount))
	{
		const float ArcCurve = FMath::Sin(PI * Alpha);
		NewLocation += BlendArcDirection * BlendArcAmount * ArcCurve;
	}

	const FQuat NewRotation = FQuat::Slerp(BlendStartRotation, BlendTargetRotation, EasedAlpha);

	Character->SetActorLocationAndRotation(NewLocation, NewRotation);

	if (Alpha >= 1.0f)
	{
		bIsBlending = false;
	}
}

ACharacter* UClimbingComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}

bool UClimbingComponent::DetectLedge(const FVector& Origin, const FRotator& SearchRotation, FLedgeTraceResult& OutResult, bool bPrintFailures) const
{
	OutResult = FLedgeTraceResult();
	const bool bDebugMessages = bPrintDebugMessages && bPrintFailures;

	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		return false;
	}

	const FVector Forward = SearchRotation.Vector();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	const FVector ForwardTraceStart = Origin + FVector(0.0f, 0.0f, SearchHeight);
	const FVector ForwardTraceEnd = ForwardTraceStart + Forward * ForwardTraceDistance;

	FHitResult WallHit;
	const bool bHitWall = GetWorld()->SweepSingleByChannel(WallHit, ForwardTraceStart, ForwardTraceEnd, FQuat::Identity,
		ECC_Climbable, FCollisionShape::MakeCapsule(Step1CapsuleRadius, Step1CapsuleHalfHeight), QueryParams);

	if (bDrawDebugShapes)
	{
		DrawDebugCapsule(GetWorld(), (ForwardTraceStart + ForwardTraceEnd) * 0.5f, Step1CapsuleHalfHeight, Step1CapsuleRadius,
			FQuat::Identity, bHitWall ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.5f);
	}

	if (!bHitWall)
	{
		if (bDebugMessages) PrintClimbDebug(TEXT("Climb failed: Step 1 - nothing found at reach height in front of the character."), FColor::Red);
		return false;
	}

	if (WallHit.bStartPenetrating)
	{
		if (bDebugMessages) PrintClimbDebug(TEXT("Climb failed: Step 1 - search started inside geometry."), FColor::Red);
		return false;
	}

	if (FMath::Abs(WallHit.ImpactNormal.Z) > MaxWallNormalZ)
	{
		if (bDebugMessages) PrintClimbDebug(FString::Printf(TEXT("Climb failed: Step 1 - surface is not a wall (normal Z %.2f, max %.2f)."), WallHit.ImpactNormal.Z, MaxWallNormalZ), FColor::Red);
		return false;
	}

	const float Step2HalfExtent = Step2CapsuleHalfHeight + Step2CapsuleRadius;
	FVector DownTraceStart = WallHit.ImpactPoint + Forward * 5.0f;
	DownTraceStart.Z = FMath::Max(WallHit.ImpactPoint.Z, Origin.Z + SearchHeight) + LedgeSearchUpMargin + Step2HalfExtent;
	FVector DownTraceEnd = DownTraceStart;
	DownTraceEnd.Z = WallHit.ImpactPoint.Z - LedgeSearchDownMargin;

	FHitResult LedgeHit;
	const bool bHitLedge = GetWorld()->SweepSingleByChannel(LedgeHit, DownTraceStart, DownTraceEnd, FQuat::Identity,
		ECC_Climbable, FCollisionShape::MakeCapsule(Step2CapsuleRadius, Step2CapsuleHalfHeight), QueryParams);

	if (bDrawDebugShapes)
	{
		DrawDebugCapsule(GetWorld(), (DownTraceStart + DownTraceEnd) * 0.5f, Step2CapsuleHalfHeight, Step2CapsuleRadius,
			FQuat::Identity, bHitLedge ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.5f);
	}

	if (!bHitLedge)
	{
		if (bDebugMessages) PrintClimbDebug(TEXT("Climb failed: Step 2 - no ledge surface found near reach height (check LedgeSearchUpMargin/LedgeSearchDownMargin)"), FColor::Red);
		return false;
	}

	if (LedgeHit.bStartPenetrating)
	{
		if (bDebugMessages) PrintClimbDebug(TEXT("Climb failed: Step 2 - sweep started inside geometry, ledge-top height would be unreliable (raise LedgeSearchUpMargin)."), FColor::Red);
		return false;
	}

	const FVector SlopeTraceStart = LedgeHit.ImpactPoint + FVector(0.0f, 0.0f, Step3SphereRadius + 5.0f);
	const FVector SlopeTraceEnd = SlopeTraceStart - FVector(0.0f, 0.0f, (Step3SphereRadius + 5.0f) * 2.0f);

	FHitResult SlopeHit;
	const bool bHitSlope = GetWorld()->SweepSingleByChannel(SlopeHit, SlopeTraceStart, SlopeTraceEnd, FQuat::Identity,
		ECC_Climbable, FCollisionShape::MakeSphere(Step3SphereRadius), QueryParams);

	if (bDrawDebugShapes)
	{
		DrawDebugSphere(GetWorld(), (SlopeTraceStart + SlopeTraceEnd) * 0.5f, Step3SphereRadius, 12, bHitSlope ? FColor::Green : FColor::Yellow, false, 2.0f, 0, 1.5f);
	}

	if (!bHitSlope)
	{
		if (bDebugMessages) PrintClimbDebug(TEXT("Climb failed: Step 3 - could not sample the ledge surface normal"), FColor::Red);
		return false;
	}

	const float SlopeDot = FVector::DotProduct(SlopeHit.ImpactNormal, FVector::UpVector);
	const float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(SlopeDot, -1.0f, 1.0f)));
	if (SlopeAngle > MaxLedgeSlopeAngle)
	{
		if (bDebugMessages) PrintClimbDebug(FString::Printf(TEXT("Climb failed: Step 3 - ledge surface too steep (%.1f degrees, max %.1f)"), SlopeAngle, MaxLedgeSlopeAngle), FColor::Red);
		return false;
	}

	// Detect space for legs contact capsule trace below the ledge, toward the wall
	const FVector LegsTraceStart = LedgeHit.ImpactPoint + FVector(0.0f, 0.0f, -LegsContactTraceDropHeight);
	const FVector LegsTraceEnd = LegsTraceStart + Forward * LegsContactTraceDistance;

	FHitResult LegsHit;
	const bool bLegsContact = GetWorld()->SweepSingleByChannel(LegsHit, LegsTraceStart, LegsTraceEnd, FQuat::Identity,
		ECC_Climbable, FCollisionShape::MakeCapsule(Step4CapsuleRadius, Step4CapsuleHalfHeight), QueryParams);

	if (bDrawDebugShapes)
	{
		DrawDebugCapsule(GetWorld(), (LegsTraceStart + LegsTraceEnd) * 0.5f, Step4CapsuleHalfHeight, Step4CapsuleRadius,
			FQuat::Identity, bLegsContact ? FColor::Green : FColor::Blue, false, 2.0f, 0, 1.5f);
	}

	if (bDebugMessages)
	{
		PrintClimbDebug(FString::Printf(TEXT("Climb valid! LegsContact=%s"), bLegsContact ? TEXT("true") : TEXT("false")), FColor::Green);
	}

	// Free-space check 
	const FVector CandidateLedgeEdge = FVector(WallHit.ImpactPoint.X, WallHit.ImpactPoint.Y, LedgeHit.ImpactPoint.Z);
	const FVector CandidateWallNormal = WallHit.ImpactNormal.GetSafeNormal2D();
	const FVector CandidateHangLocation = CandidateLedgeEdge + CandidateWallNormal * ForwardOffset + FVector(0.0f, 0.0f, HeightOffset);

	float CapsuleRadius = 0.0f;
	float CapsuleHalfHeight = 0.0f;
	Character->GetSimpleCollisionCylinder(CapsuleRadius, CapsuleHalfHeight);

	const bool bBlocked = GetWorld()->OverlapBlockingTestByChannel(
		CandidateHangLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeCapsule(CapsuleRadius * HangSpaceCheckScale, CapsuleHalfHeight * HangSpaceCheckScale),
		QueryParams
	);

	if (bDrawDebugShapes)
	{
		DrawDebugCapsule(GetWorld(), CandidateHangLocation, CapsuleHalfHeight * HangSpaceCheckScale, CapsuleRadius * HangSpaceCheckScale,
			FQuat::Identity, bBlocked ? FColor::Red : FColor::Cyan, false, 2.0f, 0, 1.0f);
	}

	if (bBlocked)
	{
		if (bDebugMessages) PrintClimbDebug(TEXT("Climb failed: Step 5 - no room for the character at the hang position (inside a wall?)."), FColor::Red);
		return false;
	}

	OutResult.bValid = true;
	OutResult.LedgeEdgeLocation = CandidateLedgeEdge;
	OutResult.WallNormal = CandidateWallNormal;
	OutResult.bLegsContact = bLegsContact;
	OutResult.HitActor = WallHit.GetActor();
	return true;
}

void UClimbingComponent::OnClimbActionPressed()
{
	if (bIsClimbingUp)
	{
		return;
	}

	if (bPrintDebugMessages)
	{
		PrintClimbDebug(TEXT("OnClimbActionPressed() called."), FColor::Cyan);
	}

	if (bIsHanging)
	{
		StopHanging();
	}
	else
	{
		TryStartHang();
	}
}

void UClimbingComponent::TryStartHang(bool bPrintFailures)
{
	ACharacter* Character = GetOwnerCharacter();
	if (!Character || bIsHanging)
	{
		return;
	}

	FLedgeTraceResult LedgeResult;
	if (DetectLedge(Character->GetActorLocation(), Character->GetActorRotation(), LedgeResult, bPrintFailures))
	{
		UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
		Movement->SetMovementMode(MOVE_Flying);
		Movement->StopMovementImmediately();

		bSavedUseControllerRotationYaw = Character->bUseControllerRotationYaw;
		Character->bUseControllerRotationYaw = false;

		if (UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr)
		{
			SavedRootMotionMode = static_cast<uint8>(AnimInstance->RootMotionMode.GetValue());
			AnimInstance->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
		}

		ApplyHangTransform(LedgeResult);

		Movement->SetPlaneConstraintEnabled(true);
		Movement->SetPlaneConstraintNormal(LedgeResult.WallNormal);

		bIsHanging = true;
	}
}

void UClimbingComponent::ApplyHangTransform(const FLedgeTraceResult& LedgeResult, float BlendDurationOverride, bool bApplyShimmyArc)
{
	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		return;
	}

	CachedLedgeEdge = LedgeResult.LedgeEdgeLocation;
	CachedWallNormal = LedgeResult.WallNormal;
	bHasLegsContact = LedgeResult.bLegsContact;
	CachedLedgeActor = LedgeResult.HitActor;

	if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
	{
		Movement->SetPlaneConstraintNormal(CachedWallNormal);
	}

	const FRotator HangRotation = (-CachedWallNormal).Rotation();
	const FVector HangRightVector = FRotationMatrix(HangRotation).GetUnitAxis(EAxis::Y);
	const FVector HangLocation = CachedLedgeEdge + CachedWallNormal * ForwardOffset
		+ HangRightVector * SidewaysOffset + FVector(0.0f, 0.0f, HeightOffset);

	const float BlendDuration = BlendDurationOverride >= 0.0f ? BlendDurationOverride : HangBlendDuration;

	if (bApplyShimmyArc)
	{
		StartBlendTo(HangLocation, HangRotation, BlendDuration, CachedWallNormal, ShimmyArcOffset);
	}
	else
	{
		StartBlendTo(HangLocation, HangRotation, BlendDuration);
	}
}

void UClimbingComponent::SetJumpModifierHeld(bool bHeld)
{
	bJumpModifierHeld = bHeld;
}

void UClimbingComponent::HandleShimmyInput(const FVector2D& MoveInput)
{
	if (!bIsHanging)
	{
		return;
	}

	LastShimmyInput = MoveInput;
	LastShimmyInputTime = GetWorld()->GetTimeSeconds();

	if (bMoveOnCooldown || bIsBlending || bIsClimbingUp)
	{
		return;
	}

	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		return;
	}

	if (FMath::Abs(MoveInput.Y) > FMath::Abs(MoveInput.X))
	{
		if (FMath::IsNearlyZero(MoveInput.Y)) return;
		const FVector Direction = FVector::UpVector * FMath::Sign(MoveInput.Y);
		TryJumpToLedge(Direction, MoveInput);
	}
	else
	{
		if (FMath::IsNearlyZero(MoveInput.X)) return;
		const FVector Direction = Character->GetActorRightVector() * FMath::Sign(MoveInput.X);

		if (bJumpModifierHeld)
		{
			TryJumpToLedge(Direction, MoveInput);
		}
		else
		{
			TryShimmyStep(Direction, ShimmyStepDistance);
		}
	}
}

bool UClimbingComponent::ScanForLedge(const FVector& Direction, float MinDistance, float MaxDistance, FLedgeTraceResult& OutResult) const
{
	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		return false;
	}

	const bool bVerticalScan = FMath::Abs(Direction.Z) > 0.5f;
	const float VerticalOffsets[3] = { 0.0f, ShimmyVerticalTolerance, -ShimmyVerticalTolerance };
	const int32 NumOffsets = bVerticalScan ? 1 : 3;

	for (float Distance = MinDistance; Distance <= MaxDistance; Distance += ScanStepSize)
	{
		for (int32 OffsetIndex = 0; OffsetIndex < NumOffsets; ++OffsetIndex)
		{
			const FVector ProbeOrigin = Character->GetActorLocation() + Direction * Distance
				+ FVector(0.0f, 0.0f, VerticalOffsets[OffsetIndex]);

			FLedgeTraceResult Candidate;
			if (!DetectLedge(ProbeOrigin, Character->GetActorRotation(), Candidate, false))
			{
				continue;
			}

			if (!IsVerticalMoveValid(Direction, Candidate))
			{
				continue;
			}

			OutResult = Candidate;
			return true;
		}
	}

	return false;
}

bool UClimbingComponent::IsVerticalMoveValid(const FVector& Direction, const FLedgeTraceResult& LedgeResult) const
{
	if (Direction.Z > 0.5f && LedgeResult.LedgeEdgeLocation.Z < CachedLedgeEdge.Z + MinVerticalLedgeSeparation)
	{
		return false;
	}

	if (Direction.Z < -0.5f && LedgeResult.LedgeEdgeLocation.Z > CachedLedgeEdge.Z - MinVerticalLedgeSeparation)
	{
		return false;
	}

	return true;
}

void UClimbingComponent::TryShimmyStep(const FVector& Direction, float StepDistance)
{
	StartMoveCooldown(MoveCooldown);

	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		return;
	}

	const bool bMovingRight = FVector::DotProduct(Direction, Character->GetActorRightVector()) > 0.0f;
	UAnimMontage* MontageToPlay = bMovingRight ? MoveRightMontage : MoveLeftMontage;

	FLedgeTraceResult LedgeResult;
	if (ScanForLedge(Direction, StepDistance, MaxShimmyDistance, LedgeResult))
	{
		const float BlendDuration = PlayMoveMontage(MontageToPlay);
		ApplyHangTransform(LedgeResult, BlendDuration, true);
		return;
	}

	const FVector ActorForward = Character->GetActorForwardVector();
	const float AlongDirOffsets[3] = { 0.0f, -40.0f, 40.0f };
	const float AwayFromWallOffsets[2] = { 0.0f, -40.0f };

	for (float AlongDir : AlongDirOffsets)
	{
		for (float AwayFromWall : AwayFromWallOffsets)
		{
			const FVector ProbeOrigin = Character->GetActorLocation()
				+ Direction * AlongDir
				+ ActorForward * AwayFromWall;

			FLedgeTraceResult CornerResult;
			if (DetectLedge(ProbeOrigin, Direction.Rotation(), CornerResult, false))
			{
				if (bPrintDebugMessages)
				{
					PrintClimbDebug(TEXT("Inside corner: turning onto the adjacent ledge."), FColor::Green);
				}

				const float BlendDuration = PlayMoveMontage(MontageToPlay);
				ApplyHangTransform(CornerResult, BlendDuration, true);
				return;
			}
		}
	}
}

void UClimbingComponent::TryJumpToLedge(const FVector& Direction, const FVector2D& RawInput)
{
	StartMoveCooldown(MoveCooldown);

	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		return;
	}

	const bool bVertical = FMath::Abs(Direction.Z) > 0.5f;
	const float MaxReach = bVertical ? MaxVerticalReach : MaxLateralJumpDistance;
	const float MinReach = bVertical ? ScanStepSize : ShimmyStepDistance;

	FLedgeTraceResult LedgeResult;
	if (!ScanForLedge(Direction, MinReach, MaxReach, LedgeResult))
	{
		if (Direction.Z > 0.5f && TryClimbUp())
		{
			return;
		}

		if (bPrintDebugMessages) PrintClimbDebug(TEXT("Jump: no reachable ledge found in that direction."), FColor::Orange);
		return;
	}

	UAnimMontage* MontageToPlay = SelectMoveMontage(RawInput);
	float BlendDuration = PlayMoveMontage(MontageToPlay);
	if (BlendDuration < 0.0f)
	{
		BlendDuration = JumpFallbackBlendDuration;
	}

	StartMoveCooldown(FMath::Max(MoveCooldown, BlendDuration + 0.05f));

	if (bVertical)
	{
		if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
		{
			SavedMoveCollisionType = static_cast<uint8>(Capsule->GetCollisionEnabled());
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			bMoveCollisionDisabled = true;
		}
		GetWorld()->GetTimerManager().SetTimer(MoveCollisionTimerHandle, this, &UClimbingComponent::RestoreMoveCollision, BlendDuration, false);
	}

	ApplyHangTransform(LedgeResult, BlendDuration, bVertical);
}

void UClimbingComponent::RestoreMoveCollision()
{
	if (!bMoveCollisionDisabled)
	{
		return;
	}

	if (ACharacter* Character = GetOwnerCharacter())
	{
		if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
		{
			Capsule->SetCollisionEnabled(static_cast<ECollisionEnabled::Type>(SavedMoveCollisionType));
		}
	}

	bMoveCollisionDisabled = false;
}

bool UClimbingComponent::TryClimbUp()
{
	ACharacter* Character = GetOwnerCharacter();
	if (!Character)
	{
		return false;
	}

	if (!CachedLedgeActor.IsValid() || !CachedLedgeActor->ActorHasTag(ClimbUpRequiredTag))
	{
		if (bPrintDebugMessages) PrintClimbDebug(TEXT("Climb up rejected: this ledge isn't tagged for climbing up."), FColor::Red);
		return false;
	}

	float CapsuleRadius = 0.0f;
	float CapsuleHalfHeight = 0.0f;
	Character->GetSimpleCollisionCylinder(CapsuleRadius, CapsuleHalfHeight);

	const FVector StandLocation = CachedLedgeEdge + CachedWallNormal * ClimbUpForwardOffset
		+ FVector(0.0f, 0.0f, CapsuleHalfHeight + 2.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	const bool bBlocked = GetWorld()->OverlapBlockingTestByChannel(
		StandLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeCapsule(CapsuleRadius * HangSpaceCheckScale, CapsuleHalfHeight * HangSpaceCheckScale),
		QueryParams
	);

	if (bDrawDebugShapes)
	{
		DrawDebugCapsule(GetWorld(), StandLocation, CapsuleHalfHeight * HangSpaceCheckScale, CapsuleRadius * HangSpaceCheckScale,
			FQuat::Identity, bBlocked ? FColor::Red : FColor::Cyan, false, 2.0f, 0, 1.0f);
	}

	if (bBlocked)
	{
		if (bPrintDebugMessages) PrintClimbDebug(TEXT("Climb up rejected: no room to stand on top of the ledge."), FColor::Red);
		return false;
	}

	bIsClimbingUp = true;
	PendingClimbUpStandLocation = StandLocation;

	// The mantle needs to move up and forward through the wall-normal axis, which the hang's plane
	// constraint would otherwise block.
	if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
	{
		Movement->SetPlaneConstraintEnabled(false);
	}

	if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
	{
		SavedCapsuleCollisionType = static_cast<uint8>(Capsule->GetCollisionEnabled());
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	float Duration = ClimbUpFallbackDuration;
	bClimbUpRootMotionDriven = false;

	UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (ClimbUpMontage && AnimInstance)
	{
		AnimInstance->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);

		const float PlayedDuration = AnimInstance->Montage_Play(ClimbUpMontage, 1.0f, EMontagePlayReturnType::Duration);
		if (PlayedDuration > 0.0f)
		{
			Duration = PlayedDuration;
			bClimbUpRootMotionDriven = true;

			FOnMontageBlendingOutStarted BlendOutDelegate;
			BlendOutDelegate.BindUObject(this, &UClimbingComponent::HandleClimbUpMontageBlendingOut);
			AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, ClimbUpMontage);
		}
	}

	if (bPrintDebugMessages) PrintClimbDebug(TEXT("Climbing up onto the ledge."), FColor::Green);

	GetWorld()->GetTimerManager().SetTimer(ClimbUpTimerHandle, this, &UClimbingComponent::FinishClimbUp, Duration, false);
	return true;
}

void UClimbingComponent::HandleClimbUpMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	FinishClimbUp();
}

void UClimbingComponent::FinishClimbUp()
{
	if (!bIsClimbingUp)
	{
		return;
	}

	if (ACharacter* Character = GetOwnerCharacter())
	{
		UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
		Movement->SetPlaneConstraintEnabled(false);
		Movement->SetMovementMode(MOVE_Walking);
		Movement->StopMovementImmediately();
		Character->bUseControllerRotationYaw = bSavedUseControllerRotationYaw;

		if (!bClimbUpRootMotionDriven)
		{
			Character->SetActorLocation(PendingClimbUpStandLocation);
		}

		if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
		{
			Capsule->SetCollisionEnabled(static_cast<ECollisionEnabled::Type>(SavedCapsuleCollisionType));
		}

		if (UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr)
		{
			AnimInstance->SetRootMotionMode(static_cast<ERootMotionMode::Type>(SavedRootMotionMode));
		}
	}

	bIsHanging = false;
	bIsClimbingUp = false;
	bHasLegsContact = false;
	LastDetachTime = GetWorld()->GetTimeSeconds();
}

UAnimMontage* UClimbingComponent::SelectMoveMontage(const FVector2D& RawInput) const
{
	if (RawInput.Y > 0.0f) return MoveUpMontage;
	if (RawInput.Y < 0.0f) return MoveDownMontage;
	if (RawInput.X > 0.0f) return MoveRightMontage;
	if (RawInput.X < 0.0f) return MoveLeftMontage;
	return nullptr;
}

float UClimbingComponent::PlayMoveMontage(UAnimMontage* Montage)
{
	if (!Montage)
	{
		return -1.0f;
	}

	ACharacter* Character = GetOwnerCharacter();
	UAnimInstance* AnimInstance = Character && Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return -1.0f;
	}

	const float PlayedDuration = AnimInstance->Montage_Play(Montage, 1.0f, EMontagePlayReturnType::Duration);
	if (PlayedDuration <= 0.0f)
	{
		return -1.0f;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UClimbingComponent::HandleMoveMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);

	return PlayedDuration;
}

void UClimbingComponent::HandleMoveMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (ACharacter* Character = GetOwnerCharacter())
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
		}
	}
}

void UClimbingComponent::StartMoveCooldown(float Duration)
{
	bMoveOnCooldown = true;
	GetWorld()->GetTimerManager().SetTimer(MoveCooldownTimerHandle, this, &UClimbingComponent::ClearMoveCooldown, Duration, false);
}

void UClimbingComponent::ClearMoveCooldown()
{
	bMoveOnCooldown = false;
}

void UClimbingComponent::JumpFromLedge()
{
	if (!bIsHanging || bIsClimbingUp)
	{
		return;
	}

	const FVector AwayFromWall = CachedWallNormal;
	FVector2D HeldInput = FVector2D::ZeroVector;
	if (GetWorld()->GetTimeSeconds() - LastShimmyInputTime < 0.15f)
	{
		HeldInput = LastShimmyInput;
	}

	StopHanging();

	if (ACharacter* Character = GetOwnerCharacter())
	{
		FVector LaunchVelocity = FVector::UpVector * JumpOffUpVelocity;

		if (FMath::Abs(HeldInput.X) > FMath::Abs(HeldInput.Y))
		{
			LaunchVelocity += Character->GetActorRightVector() * FMath::Sign(HeldInput.X) * JumpOffSideVelocity;
		}
		else if (HeldInput.Y < -0.1f)
		{
			LaunchVelocity += AwayFromWall * JumpOffBackVelocity;
		}

		Character->LaunchCharacter(LaunchVelocity, true, true);
	}
}

void UClimbingComponent::CancelHanging()
{
	StopHanging();
}

void UClimbingComponent::StopHanging()
{
	if (!bIsHanging)
	{
		return;
	}

	if (ACharacter* Character = GetOwnerCharacter())
	{
		UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
		Movement->SetPlaneConstraintEnabled(false);
		Movement->SetMovementMode(MOVE_Falling);

		Character->bUseControllerRotationYaw = bSavedUseControllerRotationYaw;

		if (UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr)
		{
			AnimInstance->SetRootMotionMode(static_cast<ERootMotionMode::Type>(SavedRootMotionMode));
		}
		if (bIsClimbingUp)
		{
			if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
			{
				Capsule->SetCollisionEnabled(static_cast<ECollisionEnabled::Type>(SavedCapsuleCollisionType));
			}
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(MoveCooldownTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ClimbUpTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(MoveCollisionTimerHandle);

	RestoreMoveCollision();

	bIsHanging = false;
	bIsClimbingUp = false;
	bHasLegsContact = false;
	bMoveOnCooldown = false;
	bJumpModifierHeld = false;
	bIsBlending = false;
	LastDetachTime = GetWorld()->GetTimeSeconds();
}
