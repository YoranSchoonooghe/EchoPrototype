// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoComponent.h"

#include "../Echo/EchoActor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UEchoComponent::UEchoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UEchoComponent::GetEchoViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	if (ActiveEcho)
	{
		if (UCameraComponent* EchoCam = ActiveEcho->FindComponentByClass<UCameraComponent>())
		{
			OutLocation = EchoCam->GetComponentLocation();
			OutRotation = EchoCam->GetComponentRotation();
			return;
		}
	}

	OutLocation = FVector::ZeroVector;
	OutRotation = FRotator::ZeroRotator;
}

// Called when the game starts
void UEchoComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UEchoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (EchoState == EEchoState::Aiming)
	{
		UpdateAimPreview(DeltaTime);
	}

	if (FovEffectPhase == EEchoFOVEffect::ZoomingIn)
	{
		UpdateTeleportFovEffect(DeltaTime);
	}
}

APawn* UEchoComponent::GetOwnerPawn() const
{
	return Cast<APawn>(GetOwner());
}

APlayerController* UEchoComponent::GetPlayerController() const
{
	if (APawn* OwnerPawn = GetOwnerPawn())
	{
		if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			return PC;
		}
	}

	return GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
}

#pragma region input

void UEchoComponent::OnEchoPressed()
{
	PressStartTime = GetWorld()->GetTimeSeconds();
	switch (EchoState)
	{
	case EEchoState::Idle:
		BeginAiming();
		break;
	case EEchoState::Aiming:
		if (bCurrentAimIsValid)
		{
			PlaceEcho();
		}
		break;
	case EEchoState::Placed:
		TeleportToEcho();
		break;
	}
}

void UEchoComponent::OnEchoReleased()
{
	if (EchoState != EEchoState::Aiming)
	{
		return;
	}

	const double HeldDuration = GetWorld()->GetTimeSeconds() - PressStartTime;
	if (HeldDuration > TapThreshold && bCurrentAimIsValid)
	{
		PlaceEcho();
	}
}

void UEchoComponent::LookThroughEcho()
{
	if (EchoState != EEchoState::Placed || !ActiveEcho)
	{
		return;
	}

	if (bIsViewingThroughEcho)
	{
		ReturnViewToSelf();
		return;
	}

	APlayerController* PC = GetPlayerController();
	if (!PC)
	{
		return;
	}

	PC->SetViewTargetWithBlend(ActiveEcho, ViewBlendTime);
	PC->Possess(ActiveEcho);

	bIsViewingThroughEcho = true;
}
void UEchoComponent::ReturnViewToSelf()
{
	APlayerController* PC = GetPlayerController();
	APawn* OwnerPawn = GetOwnerPawn();
	if (PC && OwnerPawn)
	{
		PC->SetViewTargetWithBlend(OwnerPawn, ViewBlendTime);
		PC->Possess(OwnerPawn);
	}
	bIsViewingThroughEcho = false;
}

void UEchoComponent::AddEchoMoveInput(const FVector2D& Value)
{
	if (ActiveEcho)
	{
		const FRotator Rotation = ActiveEcho->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ActiveEcho->AddMovementInput(ForwardDirection, Value.Y);

		ActiveEcho->AddMovementInput(RightDirection, Value.X);
	}
}

void UEchoComponent::AddEchoLookInput(float Rate, float Yaw)
{
	if (ActiveEcho)
	{
		ActiveEcho->AddControllerYawInput(Rate);
		ActiveEcho->AddControllerPitchInput(Yaw);
	}
}

void UEchoComponent::TeleportToEcho()
{
	if (EchoState != EEchoState::Placed || !ActiveEcho)
	{
		return;
	}

	APawn* OwnerPawn = GetOwnerPawn();
	if (!OwnerPawn)
	{
		return;
	}

	const FVector TargetLocation = ActiveEcho->GetActorLocation();
	const FRotator TargetRotation = ActiveEcho->GetActorRotation();

	StartTeleportFovEffect();

	if (!OwnerPawn->TeleportTo(TargetLocation, TargetRotation, false, false))
	{
		if (FovEffectCamera.IsValid())
		{
			FovEffectCamera->SetFieldOfView(FovEffectBaseFOV);
		}
		FovEffectPhase = EEchoFOVEffect::None;
		return;
	}

	ReturnViewToSelf();
	DestroyActiveEcho();
}

#pragma endregion input


void UEchoComponent::StartTeleportFovEffect()
{
	APlayerController* PC = GetPlayerController();
	UCameraComponent* Cam = (PC && PC->GetPawn()) ? PC->GetPawn()->FindComponentByClass<UCameraComponent>() : nullptr;
	if (!Cam)
	{
		return;
	}

	FovEffectCamera = Cam;
	FovEffectBaseFOV = Cam->FieldOfView;
	FovEffectStartFOV = TeleportSpikeFOV;
	Cam->SetFieldOfView(TeleportSpikeFOV);
	FovEffectElapsed = 0.0f;
	FovEffectPhase = EEchoFOVEffect::ZoomingIn;
}

void UEchoComponent::UpdateTeleportFovEffect(float DeltaSeconds)
{
	if (!FovEffectCamera.IsValid())
	{
		FovEffectPhase = EEchoFOVEffect::None;
		return;
	}

	FovEffectElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(FovEffectElapsed / TeleportZoomInDuration, 0.0f, 1.0f);
	const float Eased = 1.0f - FMath::Pow(1.0f - Alpha, 3.0f);
	FovEffectCamera->SetFieldOfView(FMath::Lerp(FovEffectStartFOV, FovEffectBaseFOV, Eased));

	if (Alpha >= 1.0f)
	{
		FovEffectCamera->SetFieldOfView(FovEffectBaseFOV);
		FovEffectPhase = EEchoFOVEffect::None;
		FovEffectCamera.Reset();
	}
}

void UEchoComponent::BeginAiming()
{
	if (!EchoActorClass)
	{
		return;
	}

	FVector SpawnLocation;
	FRotator SpawnRotation;
	bool bValid = false;
	TraceForEchoLocation(SpawnLocation, SpawnRotation, bValid);
	bCurrentAimIsValid = bValid;

	const FRotator BodyRotation(0.0f, SpawnRotation.Yaw, 0.0f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ActiveEcho = GetWorld()->SpawnActor<AEchoCharacter>(EchoActorClass, FTransform(BodyRotation, SpawnLocation), SpawnParams);
	if (ActiveEcho)
	{
		ActiveEcho->SetVisualState(EEchoVisualState::Preview);
		ActiveEcho->SetPreviewValidity(bValid);
		EchoState = EEchoState::Aiming;
	}
}

void UEchoComponent::UpdateAimPreview(float DeltaSeconds)
{
	if (!ActiveEcho)
	{
		EchoState = EEchoState::Idle;
		return;
	}

	FVector TargetLocation;
	FRotator TargetRotation;
	bool bValid = false;
	TraceForEchoLocation(TargetLocation, TargetRotation, bValid);
	bCurrentAimIsValid = bValid;

	const FVector NewLocation = FMath::VInterpTo(ActiveEcho->GetActorLocation(), TargetLocation, DeltaSeconds, PreviewInterpSpeed);
	ActiveEcho->SetActorLocation(NewLocation);
	ActiveEcho->SetActorRotation(FRotator(0.0f, TargetRotation.Yaw, 0.0f));
	ActiveEcho->SetPreviewValidity(bValid);
}

void UEchoComponent::PlaceEcho()
{
	if (!ActiveEcho)
	{
		return;
	}
	ActiveEcho->SetVisualState(EEchoVisualState::Placed);
	EchoState = EEchoState::Placed;
}

void UEchoComponent::DestroyActiveEcho()
{
	if (ActiveEcho)
	{
		ActiveEcho->Destroy();
		ActiveEcho = nullptr;
	}
	EchoState = EEchoState::Idle;
	bIsViewingThroughEcho = false;
	bCurrentAimIsValid = false;
}


bool UEchoComponent::TraceForEchoLocation(FVector& OutLocation, FRotator& OutRotation, bool& bOutValid) const
{
	APawn* OwnerPawn = GetOwnerPawn();
	APlayerController* PC = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;

	if (!PC)
	{
		OutLocation = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
		OutRotation = FRotator::ZeroRotator;
		bOutValid = false;
		return false;
	}

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);
	OutRotation = CamRot;

	const FVector TraceEnd = CamLoc + (CamRot.Vector() * EchoTraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	if (ActiveEcho)
	{
		QueryParams.AddIgnoredActor(ActiveEcho);
	}

	//Forward sweep along aim direction
		FHitResult ForwardHit;
	const bool bForwardHit = GetWorld()->SweepSingleByChannel(
		ForwardHit, CamLoc, TraceEnd, FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(EchoTraceRadius), QueryParams
	);

	if (!bForwardHit)
	{
		//Nothing in range
		OutLocation = TraceEnd;
		bOutValid = false;
		return true;
	}


	if (ForwardHit.ImpactNormal.Z >= MinWalkableNormalZ)
	{
		OutLocation = ForwardHit.ImpactPoint + (ForwardHit.ImpactNormal * GroundPlacementOffset);
		bOutValid = true;
		return true;
	}

	//If forward hit was a wall, search straight down
	const FVector DownTraceStart = ForwardHit.ImpactPoint + FVector(0.0f, 0.0f, GroundSearchHeight);
	const FVector DownTraceEnd = ForwardHit.ImpactPoint - FVector(0.0f, 0.0f, GroundSearchDepth);

	FHitResult DownHit;
	const bool bDownHit = GetWorld()->SweepSingleByChannel(
		DownHit, DownTraceStart, DownTraceEnd, FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(EchoTraceRadius), QueryParams
	);

	if (bDownHit && DownHit.ImpactNormal.Z >= MinWalkableNormalZ)
	{
		OutLocation = DownHit.ImpactPoint + (DownHit.ImpactNormal * GroundPlacementOffset);
		bOutValid = true;
	}
	else
	{
		OutLocation = ForwardHit.ImpactPoint;
		bOutValid = false;
	}

	return true;
}